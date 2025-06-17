#include "m6502_2A03.h"
#include "Sistema.h"
#include <iostream>
#include <iomanip> // Para manejar la impresión con formato

//std::ofstream logFile("nestest.txt");
// Constructor y destructor
m6502_2A03::m6502_2A03()
{
	status.reg = 0x20;

//TABLA DE INSTRUCIONES
	using cpu = m6502_2A03;
	tabla =
	{
		{ "BRK", &cpu::BRK, &cpu::IMP, 7 },{ "ORA", &cpu::ORA, &cpu::IZX, 6 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 8 },{ "NOP", &cpu::NOP, &cpu::IMP, 3 },{ "ORA", &cpu::ORA, &cpu::ZPG, 3 },{ "ASL", &cpu::ASL, &cpu::ZPG, 5 },{ "XXX", &cpu::XXX, &cpu::IMP, 5 },{ "PHP", &cpu::PHP, &cpu::IMP, 3 },{ "ORA", &cpu::ORA, &cpu::IMM, 2 },{ "ASL", &cpu::ASL, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "NOP", &cpu::NOP, &cpu::IMP, 4 },{ "ORA", &cpu::ORA, &cpu::ABS, 4 },{ "ASL", &cpu::ASL, &cpu::ABS, 6 },{ "XXX", &cpu::XXX, &cpu::IMP, 6 },
		{ "BPL", &cpu::BPL, &cpu::REL, 2 },{ "ORA", &cpu::ORA, &cpu::IZY, 5 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 8 },{ "NOP", &cpu::NOP, &cpu::IMP, 4 },{ "ORA", &cpu::ORA, &cpu::ZPX, 4 },{ "ASL", &cpu::ASL, &cpu::ZPX, 6 },{ "XXX", &cpu::XXX, &cpu::IMP, 6 },{ "CLC", &cpu::CLC, &cpu::IMP, 2 },{ "ORA", &cpu::ORA, &cpu::ABY, 4 },{ "NOP", &cpu::NOP, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 7 },{ "NOP", &cpu::NOP, &cpu::IMP, 4 },{ "ORA", &cpu::ORA, &cpu::ABX, 4 },{ "ASL", &cpu::ASL, &cpu::ABX, 7 },{ "XXX", &cpu::XXX, &cpu::IMP, 7 },
		{ "JSR", &cpu::JSR, &cpu::ABS, 6 },{ "AND", &cpu::AND, &cpu::IZX, 6 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 8 },{ "BIT", &cpu::BIT, &cpu::ZPG, 3 },{ "AND", &cpu::AND, &cpu::ZPG, 3 },{ "ROL", &cpu::ROL, &cpu::ZPG, 5 },{ "XXX", &cpu::XXX, &cpu::IMP, 5 },{ "PLP", &cpu::PLP, &cpu::IMP, 4 },{ "AND", &cpu::AND, &cpu::IMM, 2 },{ "ROL", &cpu::ROL, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "BIT", &cpu::BIT, &cpu::ABS, 4 },{ "AND", &cpu::AND, &cpu::ABS, 4 },{ "ROL", &cpu::ROL, &cpu::ABS, 6 },{ "XXX", &cpu::XXX, &cpu::IMP, 6 },
		{ "BMI", &cpu::BMI, &cpu::REL, 2 },{ "AND", &cpu::AND, &cpu::IZY, 5 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 8 },{ "NOP", &cpu::NOP, &cpu::IMP, 4 },{ "AND", &cpu::AND, &cpu::ZPX, 4 },{ "ROL", &cpu::ROL, &cpu::ZPX, 6 },{ "XXX", &cpu::XXX, &cpu::IMP, 6 },{ "SEC", &cpu::SEC, &cpu::IMP, 2 },{ "AND", &cpu::AND, &cpu::ABY, 4 },{ "NOP", &cpu::NOP, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 7 },{ "NOP", &cpu::NOP, &cpu::IMP, 4 },{ "AND", &cpu::AND, &cpu::ABX, 4 },{ "ROL", &cpu::ROL, &cpu::ABX, 7 },{ "XXX", &cpu::XXX, &cpu::IMP, 7 },
		{ "RTI", &cpu::RTI, &cpu::IMP, 6 },{ "EOR", &cpu::EOR, &cpu::IZX, 6 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 8 },{ "NOP", &cpu::NOP, &cpu::IMP, 3 },{ "EOR", &cpu::EOR, &cpu::ZPG, 3 },{ "LSR", &cpu::LSR, &cpu::ZPG, 5 },{ "XXX", &cpu::XXX, &cpu::IMP, 5 },{ "PHA", &cpu::PHA, &cpu::IMP, 3 },{ "EOR", &cpu::EOR, &cpu::IMM, 2 },{ "LSR", &cpu::LSR, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "JMP", &cpu::JMP, &cpu::ABS, 3 },{ "EOR", &cpu::EOR, &cpu::ABS, 4 },{ "LSR", &cpu::LSR, &cpu::ABS, 6 },{ "XXX", &cpu::XXX, &cpu::IMP, 6 },
		{ "BVC", &cpu::BVC, &cpu::REL, 2 },{ "EOR", &cpu::EOR, &cpu::IZY, 5 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 8 },{ "NOP", &cpu::NOP, &cpu::IMP, 4 },{ "EOR", &cpu::EOR, &cpu::ZPX, 4 },{ "LSR", &cpu::LSR, &cpu::ZPX, 6 },{ "XXX", &cpu::XXX, &cpu::IMP, 6 },{ "CLI", &cpu::CLI, &cpu::IMP, 2 },{ "EOR", &cpu::EOR, &cpu::ABY, 4 },{ "NOP", &cpu::NOP, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 7 },{ "NOP", &cpu::NOP, &cpu::IMP, 4 },{ "EOR", &cpu::EOR, &cpu::ABX, 4 },{ "LSR", &cpu::LSR, &cpu::ABX, 7 },{ "XXX", &cpu::XXX, &cpu::IMP, 7 },
		{ "RTS", &cpu::RTS, &cpu::IMP, 6 },{ "ADC", &cpu::ADC, &cpu::IZX, 6 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 8 },{ "NOP", &cpu::NOP, &cpu::IMP, 3 },{ "ADC", &cpu::ADC, &cpu::ZPG, 3 },{ "ROR", &cpu::ROR, &cpu::ZPG, 5 },{ "XXX", &cpu::XXX, &cpu::IMP, 5 },{ "PLA", &cpu::PLA, &cpu::IMP, 4 },{ "ADC", &cpu::ADC, &cpu::IMM, 2 },{ "ROR", &cpu::ROR, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "JMP", &cpu::JMP, &cpu::IND, 5 },{ "ADC", &cpu::ADC, &cpu::ABS, 4 },{ "ROR", &cpu::ROR, &cpu::ABS, 6 },{ "XXX", &cpu::XXX, &cpu::IMP, 6 },
		{ "BVS", &cpu::BVS, &cpu::REL, 2 },{ "ADC", &cpu::ADC, &cpu::IZY, 5 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 8 },{ "NOP", &cpu::NOP, &cpu::IMP, 4 },{ "ADC", &cpu::ADC, &cpu::ZPX, 4 },{ "ROR", &cpu::ROR, &cpu::ZPX, 6 },{ "XXX", &cpu::XXX, &cpu::IMP, 6 },{ "SEI", &cpu::SEI, &cpu::IMP, 2 },{ "ADC", &cpu::ADC, &cpu::ABY, 4 },{ "NOP", &cpu::NOP, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 7 },{ "NOP", &cpu::NOP, &cpu::IMP, 4 },{ "ADC", &cpu::ADC, &cpu::ABX, 4 },{ "ROR", &cpu::ROR, &cpu::ABX, 7 },{ "XXX", &cpu::XXX, &cpu::IMP, 7 },
		{ "NOP", &cpu::NOP, &cpu::IMP, 2 },{ "STA", &cpu::STA, &cpu::IZX, 6 },{ "NOP", &cpu::NOP, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 6 },{ "STY", &cpu::STY, &cpu::ZPG, 3 },{ "STA", &cpu::STA, &cpu::ZPG, 3 },{ "STX", &cpu::STX, &cpu::ZPG, 3 },{ "XXX", &cpu::XXX, &cpu::IMP, 3 },{ "DEY", &cpu::DEY, &cpu::IMP, 2 },{ "NOP", &cpu::NOP, &cpu::IMP, 2 },{ "TXA", &cpu::TXA, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "STY", &cpu::STY, &cpu::ABS, 4 },{ "STA", &cpu::STA, &cpu::ABS, 4 },{ "STX", &cpu::STX, &cpu::ABS, 4 },{ "XXX", &cpu::XXX, &cpu::IMP, 4 },
		{ "BCC", &cpu::BCC, &cpu::REL, 2 },{ "STA", &cpu::STA, &cpu::IZY, 6 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 6 },{ "STY", &cpu::STY, &cpu::ZPX, 4 },{ "STA", &cpu::STA, &cpu::ZPX, 4 },{ "STX", &cpu::STX, &cpu::ZPY, 4 },{ "XXX", &cpu::XXX, &cpu::IMP, 4 },{ "TYA", &cpu::TYA, &cpu::IMP, 2 },{ "STA", &cpu::STA, &cpu::ABY, 5 },{ "TXS", &cpu::TXS, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 5 },{ "NOP", &cpu::NOP, &cpu::IMP, 5 },{ "STA", &cpu::STA, &cpu::ABX, 5 },{ "XXX", &cpu::XXX, &cpu::IMP, 5 },{ "XXX", &cpu::XXX, &cpu::IMP, 5 },
		{ "LDY", &cpu::LDY, &cpu::IMM, 2 },{ "LDA", &cpu::LDA, &cpu::IZX, 6 },{ "LDX", &cpu::LDX, &cpu::IMM, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 6 },{ "LDY", &cpu::LDY, &cpu::ZPG, 3 },{ "LDA", &cpu::LDA, &cpu::ZPG, 3 },{ "LDX", &cpu::LDX, &cpu::ZPG, 3 },{ "XXX", &cpu::XXX, &cpu::IMP, 3 },{ "TAY", &cpu::TAY, &cpu::IMP, 2 },{ "LDA", &cpu::LDA, &cpu::IMM, 2 },{ "TAX", &cpu::TAX, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "LDY", &cpu::LDY, &cpu::ABS, 4 },{ "LDA", &cpu::LDA, &cpu::ABS, 4 },{ "LDX", &cpu::LDX, &cpu::ABS, 4 },{ "XXX", &cpu::XXX, &cpu::IMP, 4 },
		{ "BCS", &cpu::BCS, &cpu::REL, 2 },{ "LDA", &cpu::LDA, &cpu::IZY, 5 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 5 },{ "LDY", &cpu::LDY, &cpu::ZPX, 4 },{ "LDA", &cpu::LDA, &cpu::ZPX, 4 },{ "LDX", &cpu::LDX, &cpu::ZPY, 4 },{ "XXX", &cpu::XXX, &cpu::IMP, 4 },{ "CLV", &cpu::CLV, &cpu::IMP, 2 },{ "LDA", &cpu::LDA, &cpu::ABY, 4 },{ "TSX", &cpu::TSX, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 4 },{ "LDY", &cpu::LDY, &cpu::ABX, 4 },{ "LDA", &cpu::LDA, &cpu::ABX, 4 },{ "LDX", &cpu::LDX, &cpu::ABY, 4 },{ "XXX", &cpu::XXX, &cpu::IMP, 4 },
		{ "CPY", &cpu::CPY, &cpu::IMM, 2 },{ "CMP", &cpu::CMP, &cpu::IZX, 6 },{ "NOP", &cpu::NOP, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 8 },{ "CPY", &cpu::CPY, &cpu::ZPG, 3 },{ "CMP", &cpu::CMP, &cpu::ZPG, 3 },{ "DEC", &cpu::DEC, &cpu::ZPG, 5 },{ "XXX", &cpu::XXX, &cpu::IMP, 5 },{ "INY", &cpu::INY, &cpu::IMP, 2 },{ "CMP", &cpu::CMP, &cpu::IMM, 2 },{ "DEX", &cpu::DEX, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "CPY", &cpu::CPY, &cpu::ABS, 4 },{ "CMP", &cpu::CMP, &cpu::ABS, 4 },{ "DEC", &cpu::DEC, &cpu::ABS, 6 },{ "XXX", &cpu::XXX, &cpu::IMP, 6 },
		{ "BNE", &cpu::BNE, &cpu::REL, 2 },{ "CMP", &cpu::CMP, &cpu::IZY, 5 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 8 },{ "NOP", &cpu::NOP, &cpu::IMP, 4 },{ "CMP", &cpu::CMP, &cpu::ZPX, 4 },{ "DEC", &cpu::DEC, &cpu::ZPX, 6 },{ "XXX", &cpu::XXX, &cpu::IMP, 6 },{ "CLD", &cpu::CLD, &cpu::IMP, 2 },{ "CMP", &cpu::CMP, &cpu::ABY, 4 },{ "NOP", &cpu::NOP, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 7 },{ "NOP", &cpu::NOP, &cpu::IMP, 4 },{ "CMP", &cpu::CMP, &cpu::ABX, 4 },{ "DEC", &cpu::DEC, &cpu::ABX, 7 },{ "XXX", &cpu::XXX, &cpu::IMP, 7 },
		{ "CPX", &cpu::CPX, &cpu::IMM, 2 },{ "SBC", &cpu::SBC, &cpu::IZX, 6 },{ "NOP", &cpu::NOP, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 8 },{ "CPX", &cpu::CPX, &cpu::ZPG, 3 },{ "SBC", &cpu::SBC, &cpu::ZPG, 3 },{ "INC", &cpu::INC, &cpu::ZPG, 5 },{ "XXX", &cpu::XXX, &cpu::IMP, 5 },{ "INX", &cpu::INX, &cpu::IMP, 2 },{ "SBC", &cpu::SBC, &cpu::IMM, 2 },{ "NOP", &cpu::NOP, &cpu::IMP, 2 },{ "???", &cpu::SBC, &cpu::IMP, 2 },{ "CPX", &cpu::CPX, &cpu::ABS, 4 },{ "SBC", &cpu::SBC, &cpu::ABS, 4 },{ "INC", &cpu::INC, &cpu::ABS, 6 },{ "XXX", &cpu::XXX, &cpu::IMP, 6 },
		{ "BEQ", &cpu::BEQ, &cpu::REL, 2 },{ "SBC", &cpu::SBC, &cpu::IZY, 5 },{ "XXX", &cpu::XXX, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 8 },{ "NOP", &cpu::NOP, &cpu::IMP, 4 },{ "SBC", &cpu::SBC, &cpu::ZPX, 4 },{ "INC", &cpu::INC, &cpu::ZPX, 6 },{ "XXX", &cpu::XXX, &cpu::IMP, 6 },{ "SED", &cpu::SED, &cpu::IMP, 2 },{ "SBC", &cpu::SBC, &cpu::ABY, 4 },{ "NOP", &cpu::NOP, &cpu::IMP, 2 },{ "XXX", &cpu::XXX, &cpu::IMP, 7 },{ "NOP", &cpu::NOP, &cpu::IMP, 4 },{ "SBC", &cpu::SBC, &cpu::ABX, 4 },{ "INC", &cpu::INC, &cpu::ABX, 7 },{ "XXX", &cpu::XXX, &cpu::IMP, 7 },
	};
}

