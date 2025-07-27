#include <iostream>
#include "SDL.h"
#include"Sistema.h"
#include"NES.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "Emulator.h"
#include "Rollback.h"
#include <winsock2.h>
#include <chrono>
#include <queue>

//Instancia de la clase emulador
Emulator* emu = Emulator::getInstance();;

// Variables SDL
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* gameTexture;
SDL_Texture* background;
SDL_Joystick* joystick = nullptr;

// Control virtual con el que se envia inputs a los emuladores.
Vcontroller emuControllers[2] = {0};

// Variables que mantiene el programa abierto
bool done = false;

// Variables para configurar inputs
struct input_info {
    int button= -1;
    int idJoystick= -1;
};
struct input_str {
    //Teclado
    int kMapping[12];
    std::unordered_map<int, int> kDic;
    //Gamepad
    input_info bMapping[12];
};

input_str phisycal_gamepad[2];
int defaultKeyboardMapping[2][12]=
{
    {SDL_SCANCODE_X, SDL_SCANCODE_Z, SDL_SCANCODE_S, SDL_SCANCODE_A,
    SDL_SCANCODE_D, SDL_SCANCODE_C, SDL_SCANCODE_2, SDL_SCANCODE_1,
    SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT
    }, 
    {SDL_SCANCODE_KP_2, SDL_SCANCODE_KP_1, SDL_SCANCODE_KP_5, SDL_SCANCODE_KP_4,
    SDL_SCANCODE_KP_6, SDL_SCANCODE_KP_3, SDL_SCANCODE_0, SDL_SCANCODE_9,
    SDL_SCANCODE_I, SDL_SCANCODE_K, SDL_SCANCODE_J, SDL_SCANCODE_L
    }
};

int Boton_a_Configurar = -1;
int player_config = 0; //que jugador se esta configurando.
int num_joysticks = 0;
std::vector<SDL_GameController*> Vec_SDLJoys;


// Variables resolucion nativa de la consola emulada, para saber el tamaño de la ventana a crear.
int native_res_with;
int native_res_height;

// Variables de tamaño de ventana principal.
int windowWidth = 1280;
int windowHeight = 720;


//Variables para saber si el emulador esta corriendo y si hay que dibujar frames y tomar inputs.
bool emuRunning = false;


//Booleanos para saber si mostrar cierta ventana debe permanecer visible o no. 
bool showConfigControllerWindow = false; //config gamepad
bool showConfigKeyboardWindow = false; // config keyboard
bool showFrameRateWindow = false;
bool showOnlineConnection = false;
bool showOnlineStats = false;
bool testMode = false;
bool ShowInputDelayWindow = false;




// Configurar keyboard
void ShowKeyboardConfigWindow(int player) {
    
    string inputs[12] = { "A", "B", "X", "Y", "L", "R", "Select", "Start", "Up", "Down", "Left", "Right" };

    static bool waitingForInput = false;

    string kwinNAME = "Configuracion de Teclado PLayer " + std::to_string(player + 1);
    ImGui::Begin(kwinNAME.c_str());

    for (int i = 0; i < 12; i++) {
        int key = phisycal_gamepad[player].kMapping[i]; 

        ImGui::Text("%s", inputs[i].c_str());
        ImGui::SameLine();

        const char* keyName = key >= 0 ? SDL_GetScancodeName(static_cast<SDL_Scancode>(key)) : "No Asignado";
        std::string buttonID = std::string(keyName) + "##" + std::to_string(i);

        // Iniciar la espera de entrada para esta acción
        if (ImGui::Button(buttonID.c_str())) {
            
            Boton_a_Configurar = i;
            waitingForInput = true;
        }
    }

    if (waitingForInput) {
        ImGui::Text("Presiona una tecla del teclado...");


        SDL_PumpEvents();  // Actualizo el estado del teclado
        const Uint8* keystate = SDL_GetKeyboardState(NULL);

        // Detecta si alguna tecla fue presionada
        for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
            if (keystate[i]) {

                int old_input = phisycal_gamepad[player].kMapping[Boton_a_Configurar];
                if (old_input != -1) {

                    phisycal_gamepad[player].kDic.erase(old_input);
                }
                phisycal_gamepad[player].kMapping[Boton_a_Configurar] = i;  
                waitingForInput = false; 
                break;

            }
        }
    }

    ImGui::Dummy(ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 30.0f)); // Ajusta 30.0f según el tamaño del botón
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x) / 2);

    if (ImGui::Button("Aceptar")) {
        showConfigKeyboardWindow = false;
        for (int i = 0; i < 12; i++) {
            
            phisycal_gamepad[player].kDic[(phisycal_gamepad[player].kMapping[i])] = i;
        
        }
    }
    // Botón para restablecer valores predeterminados
    ImGui::SameLine();
    if (ImGui::Button("Default")) {
        for (int i = 0; i < 12; i++) {
         
                phisycal_gamepad[player].kMapping[i] = defaultKeyboardMapping[player][i];
        }
    }
    ImGui::End();
}

