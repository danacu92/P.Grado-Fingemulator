#include "Mapper_0.h"

Mapper_0::Mapper_0(uint8_t PRGbanks, uint8_t CHRbanks) : Mapper(PRGbanks, CHRbanks)
{
}


Mapper_0::~Mapper_0()
{
}

void Mapper_0::reset()
{

}

MIRROR Mapper_0::getMirror()
{
	return mirror;
}

bool Mapper_0::cpu_read(Word address, uint32_t& mapeada, Byte& data)
{
	if (address >= 0x8000 && address <= 0xFFFF)
	{
		mapeada = address & (bancosPRG > 1 ? 0x7FFF : 0x3FFF);
		return true;
	}

	return false;
}

bool Mapper_0::cpu_write(Word address, uint32_t& mapeada, Byte data)
{
	if (address >= 0x8000 && address <= 0xFFFF)
	{
		mapeada = address & (bancosPRG > 1 ? 0x7FFF : 0x3FFF);
		return true;
	}

	return false;
}

bool Mapper_0::ppu_read(Word address, uint32_t& mapeada)
{

	if (address >= 0x0000 && address <= 0x1FFF)
	{
		mapeada = address;
		return true;
	}

	return false;
}

bool Mapper_0::ppu_write(Word address, uint32_t& mapeada)
{
	if (address >= 0x0000 && address <= 0x1FFF)
	{
		if (bancosCHR == 0) // Si es una RAM
		{
			mapeada = address;
			return true;
		}
	}

	return false;
}