m6502_2A03::~m6502_2A03()
{
}

void m6502_2A03::conectarBus(Sistema* s)
{
	busesito = s;
}

Byte m6502_2A03::read(Word dir)
{
	return busesito->cpu_Read(dir);

}

void m6502_2A03::write(Word dir, Byte data)
{
	busesito->cpu_Write(dir, data);
}


void  m6502_2A03::logInstruction(Word adress, Byte opcode) {

	logoutput << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << adress << "  ";
	logoutput << std::setfill('0') << std::right << std::setw(2) << std::hex << std::uppercase << static_cast<int>(opcode) << " ";

	/*if (tabla[opcode].modo_dir == &m6502_2A03::IMP) {
		operator1 << "  ";
		operator2 << "  ";
		operation << "";
	}
	else if (tabla[opcode].modo_dir == &m6502_2A03::IMM) {
		Byte data = busesito->cpu_Read(++adress, true);
		operator1 << std::setfill('0')<<std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
		operator2 << "  ";
		operation << "#$" << operator1.str();
	}
	else if (tabla[opcode].modo_dir == &m6502_2A03::ZPG){
		Byte data = busesito->cpu_Read(++adress, true);
		operator1 << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
		operator2 << "  ";
		Byte data2 = busesito->cpu_Read((Word)(0x00FF & data), true);
		operation << "$" << operator1.str()<<" = "<< std::setfill('0') <<std::setw(2) << std::hex << std::uppercase << static_cast<int>(data2);
	}
	else if (tabla[opcode].modo_dir == &m6502_2A03::ZPX) {
		Byte data = busesito->cpu_Read(++adress, true);
		operator1 << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << static_cast<int>(data);
		operator2 << "  ";
		adress = (0x00FF & (Word)(data + rx));
		Byte data2 = busesito->cpu_Read(adress, true);
		operation << "$" << operator1.str() << ",X @ " << std::setw(2) << std::hex << std::uppercase << static_cast<int>((Byte)adress) << " = " << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data2);
	}
	else if (tabla[opcode].modo_dir == &m6502_2A03::ZPY) {
		Byte data = busesito->cpu_Read(++adress, true);
		operator1 << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
		operator2 << "  ";
		adress = (0x00FF & (Word)(data + ry));
		Byte data2 = busesito->cpu_Read(adress, true);
		operation << "$" << operator1.str() << ",Y @ " << std::setw(2) << std::hex << std::uppercase << static_cast<int>((Byte)adress) << " = " << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data2);
	}
	else if (tabla[opcode].modo_dir == &m6502_2A03::REL) {
		Byte data = busesito->cpu_Read(++adress, true);
		operator1 << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
		operator2 << "  ";
		operation << "$" << std::hex << std::uppercase << static_cast<int>((++adress + data));
	}
	else if (tabla[opcode].modo_dir == &m6502_2A03::ABS) {
		Byte data = busesito->cpu_Read(adress + 1, true);
		Byte data2 = busesito->cpu_Read(adress + 2, true);
		operator1 << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
		operator2 << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>((data2));
		Word absolute = (Word)(data2 << 8) | (Word)data;
		if (opcode == 0x4c) {
			operation << "$" << std::hex << std::uppercase << absolute;
		}
		else {
			Byte data = busesito->cpu_Read(absolute, true);
			operation << "$" << std::hex << std::uppercase << absolute << " = " << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
			operation << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);

		}
	}
	else if (tabla[opcode].modo_dir == &m6502_2A03::ABX) {
		Byte data = busesito->cpu_Read(adress + 1, true);
		Byte data2 = busesito->cpu_Read(adress + 2, true);
		operator1 << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
		operator2 << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data2);
		Word absolute = ((Word)(data2 << 8) | (Word)data) + rx;
		Byte data = busesito->cpu_Read(absolute, true);
		operation << "$" << std::hex << std::uppercase << absolute << " = " << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
		operation << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
	}
	else if (tabla[opcode].modo_dir == &m6502_2A03::ABY) {
		Byte data = busesito->cpu_Read(adress + 1, true);
		Byte data2 = busesito->cpu_Read(adress + 2, true);
		operator1 << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
		operator2 << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data2);
		Word absolute = ((Word)(data2 << 8) | (Word)data) + ry;
		Byte data = busesito->cpu_Read(absolute, true);
		operation << "$" << std::hex << std::uppercase << absolute << " = " << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
		operation << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
	}
	else if (tabla[opcode].modo_dir == &m6502_2A03::IND) {

	}
	else if (tabla[opcode].modo_dir == &m6502_2A03::IZX) {
		Byte data = busesito->cpu_Read(++adress, true);
		operator1 << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
		operator2 << "  ";
		Byte data2 = busesito->cpu_Read(addr_abs, true);
		operation << "($" << operator1.str() << ",X) @ " << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data + rx) << " = " << std::setw(4) << std::hex << std::uppercase << addr_abs << " = " << std::setw(2) << std::hex << std::uppercase<< static_cast<int>(data2);
	}
	else if (tabla[opcode].modo_dir == &m6502_2A03::IZY) {
		Byte data = busesito->cpu_Read(++adress, true);
		operator1 << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
		operator2 << "  ";
		Byte data2 = busesito->cpu_Read(addr_abs, true);
		operation << "($" << operator1.str() << ",Y) @ " << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data + ry) << " = " << std::setw(4) << std::hex << std::uppercase << addr_abs << " = " << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data2);
	}*/
	logoutput << operator1.str() << " ";
	logoutput << operator2.str() << "  ";
	logoutput << std::setw(3) << std::left << tabla[opcode].name << " ";
	logoutput << std::setw(28) << std::setfill(' ') << std::left << operation.str();
	logoutput << "A:" << std::setfill('0') << std::right << std::setw(2) << std::hex << std::uppercase << static_cast<int>(ra) << " ";
	logoutput << "X:" << std::setfill('0') << std::right << std::setw(2) << std::hex << std::uppercase << static_cast<int> (rx) << " ";
	logoutput << "Y:" << std::setfill('0') << std::right << std::setw(2) << std::hex << std::uppercase << static_cast<int>(ry) << " ";
	logoutput << "P:" << std::setfill('0') << std::right << std::setw(2) << std::hex << std::uppercase << static_cast<int>(status.reg) << " ";
	logoutput << "SP:" << std::setfill('0') << std::right << std::setw(2) << std::hex << std::uppercase << static_cast<int>(sp) << " ";
	logoutput << "CYC:" << std::dec << clock_count;

}