void update_key(int key, int player, bool active) {

    switch (key) {
    case 0:
        emuControllers[player].Button.A = active;
        break;
    case 1:
        emuControllers[player].Button.B = active;
        break;
    case 2:
        emuControllers[player].Button.X = active;
        break;
    case 3:
        emuControllers[player].Button.Y = active;
        break;
    case 4:
        emuControllers[player].Button.L = active;
        break;
    case 5:
        emuControllers[player].Button.R = active;
        break;
    case 6:
        emuControllers[player].Button.Select = active;
        break;
    case 7:
        emuControllers[player].Button.Start = active;
        break;
    case 8:
        emuControllers[player].Button.Up = active;
        break;
    case 9:
        emuControllers[player].Button.Down = active;
        break;
    case 10:
        emuControllers[player].Button.Left = active;
        break;
    case 11:
        emuControllers[player].Button.Right = active;
        break;
    }
}


void ShowJoystickConfigWindow(int player) {
    string inputs[12] = { "A", "B", "X", "Y", "L", "R", "Select", "Start", "Up", "Down", "Left", "Right" };
   
    static bool waitingForInput = false;
    
    string bwinNAME = "Configuracion de Gamepad PLayer " + std::to_string(player + 1);
    ImGui::Begin(bwinNAME.c_str());
    for (int i = 0; i < 12; i++) {
        string joynum = "Joy" + to_string(phisycal_gamepad[player].bMapping[i].idJoystick)+ " ";
        string button = "B" + to_string(phisycal_gamepad[player].bMapping[i].button);
        ImGui::Text("%s", inputs[i].c_str());
        ImGui::SameLine();
        
        string bID = (phisycal_gamepad[player].bMapping[i].button >= 0 ? joynum + button +"##"+ std::to_string(i) : "No Asignado""##" + std::to_string(i));
        if (ImGui::Button(bID.c_str())) {
            // Iniciar la espera de entrada para esta acción
            Boton_a_Configurar = i;
            waitingForInput = true;
        }
    }

    if (waitingForInput) {
        ImGui::Text("Presiona un boton del joystick...");
    }

 
    // Si estamos esperando la entrada de un botón de joystick
    if (waitingForInput) {
        SDL_GameControllerUpdate();  // Actualiza el estado del joystick
    }

    // Verifica cada botón de cada joystick
    if (waitingForInput) {
        for (int j = 0; j < num_joysticks; j++) {

            for (int button = 0; button < SDL_CONTROLLER_BUTTON_MAX; button++) {
                // Verificar si el botón está presionado en el joystick
                if (SDL_GameControllerGetButton(Vec_SDLJoys[j], static_cast<SDL_GameControllerButton>(button)) == 1) {
                    //Borra la asignacion si ya existia en otro boton.
                    for (int i = 0; i < 12; i++) {
                        if (phisycal_gamepad[player].bMapping[i].button == button && phisycal_gamepad[player].bMapping[i].idJoystick == j) {
                            phisycal_gamepad[player].bMapping[i].button = -1;
                            phisycal_gamepad[player].bMapping[i].idJoystick = -1;
                        }

                   }
                    // Asigna el botón presionado a la acción específica
                    phisycal_gamepad[player].bMapping[Boton_a_Configurar].button = button;
                    phisycal_gamepad[player].bMapping[Boton_a_Configurar].idJoystick = j;
                    // Sale del modo de espera
                    waitingForInput = false;
                    break;
                }
            }
        }
    }

    ImGui::Dummy(ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 30.0f)); // Ajusta 30.0f según el tamaño del botón
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x)/2);

    if (ImGui::Button("Aceptar")) {
        showConfigControllerWindow = false;
    }
    ImGui::End();
}

