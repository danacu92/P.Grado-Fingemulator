#pragma once
#include "ggponet.h"

namespace Rollback {
   extern GGPOSession* ggpo;
   extern GGPOErrorCode result;
   extern GGPOSessionCallbacks cb;
   extern GGPOPlayer p1;
   extern GGPOPlayer p2;
   extern GGPOPlayerHandle p1Handle;
   extern GGPOPlayerHandle p2Handle;  
   extern int player_local;

    void iniciar_conexion(char* remote_ip, unsigned short remote_port, unsigned short local_port, int input_delay, int player);
    void cerrar_conexion();
    void idle(int time);
    void get_stats(GGPONetworkStats &stats);
    void sync_test();
}