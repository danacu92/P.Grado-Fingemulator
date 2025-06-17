#include "PPUv2.h"
#include <cstdint>

using Byte = uint8_t;
using Word = uint16_t;



PPUv2::PPUv2()
{
}

PPUv2::~PPUv2()
{
}

Byte PPUv2::cpuBusRead(Word address, bool read_only) //ok
{
	Byte data = 0x00;
	if (read_only)
	{
		switch (address)
		{
		case 0x0000: // Control
			data = control.reg;
			break;
		case 0x0001: // Mask
			data = mask.reg;
			break;
		case 0x0002: // Status
			data = status.reg;
			break;
		case 0x0003: // OAM Address
			data = oamAddr;
			break;
		case 0x0004: // OAM Data
			data = oam_ram[oamAddr];
			break;
		case 0x0005: // Scroll
			break;
		case 0x0006: // PPU Address
			break;
		case 0x0007: // PPU Data
			break;
		}
	}
	else
	{
		
		switch (address)
		{
			// Control - no legible
		case 0x0000: 
			break;

			// Mask - no legible
		case 0x0001: 
			break;

			// Status
		case 0x0002:
			// Los ultimos 5 bits se devuelven residuos de la lectura anterior
			// leer este registro resetea el latch y el bit vblank

			if (scanline == 241 && cycle < 3) {
				// https://wiki.nesdev.com/w/index.php/PPU_frame_timing si se lee antes de vblank desactiva el nmi pero lo devuelve como 1, si lo hace en ciclo 0 desactiva el bit de vblank y desactiva el nmi
				no_nmi_este_frame = 1;
				if (cycle == 0) 
					status.bit.VBlank = 0;
			}

			data = (status.reg & 0xE0) | (buffer_ppudata & 0x1F);
			buffer_ppudata = data;
			status.bit.VBlank = 0;
			scroll_latch = 0;
			break;

			// OAM Address No legible
		case 0x0003: 
			break;

			// OAM Data
		case 0x0004:
			data = oam_ram[oamAddr];
			buffer_ppudata = data;
			break;

			// Scroll No legible
		case 0x0005:
			break;

			// PPU Address No legible
		case 0x0006: 
			break;

			// PPU Data
		case 0x0007:
			//buffer delay
			data = buffer_ppudata;
			buffer_ppudata = ppuBusRead(vram_address.reg);
			// palet table no tiene delay
			if (vram_address.reg >= 0x3F00) data = buffer_ppudata;
			// cada lectura incrementa en 1 el vram address
			vram_address.reg += (control.bit.increment_mode ? 32 : 1);
			break;
		}
	}

	return data;
}

void PPUv2::cpuBusWrite(Word addr, Byte data) //ok
{
	addr &= 0x0007;

	buffer_ppudata = data;

	switch (addr)
	{
	case 0x0000: // PPUCTRL
		if ((data & 0x80) != 0) {
			if ((~control.reg & 0x80) && status.bit.VBlank) {
				nmi = true;
			}
		}
		control.reg = data;
		tram_address.nametable_x = (control.bit.nametable & 0x01 ? 1 : 0);
		tram_address.nametable_y = (control.bit.nametable & 0x02 ? 1 : 0);
		break;

	case 0x0001: // PPUMASK
		if ((data & 0x18) == 0 && RENDERING) {
			ppu_bus_address = vram_address.reg & 0x3fff;
		}
		mask.reg = data;
		break;

	case 0x0003: // OAMADDR
		oamAddr = data;
		break;

	case 0x0004: // OAMDATA
		if (RENDERING && (scanline < SCANLINE_NMI || scanline == SCANLINE_END)) {
			return;
		}
		if ((oamAddr & 3) == 2) data &= 0xe3;
		oam_ram[oamAddr] = data;
		oamAddr = (oamAddr + 1);
		break;

	case 0x0005: // PPUSCROLL
		if (!scroll_latch) {
			// Se escribe primero el coarse x y fine x
			fine_x = data & 0x07;
			tram_address.coarse_x = data >> 3;;
		}
		else {
			// en la segunda escritura el coarse y el fine y
			tram_address.fine_y = data & 0x07;
			tram_address.coarse_y = data >> 3;
		}
		scroll_latch = !scroll_latch;
		break;
	case 0x0006: // PPUADDR
		if (!scroll_latch) {
			// Primero escribe el byte alto de la direccion. 
			tram_address.reg = ((Word)(data & 0x3F) << 8) | (tram_address.reg & 0x00FF);
		}
		else { // Luego escribe el byte bajo de la direccion.
			tram_address.reg = (tram_address.reg & 0xFF00) | data;
			vram_address.reg = tram_address.reg;
		}
		scroll_latch = !scroll_latch;
		break;
	case 0x0007: // PPUDATA
		ppu_bus_address = vram_address.reg;
		ppuBusWrite(ppu_bus_address, data);
		vram_address.reg += (control.bit.increment_mode ? 32 : 1);
		break;
	}
}

