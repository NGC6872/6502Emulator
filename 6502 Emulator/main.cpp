#include "main.h"

using namespace m6502;

//  ============
	int main() {
	
	    Mem mem;
	    CPU cpu;
	    cpu.Reset(mem);
	    mem[0xFFFC] = CPU::INS_LDA_ZP;
	    mem[0xFFFD] = 0x42;
		mem[0x0042] = 0x84;
        cpu.Execute(3, mem);

	  return 0;

} // function main()
//  ================