// Runahead
bool runahead = false;

// Accion cargar rom
void cagarROM_nes() {
    emu->set_System(nes);
    if (emu->loadRom() > 0) {
        emu->PowerOn();
        runahead = false;
        native_res_height = 240;
        native_res_with = 256;
    }
    
} 

//Ventanita de framerate
void show_FrameRate(int counter) {
    ImGui::Begin("FrameRate", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::Text("FPS: %d", counter);
    ImGui::End();
};



// Ventana inicio Juego online
static char ip_input[32] = "127.0.0.1";           // IP predeterminada
static unsigned short remote_port = 7001;         // Puerto remoto predeterminado
static unsigned short local_port = 7000;          // Puerto local predeterminado
static int player_role = 0;                       // Rol del jugador, 0 = Player 1, 1 = Player 2
static int input_delay = 1;
void ShowOnlineConnectionWindow() {
    // Tamaño inicial de la ventana
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);

    ImGui::Begin("Partida Online");

    // Campo de entrada para la IP
    ImGui::Text("IP del jugador remoto:");
    ImGui::InputText("##IP", ip_input, sizeof(ip_input));

    // Campo de entrada para el puerto remoto
    ImGui::Text("Puerto remoto:");
    ImGui::InputScalar("##PuertoRemoto", ImGuiDataType_U16, &remote_port);

    // Campo de entrada para el puerto local
    ImGui::Text("Puerto local:");
    ImGui::InputScalar("##PuertoLocal", ImGuiDataType_U16, &local_port);

    // Selección de rol de jugador
    ImGui::Text("Rol del jugador:");
    ImGui::RadioButton("Jugador 1", &player_role, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Jugador 2", &player_role, 2);

    ImGui::Text("Retraso de Input:");
    ImGui::RadioButton("0", &input_delay, 0); ImGui::SameLine();
    ImGui::RadioButton("1", &input_delay, 1); ImGui::SameLine();
    ImGui::RadioButton("2", &input_delay, 2);

    // Botón para iniciar la conexión
    if (ImGui::Button("Conectar")) {
        runahead = false;
        Rollback::iniciar_conexion(ip_input, remote_port, local_port, input_delay, player_role);
        showOnlineConnection = false;
    }
    if (ImGui::Button("Cancelar")) {
        showOnlineConnection = false;
    }
    ImGui::End();
}


// Ventana de Stats en el modo online.
 int previous_ping;
 int jitter;
 int sample_count;

void jitter_calculator(int current_ping, int &previous_ping, int &jitter, int & sample_count) {
    if (sample_count > 0) {  
        int ping_diff = abs(current_ping - previous_ping);
        jitter = ((jitter * sample_count) + ping_diff) / (sample_count + 1);
        sample_count++;
    }
    else {
        sample_count = 1; 
    }
    previous_ping = current_ping;
}

void ShowNetworkStatsWindow() {
    GGPONetworkStats stats = {0};
    Rollback::get_stats( stats );
    
   // jitter_calculator(stats.network.ping,previous_ping,jitter,sample_count);

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 250, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(200, 150), ImGuiCond_FirstUseEver);

    ImGui::Begin("Estado de la Conexión");

    ImGui::Text("Red:");
    ImGui::Text("Ping: %d ms", stats.network.ping);
    // ImGui::Text("Jitter: %d ms", jitter);
    ImGui::Text("Envio kbps: %d", stats.network.kbps_sent);
    ImGui::Text("Cola envio: %d", stats.network.send_queue_len);
    ImGui::Text("Cola recepcion: %d", stats.network.recv_queue_len);

    ImGui::Separator();

    ImGui::Text("Sincronización:");
    ImGui::Text("Frames locales atrasados: %d", stats.timesync.local_frames_behind);
    ImGui::Text("Frames remotos atrasados: %d", stats.timesync.remote_frames_behind);

    ImGui::End();
}



