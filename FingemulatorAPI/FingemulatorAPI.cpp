#include "pch.h" // Incluir solo si estás usando precompiled headers
#include "FingemulatorAPI.h"
#include "EmuAPI.h" 

// Instancia única de la clase EmuAPI para gestionar el emulador
static IEmulatorAPI* g_emulator = nullptr;


void init() {
    if (!g_emulator) {
        g_emulator = new EmuAPI(); // Inicializar si aún no está asignado
    }
}
// Configura el sistema a emular (e.g., NES)
void set_system(int system) {
    g_emulator->setSystem(system);
}

// Carga una ROM desde la ruta especificada
bool load_rom(const char* path) {
    if (!path || path[0] == '\0') {
        return false; // Retorna falso si el path es inválido
    }
    
    return g_emulator->load_rom(path);
}

// Actualiza los inputs del emulador
void update_inputs(uint16_t inputs) {
    g_emulator->updateInputs(inputs);
}

// Avanza un frame  retorna el frame renderizado
uint32_t* advanceFrameOffline(bool muted) {
    return g_emulator->advanceFrameOffline(true);
}

// Lee un byte de memoria de la CPU del emulador
uint8_t read_memory(uint16_t address) {
    return g_emulator->readMemory(address);
}

FINGEMULATOR_API void destroy()
{
    if (g_emulator) {
        delete g_emulator;
        g_emulator = nullptr;
    }
}
