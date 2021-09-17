#include "Memory.h"
//#include "StatusFlags.h"

//  ===================
    using SByte = char;
    using Byte = unsigned char;
    using Word = unsigned short;
    using u32 = unsigned int;
    using s32 = signed   int;
//  ============================

class CPU {

	public:

		CPU();
		~CPU();

		void Reset(Memory& memory);

		Byte FetchByte(s32& Cycles, const Memory& memory);

		SByte FetchSByte(s32& Cycles, const Memory& memory);

		Word FetchWord(s32& Cycles, const Memory& memory);

		Byte ReadByte(s32& Cycles, Word Address, const Memory& memory);

		Byte ReadWord(s32& Cycles, Word Address, const Memory& memory);

		void WriteByte(Byte Value, s32& Cycles, Word Address, Memory& memory);

		void WriteWord(Word DataToWrite, s32& Cycles, Word Address, Memory& memory);

		Word SPToAddress() const;

		void PushPCMinusOneToStack(s32& Cycles, Memory& memory);

		void PushPCToStack(s32& Cycles, Memory& memory);

		void PushPCPlusOneToStack(s32& Cycles, Memory& memory);

		void PushWordToStack(s32& Cycles, Memory& memory, Word Value);

		void PushByteOntoStack(s32& Cycles, Byte Value, Memory& memory);

		Byte PopByteFromStack(s32& Cycles, Memory& memory);

		Word PopWordFromStack(s32& Cycles, Memory& memory);




	public:


		void SetZeroAndNegativeFlags(Byte Register);

		s32 Execute(s32 Cycles, Memory& memory);

		Word AddrZeroPage(s32& Cycles, const Memory& memory);

		Word AddrZeroPageX(s32& Cycles, const Memory& memory);

		Word AddrZeroPageY(s32& Cycles, const Memory& memory);

		Word AddrAbsolute(s32& Cycles, const Memory& memory);

		Word AddrAbsoluteX(s32& Cycles, const Memory& memory);

		Word AddrAbsoluteX_5(s32& Cycles, const Memory& memory);

		Word AddrAbsoluteY(s32& Cycles, const Memory& memory);

		Word AddrAbsoluteY_5(s32& Cycles, const Memory& memory);

		Word AddrIndirectX(s32& Cycles, const Memory& memory);

		Word AddrIndirectY(s32& Cycles, const Memory& memory);

		Word AddrIndirectY_6(s32& Cycles, const Memory& memory);

		Word LoadProgram(Byte* Program, u32 NumBytes, Memory& memory);

		public:

			Word PC; // program counter
			Byte SP; // stack pointer

			Byte A, X, Y; // registers

};