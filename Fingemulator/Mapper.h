#pragma once
#include <cstdint>
using Byte = uint8_t;
using Word = uint16_t;

enum class MIRROR
{
	ONESCREEN_LOW,
	ONESCREEN_UP,
	VERTICAL,
	HORIZONTAL,
	FOURSCREEN
};

class Mapper
{
public:
	Mapper(uint8_t PRGbanks, uint8_t CGRbanks);
	~Mapper();


public:

	virtual bool cpu_read(Word addres, uint32_t& mapeada, Byte& data) = 0;
	virtual bool cpu_write(Word addres, uint32_t& mapeada, Byte data = 0) = 0;

	virtual bool ppu_read(Word addres, uint32_t& mapeada) = 0;
	virtual bool ppu_write(Word addres, uint32_t& mapeada) = 0;

	virtual void reset() = 0;
	virtual void setMirror(MIRROR mirr) { mirror = mirr; };
	virtual MIRROR getMirror() { return mirror; };
	virtual bool get_IRQ() { return IRQ; };
	virtual void ScanlineSignal() { };
	virtual void IRQ_off() { IRQ = false; };

protected:
	Byte bancosPRG = 0;
	Byte bancosCHR = 0;
	Byte* prg;
	Byte* chr;
	Byte* pgrRAM;
	MIRROR mirror;
	bool IRQ = false;
	uint32_t pgrSize;
	uint32_t chrSize;
	uint32_t pgrRAMSize;

	bool CHR_RAM = false;  

};