void m6502_2A03::clock()
{

	if (cycles == 0)
	{
		if (NMIcall) {
			NMIcall = false;
			NMI();
			//print_cpu_status("NMI");
		}
		else if (IRQcall) {
			IRQcall = false;
			IRQ();
			//print_cpu_status("IRQ");
		}

		else {
			opcode = read(pc);
			ppc = pc;
			pc++;
			status.flag.U = 1;

			cycles = this->tabla[opcode].cycles;
			Byte ciclosAdicionales1 = (this->*tabla[opcode].modo_dir)();

			if (logmode && clock_count > 0) {
				/*if (!logFile.is_open()) {
					std::cerr << "Error al abrir el archivo de log" << std::endl;
				}*/
				//logInstruction(ppc, opcode);
				//cout << logoutput.str() << std::endl;
				/*logFile << logoutput.str() << std::endl;*/
				print_cpu_status(tabla[opcode].name);
				/*logFile.flush();*/
				/*if (clock_count > 107025) {
					logFile.close();
				};*/

				operation.str("");
				operator1.str("");
				operator2.str("");
				logoutput.str("");
			}

			Byte ciclosAdicionales2 = (this->*tabla[opcode].op)();
			cycles += (ciclosAdicionales1 & ciclosAdicionales2);
			status.flag.U = 1;
			//print_cpu_status(this->tabla[opcode].name);


		}
	}

	else if ((cycles > 0) && IRQcall && status.flag.I == true) {
		IRQcall = false;
	};

	cycles--;
}

