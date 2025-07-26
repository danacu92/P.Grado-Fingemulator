#include "NES.h"
#include <iostream>
#include "SDL.h"
#include "APU/nes_apu/apu_snapshot.h"
using Byte = uint8_t;
using Word = uint16_t;

NES::NES()
{
	this->cpu.conectarBus(this);
	memset(cpu_ram, 0x00, sizeof(cpu_ram));
	memset(controles, 0x00, sizeof(controles));
}

NES::~NES()
{
}

// Si bien el rango de direccionamiento de la NES es de 8Kb, el NES tiene 2Kb de ram efectiva y el resto son "espejos" de estos 2Kb 
// es decir que el contenido de la direccion 0X0000 seria el mismo que en 0X0800 y en 0x1000 y asi sucesivamente, por eso se hace modulo a la direccion utilizando la operacion &. 

Byte NES::cpu_Read(Word addres, bool ReadOnly)
{
	Byte data = 0x00;

	// RAM
	if (addres >= 0x0000 and addres <= 0x1FFF)
	{
		data = cpu_ram[addres & 0x07FF];
	}
	else if (addres >= 0x2000 && addres <= 0x3FFF)
	{
		// PPU 
		data = ppu.cpuBusRead(addres & 0x0007, ReadOnly);
	}
	else if (addres == 0x4015)
	{
#ifdef ENABLE_APU
		if (!sound_Disabled)
		data = APU::read8(cpu.elapsed);
#endif
	}
	else if (addres >= 0x4016 && addres <= 0x4017)
	{
		if (strobe) {
			data = 0x40 | ((Byte)controles[addres & 0x0001].Button.A);
		}
		else {
			data = 0x40 | controles[addres & 0x0001].reg & 0x01;
			controles[addres & 0x0001].reg >>= 1;
		}
	}
	else if (cartucho->cpu_read(addres, data))
	{
		//rango cartucho.
	}
	return data;
}

void NES::cpu_Write(Word addres, Byte data)
{
	if (cartucho->cpu_write(addres, data))
	{

	}
	// RAM
	else if (addres >= 0x0000 and addres <= 0x1FFF)
	{
		cpu_ram[addres & 0x07FF] = data;
	}

	else if (addres >= 0x2000 && addres <= 0x3FFF)
	{
		// PPU registers
		ppu.cpuBusWrite(addres & 0x0007, data);
	}
	// APU
	else if ((addres >= 0x4000 && addres <= 0x4013) || addres == 0x4015)
	{
#ifdef ENABLE_APU
		if (!sound_Disabled)
			APU::write8(cpu.elapsed, addres, data);
#endif
	}

	else if (addres == 0x4014)
	{
		// DMA
		dma_page = data;
		dma_addr = 0x00;
		transfer_dma = true;
	}
	else if (addres == 0x4016) {
		if (data & 0x01) {
			strobe = true;
			//Player 1
			controles[0].Button.A = new_controller_state[0].Button.A;
			controles[0].Button.B = new_controller_state[0].Button.B;
			controles[0].Button.Select = new_controller_state[0].Button.Select;
			controles[0].Button.Start = new_controller_state[0].Button.Start;
			controles[0].Button.Up = new_controller_state[0].Button.Up;
			controles[0].Button.Down = new_controller_state[0].Button.Down;
			controles[0].Button.Left = new_controller_state[0].Button.Left;
			controles[0].Button.Right = new_controller_state[0].Button.Right;

			//Player 2
			controles[1].Button.A = new_controller_state[1].Button.A;
			controles[1].Button.B = new_controller_state[1].Button.B;
			controles[1].Button.Select = new_controller_state[1].Button.Select;
			controles[1].Button.Start = new_controller_state[1].Button.Start;
			controles[1].Button.Up = new_controller_state[1].Button.Up;
			controles[1].Button.Down = new_controller_state[1].Button.Down;
			controles[1].Button.Left = new_controller_state[1].Button.Left;
			controles[1].Button.Right = new_controller_state[1].Button.Right;
		}
		else {
			strobe = false;
		};
	}
}



