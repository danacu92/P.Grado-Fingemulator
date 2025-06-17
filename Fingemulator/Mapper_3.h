#pragma once
#include "Mapper.h"

class Mapper_3 : public Mapper
{
public:
	Mapper_3(Byte prgBanks, Byte chrBanks);
	~Mapper_3();

public:
	bool cpu_read(Word addres, uint32_t& mapeada, Byte& data) override;
	bool cpu_write(Word addres, uint32_t& mapeada, Byte data = 0) override;
	bool ppu_read(Word addres, uint32_t& mapeada) override;
	bool ppu_write(Word addres, uint32_t& mapeada) override;
	void reset() override;
	MIRROR getMirror() override;

private:
	Byte CHRmap;
};