void m6502_2A03::print_cpu_status(std::string name) {
	//logInstruction(ppc, opcode, addr_abs)
	// Imprimir los registros principales
	std::cout << "CPU State:\n";
	std::cout << "-------------------------\n";
	std::cout << " Operacion:      " << name << "\n";
	// Imprimir registros A, X, Y, SP y PC
	std::cout << "Registers:\n";
	std::cout << "  RA: 0x" << std::hex << std::setw(2) << std::setfill('0') << +ra << "\n";
	std::cout << "  RX: 0x" << std::hex << std::setw(2) << std::setfill('0') << +rx << "\n";
	std::cout << "  RY: 0x" << std::hex << std::setw(2) << std::setfill('0') << +ry << "\n";
	std::cout << "  SP: 0x" << std::hex << std::setw(2) << std::setfill('0') << +sp << "\n";
	std::cout << "  PC: 0x" << std::hex << std::setw(4) << std::setfill('0') << pc << "\n";
	std::cout << "  PPC: 0x" << std::hex << std::setw(4) << std::setfill('0') << ppc << "\n";

	// Imprimir el estado de los flags
	std::cout << "\nFlags:\n";
	std::cout << "  Carry (C):      " << +status.flag.C << "\n";
	std::cout << "  Zero (Z):       " << +status.flag.Z << "\n";
	std::cout << "  Interrupt (I):  " << +status.flag.I << "\n";
	std::cout << "  Decimal (D):    " << +status.flag.D << "\n";
	std::cout << "  Break (B):      " << +status.flag.B << "\n";
	std::cout << "  Unused (U):     " << +status.flag.U << "\n";
	std::cout << "  Overflow (V):   " << +status.flag.V << "\n";
	std::cout << "  Negative (N):   " << +status.flag.N << "\n";

	// Imprimir otros datos de interés
	std::cout << "\nAdditional Info:\n";
	std::cout << "  Fetched:        0x" << std::hex << std::setw(2) << std::setfill('0') << +fetched << "\n";
	std::cout << "  Address Abs:    0x" << std::hex << std::setw(4) << std::setfill('0') << addr_abs << "\n";
	std::cout << "  Address Rel:    0x" << std::hex << std::setw(4) << std::setfill('0') << addr_rel << "\n";
	std::cout << "  Opcode:         0x" << std::hex << std::setw(2) << std::setfill('0') << +opcode << "\n";
	std::cout << "  Cycles:         " << std::dec << +cycles << "\n";
	std::cout << "  Clock Count:    " << std::dec << clock_count << "\n";


	printf("Stack contents (from SP = 0x%02X):\n", sp);
	printf("| ");

	int count = 0;
	for (int i = 0x01FF; i > 0x0100 + sp; i--) {
		Byte value = busesito->cpu_Read(i);
		printf("0x%02X | ", value);

		count++;
		if (count % 8 == 0) {
			printf("\n| ");  // Saltar línea después de 8 valores
		}
	}
	printf("\n");
	std::cout << "-------------------------\n";
}



//// Imprimir la dirección (address)
//std::cout << std::hex << std::uppercase<< std::setw(4) << std::setfill('0') << address << "  ";

//// Imprimir el opcode
//std::cout << std::hex << std::uppercase << std::setw(2) << opcode << " ";

//// Imprimir los bytes bajo y alto de la dirección
//std::cout << std::hex << std::uppercase << std::setw(2) << lowByte << " " << std::setw(2) << highByte << "  ";

//// Imprimir la instrucción y la dirección completa (para una instrucción como JMP)
//std::cout << name << std::setw(4) << "                       ";

//// Imprimir los registros
//std::cout << std::hex << std::uppercase << "A:" << std::setw(2) << std::setfill('0') << A << " ";
//std::cout << std::hex << std::uppercase << "X:" << std::setw(2) << std::setfill('0') << X << " ";
//std::cout << std::hex << std::uppercase << "Y:" << std::setw(2) << std::setfill('0') << Y << " ";
//std::cout << std::hex << std::uppercase << "SP:" << std::setw(2) << std::setfill('0') << SP << " ";
//std::cout << std::hex << std::uppercase << "CYC:" << std::dec << CYC; // CYC en decimal

//std::cout << std::endl;



bool m6502_2A03::instructionCompleted()
{
	return cycles == 0;
}

///////////////////////////////////////////////////////////////////////////////
// ADDRESSING MODES

// Implied: El operando está implícito en la instrucción.
Byte m6502_2A03::IMP()
{
	fetched = ra;

	if (logmode) {
		operator1 << "  ";
		operator2 << "  ";
		operation << "";
	}

	return 0;

}


// Immediate: El operando es un valor inmediato.
Byte m6502_2A03::IMM()
{
	addr_abs = pc++;

	if (logmode) {
		Byte data = busesito->cpu_Read(addr_abs, true);
		operator1 << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
		operator2 << "  ";
		operation << "#$" << operator1.str();
	}

	return 0;


}





// Zero Page: Dirección de 8 bits dentro de la página cero.
Byte m6502_2A03::ZPG()
{
	addr_abs = read(pc);
	pc++;
	addr_abs &= 0x00FF;


	if (logmode) {
		operator1 << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>((Byte)addr_abs);
		operator2 << "  ";
		Byte data2 = busesito->cpu_Read(addr_abs, true);
		operation << "$" << operator1.str() << " = " << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data2);
	}

	return 0;


}



// // Zero Page, X: Dirección de 8 bits desplazada por el registro X.
Byte m6502_2A03::ZPX()
{
	Byte data = read(pc);
	addr_abs = (data + rx);
	pc++;
	addr_abs &= 0x00FF;
	if (logmode) {
		operator1 << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
		operator2 << "  ";
		Byte data2 = busesito->cpu_Read(addr_abs, true);
		operation << "$" << operator1.str() << ",X @ " << std::setw(2) << std::hex << std::uppercase << static_cast<int>((Byte)addr_abs) << " = " << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data2);
	}
	return 0;

}


// Zero Page, Y: Dirección de 8 bits desplazada por el registro Y.
Byte m6502_2A03::ZPY()
{
	Byte data = read(pc);
	addr_abs = (data + ry);
	pc++;
	addr_abs &= 0x00FF;
	if (logmode) {
		operator1 << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
		operator2 << "  ";
		Byte data2 = busesito->cpu_Read(addr_abs, true);
		operation << "$" << operator1.str() << ",Y @ " << std::setw(2) << std::hex << std::uppercase << static_cast<int>((Byte)addr_abs) << " = " << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data2);
	}
	return 0;

}


// Relative se provee un offset con signo que sera agregado al Pc para obtener efectivamente la direccion
// el offset debe estar comprendido en el rango -128 a 127.
Byte m6502_2A03::REL()
{
	addr_rel = read(pc);
	pc++;
	if (addr_rel & 0x80)
		addr_rel |= 0xFF00;
	if (logmode) {
		operator1 << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>((Byte)addr_rel);
		operator2 << "  ";
		operation << "$" << std::hex << std::uppercase << static_cast<int>((pc + addr_rel));
	}
	return 0;
}


