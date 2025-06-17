#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>

using Byte = uint8_t;
using Word = uint16_t;

struct CPUState {
	// Registro de FLAGS
	Byte status;
	Byte ra;
	Byte rx;
	Byte ry;
	Byte sp;
	Word pc;
	Word addr_abs;
	Byte cycles;
	bool IRQcall;
	bool NMIcall;
	uint32_t elapsed;
};

// Forward declaration para inclusion circular
class Sistema;

class m6502_2A03
{
public:
	m6502_2A03();
	~m6502_2A03();

private:
	Sistema* busesito = nullptr;
public:
	void conectarBus(Sistema* s);

private:
	// FLAGS
	union FLAGS
	{
		struct {
			Byte C : 1;	// Carry Bit
			Byte Z : 1;	// Zero
			Byte I : 1;	// Disable Interrupts
			Byte D : 1;	// Decimal Mode (unused in this implementation)
			Byte B : 1;	// Break
			Byte U : 1;	// Unused
			Byte V : 1;	// Overflow
			Byte N : 1;	// Negative
		} flag;
		Byte reg;
	};
	FLAGS status;
	Byte ra = 0x00;
	Byte rx = 0x00;
	Byte ry = 0x00;
	Byte sp = 0x00;
	Word pc = 0xFFFC;
	Word ppc = 0x0000; //pc anterior.

public:
	Byte read(Word dir);
	void write(Word dir, Byte data);
	void reset();
	void init();
	void test();
	void IRQ();
	void NMI();
	bool IRQcall = false;
	bool NMIcall = false;
	void clock();
	bool instructionCompleted();
	void print_cpu_status(std::string name);
	
	void saveCPUState(CPUState &state);
	void loadCPUState(CPUState state);
	Byte fetch();
	Byte fetched = 0x00;
	Word addr_abs = 0x0000;
	Word addr_rel = 0x00;
	Byte opcode = 0x00;
	Byte cycles = 0;
	uint32_t clock_count = 0;
	uint32_t elapsed = 0;

private:
	struct instruccion
	{
		std::string name;
		Byte(m6502_2A03::* op)(void) = nullptr;
		Byte(m6502_2A03::* modo_dir)(void) = nullptr;
		Byte     cycles = 0;
	};

	std::vector<instruccion> tabla;
	//log 
	//std::unordered_map<uint8_t, std::string> testErrors2H;
	bool logmode = false; // Para debuguear.
	std::stringstream operation;
	std::stringstream operator1;
	std::stringstream operator2;
	std::stringstream logoutput;
	void  logInstruction(Word adress, Byte opcode);

	// Addressing Modes ==============================================
private:
	Byte IMP(); // Implied: El operando está implícito en la instrucción.
	Byte IMM(); // Immediate: El operando es un valor inmediato.
	Byte ZPG(); // Zero Page: Dirección de 8 bits dentro de la página cero.
	Byte ZPX(); // Zero Page, X: Dirección de 8 bits desplazada por el registro X.
	Byte ZPY(); // Zero Page, Y: Dirección de 8 bits desplazada por el registro Y.
	Byte REL(); // Relative: Desplazamiento relativo para instrucciones de salto.
	Byte ABS(); // Absolute: Dirección completa de 16 bits.
	Byte ABX(); // Absolute, X: Dirección absoluta desplazada por el registro X.
	Byte ABY(); // Absolute, Y: Dirección absoluta desplazada por el registro Y.
	Byte IND(); // Indirect: Dirección apuntada por un puntero de 16 bits.
	Byte IZX(); // Indexed Indirect (X): Dirección indirecta usando X como índice.
	Byte IZY(); // Indirect Indexed (Y): Dirección indirecta desplazada por Y.

private:
	// Opcodes ======================================================
	private:
    // ** Operaciones Aritméticas y Lógicas
    Byte ADC(); // Add with Carry: Suma con acarreo.
    Byte SBC(); // Subtract with Carry: Resta con acarreo.
    Byte AND(); // Logical AND: Operación lógica AND.
    Byte ORA(); // Logical OR: Operación lógica OR.
    Byte EOR(); // Exclusive OR: Operación XOR lógica.
    Byte ASL(); // Arithmetic Shift Left: Desplaza bits a la izquierda.
    Byte LSR(); // Logical Shift Right: Desplaza bits a la derecha.
    Byte ROL(); // Rotate Left: Rotación de bits a la izquierda.
    Byte ROR(); // Rotate Right: Rotación de bits a la derecha.

    // ** Operaciones de Comparación
    Byte CMP(); // Compare Accumulator: Compara el acumulador.
    Byte CPX(); // Compare X Register: Compara el registro X.
    Byte CPY(); // Compare Y Register: Compara el registro Y.

