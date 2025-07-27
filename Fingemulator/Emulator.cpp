#include "Emulator.h"
#include <windows.h>
#include <commdlg.h>
#include <string>
#include <iostream>
//#include "SDL_Handler.h"
#include "NES.h"
#include "Rollback.h"



Emulator::Emulator()
{
    ahaead_state.length = 0;
    quick_save.length = 0;
}

Emulator::~Emulator()
{
}

void Emulator::set_System(SYSTEM_NAME system)
{
    sys = system;
    consola = nullptr;
}

void Emulator::cpu_Memory_Write(Word addres, Byte data)
{
    consola->cpu_Write(addres, data);
}

Byte Emulator::cpu_Memory_Read(Word addres, bool ReadOnly)
{
    return consola->cpu_Read(addres, ReadOnly);
}

int Emulator::loadRom()
{
    emuState = Opening;
    OPENFILENAMEA ofn;
    char szFile[260];

    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string exeDir = exePath;
    exeDir = exeDir.substr(0, exeDir.find_last_of("\\/"));

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);

    ofn.lpstrInitialDir = exeDir.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    switch (sys) {
    case nes:
        ofn.lpstrFilter = "NES ROM Files\0*.nes\0 Any File\0*.*\0";

        break;
    }
    if (GetOpenFileNameA(&ofn) == TRUE)
    {
        std::string path = std::string(ofn.lpstrFile);
        std::cout << "Archivo seleccionado: " << path << std::endl;
        switch (sys) {
        case nes:
            consola = std::make_shared<NES>(); break;
            break;
        }
        if (consola->load_rom(path)) {
            return 1;
        }
        else {
            return -1;
            sys = NoSystem;
            consola = nullptr;
        }
    }
    else
    {
        std::cout << "No se seleccionó ningún archivo." << std::endl;
        sys = NoSystem;
        return 0;
    }
}

void Emulator::pauseEmulator()
{
    emuState = Paused;

}

void Emulator::reanudeEmulator()
{
    emuState = RunningOffline;

}
void Emulator::PowerOn()
{
    Frame_counter = 0;
    consola->turnON();
    emuState = RunningOffline;
    if (ahaead_state.length > 0){
        deleteSavestate(ahaead_state.serialized_state);
        ahaead_state.length = 0;
    }
    switch (sys) {
    case nes:
        res_WIDTH = 256;
        res_HEIGHT = 240;
        break;
    }
    //sdl = std::make_shared<SDL_Handler>();
    correrEmulador = true;
}

uint32_t* Emulator::RunFrame()
{       
    if (emuState == EMULATOR_STATE::RunningOffline) {
        consola->UpdateInputs(inputsP1);
        consola->UpdateInputs(inputsP2);
        frame = consola->run_frame();
        Frame_counter++;
    }
    else if (emuState == EMULATOR_STATE::RunningOnline) {
        Word p[2];
        int disconnect_flags;
        if (Rollback::player_local == 1) {
            p[0] = inputsP1;
            Rollback::result = ggpo_add_local_input(Rollback::ggpo, Rollback::p1Handle, &p[0], sizeof(p[0]));
        }
        else if (Rollback::player_local == 2)
        {
            p[1] = inputsP2;
            Rollback::result = ggpo_add_local_input(Rollback::ggpo, Rollback::p2Handle, &p[1], sizeof(p[1]));
        }
        if (Rollback::result == GGPO_ERRORCODE_SUCCESS) 
        {
            Word syncro_inputs[2];
            Rollback::result = ggpo_synchronize_input(Rollback::ggpo, syncro_inputs, sizeof(syncro_inputs), &disconnect_flags);
            if (Rollback::result == GGPO_ERRORCODE_SUCCESS) 
            {
                consola->UpdateInputs(syncro_inputs[0]);
                consola->UpdateInputs(syncro_inputs[1]);
                frame = consola->run_frame();
                Frame_counter++;
                ggpo_advance_frame(Rollback::ggpo);
               
            }
            else {
               /* cout << "Error al sincronizar inputs!\n";*/
            }
        }
        else {
           
        }
    }
        return frame;
}

uint32_t* Emulator::Advance_frame_offline(bool Muted)
{
    consola->UpdateInputs(inputsP1);
    consola->UpdateInputs(inputsP2);
   return  frame = consola->run_frame(Muted);
}

void Emulator::PowerOff()
{
    consola->reset();
    emuState = Empty;
    sys = NoSystem;
    consola = nullptr;
    Frame_counter = 0;
    if (ahaead_state.length >0)
        deleteSavestate(ahaead_state.serialized_state);
    ahaead_state.length = 0;
}

bool Emulator::emuRunning()
{
    return (emuState == RunningOffline || emuState == RunningOnline || emuState == Paused);
}

void Emulator::DesactivarSonido()
{
    consola->DesactivarSonido();
}

void Emulator::ActivarSonido()
{
    consola->ActivarSonido();
}

void Emulator::reset()
{
    if (emuState == EMULATOR_STATE::RunningOffline || emuState == EMULATOR_STATE::Paused) {
        consola->reset();
        emuState = RunningOffline;
        Frame_counter = 0;
        if (ahaead_state.length >0)
            deleteSavestate(ahaead_state.serialized_state);
        ahaead_state.length = 0;
    }
}

void Emulator::close()
{
    consola = nullptr;
    emuState = Empty;
}

void Emulator::updateInputs(Word status)
{
    if (latch)
        inputsP2 = status;
    else 
        inputsP1 = status;
    latch = !latch;
}

void Emulator::QuickSave()
{
    if (quick_save.length >0) {
        consola->DeleteState(quick_save.serialized_state);  // Libera la memoria previamente asignada
    }

    consola->SaveState(quick_save.serialized_state, quick_save.length);
}


void Emulator::LoadState(unsigned char* save) {
    if (save == nullptr) {
        if (quick_save.length > 0)
        consola->LoadState(quick_save.serialized_state);
    }
    else {
        consola->LoadState(save);
    }
}

uint32_t* Emulator::RunAhead()
{
    if (emuState == EMULATOR_STATE::RunningOffline) {
        if (ahaead_state.length > 0) {
            LoadState(ahaead_state.serialized_state);
            consola->DeleteState(ahaead_state.serialized_state);
            ahaead_state.length = 0;
        }
        consola->UpdateInputs(inputsP1);
        consola->UpdateInputs(inputsP2);
        frame = consola->run_frame();
        Frame_counter++;
        consola->SaveState(ahaead_state.serialized_state, ahaead_state.length);
        DesactivarSonido();
        uint32_t * frame =  Advance_frame_offline(true);
        ActivarSonido();
        return frame;
    }
}

void Emulator::deleteSavestate(unsigned char* save)
{
    consola->DeleteState(save);   
}

void Emulator::cleanAheadState()
{
    if (ahaead_state.length > 0) {
        consola->DeleteState(ahaead_state.serialized_state);
        ahaead_state.length = 0;
    }
}
    

void Emulator::advance_frame()
{
    Word p[2];
    int disconnect_flags;
    ggpo_synchronize_input(Rollback::ggpo, p, sizeof(p), &disconnect_flags);
   
    
       /* cout << "Advance frame!!!1\n";*/
        consola->UpdateInputs(p[0]);
        consola->UpdateInputs(p[1]);
        DesactivarSonido();
        consola->run_frame(true);
        ActivarSonido();
        ggpo_advance_frame(Rollback::ggpo);
    
}


void Emulator::rollback_save(unsigned char*& buffer, int* &length)
{
    consola->SaveState(buffer, *length);
}


