# 6502 Emulator
A MOS 6502 CPU Emulator written in C++. The MOS 6502 CPU is an 8-bit microprocessor that was used in video game consoles and computers such as
Apple II, Commodore 64, and NES.
# About
This is a switch-case based implementation of the 6502. There are a few different methods of emulating a CPU such as switch-case based, jump-table based, or using a table of functions to translate the opcode. Depending on the compiler, the switch-case based implementation can be fast if the switch statement is compiled to a jump table, but if it is compiled to a list of if/else statements, it is slower than using the table of functions method.

# 6502 Architecture
The 6502 has 6 registers: 

                          Program Counter (PC): Points to the location in memory where the next instruction to be executed. 

                          Stack Pointer (SP): Points to the location in memory where the stack is located
                          
                          Accumulator (aka the A register): Temporarily stores data
                          
                          Registers X and Y: The X and Y registers can be operated on by certain instructions
                                             such as load, store, increment, decrement, and exchange data.
                                             
                          Prcessor Status Register: Used to indicate the status of certain processor functions.

The 6502 has fewer registers compared to others processors at the time like the Z80, which has
a total of 16 general purpose registers. However, the 6502 has 10-13 addressing modes, which makes the 6502 either equal to the Z80, or better by three addressing modes.

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