// Absolute: Dirección completa de 16 bits.
Byte m6502_2A03::ABS()
{
	Word low = read(pc);
	pc++;
	Word high = read(pc);
	pc++;

	addr_abs = (high << 8) | low;

	if (logmode) {

		operator1 << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>((Byte)low);
		operator2 << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(((Byte)high));
		if (opcode == 0x4c || opcode == 0x20) {
			operation << "$" << std::setfill('0') << std::right << std::setw(4) << std::hex << std::uppercase << addr_abs;
		}
		else {
			Byte data = busesito->cpu_Read(addr_abs, true);
			operation << "$" << std::setfill('0') << std::right << std::setw(4) << std::hex << std::uppercase << addr_abs << " = " << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
		}
	}
	return 0;
}



// Absolute, X: Dirección absoluta desplazada por el registro X.
// Si se cambia de pagina requiere un ciclo más.
Byte m6502_2A03::ABX()
{
	Word low = read(pc);
	pc++;
	Word high = read(pc);
	pc++;

	Word temp = (high << 8) | low;
	addr_abs = temp + rx;

	if (logmode) {

		operator1 << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>((Byte)low);
		operator2 << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(((Byte)high));
		Byte data = busesito->cpu_Read(addr_abs, true);
		operation << "$" << std::setfill('0') << std::right << std::setw(4) << std::hex << std::uppercase << temp << ",X @ " << std::setfill('0') << std::right << std::setw(4) << std::hex << std::uppercase << addr_abs << " = " << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
	}

	if ((addr_abs & 0xFF00) != (high << 8))
		return 1;
	else
		return 0;
}


// Absolute, Y: Dirección absoluta desplazada por el registro Y.
// Si se cambia de pagina requiere un ciclo más.
Byte m6502_2A03::ABY()
{
	Word low = read(pc);
	pc++;
	Word high = read(pc);
	pc++;

	addr_abs = (high << 8) | low;
	Word temp = addr_abs;
	addr_abs += ry;

	if (logmode) {

		operator1 << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>((Byte)low);
		operator2 << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>((Byte)high);
		Byte data = busesito->cpu_Read(addr_abs, true);
		operation << "$" << std::setfill('0') << std::setw(4) << std::hex << std::uppercase << temp << ",Y @ " << std::setfill('0') << std::setw(4) << std::hex << std::uppercase << addr_abs << " = " << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
	}

	if ((addr_abs & 0xFF00) != (high << 8))
		return 1;
	else
		return 0;
}

// Indirect
// Los 16 bits de direccion proporcionados sirven para obtener la direccion del dato al que hacer fetch (como un puntero)
// Este modo de direccionamiento tiene un bug en el hardware, si el low byte es 0xFF no cambiara de pagina sino que 
// se mantiene en la misma pagina dandose vuelta volviendo al 0x00 y devolviendo una direccion invalida.
Byte m6502_2A03::IND()
{
	Word low = read(pc);
	pc++;
	Word high = read(pc);
	pc++;

	Word ptr = (high << 8) | low;

	if (low == 0x00FF) // Hardware bug
	{
		addr_abs = (read(ptr & 0xFF00) << 8) | read(ptr + 0);
	}
	else 
	{
		addr_abs = (read(ptr + 1) << 8) | read(ptr + 0);
	}

	if (logmode) {

		operator1 << std::setw(2) << std::hex << std::uppercase << static_cast<int>((Byte)low);
		operator2 << std::setw(2) << std::hex << std::uppercase << static_cast<int>((Byte)high);

		operation << "($" << operator2.str() << operator1.str() << ") = " << std::setw(4) << std::hex << std::uppercase << addr_abs;
	}


	return 0;
}


// Indirect X: Se suma el registro x a la direccion de 8 bits provista y este sera el offset para indexar en la pagina zero y obtener el la direccion de 16 bits que buscamos.

Byte m6502_2A03::IZX()
{
	Word t = read(pc);
	pc++;

	Word low = read((Word)(t + (Word)rx) & 0x00FF); // habra que castear?
	Word high = read((Word)(t + (Word)rx + 1) & 0x00FF);

	addr_abs = (high << 8) | low;
	if (logmode) {

		operator1 << std::setw(2) << std::hex << std::uppercase << static_cast<int>((Byte)t);
		operator2 << "  ";
		Byte data = (busesito->cpu_Read(addr_abs, true));
		operation << "($" << operator1.str() << ",X) @ " << std::setw(2) << std::hex << std::uppercase << static_cast<int>((Byte)(t + rx)) << " = " << std::setw(4) << std::hex << std::uppercase << addr_abs << " = " << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
	}
	return 0;
}


// Indirect Y: La direccion de 8 bits provista es un offset en la pagina zero,  leyendo esa celda y la siguiente se obtiene la direccion efectiva de 16bits 
// a esta direccion se le sumara el valor del registro y, si esto causa un cambio de pagina se agrega un ciclo.
Byte m6502_2A03::IZY()
{
	Word t = read(pc);
	pc++;

	Word low = read(t & 0x00FF);
	Word high = read((t + 1) & 0x00FF);

	Word temp = (high << 8) | low;
	addr_abs = temp + ry;

	if (logmode) {

		operator1 << std::setw(2) << std::hex << std::uppercase << static_cast<int>((Byte)t);
		operator2 << "  ";
		Byte data = (busesito->cpu_Read(addr_abs, true));
		operation << "($" << operator1.str() << "),Y = " << std::setw(4) << std::hex << std::uppercase << temp << " @ " << std::setw(4) << std::hex << std::uppercase << addr_abs << " = " << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(data);
	}

	if ((addr_abs & 0xFF00) != (high << 8))
		return 1;
	else
		return 0;
}




/////////////////////////////////////////////////////////////////////////////// INSTRUCCIONES //////////////////////////////////////////////////////////////////////////////////

 // ** Operaciones Aritméticas y Lógicas


// ADC - Add with Carry
// A = A + memory + C
// ADC adds the carry flag and a memory value to the accumulator.
Byte m6502_2A03::ADC()
{
	
	fetch();

	Word temp = (Word)ra + (Word)fetched + (Word)status.flag.C;

	status.flag.N = (temp & 0x80) ? 1 : 0;
	status.flag.Z = ((temp & 0x00FF) == 0);
	status.flag.C = (temp > 255);
	status.flag.V = ((~((Word)ra ^ (Word)fetched) & ((Word)ra ^ (Word)temp)) & 0x0080) ? 1 : 0;
	
	ra = temp & 0x00FF;
	return 1;

}

// ADC - Add with Carry
// A = A + memory + C
// ADC adds the carry flag and a memory value to the accumulator.
Byte m6502_2A03::SBC()
{
	fetch();

	Word complementado = ((Word)fetched) ^ 0x00FF; //  Paso a complemento a 2
	Word temp = (Word)ra + complementado + (Word)status.flag.C;
	status.flag.N = (temp & 0x0080) ? 1 : 0;
	status.flag.Z = ((temp & 0x00FF) == 0);
	status.flag.C = (temp & 0xFF00) ? 1 : 0;
	status.flag.V = ((temp ^ (Word)ra) & (temp ^ complementado) & 0x0080) ? 1 : 0;

	ra = temp & 0x00FF;
	return 1;
}


