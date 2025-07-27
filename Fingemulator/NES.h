#pragma once
#include <cstdint>
#include "m6502_2A03.h"
#include "PPUv2.h"
#include "Cartucho.h"
#include "Sistema.h"
#include <iostream>
#include <fstream>
#ifdef ENABLE_APU
#include "APU/apu.h"
#include "APU/nes_apu/apu_snapshot.h"
#endif
using Byte = uint8_t;
using Word = uint16_t;
union Controller
{
	struct {
		Byte A : 1;	
		Byte B : 1;	
		Byte Select : 1;	
		Byte Start : 1;	
		Byte Up : 1;	
		Byte Down : 1;	
		Byte Left : 1;	
		Byte Right : 1;	
	} Button;
	Byte reg;
};

union Vcontroller
{
	struct {
		Word A : 1;
		Word B : 1;
		Word X : 1;
		Word Y : 1;
		Word L : 1;
		Word R : 1;
		Word Select : 1;
		Word Start : 1;
		Word Up : 1;
		Word Down : 1;
		Word Left : 1;
		Word Right : 1;
	} Button;
	Word reg;
};

struct NESState
{
	// Estado de la memoria RAM de la CPU
	Byte cpu_ram[2048];

	// Estado de la CPU (los registros)
	CPUState cpu;

	// Estado de la PPU (registro, tablas, buffers, etc.)
	PPUState ppu;
//
//#ifdef ENABLE_APU
//	//Estado de la APU
//	apu_snapshot_t apu;
//#endif

	// Estado mapper
	int mapperSize;

	// Estado de los controles (inputs)
	Controller controles[2];
	Vcontroller new_controller_state[2];
	bool strobe;
	bool playerpar;

	// Estado dma
	bool transfer_dma;
	bool transfiriendo;
	Byte dma_page;
	Byte dma_addr;
	Byte dma_data;

	// Otros atributos 
	// uint32_t framecounter; no es necesario
	uint32_t ClockCounter;


};


class NES : public Sistema
{


public:
	NES();
	~NES();


	m6502_2A03 cpu;
	PPUv2 ppu;
	Byte cpu_ram[2048];
	std::shared_ptr<Cartucho> cartucho = nullptr;

	// CPU read  write
	Byte cpu_Read(Word addres, bool ReadOnly = false) override;
	void cpu_Write(Word addres, Byte data) override;
	
	//load rom
	bool load_rom(const std::string& filename) override;

	// reset prender
	void reset() override;
	void turnON() override;
	
	//load save state
	void LoadState(unsigned char* buffer) override;
	void SaveState(unsigned char* &buffer, int &lenght) override;
	void DeleteState(unsigned char* buffer) override;
	void SavesNESState(NESState &state);

	void LoadNESState(NESState* state);

	
	//operaciones de correr
	void clock() override;
	uint32_t framecounter = 0;
	uint32_t* run_frame(bool soundMuted = false) override;

	//latch
	bool playerpar = 0;
	void UpdateInputs(Word status) override;
private:

	//Contador de ticks
	uint32_t ClockCounter = 0;
	
	//DMA
	bool transfer_dma = false;
	bool transfiriendo = false;
	Byte dma_page = 0x00;
	Byte dma_addr = 0x00;
	Byte dma_data = 0x00;
	
	//Conecta el cartucho a los busses.
	void insertCartridge(const std::shared_ptr<Cartucho>& juego);
	//Controller

	Controller controles[2];
	Vcontroller new_controller_state[2];
	bool strobe;

	bool sound_Disabled = false;
	void DesactivarSonido() override;
	void ActivarSonido() override;
};

