
#pragma once

// Exportación/importación de funciones en la DLL o biblioteca estática
#ifdef FINGEMULATORAPI_EXPORTS
#define FINGEMULATOR_API __declspec(dllexport)
#else
#define FINGEMULATOR_API __declspec(dllimport)
#endif

#include <stdint.h> // Tipos enteros como uint16_t, uint32_t
#include <stdbool.h> // Tipo booleano en C

#ifdef __cplusplus
extern "C" {
#endif

    //// Enum para los sistemas soportados
    //typedef enum {
    //    SYSTEM_NONE = 0,
    //    SYSTEM_NES = 1
    //} SYSTEM_NAME;

    // Funciones de la API
    FINGEMULATOR_API void init();
    FINGEMULATOR_API void set_system(int system);
    FINGEMULATOR_API bool load_rom(const char* path);
    FINGEMULATOR_API void update_inputs(uint16_t inputs);
    FINGEMULATOR_API uint32_t* advanceFrameOffline(bool muted);
    FINGEMULATOR_API uint8_t read_memory(uint16_t address);
    FINGEMULATOR_API void destroy();

#ifdef __cplusplus
}
#endif