Byte PPUv2::ppuBusRead(Word address)
{
	Byte data = 0x00;
	address &= 0x3FFF;

	if (cartucho->ppu_read(address, data))
	{
		return data;
	}

	else if (address >= PT_BEGIN && address <= PT_END)
	{
		return tblPattern[(address & 0x1000) >> 12][address & 0x0FFF];
	}

	else if (address >= NT_BEGIN && address <= NT_END)
	{
		address &= 0x0FFF;
		if (cartucho->GetMirrorType() == MIRROR::HORIZONTAL)
		{
			if (address >= 0x0000 && address <= 0x03FF)
				return tblName[0][address & 0x03FF];
			if (address >= 0x0400 && address <= 0x07FF)
				return tblName[0][address & 0x03FF];
			if (address >= 0x0800 && address <= 0x0BFF)
				return tblName[1][address & 0x03FF];
			if (address >= 0x0C00 && address <= 0x0FFF)
				return tblName[1][address & 0x03FF];
		}

		else if (cartucho->GetMirrorType() == MIRROR::VERTICAL)
		{
			if (address >= 0x0000 && address <= 0x03FF)
				return tblName[0][address & 0x03FF];
			if (address >= 0x0400 && address <= 0x07FF)
				return tblName[1][address & 0x03FF];
			if (address >= 0x0800 && address <= 0x0BFF)
				return tblName[0][address & 0x03FF];
			if (address >= 0x0C00 && address <= 0x0FFF)
				return tblName[1][address & 0x03FF];
		}
	}

	else if (address >= PALETTE_BEGIN && address <= PALETTE_END)
	{
		address &= 0x001F;
		if (address == 0x0010) address = 0x0000;
		if (address == 0x0014) address = 0x0004;
		if (address == 0x0018) address = 0x0008;
		if (address == 0x001C) address = 0x000C;
		return tblPalette[address] & (mask.bit.gray ? 0x30 : 0x3F);
	}
}

