#pragma once
#include <cstdint>
#include <vector>
#include <iostream>
#include "Mapper.h"
#include "Mapper_0.h"
#include "Mapper_2.h"
#include "Mapper_3.h"
#include "Mapper_4.h"

using namespace std;
using Byte = uint8_t;
using Word = uint16_t;


class Cartucho
{
public:

	Cartucho(const std::string& sFileName);

	~Cartucho();

private:
	bool rom_valida = false;
	Byte mapperID = 0;
	Byte bancosPRG = 0;
	Byte bancosCHR = 0;
	std::vector<Byte> PRGmemory;
	std::vector<Byte> CHRmemory;
	
	std::shared_ptr<Mapper> mapper;

public:
	
	bool valid_rom();
	bool cpu_read(Word addr, Byte& data);
	bool cpu_write(Word addr, Byte data);

	bool ppu_read(Word addr, Byte& data);
	bool ppu_write(Word addr, Byte data);
	bool IRQ();
	void IRQ_off();
	void scanlineSignal();
	MIRROR GetMirrorType();
	void Reset();
};

