#include "Mapper_3.h"
#define CHR_BLOCK_SIZE   8192 // 8 KB
Mapper_3::Mapper_3(Byte prgBanks, Byte chrBanks) : Mapper(prgBanks, chrBanks)
{
	CHRmap = 0;
}

Mapper_3::~Mapper_3()
{
}

bool Mapper_3::cpu_read(Word addres, uint32_t& mapeada, Byte& data)
{
	if (addres >= 0x8000 && addres <= 0xFFFF)
	{
		mapeada = addres & (bancosPRG > 1 ? 0x7FFF : 0x3FFF);
		return true;
	}

	return false;
}

bool Mapper_3::cpu_write(Word addres, uint32_t& mapeada, Byte data)
{
	if (addres >= 0x8000)
	{
		CHRmap = data & 0x03;
		mapeada = addres;
	}

	return false;
}

bool Mapper_3::ppu_read(Word address, uint32_t& mapeada)
{

	if (address >= 0x0000 && address <= 0x1FFF)
	{
		mapeada = (CHRmap * CHR_BLOCK_SIZE) + address; // armo la direccion del chr rom
		return true;
	}
	return false;
}

bool Mapper_3::ppu_write(Word addres, uint32_t& mapeada)
{
	return false;
}

void Mapper_3::reset()
{
	CHRmap = 0;
}

MIRROR Mapper_3::getMirror()
{
	return mirror;
}

void Mapper_3::saveMapperState(unsigned char*& buffer, int& size)
{
	buffer = new unsigned char;
	*buffer = (unsigned char)CHRmap;
	size = sizeof(CHRmap);
}

void Mapper_3::loadMapperState(unsigned char* buffer, int size)
{
	CHRmap = (Byte)*buffer;
}
