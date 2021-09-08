# 6502 Emulator
A MOS 6502 CPU Emulator written in C++. The MOS 6502 CPU is an 8-bit microprocessor that was used in video game consoles and computers such as
the Apple II, Commodore 64, and NES.
# About
This is a switch-case based implementation of the 6502. There are a few different methods of emulating a CPU such as switch-case based or jump-table based. Depending on the compiler, the switch-case based implementation can be fast if the switch statement is compiled to a jump table, but if it is compiled to a list of if/else statements, it is slower than using a jump table.

# Usage

```c++
                // Create a program
                Byte Program[] = { 0x0,0x10,0xA9,0x00,0x85,0x42,0xE6,0x42,0xA6,0x42,0xE8,0x4C,0x04,0x10 };

                // Initialize the CPU and memory
		Mem mem;
		CPU cpu;
		
		// Reset the CPU
		CPU.Reset(mem);
		
		// Load the program into memory
		Word StartAddress = cpu.LoadProgram(Program, sizeof(Program), mem);
		
		// Set the program counter to the starting address
		cpu.PC = StartAddress;
		
		// Execute instructions for 1000 clock cycles
		
	        for (s32 Clock = 1000; Clock > 0;) {

	            Clock -= cpu.Execute(1, mem);

		    printf("A: %d X: %d Y: %d\n", cpu.A, cpu.X, cpu.Y);
		    printf("PC: %d SP: %d\n", cpu.PC, cpu.SP);

		    std::cout << std::endl;
			
		 }	
```

# 6502 Architecture
The 6502 has 6 registers: 

                          Program Counter (PC): Points to the location in memory where the next instruction to be executed. 

                          Stack Pointer (SP): Points to the location in memory where the stack is located
                          
                          Accumulator (aka the A register): Temporarily stores data
                          
                          Registers X and Y: The X and Y registers can be operated on by certain instructions
                                             such as load, store, increment, decrement, and exchange data.
                                             
                          Prcessor Status Register: Used to indicate the status of certain processor functions.

The 6502 has the following addressing modes: 

                                             Accumulator addressing mode
                                             
                                             Relative addressing mode 
                                             
                                             Immediate addressing mode
                                             
                                             Absolute addressing mode
                                             
					         Implied addressing mode 
               
                                             Zero Page addressing mode
                                             
                                             Indirect Absolute addressing mode
                                             
                                             Absolute Indexed X and Y addressing mode
                                             
                                             Zero Page Indexed X and Y addressing mode
                                             
                                             Indirect Indexed addressing mode
                                             
                                             Indexed Indirect addressing mode
					     
# Resources
Here are a few resources I've used so far:

https://en.wikipedia.org/wiki/MOS_Technology_6502

https://www.youtube.com/watch?v=qJgsuQoy9bc

https://archive.org/details/6502UsersManual/page/n31/mode/2up

http://emubook.emulation64.com/cpu.htm

# TODO:

 Finish implementing all of the instructions
 Seperate memory and CPU into different classes
 Create an interface to visualize instructions being executed