// AND - Bitwise AND
// A = A & memory
// This ANDs a memory value and the accumulator.
Byte m6502_2A03::AND()
{
	fetch();
	ra = ra & fetched;
	status.flag.N = (ra & 0x80) ? 1 : 0;
	status.flag.Z = (ra == 0x00);
	return 1;
}

//ORA - Bitwise OR
//A = A | memory
//ORA inclusive - ORs a memory value and the accumulator.
Byte m6502_2A03::ORA()
{
	fetch();
	ra = ra | fetched;
	status.flag.Z = (ra == 0x00);
	status.flag.N = (ra & 0x80) ? 1 : 0;
	return 1;
}

// EOR - Bitwise Exclusive OR
// A = A ^ memory
// EOR exclusive - ORs a memory value and the accumulator,
Byte m6502_2A03::EOR()
{
	fetch();
	ra = ra ^ fetched;
	status.flag.Z = (ra == 0x00);
	status.flag.N = (ra & 0x80 ? 1 : 0) ? 1 : 0;
	return 1;
}

// ASL - Arithmetic Shift Left
// value = value << 1, or visually: C < -[76543210] < -0
// ASL shifts all of the bits of a memory value or the accumulator one position to the left
Byte m6502_2A03::ASL()
{
	fetch();
	Word temp = (Word)fetched << 1;
	status.flag.N = (temp & 0x80) ? 1 : 0;
	status.flag.Z = (temp & 0x00FF) == 0x00;
	status.flag.C = (temp & 0xFF00) > 0;
	if (tabla[opcode].modo_dir == &m6502_2A03::IMP)
		ra = temp & 0x00FF;
	else
		write(addr_abs, temp & 0x00FF);

	return 0;
}

// LSR - Logical Shift Right
// value = value >> 1, or visually: 0 ->[76543210]->C
// LSR shifts all of the bits of a memory value or the accumulator one position to the right.

Byte m6502_2A03::LSR()
{
	fetch();
	status.flag.C = (fetched & 0x0001) ? 1 : 0;
	Word temp = fetched >> 1;
	status.flag.Z = ((temp & 0x00FF) == 0x0000);
	status.flag.N = (temp & 0x0080) ? 1 : 0;
	if (tabla[opcode].modo_dir == &m6502_2A03::IMP)
		ra = temp & 0x00FF;
	else
		write(addr_abs, temp & 0x00FF);
	return 0;
}

// ROL - Rotate Left
// value = value << 1 through C, or visually: C < -[76543210] < -C
// ROL shifts a memory value or the accumulator to the left,
Byte m6502_2A03::ROL()
{
	fetch();
	Word temp = ((Word)(fetched << 1) | status.flag.C);
	status.flag.C = (temp & 0xFF00) ? 1 : 0;
	status.flag.Z = ((temp & 0x00FF) == 0x0000);
	status.flag.N = (temp & 0x0080) ? 1 : 0;
	if (tabla[opcode].modo_dir == &m6502_2A03::IMP)
		ra = temp & 0x00FF;
	else
		write(addr_abs, temp & 0x00FF);
	return 0;
}

// OR - Rotate Right
// value = value >> 1 through C, or visually: C ->[76543210]->C
// ROR shifts a memory value or the accumulator to the right,
Byte m6502_2A03::ROR()
{
	fetch();
	Word temp = (Word)(status.flag.C << 7) | (fetched >> 1);
	status.flag.C = (fetched & 0x01) ? 1 : 0;
	status.flag.Z = ((temp & 0x00FF) == 0x00);
	status.flag.N = (temp & 0x0080) ? 1 : 0;
	if (tabla[opcode].modo_dir == &m6502_2A03::IMP)
		ra = temp & 0x00FF;
	else
		write(addr_abs, temp & 0x00FF);
	return 0;
}


// ** Operaciones de Comparación

// CMP - Compare A
// A - memory
// CMP compares A to a memory value, setting flags as appropriate but not modifying any registers.
Byte m6502_2A03::CMP()
{
	fetch();
	Word temp = (Word)ra - (Word)fetched;
	status.flag.C = (ra >= fetched);
	status.flag.Z = ((temp & 0x00FF) == 0x0000);
	status.flag.N = (temp & 0x0080) ? 1 : 0;
	return 1;
}

// CPX - Compare X
// X - memory
// CPX compares X to a memory value, setting flags as appropriate but not modifying any registers.
Byte m6502_2A03::CPX()
{
	fetch();
	Word temp = (Word)rx - (Word)fetched;
	status.flag.C = (rx >= fetched);
	status.flag.Z = ((temp & 0x00FF) == 0x0000);
	status.flag.N = (temp & 0x0080) ? 1 : 0;
	return 0;
}

// CPY - Compare Y
// Y - memory
// CPY compares Y to a memory value, setting flags as appropriate but not modifying any registers.
Byte m6502_2A03::CPY()
{
	fetch();
	Word temp = (Word)ry - (Word)fetched;
	status.flag.C = (ry >= fetched);
	status.flag.Z = ((temp & 0x00FF) == 0x0000);
	status.flag.N = (temp & 0x0080) ? 1 : 0;
	return 0;
}

// ** Operaciones de Salto

// JMP - Jump
// PC = memory
// JMP sets the program counter to a new value, allowing code to execute from a new location.If you wish to be able to return from that location, JSR should normally be used, instead.
Byte m6502_2A03::JMP()
{
	pc = addr_abs;
	return 0;
}

// JSR - Jump to Subroutine
// push PC + 2 to stack
// PC = memory
// JSR pushes the current program counter to the stack and then sets the program counter to a new value.This allows code to call a function and return with RTS back to the instruction after the JSR.
Byte m6502_2A03::JSR()
{
	pc--;

	write(0x0100 + sp, (pc >> 8) & 0x00FF);
	sp--;
	write(0x0100 + sp, pc & 0x00FF);
	sp--;

	pc = addr_abs;
	return 0;
}



Byte m6502_2A03::RTS()
{
	sp++;
	pc = (Word)read(0x0100 + sp);
	sp++;
	pc |= (Word)read(0x0100 + sp) << 8;

	pc++;
	return 0;
}

//BRK - Break(software IRQ)
//push PC + 2 to stack
//push NV11DIZC flags to stack
//PC = ($FFFE)
//
//BRK triggers an interrupt request(IRQ).IRQs are normally triggered by external hardware, and BRK is the only way to do it in software.Like a typical IRQ, 
//it pushes the current program counterand processor flags to the stack, sets the interrupt disable flag, and jumps to the IRQ handler.

Byte m6502_2A03::BRK()
{
	pc++;
	write(0x0100 + sp, (pc >> 8) & 0x00FF);
	sp--;
	write(0x0100 + sp, pc & 0x00FF);
	sp--;

	status.flag.B = 1;
	write(0x0100 + sp, status.reg);
	status.flag.I = 1;
	sp--;
	status.flag.B = 0;

	pc = (Word)read(0xFFFE) | ((Word)read(0xFFFF) << 8);
	return 0;
}

Byte m6502_2A03::RTI()
{
	sp++;
	status.reg = read(0x0100 + sp);
	status.flag.B = 0;
	status.flag.U = 0;

	sp++;
	pc = (Word)read(0x0100 + sp);
	sp++;
	pc |= (Word)read(0x0100 + sp) << 8;
	return 0;
}

// ** Operaciones de Bit Test

//BIT - Bit Test
//A & memory
//BIT modifies flags, but does not change memory or registers.
Byte m6502_2A03::BIT()
{
	fetch();
	Word temp = ra & fetched;
	status.flag.Z = (temp & 0x00FF) == 0x00;
	status.flag.N = (fetched & (1 << 7)) ? 1 : 0;
	status.flag.V = (fetched & (1 << 6)) ? 1 : 0;
	return 0;
}