bool NES::load_rom(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Error: No se pudo abrir el archivo " << filename << std::endl;
		return false;
	}

	// Leer el encabezado iNES
	Byte header[4];
	file.read(reinterpret_cast<char*>(header), 4);

	// Validar el encabezado
	if (header[0] != 'N' || header[1] != 'E' || header[2] != 'S' || header[3] != 0x1A) {
		std::cerr << "Error: Archivo no es una ROM de NES válida" << std::endl;
		return false;
	}
	file.close();
	std::shared_ptr<Cartucho> juego = std::make_shared<Cartucho>(filename);
	if (juego->valid_rom()) {
		insertCartridge(juego);
		return true;
	}
	return false;
}

void NES::insertCartridge(const std::shared_ptr<Cartucho>& juego)
{

	cartucho = juego;
	ppu.ConectarCartucho(juego);
	/*cpu.init();*/
	//cpu.reset();
	//cpu.test();
}

void NES::DesactivarSonido()
{
	sound_Disabled = true;
}

void NES::ActivarSonido()
{
	sound_Disabled = false;
}
void NES::UpdateInputs(Word status)
{
	if (playerpar) {
		new_controller_state[1].reg = status;
	}
	else {
		new_controller_state[0].reg = status;
	}
	playerpar = !playerpar;
}


// Resets the system

void NES::reset()
{
	cpu.reset();
	ClockCounter = 0;
	cartucho->Reset();
	ppu.reset();
	memset(cpu_ram, 0x00, sizeof(cpu_ram));
	ClockCounter = 0;
	transfer_dma = false;
	transfiriendo = false;
	dma_page = 0x00;
	dma_addr = 0x00;
	dma_data = 0x00;
	memset(controles, 0x00, sizeof(controles));
	framecounter = 0;
#ifdef ENABLE_APU
	APU::Reset();
#endif
}

void NES::turnON()
{
	cpu.init();
#ifdef ENABLE_APU
	APU::Init();
#endif
	ClockCounter = 0;
	cartucho->Reset();
	ppu.reset();
	memset(cpu_ram, 0x00, sizeof(cpu_ram));
	ClockCounter = 0;
	transfer_dma = false;
	transfiriendo = false;
	dma_page = 0x00;
	dma_addr = 0x00;
	dma_data = 0x00;
	memset(controles, 0x00, sizeof(controles));
	memset(new_controller_state, 0x00, sizeof(new_controller_state));
}


void NES::LoadState(unsigned char* buffer) {
	NESState* state = new NESState;
	int nesStateSize = sizeof(NESState);
	memcpy(state, buffer, nesStateSize);
	int mapperSize = state->mapperSize;
	LoadNESState(state);
	if (mapperSize > 0) {
		unsigned char* mapperState = new unsigned char[mapperSize];
		memcpy(mapperState, buffer + nesStateSize, mapperSize);
		cartucho->loadMapperState(mapperState, mapperSize);
		delete[] mapperState;
	}
	delete state;
}

void NES::SaveState(unsigned char* &buffer, int &lenght)
{
	NESState* state = new NESState;
	SavesNESState(*state);
	unsigned char* mapperState;
	cartucho->getMapperState(mapperState, state->mapperSize);
	int nesStateSize = sizeof(NESState);
	int mapperSize = state->mapperSize;
	lenght = nesStateSize + mapperSize;
	buffer = new unsigned char[lenght];
	memcpy(buffer, state, nesStateSize);
	if (mapperSize > 0) {
		memcpy(buffer + nesStateSize, mapperState, mapperSize);
		delete mapperState;
	}
	delete state;
}

void NES::DeleteState(unsigned char* buffer)
{
	if (!buffer) return;
	delete[] buffer;
}

