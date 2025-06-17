#include "Rollback.h"
#include "Emulator.h"
#include <windows.h>

//cheksum para los saves

int fletcher32_checksum(short* data, size_t len)
{
    int sum1 = 0xffff, sum2 = 0xffff;

    while (len) {
        size_t tlen = len > 360 ? 360 : len;
        len -= tlen;
        do {
            sum1 += *data++;
            sum2 += sum1;
        } while (--tlen);
        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }

    /* Second reduction step to reduce sums to 16 bits */
    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    return sum2 << 16 | sum1;
}


namespace Rollback {

    GGPOSession* ggpo = nullptr;  // Inicialización
    GGPOErrorCode result = GGPO_OK;
    GGPOSessionCallbacks cb = {};  
    GGPOPlayer p1 = {};  
    GGPOPlayer p2 = {}; 
    GGPOPlayerHandle p1Handle = {};  
    GGPOPlayerHandle p2Handle = {};
    int player_local = 0;



    bool __cdecl vw_begin_game_callback(const char* game) {
        Emulator::getInstance()->reset();
        Emulator::getInstance()->emuState = EMULATOR_STATE::RunningOnline;
        return true;
    }

    bool __cdecl vw_advance_frame_callback(int) {
        Emulator::getInstance()->advance_frame();
        return true;

    }

    bool __cdecl
        vw_load_game_state_callback(unsigned char* buffer, int len)
    {

        Emulator::getInstance()->LoadState(buffer);
        return true;
    }

    bool __cdecl
        vw_save_game_state_callback(unsigned char** buffer, int* len, int* checksum, int)
    {
        Emulator::getInstance()->rollback_save(*buffer, len);
        *checksum = fletcher32_checksum((short*)*buffer, *len / 2);
        return true;
    }

    bool __cdecl
        vw_log_game_state(char* filename, unsigned char* buffer, int)
    {
        return true;
    }

    void __cdecl
        vw_free_buffer(void* buffer)
    {
        free (buffer);
    }


    bool __cdecl
        vw_on_event_callback(GGPOEvent* info)
    {
        switch (info->code) {
        case GGPO_EVENTCODE_CONNECTED_TO_PEER:
            cout << "Conectando con el par \n";
            break;
        case GGPO_EVENTCODE_SYNCHRONIZING_WITH_PEER:
            cout << "Sincronizando con el par\n";
            break;
        case GGPO_EVENTCODE_SYNCHRONIZED_WITH_PEER:
            cout << "Se ha sincronizado con el par!\n";
            break;
        case GGPO_EVENTCODE_RUNNING:
            cout << "Corriendo\n";
            break;
        case GGPO_EVENTCODE_CONNECTION_INTERRUPTED:
            cout << "Conexion interumpida!\n";

            break;
        case GGPO_EVENTCODE_CONNECTION_RESUMED:
            cout << "Conexion Resumida!\n";
            break;
        case GGPO_EVENTCODE_DISCONNECTED_FROM_PEER:
            cout << "Desconectado del par!\n";
            Emulator::getInstance()->emuState = EMULATOR_STATE::Paused;
            Rollback::cerrar_conexion();
            break;
        case GGPO_EVENTCODE_TIMESYNC:
            //cout << "Demasiados frames de diferencia! Sincronizando...\n"; 
            Sleep(1000 * info->u.timesync.frames_ahead / 60);
            break;
        }
        return true;
    }


    void iniciar_conexion(char* remote_ip, unsigned short remote_port, unsigned short local_port, int input_delay, int player) {
        
        player_local = player;

        // Set callbacks
        cb.begin_game = vw_begin_game_callback;
        cb.advance_frame = vw_advance_frame_callback;
        cb.load_game_state = vw_load_game_state_callback;
        cb.save_game_state = vw_save_game_state_callback;
        cb.free_buffer = vw_free_buffer;
        cb.on_event = vw_on_event_callback;
        cb.log_game_state = vw_log_game_state;

        //ggpo_set_frame_delay(ggpo, handle, FRAME_DELAY); // se puede agregar input delay!
        //Set players
        p1.size = p2.size = sizeof(GGPOPlayer);
        p1.player_num = 1;
        p2.player_num = 2;
        if (player == 1) {
            p1.type = GGPO_PLAYERTYPE_LOCAL;                // local player
            
            p2.type = GGPO_PLAYERTYPE_REMOTE;               // remote player
            strcpy_s(p2.u.remote.ip_address, remote_ip);  // ip addess of the player
            p2.u.remote.port = remote_port;               // port of that player
        }
        if (player == 2) {
            p2.type = GGPO_PLAYERTYPE_LOCAL;                // local player
            p1.type = GGPO_PLAYERTYPE_REMOTE;               // remote player
            strcpy_s(p1.u.remote.ip_address, remote_ip);  // ip addess of the player
            p1.u.remote.port = remote_port;               // port of that player
        }

        result = ggpo_start_session(&ggpo,&cb,"Fingemulator", 2, sizeof(Word),local_port);

        ggpo_set_disconnect_timeout(ggpo, 5000);
        ggpo_set_disconnect_notify_start(ggpo, 2000);
        if (result == GGPO_ERRORCODE_SUCCESS) {
            result = ggpo_add_player(ggpo, &p1, &p1Handle);
            result = ggpo_add_player(ggpo, &p2, &p2Handle);
            if (player_local == 1) ggpo_set_frame_delay(ggpo, p1Handle, input_delay);
            else ggpo_set_frame_delay(ggpo, p2Handle, input_delay);
        }
        else {
            cout << "fallo el start session";
        }
        
    }

    void cerrar_conexion()
    {
        ggpo_close_session(ggpo);
        Emulator::getInstance()->PowerOff();
    }

    void idle(int time)
    {
        ggpo_idle(ggpo, time);
    }

    void sync_test() // por probar
    {
        cb.begin_game = vw_begin_game_callback;
        cb.advance_frame = vw_advance_frame_callback;
        cb.load_game_state = vw_load_game_state_callback;
        cb.save_game_state = vw_save_game_state_callback;
        cb.free_buffer = vw_free_buffer;
        cb.on_event = vw_on_event_callback;
        cb.log_game_state = vw_log_game_state;

        p1.size = p2.size = sizeof(GGPOPlayer);
        p1.player_num = 1;
        p2.player_num = 2;
        p1.type = GGPO_PLAYERTYPE_LOCAL;                // local player
        p2.type = GGPO_PLAYERTYPE_LOCAL;                // local player


        result = ggpo_start_synctest(&ggpo, &cb, const_cast<char*>("test"), 2, sizeof(Word), 60);
        result = ggpo_add_player(ggpo, &p1, &p1Handle);
        result = ggpo_add_player(ggpo, &p2, &p1Handle);
        if (result != GGPO_OK) {
            std::cerr << "Error al iniciar el sync test de GGPO" << std::endl;
            return;
        }
    }

    void get_stats(GGPONetworkStats &stats)
    {
        if (player_local == 1) ggpo_get_network_stats(ggpo, p2Handle, &stats);
        else if (player_local == 2) ggpo_get_network_stats(ggpo, p1Handle, &stats);
    }
    
}