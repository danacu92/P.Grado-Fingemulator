#pragma once
#include "Mapper.h"


class Mapper_2 : public Mapper
{
public:
	Mapper_2(Byte prgBanks, Byte chrBanks);
	~Mapper_2();

public:
	bool cpu_read(Word addres, uint32_t& mapeada, Byte& data) override;
	bool cpu_write(Word addres, uint32_t& mapeada, Byte data = 0) override;
	bool ppu_read(Word addres, uint32_t& mapeada) override;
	bool ppu_write(Word addres, uint32_t& mapeada) override;
	void reset() override;
	void saveMapperState(unsigned char*& buffer, int& size) override;
	void loadMapperState(unsigned char* buffer, int size) override;

private:
	Byte PGRmap[2];

};