void NES::SavesNESState(NESState& state)
{
#ifdef ENABLE_APU
	/*APU::ApuSaveState(state.apu);*/
#endif
	cpu.saveCPUState(state.cpu); // Copiar el estado de la CPU
	ppu.SavePPUState(state.ppu); // Copiar el estado de la PPU
	memcpy(state.cpu_ram, cpu_ram, sizeof(cpu_ram));
	//state.framecounter = framecounter;
	state.ClockCounter = ClockCounter;
	state.strobe = strobe;
	//memcpy(state.controles, controles, sizeof(controles));
	//memcpy(state.new_controller_state, new_controller_state, sizeof(new_controller_state));
	

	state.playerpar = playerpar;
	state.transfer_dma = transfer_dma;
	state.transfiriendo = transfiriendo;
	state.dma_page = dma_page;
	state.dma_addr = dma_addr;
	state.dma_data = dma_data;

}

void NES::LoadNESState(NESState* state)
{
#ifdef ENABLE_APU
	/*APU::ApuLoadstate(state->apu);*/
#endif
	cpu.loadCPUState (state->cpu);  // Restaurar el estado de la CPU
	ppu.LoadPPUState (state->ppu);  // Restaurar el estado de la PPU
	memcpy(cpu_ram, state->cpu_ram, sizeof(cpu_ram));
	//framecounter = state->framecounter;
	ClockCounter = state->ClockCounter;
	strobe = state->strobe;
	//memcpy(controles, state->controles, sizeof(controles));
	//memcpy(new_controller_state, state->new_controller_state, sizeof(new_controller_state));

	// Restaurar las nuevas variables
	playerpar = state->playerpar;
	transfer_dma = state->transfer_dma;
	transfiriendo = state->transfiriendo;
	dma_page = state->dma_page;
	dma_addr = state->dma_addr;
	dma_data = state->dma_data;
	/*printf("load state y el frame counter es %lu \n", framecounter);*/   // Testing
}


void NES::clock()
{
	// tick base del sistema
	ppu.clock();
	// cada 3 ticks de ppu va uno de cpu
	if (ClockCounter % 3 == 0) {
		// reviso si hay un DMA activado y se termino de ejecutar la instruccion
		if (transfer_dma && cpu.instructionCompleted()) {
			// iniciar la transferencia si no está en progreso
			if (!transfiriendo) {
				// comenzar la transferencia en un ciclo impar
				if (ClockCounter % 2 == 1) {
					transfiriendo = true;
				}
			}
			if (transfiriendo) {
				// Alternar entre leer y escribir datos
				if (ClockCounter % 2 == 1) {
					dma_data = cpu_Read(dma_page << 8 | dma_addr);
				}
				else {
					ppu.writeOAM(dma_addr, dma_data);
					dma_addr++;
					if (dma_addr == 0x00) {
						transfer_dma = false;
						transfiriendo = false;
					}
				}

			}
		}
		// si no esta en dma hago tick al cpu
		else {
			cpu.clock();
		}
		// elapsed para la apu
		cpu.elapsed++;
		cpu.clock_count++;
	}
	// reviso si hay NMI
	if (ppu.nmi) {
		ppu.nmi = false;
		cpu.NMIcall = true;
	}
	// reviso IRQ
	if (cartucho->IRQ())
	{
		cpu.IRQcall = true;
		cartucho->IRQ_off();
	}

	ClockCounter++;
}

uint32_t* NES::run_frame(bool soundMuted)
{
	while (ppu.newFrame == false) {
		clock();
	}
	framecounter++; 
	ppu.newFrame = false;
	/*printf("Frame_counter NES: %lu\n", framecounter);*/
	/*if (new_controller_state[0].reg > 0 || new_controller_state[1].reg > 0)	*/				/*Para testing!*/
		/*printf("Frame_counter: %lu   E\n", new_controller_state[0].reg, new_controller_state[1].reg, framecounter);*/
#ifdef ENABLE_APU
	if (!soundMuted && !sound_Disabled) APU::RunFrame(cpu.elapsed);
#endif	
	cpu.elapsed = 0;
	return ppu.pixelBuffer;
}
