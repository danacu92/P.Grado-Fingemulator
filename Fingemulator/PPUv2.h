#pragma once
#include <iostream>
#include "Cartucho.h"
using Byte = uint8_t;
using Word = uint16_t;

#define WIDTH 256 
#define HEIGHT 240
#define SPRITE_TOTAL 64
#define SPRITE_LIMIT 8 

// Memory Map
#define PT_BEGIN		0x0000
#define PT_END			0x1FFF
#define NT_BEGIN		0x2000
#define NT_END			0x3EFF
#define AT_BEGIN		0x23C0
#define PALETTE_BEGIN	0x3F00
#define PALETTE_END		0x3FFF


// Marcadores
#define CYCLE_END				340
#define SCANLINE_PRE			-1
#define SCANLINE_VISIBLE_END	239
#define SCANLINE_POST			240
#define SCANLINE_NMI			241
#define SCANLINE_NMI_END		260
#define SCANLINE_END			261

#define FETCH_RANGE	(pixel >= 2 && pixel <= 255) || (pixel >= 322 && pixel <= 337) //Rango donde se va fetcheando tiles a los shifters para el background
#define RENDERING mask.reg & 0x18

union loopy_register
{
	struct
	{
		// estos union van de menos significativo a mas significativo
		Word coarse_x : 5;
		Word coarse_y : 5;
		Word nametable_x : 1;
		Word nametable_y : 1;
		Word fine_y : 3;
		Word unused : 1;
	};

	Word reg = 0x0000;
};

struct OAM_STRUCT
{
	Byte idx;   // # in OAM 
	Byte x;     // coordenada x
	Byte y;     // coordenada i 
	Byte tile; // Offset del tile del sprite
	Byte attr; // Flags del sprite
	Byte tileL; // los patrones
	Byte tileH;
};

// pARA SAVESTATE
struct PPUState {
	// Registros
	Byte control;
	Byte mask;
	Byte status;
	Word vram_address;
	Word tram_address;
	Byte fine_x;

	// Tablas
	Byte tblName[2][1024];
	Byte tblPalette[32];

	// Variables de renderizacion
	uint8_t attribute_byte;
	uint8_t bg_next_tile_ID;
	uint8_t bg_next_attrib_L;
	uint8_t bg_next_attrib_H;
	uint8_t bg_next_tile_L;
	uint8_t bg_next_tile_H;
	uint16_t bg_tile_shifter_L;
	uint16_t bg_tile_shifter_H;
	uint8_t bg_attribute_shifter_L;
	uint8_t bg_attribute_shifter_H;

	// OAM
	Byte oam_ram[256];
	Byte oamAddr;
	OAM_STRUCT oam[SPRITE_TOTAL];
	uint32_t oam_cnt;
	OAM_STRUCT oam2[SPRITE_TOTAL];
	uint32_t oam2_cnt;

	Word ppu_bus_address;
	Byte buffer_ppudata;
	bool scroll_latch;

	// Ciclo principal
	int scanline;
	uint32_t cycle;
	uint32_t ppu_framecycles;
	bool odd_frame;
	bool newFrame;
	bool nmi;
	bool no_nmi_este_frame;

};

class PPUv2
{
public:
	PPUv2();
	~PPUv2();
	//--------------------------- TABLAS ---------------------//
private:
	uint32_t nes_palette[0x40] =
	{
		0x5c5c5c, 0x00237e, 0x100e9e, 0x33009b, 0x520074, 0x630037, 0x610500, 0x4c1700, 0x2b2c00, 0x093e00, 0x004700, 0x004505, 0x003744, 0x000000, 0x000000, 0x000000,
		0xa7a7a7, 0x1157d6, 0x3b38ff, 0x6d21fe, 0x9916c8, 0xb11973, 0xae2a1a, 0x904500, 0x626400, 0x307d00, 0x0a8a00, 0x00862a, 0x007385, 0x000000, 0x000000, 0x000000,
		0xfeffff, 0x5ba9ff, 0x8a88ff, 0xc16eff, 0xef61ff, 0xff65c7, 0xff7866, 0xe6961b, 0xb4b700, 0x7fd200, 0x53e027, 0x3cdc79, 0x3fc7da, 0x454545, 0x000000, 0x000000,
		0xfeffff, 0xbadbff, 0xcecdff, 0xe5c2ff, 0xf8bcff, 0xffbee7, 0xffc6be, 0xf4d39c, 0xe0e18a, 0xc9ec8c, 0xb6f2a2, 0xacf0c6, 0xade8ef, 0xb0b0b0, 0x000000, 0x000000,
	};
	Byte tblName[2][1024];
	Byte tblPattern[2][4096];
	Byte tblPalette[32]; //Paleta de colores


///////////////////////////////////////////////////////////////////////////////////////////////////////////

