#include "Mapper_2.h"
#define PRG_BLOCK_SIZE  16384 // 16 KB
#define CHR_BLOCK_SIZE   8192 // 8 KB

Mapper_2::Mapper_2(Byte prgBanks, Byte chrBanks) : Mapper(prgBanks, chrBanks)
{
	PGRmap[0] = 0;
	PGRmap[1] = bancosPRG - 1;
}

Mapper_2::~Mapper_2()
{
}

void Mapper_2::reset()
{
	PGRmap[0] = 0;
	PGRmap[1] = bancosPRG - 1;

}

void Mapper_2::saveMapperState(unsigned char*& buffer, int& size)
{
	// Para guardar el estado del mapper, solo se necesita el primer byte PGRmap, el segundo es fijo.
	size = sizeof(PGRmap[0]);
	buffer = new unsigned char[size];
	*buffer = (unsigned char)PGRmap[0];
	
}

void Mapper_2::loadMapperState(unsigned char* buffer, int size)
{
	PGRmap[0] = (Byte)*buffer;
}

bool Mapper_2::cpu_read(Word addres, uint32_t& mapeada, Byte& data)
{

	if (addres >= 0x8000) // Si estoy en rango de lectura de cartucho
	{
		Byte window = (addres <= 0xBFFF ? 0 : 1); //Revisa en cual de los dos windows es la direccion.
		mapeada = PGRmap[window] * PRG_BLOCK_SIZE + (addres & 0x3FFF); // armo la direccion
		return true;
	}

	return false;
}

bool Mapper_2::cpu_write(Word addres, uint32_t& mapeada, Byte data)
{
	if (addres >= 0x8000 && addres <= 0xFFFF)
	{
		PGRmap[0] = data & 0x0F;
	}
	return false;
}

bool Mapper_2::ppu_read(Word address, uint32_t& mapeada)
{

	if (address >= 0x0000 && address <= 0x1FFF)
	{
		mapeada = address;
		return true;
	}
	return false;
}

bool Mapper_2::ppu_write(Word addres, uint32_t& mapeada)
{
	if (addres >= 0x0000 && addres <= 0x1FFF)
	{
		if (bancosCHR == 0) // Si es una RAM
		{
			mapeada = addres;
			return true;
		}
	}
	return false;
}