bool input_delay_mode_enabled = false;  // Estado del modo (ON/OFF)
bool show_input_delay_window = false;   // Controla si se muestra la ventana
int lag_delay_simulado = 0;             // Valor actual del input delay
int temp_input_delay = 0;               // Valor temporal mientras se edita
struct input_simulator {
    Vcontroller inputs[2];
    int delay;
};
std::queue <input_simulator> colaInputs;
uint32_t input_start = 0;

//simulador de input delay netcode
void LagSimulator() {
    if (show_input_delay_window) {
        // Centrar la ventana en pantalla
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2),
            ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        // Configurar ventana
        ImGui::Begin("Configurar Input Delay Netcode Simulator", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

        // Campo de entrada para modificar el valor de input delay
        ImGui::InputInt("Valor de lag que quiere simular (ms)", &temp_input_delay, 1, 5);
        ImGui::Spacing();


        if (input_delay_mode_enabled) {
            // Botón "Desactivar"
            if (ImGui::Button("Desactivar", ImVec2(120, 30))) {
                input_delay_mode_enabled = false;  // Desactivar el modo
                show_input_delay_window = false;  // Cerrar la ventana
                while (!colaInputs.empty()) {
                    colaInputs.pop();
                }
            }
        }

        else {
            // Botón "Activar"
            if (ImGui::Button("Activar", ImVec2(120, 30))) {
                lag_delay_simulado = temp_input_delay;
                input_delay_mode_enabled = true;  // Desactivar el modo
                show_input_delay_window = false;  // Cerrar la ventana
            }
        }
        ImGui::SameLine();

        // Botón "Cancelar"
        if (ImGui::Button("Cancelar", ImVec2(120, 30))) {
            show_input_delay_window = false; // Cerrar la ventana

        }

        ImGui::End();
    }
}




// Mostrar juego
void RenderGameInImGui(SDL_Renderer* renderer) {

    static int lastWindowWidth = 0;
    static int lastWindowHeight = 0;
    static float lastMenuHeight = 0.0f;

    // Obtener el tamaño de la ventana principal SDL solo si ha cambiado
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    if (windowWidth != lastWindowWidth || windowHeight != lastWindowHeight) {
        lastWindowWidth = windowWidth;
        lastWindowHeight = windowHeight;
    }

    // Calcular la altura ocupada por el menú principal solo si ha cambiado
    float menuHeight = ImGui::GetFrameHeight();
    if (menuHeight != lastMenuHeight) {
        lastMenuHeight = menuHeight;
    }

    // Calcular el tamaño y posición disponibles para el juego
    ImVec2 availableSize(windowWidth, windowHeight - lastMenuHeight);
    ImVec2 gamePosition(0, lastMenuHeight);

    // Calcular la relación de aspecto de la textura del juego
    float aspectRatio = static_cast<float>(WIDTH) / HEIGHT;

    // Ajustar la textura para ocupar el máximo espacio en el área disponible
    ImVec2 textureSize;
    if (availableSize.x / availableSize.y > aspectRatio) {
        // La altura es el factor limitante
        textureSize.y = availableSize.y;
        textureSize.x = availableSize.y * aspectRatio;
    }
    else {
        // El ancho es el factor limitante
        textureSize.x = availableSize.x;
        textureSize.y = availableSize.x / aspectRatio;
    }

    // Crear una ventana ImGui para el juego con el área calculada
    ImGui::SetNextWindowPos(gamePosition);
    ImGui::SetNextWindowSize(availableSize);

    // Renderizar la textura del juego dentro de la ventana ImGui centrada
    ImGui::Begin("Game Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus 
        | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNav);
    ImGui::SetCursorPos(ImVec2((availableSize.x - textureSize.x) / 2, (availableSize.y - textureSize.y) / 2));
    ImGui::Image(reinterpret_cast<ImTextureID>(gameTexture), textureSize);
    ImGui::End();
}


