#include "Mapper_4.h"

Mapper_4::Mapper_4(Byte prgBanks, Byte chrBanks): Mapper(prgBanks, chrBanks)
{
	PRGmap[0] = 0;
	PRGmap[1] = 1;
	PRGmap[2] = (bancosPRG * 2) - 2;
	PRGmap[3] = (bancosPRG * 2) - 1;
}

Mapper_4::~Mapper_4()
{

}

bool Mapper_4::cpu_read(Word addres, uint32_t& mapeada, Byte& data)
{
	if (addres >= 0x6000 && addres <= 0x7FFF) // RAM
	{
		mapeada = 0xFFFFFFFF;
		data = c_PRGRAM[addres & 0x1FFF];
		return true;
	}

	if (addres >= 0x8000 && addres <= 0x9FFF)
	{
		mapeada = PRGmap[0] * BLOCK_8KB + (addres & 0x1FFF);
		return true;
	}

	if (addres >= 0xA000 && addres <= 0xBFFF)
	{
		mapeada = PRGmap[1] * BLOCK_8KB + (addres & 0x1FFF);
		return true;
	}

	if (addres >= 0xC000 && addres <= 0xDFFF)
	{
		mapeada = PRGmap[2] * BLOCK_8KB + (addres & 0x1FFF);
		return true;
	}

	if (addres >= 0xE000 && addres <= 0xFFFF)
	{
		mapeada = PRGmap[3] * BLOCK_8KB + (addres & 0x1FFF);
		return true;
	}
	return false;
}

void Mapper_4::set_banks() {

	if (select_register.A12_inversion) {
		CHRmap[0] = map_registers[2];
		CHRmap[1] = map_registers[3];
		CHRmap[2] = map_registers[4];
		CHRmap[3] = map_registers[5];
		CHRmap[4] = map_registers[0] & 0xFE; // Bancos de 2KB Ignora el ultimo bit
		CHRmap[5] = CHRmap[4] + 1;
		CHRmap[6] = map_registers[1] & 0xFE; // Bancos de 2KB Ignora el ultimo bit
		CHRmap[7] = CHRmap[6] + 1;
	}
	else {
		CHRmap[0] = map_registers[0] & 0xFE; // Bancos de 2KB Ignora el ultimo bit
		CHRmap[1] = CHRmap[0] + 1;
		CHRmap[2] = map_registers[1] & 0xFE; // Bancos de 2KB Ignora el ultimo bit
		CHRmap[3] = CHRmap[2] + 1;
		CHRmap[4] = map_registers[2];
		CHRmap[5] = map_registers[3];
		CHRmap[6] = map_registers[4];
		CHRmap[7] = map_registers[5];
	}
	if (select_register.bank_mode) {

		PRGmap[0] = (bancosPRG * 2) - 2; //Bancos PRG son de 16 kb, multiplico por 2 para calcular bancos de 8KB y tomo el penultimo.
		PRGmap[1] = (map_registers[7] & 0x3F);
		PRGmap[2] = (map_registers[6] & 0x3F);
		PRGmap[3] = (bancosPRG * 2) - 1; //Bancos PRG son de 16 kb, multiplico por 2 para calcular bancos de 8KB y tomo el ultimo.
	}
	else {
		PRGmap[0] = (map_registers[6] & 0x3F);
		PRGmap[1] = (map_registers[7] & 0x3F);
		PRGmap[2] = (bancosPRG * 2) - 2; //Bancos PRG son de 16 kb, multiplico por 2 para calcular bancos de 8KB y tomo el penultimo.
		PRGmap[3] = (bancosPRG * 2) - 1; //Bancos PRG son de 16 kb, multiplico por 2 para calcular bancos de 8KB y tomo el ultimo.
	}
}

bool Mapper_4::cpu_write(Word addres, uint32_t& mapeada, Byte data)
{
	if (addres >= 0x6000 && addres <= 0x7FFF) {    // RAM
		mapeada = 0xFFFFFFFF;
		c_PRGRAM[addres & 0x1FFF] = data;
		return true;
	}

	if (addres >= 0x8000 && addres <= 0x9FFF){ // select bank

		if ((addres & 0x0001)) {
			if (select_register.Bank >= 6) {
				map_registers[select_register.Bank] = data % (bancosPRG * 2);
			}
			else {
				map_registers[select_register.Bank] = data % (bancosCHR * 8);
				data = data;
			}
		}
		else {
			select_register.reg = data;
		}
		set_banks();
		return false;
	}

	else if (addres >= 0xA000 && addres <= 0xBFFF) { // Mirroring

		if (!(addres & 0x0001))
		{
			
			if (data & 0x01)
				mirror = MIRROR::HORIZONTAL;
			else
				mirror = MIRROR::VERTICAL;
		}
		return false;
	}

	else if (addres >= 0xC000 && addres <= 0xDFFF)
	{
		if ((addres & 0x0001))
		{
			IRQ_counter = 0x0000;
		}
		else
		{
			IRQ_latch = data;
		}
		return false;
	}

	if (addres >= 0xE000 && addres <= 0xFFFF)
	{
		if ((addres & 0x0001))
		{
			IRQ_Enabled = true;
		}
		else
		{
			IRQ_Enabled = false;
			IRQ = false;
		}
		return false;
	}

	return false;
}

