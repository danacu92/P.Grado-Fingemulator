#include <cstdint>
#include <string>
#pragma once
using Byte = uint8_t;
using Word = uint16_t;

class Sistema
{
public:
	virtual void cpu_Write(Word addres, Byte data) = 0;
	virtual Byte cpu_Read(Word addres, bool ReadOnly = false) = 0;
	// Resets the system
	virtual void turnON() = 0;
	virtual void reset() = 0;
	virtual void clock() = 0;
	virtual uint32_t* run_frame(bool soundMuted = false) = 0;
	virtual void UpdateInputs(Word newInputs) = 0;
	virtual bool load_rom(const std::string& filename) = 0;
	virtual void SaveState(unsigned char* &buffer, int &lenght) = 0;
	virtual void LoadState(unsigned char* buffer) = 0;
	virtual void DeleteState(unsigned char* buffer) = 0;
	virtual void DesactivarSonido() = 0;
	virtual void ActivarSonido() = 0;
};