// ** Operaciones de Incremento/Decremento

// Instruction: Increment Value at Memory Location
// Function:    M = M + 1
// Flags Out:   N, Z
Byte m6502_2A03::INC()
{
	fetch();
	Word temp = fetched + 1;
	write(addr_abs, temp & 0x00FF);
	status.flag.Z = ((temp & 0x00FF) == 0x0000);
	status.flag.N = (temp & 0x0080) ? 1 : 0;
	return 0;
}


// Instruction: Increment X Register
// Function:    X = X + 1
// Flags Out:   N, Z
Byte m6502_2A03::INX()
{
	rx++;
	status.flag.Z = (rx == 0x00);
	status.flag.N = (rx & 0x80) ? 1 : 0;
	return 0;
}

// Instruction: Increment Y Register
// Function:    Y = Y + 1
// Flags Out:   N, Z
Byte m6502_2A03::INY()
{
	ry++;
	status.flag.Z = (ry == 0x00);
	status.flag.N = (ry & 0x80) ? 1 : 0;
	return 0;
}

// Instruction: Decrement Value at Memory Location
// Function:    M = M - 1
// Flags Out:   N, Z
Byte m6502_2A03::DEC()
{
	fetch();
	Word temp = fetched - 1;
	write(addr_abs, temp & 0x00FF);
	status.flag.Z = ((temp & 0x00FF) == 0x0000);
	status.flag.N = (temp & 0x0080) ? 1 : 0;
	return 0;
}


// Instruction: Decrement X Register
// Function:    X = X - 1
// Flags Out:   N, Z
Byte m6502_2A03::DEX()
{
	rx--;
	status.flag.Z = (rx == 0x00);
	status.flag.N = (rx & 0x80) ? 1 : 0;
	return 0;
}


// Instruction: Decrement Y Register
// Function:    Y = Y - 1
// Flags Out:   N, Z
Byte m6502_2A03::DEY()
{
	ry--;
	status.flag.Z = (ry == 0x00);
	status.flag.N = (ry & 0x80) ? 1 : 0;
	return 0;
}


// ** Operaciones de Estado

// CLC - Clear Carry
// C = 0
// CLC clears the carry flag.
Byte m6502_2A03::CLC()
{
	status.flag.C = 0;
	return 0;
}

// CLD - Clear Decimal
// D = 0
// CLD clears the decimal flag.
Byte m6502_2A03::CLD()
{
	status.flag.D = 0;
	return 0;
}

//CLI - Clear Interrupt Disable
//I = 0
//CLI clears the interrupt disable flag, enabling the CPU to handle hardware IRQs.
Byte m6502_2A03::CLI()
{
	status.flag.I = 0;
	return 0;
}


//CLV - Clear Overflow
//V = 0
//CLV clears the overflow flag.
Byte m6502_2A03::CLV()
{
	status.flag.V = 0;
	return 0;
}

//SEC - Set Carry
//C = 1
//SEC sets the carry flag.
Byte m6502_2A03::SEC()
{
	status.flag.C = 1;
	return 0;
}

//SED - Set Decimal
//D = 1
//SED sets the decimal flag.

Byte m6502_2A03::SED()
{
	status.flag.D = 1;
	return 0;
}

//SEI - Set Interrupt Disable
//I = 1
//SEI sets the interrupt disable flag, preventing the CPU from handling hardware IRQs.

Byte m6502_2A03::SEI()
{
	status.flag.I = 1;
	return 0;
}


// ** Operaciones de Pila

//PHA - Push A
//($0100 + SP) = A
//SP = SP - 1
//PHA stores the value of A to the current stack position and then decrements the stack pointer.

Byte m6502_2A03::PHA()
{
	write(0x0100 + sp, ra);
	sp--;
	return 0;
}

//PHP - Push Processor Status
//($0100 + SP) = NV11DIZC
//SP = SP - 1
//PHP stores a byte to the stack containing the 6 status flags and B flag and then decrements the stack pointer.The B flag and extra bit are both pushed as 1. The bit order is NV1BDIZC(high to low).

Byte m6502_2A03::PHP()
{
	status.flag.B = 1;
	write(0x0100 + sp, status.reg);
	status.flag.B = 0;
	status.flag.U = 0;
	sp--;
	return 0;
}

//PLA - Pull A
//SP = SP + 1
//A = ($0100 + SP)
//PLA increments the stack pointer and then loads the value at that stack position into A.
Byte m6502_2A03::PLA()
{
	sp++;
	ra = read(0x0100 + sp);
	status.flag.Z = (ra == 0x00);
	status.flag.N = (ra & 0x80) ? 1 : 0;
	return 0;
}

//PLP - Pull Processor Status
//SP = SP + 1
//NVxxDIZC = ($0100 + SP)
//PLP increments the stack pointer and then loads the value at that stack position into the 6 status flags.The bit order is NVxxDIZC(high to low).The B flag and extra bit are ignored.
Byte m6502_2A03::PLP()
{
	sp++;
	status.reg = read(0x0100 + sp);
	status.flag.U = 1;
	status.flag.B = 0;
	return 0;
}

// ** Transferencias


//TAX - Transfer A to X
//X = A
//TAX copies the accumulator value to the X register.
Byte m6502_2A03::TAX()
{
	rx = ra;
	status.flag.Z = (rx == 0x00);
	status.flag.N = (rx & 0x80) ? 1 : 0;
	return 0;
}

//TAX - Transfer A to X
//X = A
//TAX copies the accumulator value to the X register.
Byte m6502_2A03::TAY()
{
	ry = ra;
	status.flag.Z = (ry == 0x00);
	status.flag.N = (ry & 0x80) ? 1 : 0;
	return 0;
}

//TAX - Transfer A to X
//X = A
//TAX copies the accumulator value to the X register.
Byte m6502_2A03::TXA()
{
	ra = rx;
	status.flag.Z = (ra == 0x00);
	status.flag.N = (ra & 0x80) ? 1 : 0;
	return 0;
}

//TYA - Transfer Y to A
//A = Y
//TYA copies the Y register value to the accumulator.
Byte m6502_2A03::TYA()
{
	ra = ry;
	status.flag.Z = (ra == 0x00);
	status.flag.N = (ra & 0x80) ? 1 : 0;
	return 0;
}

//TSX - Transfer Stack Pointer to X
//X = SP
//TSX copies the stack pointer value to the X register.
Byte m6502_2A03::TSX()
{
	rx = sp;
	status.flag.Z = (rx == 0x00);
	status.flag.N = (rx & 0x80) ? 1 : 0;
	return 0;
}

//TXS - Transfer X to Stack Pointer
//SP = X
//TXS copies the X register value to the stack pointer.
Byte m6502_2A03::TXS()
{
	sp = rx;
	return 0;
}



// ** Operaciones de Almacenamiento y Carga

//LDA - Load A
//A = memory
//
//LDA loads a memory value into the accumulator.

Byte m6502_2A03::LDA()
{
	fetch();
	ra = fetched;
	status.flag.Z = (ra == 0x00);
	status.flag.N = (ra & 0x80) ? 1 : 0;
	return 1;
}

//LDX - Load X
//X = memory
//
//LDX loads a memory value into the X register.
Byte m6502_2A03::LDX()
{
	fetch();
	rx = fetched;
	status.flag.Z = (rx == 0x00);
	status.flag.N = (rx & 0x80) ? 1 : 0;
	return 1;
}

//LDY - Load Y
//Y = memory
//
//LDX loads a memory value into the Y register.