	//--------------------------- REGISTROS ---------------------//
	union PPUCTRL
	{
		struct {
			Byte nametable : 2;  // Nametable (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
			Byte increment_mode : 1;  // Incremento de dirección VRAM (0 = 1, 1 = 32)
			Byte sprite_table : 1;  // Dirección de la tabla de patrones de sprites para sprites 8x8
			Byte bg_table : 1;  // Dirección de la tabla de patrones de fondo
			Byte sprite_size : 1;  // Tamaño de sprite, 8x8 / 8x16
			Byte slave : 1;  // Selección de PPU maestro/esclavo (no se usa)
			Byte nmi : 1;  // generar nmi
		} bit;
		Byte reg;
	};

	union PPUMASK
	{
		struct {
			Byte gray : 1;  // Escala de grises
			Byte bg_left : 1;  // Mostrar fondo en los 8 px mas a la izquierda de la pantalla
			Byte sprite_left : 1;  // Mostrar sprite en los 8 px mas a la izquierda de la pantalla
			Byte render_bg : 1;  // Mostrar fondo
			Byte render_sprite : 1;  // Mostrar sprites
			Byte red : 1;  // Resaltar rojo
			Byte green : 1;  // Resaltar verde
			Byte blue : 1;  // Resaltar azul
		} bit;
		Byte reg;
	};

	union PPUSTATUS
	{
		struct {
			Byte busInfo : 5;  // Open bus datos que quedan en el bus de una transaccion anterior.
			Byte sprite_overflow : 1;  // Sprite overflow (más de 8 sprites en una scanline)
			Byte sprite_zero_hit : 1;  // Sprite zero hit
			Byte VBlank : 1;  // VBlank
		} bit;
		Byte reg;
	};

	PPUCTRL control;
	PPUMASK mask;
	PPUSTATUS status;
	


	loopy_register vram_address;
	loopy_register tram_address;

	// Pixel offset horizontally
	uint8_t fine_x = 0x00;

	//////////////////////////// BACKGROUND ////////////////////////////////

private:
	uint8_t attribute_byte = 0x00;
	uint8_t bg_next_tile_ID = 0x00;
	uint8_t bg_next_attrib_L = 0x00;
	uint8_t bg_next_attrib_H = 0x00;
	uint8_t bg_next_tile_L = 0x00;
	uint8_t bg_next_tile_H = 0x00;
	uint16_t bg_tile_shifter_L = 0x0000;
	uint16_t bg_tile_shifter_H = 0x0000;
	uint8_t bg_attribute_shifter_L = 0x00;
	uint8_t bg_attribute_shifter_H = 0x00;
	void ReloadShifters();

	//////////////////////////////////  SPRITES  ////////////////////////////////////////

public:
	void writeOAM(Byte addres, Byte data);

private:
	
	Byte oam_ram[256];  // aca se cargan los juegos por DMA
	Byte oamAddr;
	OAM_STRUCT oam[SPRITE_TOTAL];   // guarda los patrones de los sprites que se dibujan
	uint32_t oam_cnt;
	OAM_STRUCT oam2[SPRITE_TOTAL];  //  guarda 8 sprites de la scanline siguiente
	uint32_t oam2_cnt;

	void EvaluarSprites(); // Prepara sprites para la proxima scanline
	void LoadSprites();  // Carga los patrones de los sprites para dibujar.
	void ClearOAM(); // Limpia oam2
	/// ////////////////////////////////////////COMUNICACIONES///////////////////////////////////////////////////////////////

	Word ppu_bus_address; // Direccion de memoria conectada al bus del ppu que uso en distintas operaciones.


	// Comunicaciones internas
	Byte buffer_ppudata = 0x00;
	bool scroll_latch = 0;

public:
	// CPU BUS
	Byte cpuBusRead(Word addr, bool read_only = false);
	void cpuBusWrite(Word addr, Byte  data);

	// PPU BUS
	Byte ppuBusRead(Word addr);
	void ppuBusWrite(Word addr, Byte data);


	// Cartucho de juego
private:
	std::shared_ptr<Cartucho> cartucho = nullptr;

public:
	void ConectarCartucho(const std::shared_ptr<Cartucho>& juego);
	void DesconectarCartucho();



	//////////////////////////////////////////Ciclo Principal///////////////////////////////////////

private:

	// Atributos
	int scanline;
	uint32_t  cycle, ppu_framecycles;
	bool odd_frame = false;
	

	//Funciones Auxiliares 
	uint32_t GetColor(Byte paleta);
	void h_scroll();
	void v_scroll();
	void ProcessPixel();
	void VisibleScanline();
	void VblankScanline();

public:

	uint32_t pixelBuffer[WIDTH * HEIGHT];	// Screen Buffer 256x240
	bool newFrame = false;
	bool nmi = false; // Booleano que se activa cuando hay una nmi.
	bool no_nmi_este_frame = false;

	//Save y load
	void SavePPUState(PPUState& state);
	void LoadPPUState(PPUState state);

	// Metodos
	void clock();
	void reset();


};