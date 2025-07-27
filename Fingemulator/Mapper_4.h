#pragma once
#include "Mapper.h"
#include <iostream>


#define BLOCK_8KB   8192 // 8 KB
#define BLOCK_1KB   1024 // 1 KB



struct mapper4_state {
	Byte c_PRGRAM[BLOCK_8KB];
	Byte select_register;
	Byte map_registers[8];
	Byte CHRmap[8];  
	Byte PRGmap[4]; 
	Byte IRQ_latch;
	Byte IRQ_counter;
	bool IRQ_Enabled;
};
class Mapper_4 : public Mapper
{
public:
	Mapper_4(Byte prgBanks, Byte chrBanks);
	~Mapper_4();

public:
	bool cpu_read(Word addres, uint32_t& mapeada, Byte& data) override;
	bool cpu_write(Word addres, uint32_t& mapeada, Byte data = 0) override;
	bool ppu_read(Word addres, uint32_t& mapeada) override;
	bool ppu_write(Word addres, uint32_t& mapeada) override;
	void reset() override;
	void set_banks();
	void ScanlineSignal() override;
	void saveMapperState(unsigned char*& buffer, int& size) override;
	void loadMapperState(unsigned char* buffer, int size)override;
private:

	Byte c_PRGRAM[BLOCK_8KB] = {}; // RAM PRG del cartucho
	union BankSelect
	{
		struct
		{
			// estos union van de menos significativo a mas significativo
			Byte Bank : 3;
			Byte unused : 3;
			Byte bank_mode : 1;
			Byte A12_inversion : 1;
		};
		Byte reg = 0x00;
	};
	BankSelect select_register; // donde se selecciona el registro a modificar
	Byte map_registers[8] = {}; // registros del mapper
	Byte CHRmap[8] = {};  // Guardo el mapeo de los CHR rom, tamaño 1KB
	Byte PRGmap[4] = {}; // guardo el mapeo de los PRG rom, tamaño 8Kb
	Byte IRQ_latch = 0;
	Byte IRQ_counter = 0;
	bool IRQ_Enabled = false;
};