Byte m6502_2A03::LDY()
{
	fetch();
	ry = fetched;
	status.flag.Z = (ry == 0x00);
	status.flag.N = (ry & 0x80) ? 1 : 0;
	return 1;
}


//STA - Store A
//memory = A
//STA stores the accumulator value into memory.
Byte m6502_2A03::STA()
{
	write(addr_abs, ra);
	return 0;
}

//STX - Store X
//memory = X
//STX stores the X register value into memory.
Byte m6502_2A03::STX()
{
	write(addr_abs, rx);
	return 0;
}

//STY - Store Y
//memory = Y
//STY stores the Y register value into memory
Byte m6502_2A03::STY()
{
	write(addr_abs, ry);
	return 0;
}


//NOP - No Operation
//NOP has no effect; it merely wastes spaceand CPU cycles.
Byte m6502_2A03::NOP()
{
	
	if (opcode == 0xFC){
		return 1;
	}
	return 0;
}

// ** Operaciones de Salto Condicional


//BCC - Branch if Carry Clear
//PC = PC + 2 + memory(signed)
//If the carry flag is clear, BCC branches to a nearby location by adding the relative offset to the program counter.The offset is signedand has a range of[-128, 127] relative to the first byte after the branch instruction.
Byte m6502_2A03::BCC()
{
	if (status.flag.C == 0)
	{
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}

//BCS - Branch if Carry Set
//PC = PC + 2 + memory(signed)
//
//If the carry flag is set, BCS branches to a nearby location by adding the branch offset to the program counter.The offset is signedand has a range of[-128, 127] relative to the first byte after the branch instruction.

Byte m6502_2A03::BCS()
{
	if (status.flag.C == 1)
	{
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}

//BEQ - Branch if Equal
//PC = PC + 2 + memory(signed)
//If the zero flag is set, BEQ branches to a nearby location by adding the branch offset to the program counter.The offset is signedand has a range of[-128, 127] relative to the first byte after the branch instruction.
Byte m6502_2A03::BEQ()
{
	if (status.flag.Z == 1)
	{
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}

//BMI - Branch if Minus
//PC = PC + 2 + memory(signed)
//
//If the negative flag is set, BMI branches to a nearby location by adding the branch offset to the program counter.The offset is signedand has a range of[-128, 127] relative to the first byte after the branch instruction.
Byte m6502_2A03::BMI()
{
	if (status.flag.N == 1)
	{
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}


//BNE - Branch if Not Equal
//PC = PC + 2 + memory(signed)
//
//If the zero flag is clear, BNE branches to a nearby location by adding the branch offset to the program counter.The offset is signedand has a range of[-128, 127] relative to the first byte after the branch instruction.
Byte m6502_2A03::BNE()
{
	if (status.flag.Z == 0)
	{
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}


//BPL - Branch if Plus
//PC = PC + 2 + memory(signed)
//
//If the negative flag is clear, BPL branches to a nearby location by adding the branch offset to the program counter.The offset is signedand has a range of[-128, 127] relative to the first byte after the branch instruction.
Byte m6502_2A03::BPL()
{
	if (status.flag.N == 0)
	{
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}


//BVC - Branch if Overflow Clear
//PC = PC + 2 + memory(signed)
//
//If the overflow flag is clear, BVC branches to a nearby location by adding the branch offset to the program counter.The offset is signedand has a range of[-128, 127] relative to the first byte after the branch instruction.
Byte m6502_2A03::BVC()
{
	if (status.flag.V == 0)
	{
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}

//BVS - Branch if Overflow Set
//PC = PC + 2 + memory(signed)
//
//If the overflow flag is set, BVS branches to a nearby location by adding the branch offset to the program counter.The offset is signedand has a range of[-128, 127] relative to the first byte after the branch instruction.
Byte m6502_2A03::BVS()
{
	if (status.flag.V == 1)
	{
		cycles++;
		addr_abs = pc + addr_rel;

		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;

		pc = addr_abs;
	}
	return 0;
}



// Para opcodes ilegales
Byte m6502_2A03::XXX()
{
	return 0;
}


void m6502_2A03::saveCPUState(CPUState& state)
{
	// Registros
	state.status = status.reg;
	state.ra	 = ra;
	state.rx	 = rx;
	state.ry	 = ry;
	state.sp	 = sp;
	state.pc	 = pc;

	// Variables
	state.addr_abs	= addr_abs;
	state.cycles	= cycles;
	state.IRQcall	= IRQcall;
	state.NMIcall	= NMIcall;
	state.elapsed	= elapsed;
}

void m6502_2A03::loadCPUState(CPUState state)
{
	status.reg = state.status;
	ra = state.ra;
	rx = state.rx;
	ry = state.ry;
	sp = state.sp;
	pc = state.pc;
	addr_abs = state.addr_abs;
	cycles = state.cycles;
	IRQcall = state.IRQcall;
	NMIcall = state.NMIcall;
	elapsed = state.elapsed;
}

Byte m6502_2A03::fetch()
{
	if (!(tabla[opcode].modo_dir == &m6502_2A03::IMP))
		fetched = read(addr_abs);
	return fetched;
}


void m6502_2A03::reset()
{

	addr_abs = 0xFFFC;
	uint16_t lo = read(addr_abs + 0);
	uint16_t hi = read(addr_abs + 1);


	pc = (hi << 8) | lo;

	ra = 0;
	rx = 0;
	ry = 0;
	sp = 0xFD;
	status.reg = 0x24;

	addr_rel = 0x0000;
	addr_abs = 0x0000;
	fetched = 0x00;
	cycles = 8;
}


void m6502_2A03::init()
{
	// Get address to set program counter to
	addr_abs = 0xFFFC;
	uint16_t lo = read(addr_abs + 0);
	uint16_t hi = read(addr_abs + 1);

	// Set it
	pc = (hi << 8) | lo;

	// Reset internal registers
	ra = 0;
	rx = 0;
	ry = 0;
	sp = 0xFD;
	status.reg = 0x24;

	// Clear internal helper variables
	addr_rel = 0x0000;
	addr_abs = 0x0000;
	fetched = 0x00;
	cycles = 7;
	clock_count = 0;
	//logmode = true;
}

void m6502_2A03::test() // Para testear con nestest
{
	
	pc = 0xC000;
	logmode = true;

	ra = 0;
	rx = 0;
	ry = 0;
	sp = 0xFD;
	status.reg = 0x24;


	addr_rel = 0x0000;
	addr_abs = 0x0000;
	fetched = 0x00;
	clock_count = 7;
}

void m6502_2A03::IRQ()
{


	if (status.flag.I == 0)
	{
		
		write(0x0100 + sp, (pc >> 8) & 0x00FF);
		sp--;
		write(0x0100 + sp, pc & 0x00FF);
		sp--;

		
		status.flag.B = 0;
		status.flag.U = 1;
		write(0x0100 + sp, status.reg);
		status.flag.I = 1;
		sp--;

		
		addr_abs = 0xFFFE;
		uint16_t lo = read(addr_abs + 0);
		uint16_t hi = read(addr_abs + 1);
		pc = (hi << 8) | lo;

		
		this->cycles = 7;
	}
}


void m6502_2A03::NMI()
{
	write(0x0100 + sp, (pc >> 8) & 0x00FF);
	sp--;
	write(0x0100 + sp, pc & 0x00FF);
	sp--;

	status.flag.B = 0;
	status.flag.U = 1;
	write(0x0100 + sp, status.reg);
	sp--;
	status.flag.I = 1;

	addr_abs = 0xFFFA;
	uint16_t lo = read(addr_abs + 0);
	uint16_t hi = read(addr_abs + 1);
	pc = (hi << 8) | lo;
	this->cycles = 8;
}

