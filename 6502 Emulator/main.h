    #pragma once

//  ====================
    #include <stdio.h>
    #include <stdlib.h>
    #include <iostream>
//  ====================

#define ASSERT(Condition, Text) {if (!Condition){throw -1; }}

//  =================
    namespace m6502 {

        using SByte = char;
        using Byte = unsigned char;
        using Word = unsigned short;
        using u32 = unsigned int;
        using s32 = signed   int;

        struct Mem;
        struct CPU;
        struct StatusFlags;

    } // namespace m6502
//  ====================

//  ============================
    using Byte = unsigned char;
    using Word = unsigned short;
    using u32  = unsigned int;
    using s32  = signed   int;
//  ============================

//  ===================
    struct m6502::Mem {

        static constexpr u32 MAX_MEM = 1024 * 64;
        Byte Data[MAX_MEM];

//		===================
        void Initialise() {

            for (u32 ii = 0; ii < MAX_MEM; ii++) {

                Data[ii] = 0;

            }

        } // Function Initialise()
//      ==========================
// 
//      ==================================== read one byte
        Byte operator[](u32 Address) const {

            return Data[Address];

        } // Function() (read one byte)

//      =============================== read one byte
        Byte& operator[](u32 Address) {

            return Data[Address];

        } // Function (read one byte)

    }; // End struct memory
//  =======================
    
    struct m6502::StatusFlags {

        Byte C : 1;
        Byte Z : 1;
        Byte I : 1;
        Byte D : 1;
        Byte B : 1;
        Byte Unused : 1;
        Byte V : 1;
        Byte N : 1;

    };