void PPUv2::ppuBusWrite(Word addr, Byte data)
{
	addr &= 0x3FFF;

	if (cartucho->ppu_write(addr, data)) {

	}

	else if (addr >= PT_BEGIN && addr <= PT_END)
	{
		tblPattern[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
	}

	else if (addr >= NT_BEGIN && addr <= NT_END)
	{
		addr &= 0x0FFF;
		if (cartucho->GetMirrorType() == MIRROR::VERTICAL)
		{
			// Vertical
			if (addr >= 0x0000 && addr <= 0x03FF)
				tblName[0][addr & 0x03FF] = data;
			else if (addr >= 0x0400 && addr <= 0x07FF)
				tblName[1][addr & 0x03FF] = data;
			else if (addr >= 0x0800 && addr <= 0x0BFF)
				tblName[0][addr & 0x03FF] = data;
			else if (addr >= 0x0C00 && addr <= 0x0FFF)
				tblName[1][addr & 0x03FF] = data;
		}
		else if (cartucho->GetMirrorType() == MIRROR::HORIZONTAL)
		{
			// Horizontal
			if (addr >= 0x0000 && addr <= 0x03FF)
				tblName[0][addr & 0x03FF] = data;
			else if (addr >= 0x0400 && addr <= 0x07FF)
				tblName[0][addr & 0x03FF] = data;
			else if (addr >= 0x0800 && addr <= 0x0BFF)
				tblName[1][addr & 0x03FF] = data;
			else if (addr >= 0x0C00 && addr <= 0x0FFF)
				tblName[1][addr & 0x03FF] = data;
		}
	}
	else if (addr >= PALETTE_BEGIN && addr <= PALETTE_END)
	{
		addr &= 0x001F;
		if (addr == 0x0010) addr = 0x0000;
		if (addr == 0x0014) addr = 0x0004;
		if (addr == 0x0018) addr = 0x0008;
		if (addr == 0x001C) addr = 0x000C;
		tblPalette[addr] = data;
	}
}

void PPUv2::ConectarCartucho(const std::shared_ptr<Cartucho>& juego)
{
	this->cartucho = juego;
}

void PPUv2::DesconectarCartucho()
{
	cartucho = nullptr;
}


uint32_t PPUv2::GetColor(Byte paleta)
{

	Byte value = ppuBusRead(PALETTE_BEGIN + paleta);
	if (value >= 0x00 && value < 0x40)
		return nes_palette[value];
	else
		return 0x3F; //Black
}

void PPUv2::h_scroll()
{
	if (RENDERING)
	{
		if (vram_address.coarse_x == 31) //si estoy al final de una pantalla paso a la otra
		{
			vram_address.coarse_x = 0;
			vram_address.nametable_x = !vram_address.nametable_x;
		}
		else
		{
			vram_address.coarse_x++;
		}
	}
}

void PPUv2::v_scroll()
{
	if (RENDERING)
	{
		
		if (vram_address.fine_y == 7)
		{
			vram_address.fine_y = 0;

			
			if (vram_address.coarse_y == 29) // veo si cruza a otra pantalla
			{
				vram_address.coarse_y = 0;
				vram_address.nametable_y = !vram_address.nametable_y;
			}
			else if (vram_address.coarse_y == 31)
			{
				vram_address.coarse_y = 0;
			}
			else
			{
				vram_address.coarse_y++;
			}
		}
		else
		{
			vram_address.fine_y++;
			
		}
	}
}

inline void PPUv2::ReloadShifters() // podria  usar inline para evitar overhead
{
	// Tile shifters
	bg_tile_shifter_L = (bg_tile_shifter_L & 0xFF00) | bg_next_tile_L;
	bg_tile_shifter_H = (bg_tile_shifter_H & 0xFF00) | bg_next_tile_H;

	// Atribute shifters
	bg_next_attrib_L = attribute_byte & 1;
	bg_next_attrib_H = (attribute_byte & 2) >> 1;
}

void PPUv2::writeOAM(Byte addres, Byte data)
{
	oam_ram[addres] = data;
}

void PPUv2::VisibleScanline()
{
	//SPRITES
	switch (cycle) {
	case 1:
		if (scanline == SCANLINE_PRE)
		{
			status.bit.VBlank = 0;
			status.bit.sprite_zero_hit = 0;
			status.bit.sprite_overflow = 0;

		}
		else {
			ClearOAM();
		}

		break;
	case 257:
		if (scanline != SCANLINE_PRE) {
			EvaluarSprites();
		}

		if (RENDERING) {
			oamAddr = 0;
		}
		break;
	case 321:
		if (RENDERING) {
			LoadSprites();
		}
		else {
			oam_cnt = 0;
		}
		break;
	}

	// Background
	if ((cycle >= 1 && cycle <= 257) || (cycle >= 321 && cycle <= 337))//Zona donde se va fetcheando tiles a los shifters para el background, a su vez se van dibujando los pixeles correspondientes de los ciclos anteriores.
	{

		ProcessPixel(); // Procesar un pixel
		switch (cycle % 8) {
		case 1:
			ppu_bus_address = 0x2000 | (vram_address.reg & 0x0fff); // Armo la direccion de la nametable
			ReloadShifters();
			if (cycle == 257 && RENDERING) {
				vram_address.nametable_x = tram_address.nametable_x;
				vram_address.coarse_x = tram_address.coarse_x;
			}
			break;
		case 2:
			bg_next_tile_ID = ppuBusRead(ppu_bus_address);
			if (bg_next_tile_ID != 0x20)
				cycle = cycle;
			break;
		case 3:
			ppu_bus_address = AT_BEGIN | (vram_address.reg & 0x0c00) | ((vram_address.coarse_y >> 2) << 3) | (vram_address.coarse_x >> 2);
			break;
		case 4:
			attribute_byte = ppuBusRead(ppu_bus_address);
			if ((vram_address.coarse_y & 0x02) != 0x00) attribute_byte >>= 4;
			if ((vram_address.coarse_x & 0x02) != 0x00) attribute_byte >>= 2;
			break;
		case 5:
			ppu_bus_address = (control.bit.bg_table << 12) + ((uint16_t)bg_next_tile_ID << 4) + (vram_address.fine_y);
			break;
		case 6:
			bg_next_tile_L = ppuBusRead(ppu_bus_address);
			break;
		case 7:
			ppu_bus_address = (control.bit.bg_table << 12) + ((uint16_t)bg_next_tile_ID << 4) + (vram_address.fine_y) + 8;
			break;
		case 0:
			bg_next_tile_H = ppuBusRead(ppu_bus_address);
			if (cycle == 256) {
				v_scroll();
			}
			h_scroll();
			break;
		}

	}
	if (scanline == -1 && cycle >= 280 && cycle <= 304 && RENDERING) {
		// actualizo vram copiando tram
		vram_address.fine_y = tram_address.fine_y;
		vram_address.nametable_y = tram_address.nametable_y;
		vram_address.coarse_y = tram_address.coarse_y;
	}

	if (cycle >= 337 && cycle <= 340 && RENDERING) {
		// dummy fetches
		ppu_bus_address = 0x2000 | (vram_address.reg & 0x0fff);
	}
} // VisibleScanline

void PPUv2::VblankScanline()
{
	if (cycle == 1 && scanline == SCANLINE_NMI)
	{
		status.bit.VBlank = 1;
		if (control.bit.nmi == 1 and no_nmi_este_frame == 0)
			nmi = true;
		no_nmi_este_frame = 0;
	}
}

// Prepara sprites para la proxima scanline
void PPUv2::EvaluarSprites()
{
	oam2_cnt = 0;

	for (int i = 0; i < SPRITE_TOTAL; i++) {
		int  line = scanline - oam_ram[(i * 4) + 0];
		if (line >= 0 && line < (control.bit.sprite_size ? 16 : 8)) {
			if (oam2_cnt == 8) {
				status.bit.sprite_overflow = 1;
				break;
			}
			oam2[oam2_cnt].idx = i;
			oam2[oam2_cnt].y = oam_ram[(i * 4) + 0];
			oam2[oam2_cnt].tile = oam_ram[(i * 4) + 1];
			oam2[oam2_cnt].attr = oam_ram[(i * 4) + 2];
			oam2[oam2_cnt].x = oam_ram[(i * 4) + 3];
			oam2_cnt++;
		}
	}
} //EvaluarSprites


// Carga los patrones de los sprites para dibujar.
void PPUv2::LoadSprites()
{
	auto flipbyteX = [](Byte b)
	{
		b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
		b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
		b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
		return b;
	};

	oam_cnt = 0;
	Byte sprite_height = control.bit.sprite_size ? 16 : 8;
	for (uint32_t i = 0; i < oam2_cnt; i++) {
		oam[oam_cnt++] = oam2[i];
		int sprY = scanline - oam[i].y;
		sprY = (oam[i].attr & 0x80) ? (sprY ^= sprite_height - 1) : sprY; //giro vertical

		if (sprite_height == 16) {
			ppu_bus_address = ((oam[i].tile & 1) * 0x1000) + ((oam[i].tile & 0xfe) * 16)
				+ ((sprY & 8) << 1) + (sprY & 7);
		}
		else {
			ppu_bus_address = (control.bit.sprite_table * 0x1000) + (oam[i].tile * 16) + (sprY & 7);
		}

		oam[i].tileL = ppuBusRead(ppu_bus_address);
		ppu_bus_address += 8;
		oam[i].tileH = ppuBusRead(ppu_bus_address);


		if (oam[i].attr & 0x40) { // giro horizontal
			oam[i].tileL = flipbyteX(oam[i].tileL);
			oam[i].tileH = flipbyteX(oam[i].tileH);
		}
	}
}


// Determina un pixel de salida.
void PPUv2::ProcessPixel()
{
#define get_bit(x, n) (((x) & (1 << (n))) != 0)

	// Coordenada actual del píxel 
	int x = cycle - 2;

	if (scanline >= 0 && scanline < 240 && x >= 0 && x < 256) {
		uint8_t pixel = 0;     // Píxel de fondo o sprite a dibujar
		uint8_t spritePalette = 0;  // Paleta del sprite
		uint8_t spritePriority = 0; // Prioridad del sprite
		uint8_t spritePattern = 0;  // Patrón del sprite
		uint16_t effectiveX = 0;    // Posición efectiva del sprite en X

		// Hack para paletas de fondo
		if (!RENDERING && ((vram_address.reg & 0x3f00) == 0x3f00)) {
			pixel = vram_address.reg & 0x1f;
		}

		// Procesar píxel de fondo
		if (mask.bit.render_bg && !(mask.bit.bg_left == 0 && x < 8)) {   //si esta activado el dibujo de background y esta en rango visible
			pixel = (get_bit(bg_tile_shifter_H, 15 - fine_x) << 1) |  //se obtiene los dos bits de patron y se alojan en los dos bits menos significativos
				(get_bit(bg_tile_shifter_L, 15 - fine_x));

			if (pixel) {
				pixel |= ((get_bit(bg_attribute_shifter_H, 7 - fine_x) << 1) |
					(get_bit(bg_attribute_shifter_L, 7 - fine_x)))  // se obtiene los bits de atributo y se colocan en el 4to y 3er bit menos significativo
					<< 2;
			}
		}

		// Procesar píxel de sprite
		if (mask.bit.render_sprite && !(mask.bit.sprite_left == 0 && x < 8)) { //si esta activado el rendering de sprite y esta en rango visible
			for (int i = oam_cnt - 1; i >= 0; i--) {  // reviso todos los sprites visibles
				if (oam[i].idx != 0xff) { //Si es un Sprite válido
					effectiveX = x - oam[i].x;

					if (effectiveX < 8) { //  Si esta visible 
						spritePattern = (get_bit(oam[i].tileH, 7 - effectiveX) << 1) |
							(get_bit(oam[i].tileL, 7 - effectiveX));

						if (spritePattern != 0) { // Si no es transparente
							if (oam[i].idx == 0 && pixel && x != 0xff) { //Sprite zero hit
								status.bit.sprite_zero_hit = 1;
							}

							spritePattern |= (oam[i].attr & 3) << 2;
							spritePalette = spritePattern + 0x10;
							spritePriority = oam[i].attr & 0x20;
						}
					}
				}
			}
		}

		// Determinar el píxel final
		if (spritePalette && (pixel == 0 || spritePriority == 0)) {
			pixel = spritePalette;
		}

		if (!RENDERING)
			pixel = 0;

		// Dibujar el píxel final en el buffer
		uint32_t finalPixel = GetColor(pixel);
		int pos = (scanline * 256) + x;
		pixelBuffer[pos] = finalPixel;
	}

	// Actualizo shifters
	bg_tile_shifter_L <<= 1;
	bg_tile_shifter_H <<= 1;
	bg_attribute_shifter_L = (bg_attribute_shifter_L << 1) | bg_next_attrib_L;
	bg_attribute_shifter_H = (bg_attribute_shifter_H << 1) | bg_next_attrib_H;
}






//run one PPU Cycle 
void PPUv2::clock()
{

	if ((scanline == SCANLINE_PRE) || (scanline >= 0 && scanline <= SCANLINE_VISIBLE_END))
	{
		VisibleScanline();
	}

	else if (scanline == SCANLINE_POST && cycle == 0)
	{
		newFrame = true;
	}
	else if (scanline >= SCANLINE_NMI && scanline <= SCANLINE_NMI_END)
	{
		VblankScanline();
	}
	cycle++;
	if (cycle > CYCLE_END)
	{
		cycle = 0;
		scanline++;
	}
	if (scanline >= SCANLINE_END)
	{
		scanline = SCANLINE_PRE;
		ppu_framecycles++;
		// isEvenFrame = !isEvenFrame;
	}

	// mapper 4 
	if (RENDERING) {
		if (cycle == 260 && scanline < 240)
			cartucho->scanlineSignal();
	}
}



void PPUv2::ClearOAM()
{
	memset(oam2, 0xff, sizeof(oam2));

} // ClearOAM()





void PPUv2::SavePPUState(PPUState& state)
{
	// Guardar los registros
	state.control = control.reg;
	state.mask = mask.reg;
	state.status = status.reg;
	state.vram_address = vram_address.reg;
	state.tram_address = tram_address.reg;
	state.fine_x = fine_x;

	// Guardar las tablas
	memcpy(state.tblName, tblName, sizeof(tblName));
	//memcpy(state.tblPattern, tblPattern, sizeof(tblPattern));
	memcpy(state.tblPalette, tblPalette, sizeof(tblPalette));

	// Guardar las variables de rasterización
	state.attribute_byte = attribute_byte;
	state.bg_next_tile_ID = bg_next_tile_ID;
	state.bg_next_attrib_L = bg_next_attrib_L;
	state.bg_next_attrib_H = bg_next_attrib_H;
	state.bg_next_tile_L = bg_next_tile_L;
	state.bg_next_tile_H = bg_next_tile_H;
	state.bg_tile_shifter_L = bg_tile_shifter_L;
	state.bg_tile_shifter_H = bg_tile_shifter_H;
	state.bg_attribute_shifter_L = bg_attribute_shifter_L;
	state.bg_attribute_shifter_H = bg_attribute_shifter_H;

	// Guardar el OAM
	memcpy(state.oam_ram, oam_ram, sizeof(oam_ram));
	memcpy(state.oam, oam, sizeof(oam));
	memcpy(state.oam2, oam2, sizeof(oam2));
	state.oam_cnt = oam_cnt;
	state.oam2_cnt = oam2_cnt;
	state.oamAddr = oamAddr;
	state.ppu_bus_address = ppu_bus_address;
	state.buffer_ppudata = buffer_ppudata;
	state.scroll_latch = scroll_latch;

	// Guardar ciclo principal y otros
	state.scanline = scanline;
	state.cycle = cycle;
	state.ppu_framecycles = ppu_framecycles;
	state.odd_frame = odd_frame;
	state.newFrame = newFrame;
	state.nmi = nmi;
	state.no_nmi_este_frame = no_nmi_este_frame;


}

void PPUv2::LoadPPUState(PPUState state)
{
		// Restaurar los registros
		control.reg = state.control;
		mask.reg = state.mask;
		status.reg = state.status;
		vram_address.reg = state.vram_address;
		tram_address.reg = state.tram_address;
		fine_x = state.fine_x;

		// Restaurar las tablas
		memcpy(tblName, state.tblName, sizeof(tblName));
		//memcpy(tblPattern, state.tblPattern, sizeof(tblPattern));
		memcpy(tblPalette, state.tblPalette, sizeof(tblPalette));

		// Restaurar las variables de rasterización
		attribute_byte = state.attribute_byte;
		bg_next_tile_ID = state.bg_next_tile_ID;
		bg_next_attrib_L = state.bg_next_attrib_L;
		bg_next_attrib_H = state.bg_next_attrib_H;
		bg_next_tile_L = state.bg_next_tile_L;
		bg_next_tile_H = state.bg_next_tile_H;
		bg_tile_shifter_L = state.bg_tile_shifter_L;
		bg_tile_shifter_H = state.bg_tile_shifter_H;
		bg_attribute_shifter_L = state.bg_attribute_shifter_L;
		bg_attribute_shifter_H = state.bg_attribute_shifter_H;

		// Restaurar el OAM
		memcpy(oam_ram, state.oam_ram, sizeof(oam_ram));
		memcpy(oam, state.oam, sizeof(oam));
		memcpy(oam2, state.oam2, sizeof(oam2));
		oam_cnt = state.oam_cnt;
		oam2_cnt = state.oam2_cnt;
		oamAddr = state.oamAddr;
		ppu_bus_address = state.ppu_bus_address;
		buffer_ppudata = state.buffer_ppudata;
		scroll_latch = state.scroll_latch;

		// Restaurar ciclo principal y otros
		scanline = state.scanline;
		cycle = state.cycle;
		ppu_framecycles = state.ppu_framecycles;
		odd_frame = state.odd_frame;
		newFrame = state.newFrame;
		nmi = state.nmi;
		no_nmi_este_frame = state.no_nmi_este_frame;
		
	
}



void PPUv2::reset()
{
	memset(tblName, 0x00, sizeof(tblName));
	memset(tblPattern, 0x00, sizeof(tblPattern));
	control.reg = 0x00;
	mask.reg = 0x00;
	status.reg = 0x00;
	vram_address.reg = 0x0000;
	tram_address.reg = 0x0000;
	fine_x = 0x00;
	attribute_byte = 0x00;
	bg_next_tile_ID = 0x00;
	bg_next_attrib_L = 0x00;
	bg_next_attrib_H = 0x00;
	bg_next_tile_L = 0x00;
	bg_next_tile_H = 0x00;
	bg_tile_shifter_L = 0x0000;
	bg_tile_shifter_H = 0x0000;
	bg_attribute_shifter_L = 0x00;
	bg_attribute_shifter_H = 0x00;
	buffer_ppudata = 0x00;
	scroll_latch = 0;
	//cartucho = nullptr;
	scanline = cycle = ppu_framecycles = 0;
	no_nmi_este_frame = false;
	newFrame = false;
	nmi = false;
	memset(pixelBuffer, 0x00, sizeof(pixelBuffer));
	memset(oam_ram, 0x00, sizeof(oam_ram));
	memset(oam, 0x00, sizeof(oam));
	memset(oam2, 0x00, sizeof(oam2));
	oam2_cnt = 0;
	oam_cnt = 0;

}


