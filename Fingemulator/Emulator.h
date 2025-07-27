#pragma once
#include "Sistema.h"
#include "NES.h"


enum EMULATOR_STATE
{
	Empty = 0,
	RunningOffline = 2,
	Paused = 1,
	Opening = 3,
	DisplayError = 4,
	RunningOnline = 5
};

enum SYSTEM_NAME
{
	NoSystem = 0,
	nes = 1
	//Se agregan los sistemas implementados
};

struct save_state{
	unsigned char* serialized_state;
	int length;
};

class Emulator
{

private:
	Emulator(); 
	~Emulator(); //Singleton
	bool correrEmulador = false;
	SYSTEM_NAME sys = NoSystem;
	int scale = 0;
	int res_WIDTH = 0; /*256*/
	int res_HEIGHT = 0; /*240*/
	std::shared_ptr<Sistema> consola = nullptr;
	/*std::shared_ptr<SDL_Handler> sdl = nullptr;*/
	uint32_t* frame= nullptr;
	Word inputsP1=0;
	Word inputsP2=0;
	bool latch = false;
	unsigned long Frame_counter=0;
	
public:
	
	static Emulator* getInstance() {
		static Emulator instance;  // Instancia única
		return &instance;
	}

	Emulator(const Emulator&) = delete;
	Emulator& operator=(const Emulator&) = delete;
	Emulator(Emulator&&) = delete;
	Emulator& operator=(Emulator&&) = delete;

	EMULATOR_STATE emuState = Empty;
	void set_System(SYSTEM_NAME  system);
	void cpu_Memory_Write(Word addres, Byte data);
	Byte cpu_Memory_Read(Word addres, bool ReadOnly = false);
	int loadRom();
	void pauseEmulator();
	void reanudeEmulator();
	void PowerOn();
	void PowerOff();
	bool emuRunning();
	void DesactivarSonido();
	void ActivarSonido();
	uint32_t* RunFrame();
	uint32_t* Advance_frame_offline(bool Muted = false);
	void reset();
	void close();
	void updateInputs(Word status);
	void QuickSave();

	void LoadState(unsigned char* save = nullptr);
	//RunAhead
	save_state ahaead_state;
	uint32_t* RunAhead();
	void deleteSavestate(unsigned char* save);
	void cleanAheadState();
	//Rollbacks
	save_state quick_save;
	void advance_frame();
	void rollback_save(unsigned char* &buffer, int* &lenght);
};

