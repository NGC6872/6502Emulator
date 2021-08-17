    #pragma once

//  ====================
    #include <stdio.h>
    #include <stdlib.h>
    #include <iostream>
//  ====================

//  =================
    namespace m6502 {

        using Byte = unsigned char;
        using Word = unsigned short;
        using u32 = unsigned int;
        using s32 = signed   int;

        struct Mem;
        struct CPU;

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
    
//  ===================
    struct m6502::CPU {

        Word PC; // program counter
        Byte SP; // stack pointer

        Byte A, X, Y; // registers

        // status flags below

        Byte C : 1;
        Byte Z : 1;
        Byte I : 1;
        Byte D : 1;
        Byte B : 1;
        Byte V : 1;
        Byte N : 1;

//      =========================
        void Reset(Mem& memory) {

            PC = 0xFFFC;
            SP = 0xFF;
            D = 0;
            C = Z = I = D = B = V = N = 0;
            A = X = Y = 0;
            memory.Initialise();

        } // function Reset()
//      =====================

//      ================================================
        Byte FetchByte(u32& Cycles, const Mem& memory) {

            Byte Data = memory[PC];
            PC++;
            Cycles--;

            return Data;

        } // Function Fetch()
//      =====================

//      ================================================
        Word FetchWord(u32& Cycles, const Mem& memory) {


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
        Byte ReadByte(u32& Cycles, Word Address, const Mem& memory) {

            Byte Data = memory[Address];
            Cycles--;
            return Data;

        } // Function ReadByte()
//      ========================

//      =============================================================
        Byte ReadWord(u32& Cycles, Word Address, const Mem& memory) {

            Byte loByte = ReadByte(Cycles, Address, memory);
            Byte hiByte = ReadByte(Cycles, Address + 1, memory);

            return loByte | (hiByte << 8);

        } // Function ReadWord()
//      ========================

//      Write 1 byte to memory
//      ====================================================================
        void WriteByte(Byte Value, u32& Cycles, Word Address, Mem& memory) {

            memory[Address] = Value;
            Cycles--;

        } // Function WriteByte()
//      =========================

//      Write 2 bytes to memory
//      ==========================================================================
        void WriteWord(Word DataToWrite, u32& Cycles, Word Address, Mem& memory) {

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
        void PushPCToStack(u32& Cycles, Mem& memory) {

            WriteWord(PC - 1, Cycles, SPToAddress() - 1, memory);

            SP -= 2;

        } // Function PushPCToStack()
//      =============================

//      =================================================
        Word PopWordFromStack(u32& Cycles, Mem& memory) {

            Word ValueFromStack = ReadWord(Cycles, SPToAddress() + 1, memory);
            SP += 2;

            Cycles--;

            return ValueFromStack;

        } // Function PopWordFromStack()
//      ================================

        static constexpr Byte

//      =============
//      OPCODES BELOW
//      =============
    
//          ===========
//          LDA Opcodes
//          ===================
            INS_LDA_IM  = 0xA9,
            INS_LDA_ZP  = 0xA5,
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
            INS_STY_ZPX  = 0x94,
            INS_STY_ABS = 0x8C,

//          ==================
            INS_JSR    = 0x20,
            INS_RTS    = 0x60;
//          ===================

//      ===========================================
        void LoadRegisterSetStatus(Byte Register) {
    
            Z = (Register == 0);
            N = (Register & 0b10000000) > 0;
    
        } // Function LDASetStatus()
//      ============================

//      =======================================
        void Execute(u32 Cycles, Mem& memory) {

            // load a register with the value from a memory address
            auto LoadRegister = [&Cycles, &memory, this](Word Address, Byte& Register) {

                Register = ReadByte(Cycles, Address, memory);

                LoadRegisterSetStatus(Register);

            };

            const u32 CyclesRequested = Cycles;

            while (Cycles > 0) {
    
                Byte Ins = FetchByte(Cycles, memory);
    
                switch (Ins) {
    
                    case INS_LDA_IM: {
    
                        Byte Value = FetchByte(Cycles, memory);
                        A = Value;
                        LoadRegisterSetStatus(A);
    
                    }
                    break;

                    case INS_LDX_IM: {

                        Byte Value = FetchByte(Cycles, memory);
                        X = Value;
                        LoadRegisterSetStatus(X);

                    }
                    break;

                    case INS_LDY_IM: {

                        Byte Value = FetchByte(Cycles, memory);
                        Y = Value;
                        LoadRegisterSetStatus(Y);

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
    
        } // Function Execute()
//      =======================

//      ===================================================
        Word AddrZeroPage(u32& Cycles, const Mem& memory) {

            Byte ZeroPageAddress = FetchByte(Cycles, memory);

            return ZeroPageAddress;

        } // Function AddrZeroPage()
//      ============================

//      ====================================================
        Word AddrZeroPageX(u32& Cycles, const Mem& memory) {

            Byte ZeroPageAddress = FetchByte(Cycles, memory);
            ZeroPageAddress += X;
            Cycles--;

            return ZeroPageAddress;

        } // Function AddrZeroPageX()
//      ============================

//      ====================================================
        Word AddrZeroPageY(u32& Cycles, const Mem& memory) {

            Byte ZeroPageAddress = FetchByte(Cycles, memory);
            ZeroPageAddress += Y;
            Cycles--;

            return ZeroPageAddress;

        } // Function AddrZeroPageY()
//      ============================

//      ===================================================
        Word AddrAbsolute(u32& Cycles, const Mem& memory) {

            Word AbsAddress = FetchWord(Cycles, memory);

            return AbsAddress;

        } // Function AddrAbsolute()
//      ============================
// 
//      ====================================================
        Word AddrAbsoluteX(u32& Cycles, const Mem& memory) {

            Word AbsAddress = FetchWord(Cycles, memory);
            Word AbsAddressX = AbsAddress + X;
            A = ReadByte(Cycles, AbsAddressX, memory);

            if (AbsAddressX - AbsAddress >= 0xFF)
                Cycles--;

            return AbsAddressX;

        } // Function AddrAbsoluteX()
//      ============================

//      ======================================================
        Word AddrAbsoluteX_5(u32& Cycles, const Mem& memory) {

            Word AbsAddress = FetchWord(Cycles, memory);
            Word AbsAddressX = AbsAddress + X;
           
            Cycles--;

            return AbsAddressX;

        } // Function AddrAbsoluteX_5()
//      ============================

//      ====================================================
        Word AddrAbsoluteY(u32& Cycles, const Mem& memory) {

            Word AbsAddress = FetchWord(Cycles, memory);
            Word AbsAddressY = AbsAddress + Y;
            A = ReadByte(Cycles, AbsAddressY, memory);

            if (AbsAddressY - AbsAddress >= 0xFF)
                Cycles--;

            return AbsAddressY;

        } // Function AddrAbsoluteY()
//      =============================

//      ======================================================
        Word AddrAbsoluteY_5(u32& Cycles, const Mem& memory) {

            Word AbsAddress = FetchWord(Cycles, memory);
            Word AbsAddressY = AbsAddress + Y;

                Cycles--;

            return AbsAddressY;

        } // Function AddrAbsoluteY_5()
//      =============================

//      ====================================================
        Word AddrIndirectX(u32& Cycles, const Mem& memory) {

            Byte ZPAddress = FetchByte(Cycles, memory);
            ZPAddress += X;
            Cycles--;
            Word EffectiveAddr = ReadWord(Cycles, ZPAddress, memory);

            return EffectiveAddr;

        } // Function AddrIndirectX()
//      =============================

//      ====================================================
        Word AddrIndirectY(u32& Cycles, const Mem& memory) {

            Byte ZPAddress = FetchByte(Cycles, memory);
           
            Word EffectiveAddr = ReadWord(Cycles, ZPAddress, memory);
            Word EffectiveAddrY = EffectiveAddr + Y;

            if (EffectiveAddrY - EffectiveAddr >= 0xFF)
                Cycles--;

            return EffectiveAddrY;

        } // Function AddrIndirectY()
//      =============================

//      ======================================================
        Word AddrIndirectY_6(u32& Cycles, const Mem& memory) {

            Byte ZPAddress = FetchByte(Cycles, memory);

            Word EffectiveAddr = ReadWord(Cycles, ZPAddress, memory);
            Word EffectiveAddrY = EffectiveAddr + Y;

            
            Cycles--;

            return EffectiveAddrY;

        } // Function AddrIndirectY_6()
//      ===============================

    }; // End struct CPU
//  ====================