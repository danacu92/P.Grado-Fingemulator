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
    //    SYSTEM_NES = 1
    //} SYSTEM_CODE;

    // Formato del input 
	//{
	//	struct {
	//		Word A : 1;
	//		Word B : 1;
	//		Word X : 1;
	//		Word Y : 1;
	//		Word L : 1;
	//		Word R : 1;
	//		Word Select : 1;
	//		Word Start : 1;
	//		Word Up : 1;
	//		Word Down : 1;
	//		Word Left : 1;
	//		Word Right : 1;
	//	} Button;
	//	Word reg;
	//};
    // Funciones de la API   
    FINGEMULATOR_API void init(); // Crea e inicializa una instancia del sistema
    FINGEMULATOR_API void set_system(int system); // Setea el sistema que se usara, usar el enumerado mas arriba como referencia para que int usar.
    FINGEMULATOR_API bool load_rom(const char* path); // Carga el rom.
    FINGEMULATOR_API void update_inputs(uint16_t inputs); // Para definir los inputs de este frame, debe llamarse dos veces por frame, la primera para el input de player 1, la segunda para player 2.
    FINGEMULATOR_API uint32_t* advanceFrameOffline(bool muted); // Corre la logica del emulador, devuelve el frame representado como un arreglo de pixeles.
    FINGEMULATOR_API uint8_t read_memory(uint16_t address); // Funcion para poder leer direcciones de memoria, puede ser util si se sabe donde el juego guarda puntuaciones por ejemplo.
    FINGEMULATOR_API void destroy(); //Destruye la instancia del sistema


#ifdef __cplusplus
}
#endif