// Menu principal
void mainMenu() {

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Abrir")) {
            if (ImGui::MenuItem("Juego NES")) { 
                cagarROM_nes(); }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Sistema")) {
            if (ImGui::MenuItem("Pausar")) { emu->pauseEmulator(); }
            if (ImGui::MenuItem("Reanudar")) { emu->reanudeEmulator(); }
            if (ImGui::MenuItem("Reset")) { emu->reset(); }
            if (ImGui::MenuItem("Cerrar Juego")) { emu->PowerOff(); }
            if (ImGui::MenuItem("Cerrar Fingmulator")) { done = true; }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Partida")) {
            if (ImGui::MenuItem("Guardado Rapido")) { emu->QuickSave(); }
            if (ImGui::MenuItem("Cargado Rapido")) { 
                emu->LoadState();
                emu->cleanAheadState();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Opciones")) {
            if (ImGui::MenuItem("Pantalla Completa")) {

                SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP); // Pantalla completa }
            }
            if (ImGui::MenuItem("Modo Ventana")) {

                SDL_SetWindowFullscreen(window, 0);  // Modo Ventana }
            }
            if (ImGui::MenuItem("Mostrar Framerate", nullptr, &showFrameRateWindow)) {

            } 
            if (ImGui::BeginMenu("Filtro Grafico")) {
                if (ImGui::MenuItem("Lineal")) {
                    SDL_SetTextureScaleMode(gameTexture, SDL_ScaleModeLinear);
                }
                if (ImGui::MenuItem("Sin filtro")) {
                    SDL_SetTextureScaleMode(gameTexture, SDL_ScaleModeNearest);
                }
                ImGui::EndMenu();
            }
            if (emu->emuState == RunningOffline) {
                if (ImGui::MenuItem("Runahead", nullptr, &runahead)) {
                    if (!runahead) {
                        emu->cleanAheadState();
                    }
                }
            }
            else
            {
                ImGui::MenuItem("Runahead", nullptr, false, false);
            }

            ImGui::EndMenu();


        }

        if (ImGui::BeginMenu("Controles")) {
            // Submenú para "Configurar Player 1"
            if (ImGui::BeginMenu("Configurar Player 1")) {
                if (ImGui::MenuItem("Keyboard Player 1")) { 
                    player_config = 0;
                    showConfigKeyboardWindow = true;
                
                }
                if (ImGui::MenuItem("Joystick Player 1")) {
                    player_config = 0;
                    showConfigControllerWindow = true;
                    
                }
                ImGui::EndMenu();
            }

            // Submenú para "Configurar Player 2"
            if (ImGui::BeginMenu("Configurar Player 2")) {
                if (ImGui::MenuItem("Keyboard Player 2")) {
                    player_config = 1;
                    showConfigKeyboardWindow = true;
                }
                if (ImGui::MenuItem("Joystick Player 2")) {
                    player_config = 1;
                    showConfigControllerWindow = true;
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Online Game")) {
            if (emu->emuState == RunningOffline) {
                if (ImGui::MenuItem("Iniciar Partida Online")) {
                    showOnlineConnection = true;
                }
            }
            else
            {
                ImGui::MenuItem("Iniciar Partida Online", nullptr, false, false);
            }
            if (Rollback::ggpo != nullptr) {
                if (ImGui::MenuItem("Mostrar Online Stats", nullptr, &showOnlineStats)) {
                   
                }
            }
            else {
                // Si `ggpo` es nullptr, muestra el menú sin tick
                ImGui::MenuItem("Mostrar Online Stats", nullptr, false, false);
            }
            if (Rollback::ggpo != nullptr) {
                if (ImGui::MenuItem("Test Mode", nullptr, &testMode)) {

                }
            }
            else {
                // Si `ggpo` es nullptr, muestra el menú sin tick
                ImGui::MenuItem("Test Mode", nullptr, false, false);
            }

            if (ImGui::MenuItem("Activar Input Delay netcode simulator")) {
                show_input_delay_window = true;  // Mostrar la ventana de configuración
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}








// ######################################################################## Main code ####################################################################################
int main(int, char**)
{
    WSADATA wsaData; // Inicializa Winsock (usa la versión 2.2)
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("Error en WSAStartup: %d\n", result);
        return 1;
    }

    //Inicializo variables 
    memset(phisycal_gamepad[0].bMapping, -1, sizeof(phisycal_gamepad[0].bMapping));
    memset(phisycal_gamepad[0].kMapping, -1, sizeof(phisycal_gamepad[0].kMapping));
    memset(phisycal_gamepad[1].bMapping, -1, sizeof(phisycal_gamepad[1].bMapping));
    memset(phisycal_gamepad[1].kMapping, -1, sizeof(phisycal_gamepad[1].kMapping));

    uint32_t frameStart, frameTime;
    int FPS = 60;
    int DELAY = 1000/ FPS;
    uint32_t frame_counter = 0;
    uint32_t frame_rate = 0;
    uint32_t Framrate_start = SDL_GetTicks();
    uint32_t* buffer;
    uint32_t* frame =nullptr;

    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }
    
    // Abro los joysticks conectados
    SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
    num_joysticks = SDL_NumJoysticks();
    Vec_SDLJoys.resize(num_joysticks);
    for (int i = 0; i < num_joysticks; i++) {
        if (SDL_IsGameController(i)) {
            Vec_SDLJoys[i] = SDL_GameControllerOpen(i);
        }
    }
    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("FingEmulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return -1;
    }
    gameTexture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
        WIDTH, HEIGHT);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Abrir el joystick si está disponible
    if (SDL_NumJoysticks() > 0) {
        joystick = SDL_JoystickOpen(0); // Abrir el primer joystick conectado
    }
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);


    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //############################################################ Main loop##############################################################################
   

    while (!done)
    {
        frameStart = SDL_GetTicks();

        SDL_Event event;
        

        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
            if (event.type == SDL_KEYDOWN) { // keyboard inputs
                int key = static_cast<int>(event.key.keysym.scancode);
                if (phisycal_gamepad[0].kDic.find(key) != phisycal_gamepad[0].kDic.end()) {
                    update_key(phisycal_gamepad[0].kDic[key], 0, 1);
                }
                if (phisycal_gamepad[1].kDic.find(key) != phisycal_gamepad[1].kDic.end()) {
                    update_key(phisycal_gamepad[1].kDic[key], 1, 1);
                }
            }
            if (event.type == SDL_KEYUP) { // keyboard inputs
                int key = static_cast<int>(event.key.keysym.scancode);
                if (phisycal_gamepad[0].kDic.find(key) != phisycal_gamepad[0].kDic.end()) {
                    update_key(phisycal_gamepad[0].kDic[key], 0, 0);
                }
                if (phisycal_gamepad[1].kDic.find(key) != phisycal_gamepad[1].kDic.end()) {
                    update_key(phisycal_gamepad[1].kDic[key], 1, 0);
                }
            }

        }


        SDL_GameControllerUpdate();
        int indx;
        int button;
        for (int i = 0; i < 12; i++) {
            indx = phisycal_gamepad[0].bMapping[i].idJoystick;
            button = phisycal_gamepad[0].bMapping[i].button;
            if (indx >= 0 && button >= 0) {
                if (SDL_GameControllerGetButton(Vec_SDLJoys[indx], static_cast<SDL_GameControllerButton>(button)) == 1) {
                    emuControllers[0].reg = emuControllers[0].reg | (0x01 << i);
 
                }
                else 
                    emuControllers[0].reg = emuControllers[0].reg & ~(0x01 << i);
            }

            indx = phisycal_gamepad[1].bMapping[i].idJoystick;
            button = phisycal_gamepad[1].bMapping[i].button;
            if (indx >= 0 && button >= 0) {
                if (SDL_GameControllerGetButton(Vec_SDLJoys[indx], static_cast<SDL_GameControllerButton>(button)) == 1) {
                    emuControllers[1].reg = emuControllers[1].reg | (0x01 << i);

                }
                else
                    emuControllers[1].reg = emuControllers[1].reg & ~(0x01 << i);
            }
        }
        
           /*  (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
            {
                SDL_Delay(10);
                continue;
            }
       */
        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        mainMenu();

        if (showConfigControllerWindow) {
            ShowJoystickConfigWindow(player_config);
        }
        if (showConfigKeyboardWindow) {
            ShowKeyboardConfigWindow(player_config);
        }

        if (showOnlineConnection) {
            ShowOnlineConnectionWindow();
        }
        
        if (showOnlineStats) {
            ShowNetworkStatsWindow();
        }

        LagSimulator();

        if (testMode) {   //Agrego inputs random para testear rollback 

            emuControllers[Rollback::player_local-1].Button.Up = rand() % 2;
            emuControllers[Rollback::player_local-1].Button.Down = rand() % 2;
            
        }
        emuRunning = emu->emuRunning();
        if (emuRunning) {
            if (input_delay_mode_enabled && emu->emuState == RunningOffline) {
                input_simulator in;
                in.inputs[0].reg = emuControllers[0].reg;
                in.inputs[1].reg = emuControllers[1].reg;
                in.delay = SDL_GetTicks();
                colaInputs.push(in);
                if (((SDL_GetTicks() - colaInputs.front().delay) > (lag_delay_simulado + rand()%20))) {
                    emu->updateInputs(colaInputs.front().inputs[0].reg);
                    emu->updateInputs(colaInputs.front().inputs[1].reg);
                    colaInputs.pop();
                }
            }
            else {
                emu->updateInputs(emuControllers[0].reg);
                emu->updateInputs(emuControllers[1].reg);
            }
            if (runahead) {
                frame = emu->RunAhead();
            }
            else {
                frame = emu->RunFrame();
            }
           
            frame_counter++;
        }
        if (emuRunning){
            SDL_UpdateTexture(gameTexture, NULL, frame, WIDTH * sizeof(uint32_t));
            SDL_RenderCopy(renderer, gameTexture, NULL, NULL);
            RenderGameInImGui(renderer);

            if (showFrameRateWindow)
                    show_FrameRate(frame_rate);
            if (SDL_GetTicks() - Framrate_start >= 1000) {
                frame_rate = frame_counter;
                frame_counter = 0;
                Framrate_start = SDL_GetTicks();
            }
        }
        Rollback::idle(max(0, 12-(SDL_GetTicks() - (int)frameStart) ));
        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
       
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < DELAY)
            SDL_Delay((int)(DELAY - frameTime));
    }

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    for (auto joystick : Vec_SDLJoys) {
        SDL_GameControllerClose(joystick);
    }
    Vec_SDLJoys.clear();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