bool Mapper_4::ppu_read(Word addres, uint32_t& mapeada)
{

	if (addres >= 0x0000 && addres <= 0x03FF)
	{
		mapeada = (CHRmap[0] *BLOCK_1KB)  + (addres & 0x03FF);
		return true;
	}

	if (addres >= 0x0400 && addres <= 0x07FF)
	{
		mapeada = (CHRmap[1] * BLOCK_1KB) + (addres & 0x03FF);
		return true;
	}

	if (addres >= 0x0800 && addres <= 0x0BFF)
	{
		mapeada = (CHRmap[2] * BLOCK_1KB)  + (addres & 0x03FF);
		return true;
	}

	if (addres >= 0x0C00 && addres <= 0x0FFF)
	{
		mapeada = (CHRmap[3] * BLOCK_1KB)  + (addres & 0x03FF);
		return true;
	}

	if (addres >= 0x1000 && addres <= 0x13FF)
	{
		mapeada = (CHRmap[4] * BLOCK_1KB)  + (addres & 0x03FF);
		return true;
	}

	if (addres >= 0x1400 && addres <= 0x17FF)
	{
		mapeada = (CHRmap[5] * BLOCK_1KB)  + (addres & 0x03FF);
		return true;
	}

	if (addres >= 0x1800 && addres <= 0x1BFF)
	{
		mapeada = (CHRmap[6] * BLOCK_1KB)  + (addres & 0x03FF);
		return true;
	}

	if (addres >= 0x1C00 && addres <= 0x1FFF)
	{
		mapeada = (CHRmap[7] * BLOCK_1KB)  + (addres & 0x03FF);
		return true;
	}

	return false;
}

bool Mapper_4::ppu_write(Word addres, uint32_t& mapeada)
{
	return false;
}

void Mapper_4::reset()
{
	select_register.reg = 0x00;
	IRQ = false;
	memset(map_registers, 0x00, sizeof(map_registers));
	IRQ_latch = 0;
	IRQ_counter = 0;
	IRQ_Enabled = false;
	PRGmap[0] = 0;
	PRGmap[1] = 1;
	PRGmap[2] = (bancosPRG * 2) - 2;
	PRGmap[3] = (bancosPRG * 2) - 1;
}

void Mapper_4::ScanlineSignal()
{
	if (IRQ_counter == 0)
		IRQ_counter = IRQ_latch;
	else
		IRQ_counter--;
	
	if (IRQ_counter == 0 && IRQ_Enabled)
		IRQ = true;
}

void Mapper_4::saveMapperState(unsigned char*& buffer, int& size)
{
	mapper4_state* state = new mapper4_state;
	size = sizeof(mapper4_state); 
	memcpy(state->c_PRGRAM, c_PRGRAM, sizeof(c_PRGRAM));
	state->select_register = select_register.reg;
	memcpy(state->map_registers, map_registers, sizeof(map_registers));
	memcpy(state->CHRmap, CHRmap, sizeof(CHRmap));
	memcpy(state->PRGmap, PRGmap, sizeof(PRGmap));
	state->IRQ_latch = IRQ_latch;
	state->IRQ_counter = IRQ_counter;
	state->IRQ_Enabled = IRQ_Enabled;
	buffer = new unsigned char[size];
	memcpy(buffer, state, size);
	delete state;
}

void Mapper_4::loadMapperState(unsigned char* buffer, int size)
{
	mapper4_state* state = new mapper4_state;

	memcpy(state, buffer, sizeof(mapper4_state));
	memcpy(c_PRGRAM, state->c_PRGRAM, sizeof(c_PRGRAM));
	select_register.reg = state->select_register;
	memcpy(map_registers, state->map_registers, sizeof(map_registers));
	memcpy(CHRmap, state->CHRmap, sizeof(CHRmap));
	memcpy(PRGmap, state->PRGmap, sizeof(PRGmap));
	IRQ_latch = state->IRQ_latch;
	IRQ_counter = state->IRQ_counter;
	IRQ_Enabled = state->IRQ_Enabled;

	delete state;
}