    // ** Operaciones de Salto
    Byte JMP(); // Jump to Address: Salta a una dirección específica.
    Byte JSR(); // Jump to Subroutine: Salta a una subrutina.
    Byte RTS(); // Return from Subroutine: Retorna de una subrutina.
    Byte BRK(); // Force Interrupt: Genera una interrupción.
    Byte RTI(); // Return from Interrupt: Retorna de una interrupción.

    // ** Operaciones de Bit Test
    Byte BIT(); // Bit Test: Prueba bits específicos en memoria.

    // ** Operaciones de Incremento/Decremento
    Byte INC(); // Increment Memory: Incrementa el valor en memoria.
    Byte INX(); // Increment X Register: Incrementa el registro X.
    Byte INY(); // Increment Y Register: Incrementa el registro Y.
    Byte DEC(); // Decrement Memory: Decrementa el valor en memoria.
    Byte DEX(); // Decrement X Register: Decrementa el registro X.
    Byte DEY(); // Decrement Y Register: Decrementa el registro Y.

    // ** Operaciones de Estado
    Byte CLC(); // Clear Carry Flag: Limpia el bit de acarreo.
    Byte CLD(); // Clear Decimal Mode: Desactiva el modo decimal.
    Byte CLI(); // Clear Interrupt Disable: Habilita interrupciones.
    Byte CLV(); // Clear Overflow Flag: Limpia el bit de desbordamiento.
    Byte SEC(); // Set Carry Flag: Establece el bit de acarreo.
    Byte SED(); // Set Decimal Mode: Activa el modo decimal.
    Byte SEI(); // Set Interrupt Disable: Deshabilita interrupciones.

    // ** Operaciones de Pila
    Byte PHA(); // Push Accumulator: Apila el acumulador.
    Byte PHP(); // Push Processor Status: Apila el registro de estado.
    Byte PLA(); // Pull Accumulator: Recupera el acumulador de la pila.
    Byte PLP(); // Pull Processor Status: Recupera el estado de la pila.

    // ** Transferencias
    Byte TAX(); // Transfer Accumulator to X: Copia el acumulador a X.
    Byte TAY(); // Transfer Accumulator to Y: Copia el acumulador a Y.
    Byte TXA(); // Transfer X to Accumulator: Copia X al acumulador.
    Byte TYA(); // Transfer Y to Accumulator: Copia Y al acumulador.
    Byte TSX(); // Transfer Stack Pointer to X: Copia el puntero de pila a X.
    Byte TXS(); // Transfer X to Stack Pointer: Copia X al puntero de pila.

    // ** Operaciones de Almacenamiento y Carga
    Byte LDA(); // Load Accumulator: Carga un valor en el acumulador.
    Byte LDX(); // Load X Register: Carga un valor en el registro X.
    Byte LDY(); // Load Y Register: Carga un valor en el registro Y.
    Byte STA(); // Store Accumulator: Almacena el acumulador en memoria.
    Byte STX(); // Store X Register: Almacena el registro X en memoria.
    Byte STY(); // Store Y Register: Almacena el registro Y en memoria.

    // ** Instrucciones de NOP y opcodes ilegales
    Byte NOP(); // No Operation: No realiza ninguna acción.


    // ** Operaciones de Salto Condicional
    Byte BCC(); // Branch if Carry Clear: Salta si el acarreo está limpio.
    Byte BCS(); // Branch if Carry Set: Salta si el acarreo está establecido.
    Byte BEQ(); // Branch if Equal: Salta si es igual.
    Byte BMI(); // Branch if Minus: Salta si el resultado es negativo.
    Byte BNE(); // Branch if Not Equal: Salta si no es igual.
    Byte BPL(); // Branch if Positive: Salta si el resultado es positivo.
    Byte BVC(); // Branch if Overflow Clear: Salta si no hay desbordamiento.
    Byte BVS(); // Branch if Overflow Set: Salta si hay desbordamiento.


	Byte XXX(); // Opcodes ilegales: Representa instrucciones no documentadas.

	//// ** Operaciones ilegales
	//Byte LAX(); 
	//Byte SAX(); // Stores the bitwise AND of A and X ( no affected Flags )
	//Byte DCP(); // Combo of Dec and CMP ( Affects Flags: N, Z, C )
	//Byte ISB(); // Equivalent to INC value then SBC value ( Affects Flags: N, V, Z, C )
	//Byte SLO(); // Equivalent to ASL value then ORA value ( Affects Flags: N, Z, C )
	//Byte RLA(); // Combo of ROL plus AND ( Affects Flags: V, N, Z, C )...no V?
	//Byte SRE(); // Combo of LSR and EOR ( Affects Flags: N, Z, C )
	//Byte RRA(); // Combo of ROR and ADC ( Affects Flags: V, N, Z, C )
};