//  ===================
    struct m6502::CPU {

        Word PC; // program counter
        Byte SP; // stack pointer

        Byte A, X, Y; // registers

        union {

            Byte PS;
            StatusFlags Flag;

        };

//      =========================
        void Reset(Mem& memory) {

            PC = 0xFFFC;
            SP = 0xFF;
            Flag.D = 0;
            Flag.C = Flag.Z = Flag.I = Flag.D = Flag.B = Flag.V = Flag.N = 0;
            A = X = Y = 0;
            memory.Initialise();

        } // function Reset()
//      =====================

//      ================================================
        Byte FetchByte(s32& Cycles, const Mem& memory) {

            Byte Data = memory[PC];
            PC++;
            Cycles--;

            return Data;

        } // Function Fetch()
//      =====================

//      ================================================
        SByte FetchSByte(s32& Cycles, const Mem& memory) {

           return FetchByte(Cycles, memory);

        } // Function Fetch()
//      =====================

//      ================================================
        Word FetchWord(s32& Cycles, const Mem& memory) {


            // 6502 is little endian
            Word Data = memory[PC];
            PC++;

            Data |= (memory[PC] << 8);
            PC++;
            Cycles -= 2;

            return Data;

        } // Function FetchWord()
//      =========================

//      =============================================================
        Byte ReadByte(s32& Cycles, Word Address, const Mem& memory) {

            Byte Data = memory[Address];
            Cycles--;
            return Data;

        } // Function ReadByte()
//      ========================

//      =============================================================
        Byte ReadWord(s32& Cycles, Word Address, const Mem& memory) {

            Byte loByte = ReadByte(Cycles, Address, memory);
            Byte hiByte = ReadByte(Cycles, Address + 1, memory);

            return loByte | (hiByte << 8);

        } // Function ReadWord()
//      ========================

//      Write 1 byte to memory
//      ====================================================================
        void WriteByte(Byte Value, s32& Cycles, Word Address, Mem& memory) {

            memory[Address] = Value;
            Cycles--;

        } // Function WriteByte()
//      =========================

//      Write 2 bytes to memory
//      ==========================================================================
        void WriteWord(Word DataToWrite, s32& Cycles, Word Address, Mem& memory) {

            memory[Address] = DataToWrite & 0xFF;
            memory[Address + 1] = (DataToWrite >> 8);
            Cycles -= 2;

        } // Function WriteWord()
//      =========================

//      Return the stack pointer as a full 16-bit address in the 1st page
//      ==========================
        Word SPToAddress() const {

            return 0x100 | SP;

        } // Function SPToWord()
//      ========================

//      Push the PC-1 onto the stack
//      ==============================================
        void PushPCToStack(s32& Cycles, Mem& memory) {

            WriteWord(PC - 1, Cycles, SPToAddress() - 1, memory);

            SP -= 2;

        } // Function PushPCToStack()
//      =============================

//      ==============================================================
        void PushByteOntoStack(s32& Cycles, Byte Value, Mem& memory) {

            Word SP16Bit = SPToAddress();
            memory[SP16Bit] = Value;
            
            Cycles--;
            SP--;
            Cycles--;


        } // Function PushByteOntoStack()
//      =================================

//      =================================================
        Byte PopByteFromStack(s32& Cycles, Mem& memory) {

            SP++;
            Word SP16Bit = SPToAddress();
            Byte Value = memory[SP16Bit];

            Cycles -= 3;

            return Value;

        } // Function PopByteFromStack()
//      =================================

//      =================================================
        Word PopWordFromStack(s32& Cycles, Mem& memory) {

            Word ValueFromStack = ReadWord(Cycles, SPToAddress() + 1, memory);
            SP += 2;

            Cycles--;

            return ValueFromStack;

        } // Function PopWordFromStack()
//      ================================

        static constexpr Byte

            NegativeFlagBit = 0b10000000,
            OverflowFlagBit = 0b01000000;

        static constexpr Byte

//      =============
//      OPCODES BELOW
//      =============

//          ===========
//          LDA Opcodes
//          ===================
            INS_LDA_IM = 0xA9,
            INS_LDA_ZP = 0xA5,
            INS_LDA_ZPX = 0xB5,
            INS_LDA_ABS = 0xAD,
            INS_LDA_ABSX = 0xBD,
            INS_LDA_ABSY = 0xB9,
            INS_LDA_INDX = 0xA1,
            INS_LDA_INDY = 0xB1,

//          ===========
//          LDX Opcodes
//          ==================
            INS_LDX_IM = 0xA2,
            INS_LDX_ZP = 0xA6,
            INS_LDX_ZPY = 0xB6,
            INS_LDX_ABS = 0xAE,
            INS_LDX_ABSY = 0xBE,

//          ===========
//          LDY Opcodes
//          =================
            INS_LDY_IM = 0xA0,
            INS_LDY_ZP = 0xA4,
            INS_LDY_ZPX = 0xB4,
            INS_LDY_ABS = 0xAC,
            INS_LDY_ABSX = 0xBC,

//          ===========
//          STA Opcodes
//          ==================
            INS_STA_ZP = 0x85,
            INS_STA_ZPX = 0x95,
            INS_STA_ABS = 0x8D,

            INS_STA_ABSX = 0x9D,
            INS_STA_ABSY = 0x99,
            INS_STA_INDX = 0x81,
            INS_STA_INDY = 0x91,
//          ===========
//          STX Opcodes
//          =================
            INS_STX_ZP = 0x86,
            INS_STX_ABS = 0x8E,
//          ===========
//          STY Opcodes
//          =================
            INS_STY_ZP = 0x84,
            INS_STY_ZPX = 0x94,
            INS_STY_ABS = 0x8C,
//          ===================

//          =========== 
//          JMP Opcodes
//          ===================
            INS_JMP_ABS = 0x4C,
            INS_JMP_IND = 0x6C,
//          ===================

            INS_TSX = 0xBA,
            INS_TXS = 0x9A,

            INS_PHA = 0x48,
            INS_PLA = 0x68,
            INS_PHP = 0x08,
            INS_PLP = 0x28,
//          ==================
            INS_JSR    = 0x20,
            INS_RTS    = 0x60,
//          ===================

            INS_AND_IM = 0x29,
            INS_AND_ZP = 0x25,
            INS_AND_ZPX = 0x35,
            INS_AND_ABS = 0x2D,
            INS_AND_ABSX = 0x3D,
            INS_AND_ABSY = 0x39,
            INS_AND_INDX = 0x21,
            INS_AND_INDY = 0x31,

            //OR
            INS_ORA_IM = 0x09,
            INS_ORA_ZP = 0x05,
            INS_ORA_ZPX = 0x15,
            INS_ORA_ABS = 0x0D,
            INS_ORA_ABSX = 0x1D,
            INS_ORA_ABSY = 0x19,
            INS_ORA_INDX = 0x01,
            INS_ORA_INDY = 0x11,

            // EOR
            INS_EOR_IM = 0x49,
            INS_EOR_ZP = 0x45,
            INS_EOR_ZPX = 0x55,
            INS_EOR_ABS = 0x4D,
            INS_EOR_ABSX = 0x5D,
            INS_EOR_ABSY = 0x59,
            INS_EOR_INDX = 0x41,
            INS_EOR_INDY = 0x51,

            // BIT
            INS_BIT_ZP = 0x24,
            INS_BIT_ABS = 0x2C,

            // Transfer register
            INS_TAX = 0xAA,
            INS_TAY = 0xA8,
            INS_TXA = 0x8A,
            INS_TYA = 0x98,

            // Increment & Decrement

            INS_INX = 0xE8,
            INS_INY = 0xC8,
            INS_DEY = 0x88,
            INS_DEX = 0xCA,
            INS_DEC_ZP = 0xC6,
            INS_DEC_ZPX = 0xD6,
            INS_DEC_ABS = 0xCE,
            INS_DEC_ABSX = 0xDE,
            INS_INC_ZP = 0xE6,
            INS_INC_ZPX = 0xF6,
            INS_INC_ABS = 0xEE,
            INS_INC_ABSX = 0xFE,
            
            // Conditional branching

            INS_BEQ = 0xF0,
            INS_BNE = 0xD0,
            INS_BCS = 0xB0,
            INS_BCC = 0x90,
            INS_BMI = 0x30,
            INS_BPL = 0x10,
            INS_BVC = 0x50,
            INS_BVS = 0x70,

            // status flag changes

            INS_CLC = 0x18,
            INS_SEC = 0x38,
            
            INS_CLD = 0xD8,
            INS_SED = 0xF8,
            INS_CLI = 0x58,
            INS_SEI = 0x78,
            INS_CLV = 0xB8,

            // Arithmetic

            INS_ADC = 0x69,
            INS_ADC_ZP = 0x65,
            INS_ADC_ZPX = 0x75,
            INS_ADC_ABS = 0x6D,
            INS_ADC_ABSX = 0x7D,
            INS_ADC_ABSY = 0x79,
            INS_ADC_INDX = 0x61,
            INS_ADC_INDY = 0x71,


            INS_SEC_ABS = 0xED,

            INS_SBC_ABS = 0xED,
            // Register Comparison

            INS_CMP = 0xC9,
            INS_CMP_ZP = 0xC5,
            INS_CMP_ZPX = 0xD5,
            INS_CMP_ABS = 0xCD,
            INS_CMP_ABSX = 0xDD,
            INS_CMP_ABSY = 0xD9,
            INS_CMP_INDX = 0xC1,
            INS_CMP_INDY = 0xD1,

            INS_CPX = 0xE0,
            INS_CPY = 0xC0,
            INS_CPX_ZP = 0xE4,
            INS_CPY_ZP = 0xC4,
            INS_CPX_ABS = 0xEC,
            INS_CPY_ABS = 0xCC,


            // Misc.

            INS_NOP = 0xEA;

//      ===========================================
        void SetZeroAndNegativeFlags(Byte Register) {
    
            Flag.Z = (Register == 0);
            Flag.N = (Register & 0b10000000) > 0;
    
        } // Function LDASetStatus()
//      ============================

//      ======================================
        s32 Execute(s32 Cycles, Mem& memory) {

            // load a register with the value from a memory address
            auto LoadRegister = [&Cycles, &memory, this](Word Address, Byte& Register) {

                Register = ReadByte(Cycles, Address, memory);

                SetZeroAndNegativeFlags(Register);

            };

            // And the A register

            auto And = [&Cycles, &memory, this](Word Address) {

                A &= ReadByte(Cycles, Address, memory);

                SetZeroAndNegativeFlags(A);

            };

            // Or the A register

            auto Ora = [&Cycles, &memory, this](Word Address) {

                A |= ReadByte(Cycles, Address, memory);

                SetZeroAndNegativeFlags(A);

            };

            // Eor the A register

            auto Eor = [&Cycles, &memory, this](Word Address) {

                A ^= ReadByte(Cycles, Address, memory);

                SetZeroAndNegativeFlags(A);

            };

            // Conditional branch

            auto BranchIf = [&Cycles, &memory, this](bool Test, bool Expected) {

                SByte Offset = FetchSByte(Cycles, memory);

                if (Test == Expected) {

                    const Word OldPC = PC;
                    PC += Offset;
                    Cycles--;

                    if ((PC >> 8) != (OldPC >> 8)) { // page changed

                        Cycles -= 2;

                    }

                }

            };

            auto ADC = [&Cycles, &memory, this](Byte Operand) {

                ASSERT(Flag.D == false, "haven't handled decimal mode! ");

                const bool AreSignBitsTheSame = !((A ^ Operand) & NegativeFlagBit);

                Word Sum = A;

                Sum += Operand;
                Sum += Flag.C;

                A = (Sum & 0xFF);

                SetZeroAndNegativeFlags(A);

                Flag.C = Sum > 0xFF;
                Flag.V = AreSignBitsTheSame && ((A ^ Operand) & NegativeFlagBit);

            };

            auto SBC = [&ADC](Byte Operand) {

                ADC(~Operand);

            };


            // Sets the processor status for a CMP/CPX/CPY Instruction
            auto RegisterCompare = [&Cycles, &memory, this](Byte Operand, Byte RegisterValue) {

                Byte Temp = RegisterValue - Operand;

                Flag.N = (Temp & NegativeFlagBit) > 0;
                Flag.Z = RegisterValue == Operand;
                Flag.C = RegisterValue >= Operand;


            };

            const s32 CyclesRequested = Cycles;

            while (Cycles > 0) {
    
                Byte Ins = FetchByte(Cycles, memory);
    
                switch (Ins) {
    
                    case INS_AND_IM: {
    
                        A &= FetchByte(Cycles, memory);
                        SetZeroAndNegativeFlags(A);

                    }
                    break;

                    case INS_ORA_IM: {

                        A |= FetchByte(Cycles, memory);
                        SetZeroAndNegativeFlags(A);

                    }
                    break;

                    case INS_EOR_IM: {

                        A ^= FetchByte(Cycles, memory);
                        SetZeroAndNegativeFlags(A);

                    }
                    break;

                    case INS_AND_ZP: {

                        Word Address = AddrZeroPage(Cycles, memory);
                        And(Address);

                    }
                    break;

                    case INS_ORA_ZP: {

                        Word Address = AddrZeroPage(Cycles, memory);
                        Ora(Address);

                    }
                    break;

                    case INS_EOR_ZP: {

                        Word Address = AddrZeroPage(Cycles, memory);
                        Eor(Address);

                    }
                    break;

                    case INS_AND_ZPX: {

                        Word Address = AddrZeroPageX(Cycles, memory);
                        And(Address);

                    }
                    break;

                    case INS_ORA_ZPX: {

                        Word Address = AddrZeroPageX(Cycles, memory);
                        Ora(Address);

                    }
                    break;

                    case INS_EOR_ZPX: {

                        Word Address = AddrZeroPageX(Cycles, memory);
                        Eor(Address);

                    }
                    break;

                    case INS_AND_ABS: {

                        Word Address = AddrAbsolute(Cycles, memory);
                        And(Address);

                    }
                    break;

                    case INS_ORA_ABS: {

                        Word Address = AddrAbsolute(Cycles, memory);
                        Ora(Address);

                    }
                    break;

                    case INS_EOR_ABS: {

                        Word Address = AddrAbsolute(Cycles, memory);
                        Eor(Address);

                    }
                    break;

                    case INS_AND_ABSX: {

                        Word Address = AddrAbsoluteX(Cycles, memory);
                        And(Address);

                    }
                    break;

                    case INS_ORA_ABSX: {

                        Word Address = AddrAbsoluteX(Cycles, memory);
                        Ora(Address);

                    }
                    break;

                    case INS_EOR_ABSX: {

                        Word Address = AddrAbsoluteX(Cycles, memory);
                        Eor(Address);

                    }
                    break;

                    case INS_AND_ABSY: {

                        Word Address = AddrAbsoluteY(Cycles, memory);
                        And(Address);

                    }
                    break;

                    case INS_ORA_ABSY: {

                        Word Address = AddrAbsoluteY(Cycles, memory);
                        Ora(Address);

                    }
                    break;

                    case INS_EOR_ABSY: {

                        Word Address = AddrAbsoluteY(Cycles, memory);
                        Eor(Address);

                    }
                    break;

                    case INS_AND_INDX: {

                        Word Address = AddrIndirectX(Cycles, memory);
                        And(Address);

                    }
                    break;

                    case INS_ORA_INDX: {

                        Word Address = AddrIndirectX(Cycles, memory);
                        Ora(Address);

                    }
                    break;

                    case INS_EOR_INDX: {

                        Word Address = AddrIndirectX(Cycles, memory);
                        Eor(Address);

                    }
                    break;

                    case INS_AND_INDY: {

                        Word Address = AddrIndirectY(Cycles, memory);
                        And(Address);

                    }
                    break;

                    case INS_ORA_INDY: {

                        Word Address = AddrIndirectY(Cycles, memory);
                        Ora(Address);

                    }
                    break;

                    case INS_EOR_INDY: {

                        Word Address = AddrIndirectY(Cycles, memory);
                        Eor(Address);

                    }
                    break;

                    case INS_BIT_ZP: {

                        Word Address = AddrZeroPage(Cycles, memory);
                        Byte Value = ReadByte(Cycles, Address, memory);

                        Flag.Z = !(A & Value);

                        Flag.N = (Value & NegativeFlagBit) != 0;

                        Flag.V = (Value & OverflowFlagBit) != 0;

                    }
                    break;

                    case INS_BIT_ABS: {

                        Word Address = AddrAbsolute(Cycles, memory);
                        Byte Value = ReadByte(Cycles, Address, memory);

                        Flag.Z = !(A & Value);

                        Flag.N = (Value & NegativeFlagBit) != 0;

                        Flag.V = (Value & OverflowFlagBit) != 0;
                    }
                    break;

                    case INS_LDA_IM: {
    
                        Byte Value = FetchByte(Cycles, memory);
                        A = Value;
                        SetZeroAndNegativeFlags(A);
    
                    }
                    break;

                    case INS_LDX_IM: {

                        Byte Value = FetchByte(Cycles, memory);
                        X = Value;
                        SetZeroAndNegativeFlags(X);

                    }
                    break;

                    case INS_LDY_IM: {

                        Byte Value = FetchByte(Cycles, memory);
                        Y = Value;
                        SetZeroAndNegativeFlags(Y);

                    }
                    break;
    
                    case INS_LDA_ZP: {
    
                        Word Address = AddrZeroPage(Cycles, memory);

                        LoadRegister(Address, A);

                    }
                    break;

                    case INS_LDX_ZP: {

                        Word Address = AddrZeroPage(Cycles, memory);

                        LoadRegister(Address, X);

                    }
                    break;

                    case INS_LDX_ZPY: {

                        Word Address = AddrZeroPageY(Cycles, memory);

                        LoadRegister(Address, X);

                    }
                    break;

                    case INS_LDY_ZP: {

                        Word Address = AddrZeroPage(Cycles, memory);

                        LoadRegister(Address, Y);

                    }

                    break;
    
                    case INS_LDA_ZPX: {
    
                        Word Address = AddrZeroPageX(Cycles, memory);
                        
                        LoadRegister(Address, A);
                    }
                    break;

                    case INS_LDY_ZPX: {

                        Word Address = AddrZeroPageX(Cycles, memory);
                        
                        LoadRegister(Address, Y);
                    }
                    break;
    
                    case INS_JSR: {
    
                        Word SubAddr = FetchWord(Cycles, memory);
                        
                        PushPCToStack(Cycles, memory);


                        PC = SubAddr;
                        Cycles--; 
                    }
                    break;

                    case INS_RTS: {

                        Word ReturnAddress = PopWordFromStack(Cycles, memory);

                        PC = ReturnAddress + 1;

                        Cycles -= 2;

                    }
                    break;

                    case INS_JMP_ABS: {

                        Word Address = AddrAbsolute(Cycles, memory);
                        PC = Address;

                    }
                    break;

                    case INS_JMP_IND: {

                        Word Address = AddrAbsolute(Cycles, memory);
                        Address = ReadWord(Cycles, Address, memory);

                        PC = Address;

                    }
                    break;

                    case INS_TSX: {

                        X = SP;
                        Cycles--;
                        SetZeroAndNegativeFlags(X);

                    }
                    break;

                    case INS_TXS: {

                        SP = X;
                        
                    }
                    break;

                    case INS_PHA : {

                        PushByteOntoStack(Cycles, A, memory);
                        Cycles--;

                    }
                    break;

                    case INS_PLA: {

                        A = PopByteFromStack(Cycles, memory);
                        SetZeroAndNegativeFlags(A);
                    }
                    break;

                    case INS_PHP: {

                        PushByteOntoStack(Cycles, PS, memory);

                    }
                    break;

                    case INS_PLP: {

                        PS = PopByteFromStack(Cycles, memory);
                       
                    }
                    break;

                    case INS_TAX: {

                       X = A;
                       Cycles--;
                       SetZeroAndNegativeFlags(X);
                    }
                    break;

                    case INS_TAY: {

                        Y = A;
                        Cycles--;
                        SetZeroAndNegativeFlags(Y);
                    }
                    break;

                    case INS_TXA: {

                        A = X;
                        Cycles--;
                        SetZeroAndNegativeFlags(A);
                    }
                    break;

                    case INS_TYA: {

                        A = Y;
                        Cycles--;
                        SetZeroAndNegativeFlags(A);
                    }
                    break;

                    case INS_INX: {

                        X++;
                        Cycles--;
                        SetZeroAndNegativeFlags(X); 
                    }
                    break;

                    case INS_INY: {

                        Y++;
                        Cycles--;
                        SetZeroAndNegativeFlags(Y);
                    }

                    break;

                    case INS_DEY: {

                        Y--;
                        Cycles--;
                        SetZeroAndNegativeFlags(Y);
                    }

                    break;

                    case INS_DEX: {

                        X--;
                        Cycles--;
                        SetZeroAndNegativeFlags(X);
                    }

                    break;

                    case INS_DEC_ZP: {

                        Word Address = AddrZeroPage(Cycles, memory);
                        Byte Value = ReadByte(Cycles, Address, memory);
                        Value--;
                        Cycles--;

                        WriteByte(Value, Cycles, Address, memory);

                        SetZeroAndNegativeFlags(Value);
                    }

                    break;

                    case INS_DEC_ZPX: {

                        Word Address = AddrZeroPageX(Cycles, memory);

                        Byte Value = ReadByte(Cycles, Address, memory);
                        Value--;
                        Cycles--;

                        WriteByte(Value, Cycles, Address, memory);

                        SetZeroAndNegativeFlags(Value);
                    }

                    break;

                    case INS_DEC_ABS: {

                        Word Address = AddrAbsolute(Cycles, memory);
                        Byte Value = ReadByte(Cycles, Address, memory);

                        Value--;
                        Cycles--;

                        WriteByte(Value, Cycles, Address, memory);

                        SetZeroAndNegativeFlags(Value);
                    }

                    break;

                    case INS_DEC_ABSX: {

                        Word Address = AddrAbsoluteX_5(Cycles, memory);

                        Byte Value = ReadByte(Cycles, Address, memory);

                        Value--;
                        Cycles--;

                        WriteByte(Value, Cycles, Address, memory);

                        SetZeroAndNegativeFlags(Value);
                    }

                    break;

                    case INS_INC_ZP: {

                        Word Address = AddrZeroPage(Cycles, memory);
                        Byte Value = ReadByte(Cycles, Address, memory);
                        Value++;
                        Cycles--;

                        WriteByte(Value, Cycles, Address, memory);

                        SetZeroAndNegativeFlags(Value);
                    }

                    break;

                    case INS_INC_ZPX: {

                        Word Address = AddrZeroPageX(Cycles, memory);

                        Byte Value = ReadByte(Cycles, Address, memory);
                        Value++;
                        Cycles--;

                        WriteByte(Value, Cycles, Address, memory);

                        SetZeroAndNegativeFlags(Value);
                    }

                    break;

                    case INS_INC_ABS: {

                        Word Address = AddrAbsolute(Cycles, memory);
                        Byte Value = ReadByte(Cycles, Address, memory);

                        Value++;
                        Cycles--;

                        WriteByte(Value, Cycles, Address, memory);

                        SetZeroAndNegativeFlags(Value);
                    }

                    break;

                    case INS_INC_ABSX: {

                        Word Address = AddrAbsoluteX_5(Cycles, memory);

                        Byte Value = ReadByte(Cycles, Address, memory);

                        Value++;
                        Cycles--;

                        WriteByte(Value, Cycles, Address, memory);

                        SetZeroAndNegativeFlags(Value);
                    }

                    break;

                    case INS_BEQ: {

                        BranchIf(Flag.Z, true);

                    }

                    break;

                    case INS_BNE: {

                        BranchIf(Flag.Z, false);

                    }
                   
                    break;

                    case INS_BCS: {

                        BranchIf(Flag.C, true);

                    }

                    break;

                    case INS_BCC: {

                        BranchIf(Flag.C, false);

                    }

                    break;

                    case INS_BMI: {

                        BranchIf(Flag.N, true);

                    }

                    break;

                    case INS_BPL: {

                        BranchIf(Flag.N, false);

                    }

                    break;

                    case INS_BVC: {

                        BranchIf(Flag.V, false);

                    }

                    break;

                    case INS_BVS: {

                        BranchIf(Flag.V, true);

                    }

                    break;

                    case INS_CLC: {

                        Flag.C = false;
                        Cycles--;

                    }

                    break;

                    case INS_SEC: {

                        Flag.C = true;
                        Cycles--;

                    }

                    break;

                    case INS_CLD: {

                        Flag.D = false;
                        Cycles--;

                    }

                    break;

                    case INS_SED: {

                        Flag.D = true;
                        Cycles--;

                    }

                    break;

                    case INS_CLI: {

                        Flag.I = false;
                        Cycles--;

                    }

                    break;

                    case INS_SEI: {

                        Flag.I = true;
                        Cycles--;

                    }

                    break;

                    case INS_CLV: {

                        Flag.V = false;
                        Cycles--;

                    }

                    break;

                    case INS_NOP: {

                        Cycles--;

                    }

                    break;

                    case INS_ADC_ABS: {

                       Word Address = AddrAbsolute(Cycles, memory);

                       Byte Operand = ReadByte(Cycles, Address, memory);

                       ADC(Operand);

                    }

                    break;

                    case INS_ADC_ABSX: {

                        Word Address = AddrAbsoluteX(Cycles, memory);

                        Byte Operand = ReadByte(Cycles, Address, memory);

                        ADC(Operand);

                    }

                    break;

                    case INS_ADC_ABSY: {

                        Word Address = AddrAbsoluteY(Cycles, memory);

                        Byte Operand = ReadByte(Cycles, Address, memory);

                        ADC(Operand);

                    }

                    break;

                    case INS_ADC_ZP: {

                        Word Address = AddrZeroPage(Cycles, memory);

                        Byte Operand = ReadByte(Cycles, Address, memory);

                        ADC(Operand);

                    }

                    break;

                    case INS_ADC_ZPX: {

                        Word Address = AddrZeroPageX(Cycles, memory);

                        Byte Operand = ReadByte(Cycles, Address, memory);

                        ADC(Operand);

                    }

                    break;

                    case INS_ADC_INDX: {

                        Word Address = AddrIndirectX(Cycles, memory);

                        Byte Operand = ReadByte(Cycles, Address, memory);

                        ADC(Operand);

                    }

                    break;

                    case INS_ADC_INDY: {

                        Word Address = AddrIndirectY(Cycles, memory);

                        Byte Operand = ReadByte(Cycles, Address, memory);

                        ADC(Operand);

                    }

                    break;

                    case INS_ADC: {

                        Byte Operand = FetchByte(Cycles, memory);

                        ADC(Operand);

                    }

                    break;

                    case INS_SBC_ABS: {

                        Word Address = AddrAbsolute(Cycles, memory);

                        Byte Operand = ReadByte(Cycles, Address, memory);

                        SBC(~Operand);

                    }

                    break;

                    case INS_CPX: {

                        Byte Operand = FetchByte(Cycles, memory);

                        RegisterCompare(Operand, X);

                    }

                    break;

                    case INS_CPY: {

                        Byte Operand = FetchByte(Cycles, memory);

                        RegisterCompare(Operand, Y);

                    }

                    break;

                    case INS_CPX_ZP: {

                        Word Address = AddrZeroPage(Cycles, memory);
                        Byte Operand = ReadByte(Cycles, Address, memory);

                        RegisterCompare(Operand, X);

                    }
                   
                    break;

                    case INS_CPY_ZP: {

                        Word Address = AddrZeroPage(Cycles, memory);
                        Byte Operand = ReadByte(Cycles, Address, memory);

                        RegisterCompare(Operand, Y);

                    }

                    break;

                    case INS_CPX_ABS: {

                        Word Address = AddrAbsolute(Cycles, memory);
                        Byte Operand = ReadByte(Cycles, Address, memory);

                        RegisterCompare(Operand, X);
                    }

                    break;

                    case INS_CPY_ABS: {

                        Word Address = AddrAbsolute(Cycles, memory);
                        Byte Operand = ReadByte(Cycles, Address, memory);

                        RegisterCompare(Operand, Y);
                    }

                    break;

                    case INS_CMP: {

                        Byte Operand = FetchByte(Cycles, memory);
                        
                        RegisterCompare(Operand, A);

                    }

                    break;

                    case INS_CMP_ZP: {

                        Word Address = AddrZeroPage(Cycles, memory);
                        Byte Operand = ReadByte(Cycles, Address, memory);

                        RegisterCompare(Operand, A);
                    }

                    break;

                    case INS_CMP_ZPX: {

                        Word Address = AddrZeroPageX(Cycles, memory);
                        Byte Operand = ReadByte(Cycles, Address, memory);

                        RegisterCompare(Operand, A);

                    }

                    break;

                    case INS_CMP_ABS: {

                        Word Address = AddrAbsolute(Cycles, memory);
                        Byte Operand = ReadByte(Cycles, Address, memory);

                        RegisterCompare(Operand, A);
                    }

                    break;

                    case INS_CMP_ABSX: {

                        Word Address = AddrAbsoluteX(Cycles, memory);
                        Byte Operand = ReadByte(Cycles, Address, memory);

                        RegisterCompare(Operand, A);
                    }

                    break;

                    case INS_CMP_ABSY: {

                        Word Address = AddrAbsoluteY(Cycles, memory);
                        Byte Operand = ReadByte(Cycles, Address, memory);

                        RegisterCompare(Operand, A);
                    }

                    break;

                    case INS_CMP_INDX: {

                        Word Address = AddrIndirectX(Cycles, memory);
                        Byte Operand = ReadByte(Cycles, Address, memory);

                        RegisterCompare(Operand, A);

                    }

                    break;

                    case INS_CMP_INDY: {

                        Word Address = AddrIndirectY(Cycles, memory);
                        Byte Operand = ReadByte(Cycles, Address, memory);

                        RegisterCompare(Operand, A);
                    }

                    break;


                    case INS_LDA_ABS: {

                        Word Address = AddrAbsolute(Cycles, memory);
                        
                        LoadRegister(Address, A);

                    }
                    break;

                    case INS_LDX_ABS: {

                        Word Address = AddrAbsolute(Cycles, memory);
                        
                        LoadRegister(Address, X);

                    }
                    break;

                    case INS_LDY_ABS: {

                        Word Address = AddrAbsolute(Cycles, memory);
                        
                        LoadRegister(Address, Y);

                    }
                    break;

                    case INS_LDA_ABSX: {

                        Word Address = AddrAbsoluteX(Cycles, memory);
                        
                        LoadRegister(Address, A);
                    }
                    break;

                    case INS_LDY_ABSX: {

                        Word Address = AddrAbsoluteX(Cycles, memory);
                        
                        LoadRegister(Address, Y);
                    }
                    break;

                    case INS_LDA_ABSY: {

                        Word Address = AddrAbsoluteY(Cycles, memory);
                        
                        
                        LoadRegister(Address, A);
                    }

                    break;

                    case INS_LDX_ABSY: {

                        Word Address = AddrAbsoluteY(Cycles, memory);

                        
                        LoadRegister(Address, X);
                    }
                    break;
        
                    case INS_LDA_INDX: {

                        Word Address = AddrIndirectX(Cycles, memory);
                        
                        LoadRegister(Address, A);
                    }
                    break;

                    case INS_STA_INDX: {

                        Word Address = AddrIndirectX(Cycles, memory);

                        WriteByte(A, Cycles, Address, memory);

                    }

                    break;

                    case INS_STA_ZP: {

                        Word Address = AddrZeroPage(Cycles, memory);
                        WriteByte(A, Cycles, Address, memory);

                    }
                    break;

                    case INS_STX_ZP: {

                        Word Address = AddrZeroPage(Cycles, memory);
                        WriteByte(X, Cycles, Address, memory);

                    }
                    break;

                    case INS_STY_ZP: {

                        Word Address = AddrZeroPage(Cycles, memory);
                        WriteByte(Y, Cycles, Address, memory);

                    }
                    break;

                    case INS_STA_ABS: {

                        Word Address = AddrAbsolute(Cycles, memory);
                        WriteByte(A, Cycles, Address, memory);

                    }
                    break;

                    case INS_STX_ABS: {

                        Word Address = AddrAbsolute(Cycles, memory);
                        WriteByte(X, Cycles, Address, memory);

                    }
                    break;

                    case INS_STY_ABS: {

                        Word Address = AddrAbsolute(Cycles, memory);
                        WriteByte(Y, Cycles, Address, memory);

                    }
                    break;

                    case INS_STA_ZPX: {

                        Word Address = AddrZeroPageX(Cycles, memory);
                        WriteByte(A, Cycles, Address, memory);

                    }
                    break;

                    case INS_STY_ZPX: {

                        Word Address = AddrZeroPageX(Cycles, memory);
                        WriteByte(Y, Cycles, Address, memory);

                    }
                    break;

                    case INS_STA_ABSX: {

                        Word Address = AddrAbsoluteX_5(Cycles, memory);
                        WriteByte(A, Cycles, Address, memory);
                        

                    }
                    break;

                    case INS_STA_ABSY: {

                        Word Address = AddrAbsoluteY_5(Cycles, memory);
                        WriteByte(A, Cycles, Address, memory);
                        

                    }
                    break;

                    case INS_LDA_INDY: {
     
                        Word Address = AddrIndirectY(Cycles, memory);
                        LoadRegister(Address, A);

                    }

                    break;

                    case INS_STA_INDY: {

                        Word Address = AddrIndirectY_6(Cycles, memory);

                        WriteByte(A, Cycles, Address, memory);
                     
                    }

                    break;

                    default: {
    
                        throw -1;
                        std::cout << "INSTRUCTION NOT HANDLED";
    
                    }

                    break;

                } // End switch

            } // End while loop
    
            const s32 NumCyclesUsed = CyclesRequested - Cycles;
            return NumCyclesUsed;

        } // Function Execute()
//      =======================

//      ===================================================
        Word AddrZeroPage(s32& Cycles, const Mem& memory) {

            Byte ZeroPageAddress = FetchByte(Cycles, memory);

            return ZeroPageAddress;

        } // Function AddrZeroPage()
//      ============================

//      ====================================================
        Word AddrZeroPageX(s32& Cycles, const Mem& memory) {

            Byte ZeroPageAddress = FetchByte(Cycles, memory);
            ZeroPageAddress += X;
            Cycles--;

            return ZeroPageAddress;

        } // Function AddrZeroPageX()
//      ============================

//      ====================================================
        Word AddrZeroPageY(s32& Cycles, const Mem& memory) {

            Byte ZeroPageAddress = FetchByte(Cycles, memory);
            ZeroPageAddress += Y;
            Cycles--;

            return ZeroPageAddress;

        } // Function AddrZeroPageY()
//      =============================

//      ===================================================
        Word AddrAbsolute(s32& Cycles, const Mem& memory) {

            Word AbsAddress = FetchWord(Cycles, memory);

            return AbsAddress;

        } // Function AddrAbsolute()
//      ============================
// 
//      ====================================================
        Word AddrAbsoluteX(s32& Cycles, const Mem& memory) {

            Word AbsAddress = FetchWord(Cycles, memory);
            Word AbsAddressX = AbsAddress + X;
            A = ReadByte(Cycles, AbsAddressX, memory);

            if (AbsAddressX - AbsAddress >= 0xFF)
                Cycles--;

            return AbsAddressX;

        } // Function AddrAbsoluteX()
//      ============================

//      ======================================================
        Word AddrAbsoluteX_5(s32& Cycles, const Mem& memory) {

            Word AbsAddress = FetchWord(Cycles, memory);
            Word AbsAddressX = AbsAddress + X;
           
            Cycles--;

            return AbsAddressX;

        } // Function AddrAbsoluteX_5()
//      ============================

//      ====================================================
        Word AddrAbsoluteY(s32& Cycles, const Mem& memory) {

            Word AbsAddress = FetchWord(Cycles, memory);
            Word AbsAddressY = AbsAddress + Y;
            A = ReadByte(Cycles, AbsAddressY, memory);

            if (AbsAddressY - AbsAddress >= 0xFF)
                Cycles--;

            return AbsAddressY;

        } // Function AddrAbsoluteY()
//      =============================

//      ======================================================
        Word AddrAbsoluteY_5(s32& Cycles, const Mem& memory) {

            Word AbsAddress = FetchWord(Cycles, memory);
            Word AbsAddressY = AbsAddress + Y;

                Cycles--;

            return AbsAddressY;

        } // Function AddrAbsoluteY_5()
//      =============================

//      ====================================================
        Word AddrIndirectX(s32& Cycles, const Mem& memory) {

            Byte ZPAddress = FetchByte(Cycles, memory);
            ZPAddress += X;
            Cycles--;
            Word EffectiveAddr = ReadWord(Cycles, ZPAddress, memory);

            return EffectiveAddr;

        } // Function AddrIndirectX()
//      =============================

//      ====================================================
        Word AddrIndirectY(s32& Cycles, const Mem& memory) {

            Byte ZPAddress = FetchByte(Cycles, memory);
           
            Word EffectiveAddr = ReadWord(Cycles, ZPAddress, memory);
            Word EffectiveAddrY = EffectiveAddr + Y;

            if (EffectiveAddrY - EffectiveAddr >= 0xFF)
                Cycles--;

            return EffectiveAddrY;

        } // Function AddrIndirectY()
//      =============================

//      ======================================================
        Word AddrIndirectY_6(s32& Cycles, const Mem& memory) {

            Byte ZPAddress = FetchByte(Cycles, memory);

            Word EffectiveAddr = ReadWord(Cycles, ZPAddress, memory);
            Word EffectiveAddrY = EffectiveAddr + Y;

            Cycles--;

            return EffectiveAddrY;

        } // Function AddrIndirectY_6()
//      ===============================

//      ============================================================
        Word LoadProgram(Byte* Program, u32 NumBytes, Mem& memory) {

            Word LoadAddress = 0;

            if (Program && NumBytes > 2) {

                u32 At = 0;
                const Word Lo = Program[At++];
                const Word Hi = Program[At++] << 8;
                LoadAddress = Lo | Hi;

                for (Word ii = LoadAddress; ii < LoadAddress + NumBytes - 2; ii++) {

                    memory[ii] = Program[At++];

                }

            }

            return LoadAddress;

        } // Function LoadProgram()
//      ===========================

    }; // End struct CPU
//  ====================