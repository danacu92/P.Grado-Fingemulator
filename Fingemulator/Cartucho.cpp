#include "Cartucho.h"
#include <string>
#include <fstream>



struct iNES
{
	char NES[4];
	Byte bloquesPRG;
	Byte bloquesCHR;
	Byte mapper1;
	Byte mapper2;
	Byte prg_ram;
	Byte tv1;
	Byte tv2;
	char unused[5];
};

Cartucho::Cartucho(const std::string& File)
{
#define TRAINER_SIZE     512
#define PRG_BLOCK_SIZE  16384 // 16 KB
#define CHR_BLOCK_SIZE   8192 // 8 KB

	iNES header;

	rom_valida = false;

	std::ifstream ifs;
	ifs.open(File, std::ifstream::binary);
	if (ifs.is_open())
	{
		//Leo el header
		ifs.read(reinterpret_cast<char*>(&header), sizeof(iNES));

		if (header.mapper1 & 0x04)
			ifs.seekg(TRAINER_SIZE, std::ios_base::cur);

		// Leo el mapper y el mirror
		mapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
		MIRROR mirrorType = (header.mapper1 & 0x01) ? MIRROR::VERTICAL : MIRROR::HORIZONTAL;

		//Creo los "roms" de programa y graficos
		bancosPRG = header.bloquesPRG;
		PRGmemory.resize(bancosPRG * PRG_BLOCK_SIZE);
		ifs.read((char*)PRGmemory.data(), PRGmemory.size());
		bancosCHR = header.bloquesCHR;
		if (bancosCHR > 0) {
			CHRmemory.resize(bancosCHR * CHR_BLOCK_SIZE);
		}
		else {
			CHRmemory.resize(CHR_BLOCK_SIZE);
		}
		ifs.read((char*)CHRmemory.data(), CHRmemory.size());


		rom_valida = true;

		// Creo el mapper correspondiente
		switch (mapperID)
		{
		case 0: 
			mapper = std::make_shared<Mapper_0>(bancosPRG, bancosCHR);
			break;
		case 2: 
			mapper = std::make_shared<Mapper_2>(bancosPRG, bancosCHR); 
			break;
		case 3: 
			mapper = std::make_shared<Mapper_3>(bancosPRG, bancosCHR); 
			break;
		case 4: 
			mapper = std::make_shared<Mapper_4>(bancosPRG, bancosCHR);
			break;
		}
		ifs.close();
		mapper->setMirror(mirrorType);
	}

}

Cartucho::~Cartucho()
{
}

MIRROR Cartucho::GetMirrorType()
{
	return mapper->getMirror();
}

void Cartucho::Reset()
{
	if (mapper != nullptr)
		mapper->reset();
}

void Cartucho::getMapperState(unsigned char*& buffer, int& size)
{
	if (bancosCHR> 0)
	mapper->saveMapperState(buffer, size);
	else {
		unsigned char* mapperState;
		int sizeMapper;
		mapper->saveMapperState(mapperState, sizeMapper);
		size = CHRmemory.size() + sizeMapper;
		buffer = new unsigned char[size];
		memcpy(buffer, CHRmemory.data(), CHRmemory.size());
		memcpy(buffer + CHRmemory.size(), mapperState, sizeMapper);
		delete[] mapperState;
	}
}

void Cartucho::loadMapperState(unsigned char* buffer, int size)
{
	if (bancosCHR > 0)
	mapper->loadMapperState(buffer, size);
	else {
		memcpy(CHRmemory.data(), buffer , CHRmemory.size());
		mapper->loadMapperState(buffer + CHRmemory.size(), size - CHRmemory.size());
	}
}


bool Cartucho::valid_rom()
{
	return rom_valida;
}

bool Cartucho::cpu_read(Word addr, Byte& data)
{
	uint32_t mapeada = 0;
	if (mapper->cpu_read(addr, mapeada, data)) {
		if (mapeada == 0xFFFFFFFF) { // lectura de ram
			return true;
		}
		else {
			data = PRGmemory[mapeada];
		}
		return true;
	}
	else
		return false;
}

bool Cartucho::cpu_write(Word addr, Byte data)
{
	uint32_t mapeada = 0;
	if (mapper->cpu_write(addr, mapeada, data)) {
		if (mapeada == 0xFFFFFFFF) { // escritura ram
			return true;
		}
		else {
			PRGmemory[mapeada] = data;
		}
		return true;
	}
	else
		return false;
}

bool  Cartucho::ppu_read(Word addr, Byte& data)
{
	uint32_t mapeada = 0;
	if (mapper->ppu_read(addr, mapeada)) {
		data = CHRmemory[mapeada];
		return true;
	}
	else
		return false;
}

bool  Cartucho::ppu_write(Word addr, Byte data)
{
	uint32_t mapeada = 0;
	if (mapper->ppu_write(addr, mapeada)) {
		CHRmemory[mapeada] = data;
		return true;
	}
	else
		return false;
}

bool Cartucho::IRQ()
{
	return mapper->get_IRQ();
}

void Cartucho::IRQ_off()
{
	mapper->IRQ_off();
}

void Cartucho::scanlineSignal()
{
	mapper->ScanlineSignal();
}
