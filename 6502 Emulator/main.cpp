#include "main.h"

using namespace m6502;

//  ============
	int main() {
	
		Byte Program[] = { 0x00,0x10,0xA9,0xFF,0x85,0x90,0x8D,0x00,0x80,0x49,0xCC,0x4C,0x02,0x10 };

		Mem mem;
		CPU cpu;

		std::cout << "Resetting CPU..." << std::endl;

		cpu.Reset(mem);

		std::cout << "CPU Reset" << std::endl << std::endl;

		std::cout << "Loading program..." << std::endl;

		Word StartAddress = cpu.LoadProgram(Program, 14, mem);

		std::cout << "Loaded program" << std::endl << std::endl;

		std::cout << "Start address: ";

		printf("%d\n", StartAddress);
		
		cpu.PC = StartAddress;
		std::cout << std::endl << "Program counter: ";
		printf("%d\n", cpu.PC);
		std::cout << std::endl;

		// using u32 instead of s32 for the clock was causing this to get stuck in an infinite loop before.

		for (s32 Clock = 1; Clock > 0;) {

			Clock -= cpu.Execute(1, mem);

			printf("A: %d X: %d Y: %d\n", cpu.A, cpu.X, cpu.Y);
			printf("PC: %d SP: %d\n", cpu.PC, cpu.SP);
			
		}

	  return 0;

} // function main()
//  ================