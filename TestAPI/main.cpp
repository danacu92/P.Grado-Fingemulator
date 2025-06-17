#include <iostream>
#include <string>
#include "FingemulatorAPI.h"
#include "SDL.h"

int main(int argc, char* argv[]) {
    // Verificar si se pasó un archivo como argumento
    std::string romPath;
    if (argc > 1) {
        romPath = argv[1]; // Ruta del archivo arrastrado
        std::cout << "Archivo cargado: " << romPath << std::endl;
    }
    else {
        std::cerr << "Por favor, arrastra un archivo ROM sobre el ejecutable para cargarlo." << std::endl;
        return -1;
    }

    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Error al inicializar SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Crear la ventana SDL
    SDL_Window* window = SDL_CreateWindow("Emulador NES", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Error al crear la ventana SDL: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Crear un renderer para la ventana
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Error al crear el renderer SDL: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Inicializar APU y cargar el sistema
    init();
    set_system(1);

    // Cargar la ROM
    if (!load_rom(romPath.c_str())) {
        std::cerr << "Error: No se pudo cargar la ROM." << std::endl;
        destroy(); // Limpia recursos antes de salir
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Preparar la textura de la ventana donde se dibujarán los frames
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 256, 240); // Tamaño de la pantalla NES

    // Bucle de renderizado
    bool running = true;
    SDL_Event event;
    while (running) {
        // Manejar eventos de SDL (como cerrar la ventana)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Actualizar entradas (puedes ajustar según sea necesario)
        update_inputs(rand() % (2 ^ 16));
        update_inputs(0x0000);

        // Obtener el siguiente frame del emulador
        uint32_t* frame = advanceFrameOffline(true);

        std::cerr << read_memory(0x0700) << std::endl;  //Test lectura ram;
       // Actualizar la textura con el nuevo frame
        SDL_UpdateTexture(texture, NULL, frame, 256 * sizeof(uint32_t)); // Actualiza la textura con el frame

        // Limpiar la pantalla
        SDL_RenderClear(renderer);

        // Dibujar la textura en la ventana
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        // Renderizar la ventana
        SDL_RenderPresent(renderer);

        // Controlar el frame rate (si es necesario)
        SDL_Delay(16); // Aproximadamente 60 FPS
    }

    // Limpiar recursos antes de salir
    destroy(); // Limpia recursos del emulador
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}