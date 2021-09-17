#pragma once

#include <iostream>

#include "CPU.h"
#include "StatusFlags.h"
#include "Opcodes.h"
#include "Memory.h"

#define ASSERT(Condition, Text) {if (!Condition){throw -1; }}

//  ===================
    using SByte = char;
    using Byte = unsigned char;
    using Word = unsigned short;
    using u32 = unsigned int;
    using s32 = signed   int;
//  =========================

        CPU::CPU() {

            

        }
        
        CPU::~CPU() {




        }

        static union {

            Byte PS;
            StatusFlags Flag;

        };

//      =================================
        void CPU::Reset(Memory& memory) {

            PC = 0xFFFC;
            SP = 0xFF;
            Flag.D = 0;
            Flag.C = Flag.Z = Flag.I = Flag.D = Flag.B = Flag.V = Flag.N = 0;
            A = X = Y = 0;
            memory.Initialise();

        } // function Reset()
//      =====================

//      ========================================================
        Byte CPU::FetchByte(s32& Cycles, const Memory& memory) {

            Byte Data = memory[PC];
            PC++;
            Cycles--;

            return Data;

        } // Function Fetch()
//      =====================

//      ================================================
        SByte CPU::FetchSByte(s32& Cycles, const Memory& memory) {

            return FetchByte(Cycles, memory);

        } // Function Fetch()
//      =====================

//      ================================================
        Word CPU::FetchWord(s32& Cycles, const Memory& memory) {


            // 6502 is little endian
            Word Data = memory[PC];
            PC++;

            Data |= (memory[PC] << 8);
            PC++;
            Cycles -= 2;

            return Data;

        } // Function FetchWord()
//      =========================

//      =====================================================================
        Byte CPU::ReadByte(s32& Cycles, Word Address, const Memory& memory) {

            Byte Data = memory[Address];
            Cycles--;
            return Data;

        } // Function ReadByte()
//      ========================

//      =============================================================
        Byte CPU::ReadWord(s32& Cycles, Word Address, const Memory& memory) {

            Byte loByte = ReadByte(Cycles, Address, memory);
            Byte hiByte = ReadByte(Cycles, Address + 1, memory);

            return loByte | (hiByte << 8);

        } // Function ReadWord()
//      ========================

//      Write 1 byte to memory
//      ====================================================================
        void CPU::WriteByte(Byte Value, s32& Cycles, Word Address, Memory& memory) {

            memory[Address] = Value;
            Cycles--;

        } // Function WriteByte()
//      =========================

//      Write 2 bytes to memory
//      ==========================================================================
        void CPU::WriteWord(Word DataToWrite, s32& Cycles, Word Address, Memory& memory) {

            memory[Address] = DataToWrite & 0xFF;
            memory[Address + 1] = (DataToWrite >> 8);
            Cycles -= 2;

        } // Function WriteWord()
//      =========================

//      Return the stack pointer as a full 16-bit address in the 1st page
//      ==========================
        Word CPU::SPToAddress() const {

            return 0x100 | SP;

        } // Function SPToWord()
//      ========================

//      Push the PC-1 onto the stack
//      ======================================================
        void CPU::PushPCMinusOneToStack(s32& Cycles, Memory& memory) {

            PushWordToStack(Cycles, memory, PC - 1);

        } // Function PushPCMinusOneToStack()
//      =====================================

//      ==============================================
        void CPU::PushPCToStack(s32& Cycles, Memory& memory) {

            PushWordToStack(Cycles, memory, PC);

        } // Function PushPCToStack()
//      =============================

//      =====================================================
        void CPU::PushPCPlusOneToStack(s32& Cycles, Memory& memory) {

            PushWordToStack(Cycles, memory, PC + 1);

        } // Function PushPCPlusOneToStack()
//      ====================================

//      ============================================================
        void CPU::PushWordToStack(s32& Cycles, Memory& memory, Word Value) {

            WriteByte(Value >> 8, Cycles, SPToAddress(), memory);

            SP--;

            WriteByte(Value & 0xFF, Cycles, SPToAddress(), memory);

            SP--;

        } // Function PushWordToStack()
//      ===============================

//      ==============================================================
        void CPU::PushByteOntoStack(s32& Cycles, Byte Value, Memory& memory) {

            Word SP16Bit = SPToAddress();
            memory[SP16Bit] = Value;

            Cycles--;
            SP--;
            Cycles--;


        } // Function PushByteOntoStack()
//      =================================

//      =========================================================
        Byte CPU::PopByteFromStack(s32& Cycles, Memory& memory) {

            SP++;
            Cycles--;

            Word SP16Bit = SPToAddress();
            Byte Value = memory[SP16Bit];

            Cycles--;

            return Value;

        } // Function PopByteFromStack()
//      =================================

//      =========================================================
        Word CPU::PopWordFromStack(s32& Cycles, Memory& memory) {

            Word ValueFromStack = ReadWord(Cycles, SPToAddress() + 1, memory);
            SP += 2;

            Cycles--;

            return ValueFromStack;

        } // Function PopWordFromStack()
//      ================================

        static constexpr Byte

            NegativeFlagBit = 0b10000000,
            OverflowFlagBit = 0b01000000,

            BreakFlagBit = 0b000010000,
            UnusedFlagBit = 0b000100000,

            ZeroBit = 0b00000001;

//      ==================================================
        void CPU::SetZeroAndNegativeFlags(Byte Register) {

            Flag.Z = (Register == 0);
            Flag.N = (Register & 0b10000000) > 0;

        } // Function LDASetStatus()
//      ============================

//      ==============================================
        s32 CPU::Execute(s32 Cycles, Memory& memory) {

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

                    const bool PageChanged = (PC >> 8) != (OldPC >> 8);

                    if (PageChanged) { // page changed

                        Cycles--;

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

            // Arithmetic shift left
            auto ASL = [&Cycles, this](Byte Operand) -> Byte {

                Flag.C = (Operand & NegativeFlagBit) > 0;
                Byte Result = Operand << 1;
                SetZeroAndNegativeFlags(Result);
                Cycles--;

                return Result;

            };

            // Logical shift right
            auto LSR = [&Cycles, this](Byte Operand) -> Byte {

                Flag.C = (Operand & ZeroBit) > 0;
                Byte Result = Operand >> 1;
                SetZeroAndNegativeFlags(Result);
                Cycles--;

                return Result;
            };

            // Rotate left

            auto ROL = [&Cycles, this](Byte Operand) -> Byte {

                Byte NewBit0 = Flag.C ? ZeroBit : 0;
                Flag.C = (Operand & NegativeFlagBit) > 0;

                Operand = Operand << 1;
                Operand |= NewBit0;
                SetZeroAndNegativeFlags(Operand);
                Cycles--;

                return Operand;

            };

            auto ROR = [&Cycles, this](Byte Operand) -> Byte {

                bool OldBit0 = (Operand & ZeroBit) > 0;

                Operand = Operand >> 1;

                if (Flag.C) {

                    Operand |= NegativeFlagBit;

                }

                Cycles--;

                Flag.C = OldBit0;

                SetZeroAndNegativeFlags(Operand);

                return Operand;

            };

            auto PushPSToStack = [&Cycles, &memory, this]() {

                Byte PS_Stack = PS | BreakFlagBit | UnusedFlagBit;

                PushByteOntoStack(Cycles, PS_Stack, memory);

            };

            auto PopPSFromStack = [&Cycles, &memory, this]() {

                PS = PopByteFromStack(Cycles, memory);

                Flag.B = false;
                Flag.Unused = false;

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

                    PushPCMinusOneToStack(Cycles, memory);


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

                case INS_PHA: {

                    PushByteOntoStack(Cycles, A, memory);
                    Cycles--;

                }

                            break;

                case INS_PLA: {

                    A = PopByteFromStack(Cycles, memory);
                    SetZeroAndNegativeFlags(A);
                    Cycles--;
                }
                            break;

                case INS_PHP: {


                    PushPSToStack();

                }

                            break;

                case INS_PLP: {

                    PopPSFromStack();
                    Cycles--;

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

                    SBC(Operand);

                }

                                break;

                case INS_SBC_ABSX: {

                    Word Address = AddrAbsoluteX(Cycles, memory);

                    Byte Operand = ReadByte(Cycles, Address, memory);

                    SBC(Operand);

                }

                                 break;

                case INS_SBC_ABSY: {

                    Word Address = AddrAbsoluteY(Cycles, memory);

                    Byte Operand = ReadByte(Cycles, Address, memory);

                    SBC(Operand);

                }

                                 break;

                case INS_SBC_INDX: {

                    Word Address = AddrIndirectX(Cycles, memory);

                    Byte Operand = ReadByte(Cycles, Address, memory);

                    SBC(Operand);

                }

                                 break;

                case INS_SBC_INDY: {

                    Word Address = AddrIndirectY(Cycles, memory);

                    Byte Operand = ReadByte(Cycles, Address, memory);

                    SBC(Operand);

                }

                                 break;

                case INS_SBC: {

                    Byte Operand = FetchByte(Cycles, memory);

                    SBC(Operand);

                }

                            break;

                case INS_SBC_ZP: {

                    Word Address = AddrZeroPage(Cycles, memory);

                    Byte Operand = ReadByte(Cycles, Address, memory);

                    SBC(Operand);

                }

                               break;

                case INS_SBC_ZPX: {

                    Word Address = AddrZeroPageX(Cycles, memory);

                    Byte Operand = ReadByte(Cycles, Address, memory);

                    SBC(Operand);

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

                case INS_ASL: {

                    A = ASL(A);

                }

                            break;

                case INS_ASL_ZP: {

                    Word Address = AddrZeroPage(Cycles, memory);
                    Byte Operand = ReadByte(Cycles, Address, memory);

                    Byte Result = ASL(Operand);

                    WriteByte(Result, Cycles, Address, memory);

                }

                               break;

                case INS_ASL_ZPX: {

                    Word Address = AddrZeroPageX(Cycles, memory);
                    Byte Operand = ReadByte(Cycles, Address, memory);

                    Byte Result = ASL(Operand);

                    WriteByte(Result, Cycles, Address, memory);

                }

                                break;

                case INS_ASL_ABS: {

                    Word Address = AddrAbsolute(Cycles, memory);
                    Byte Operand = ReadByte(Cycles, Address, memory);

                    Byte Result = ASL(Operand);

                    WriteByte(Result, Cycles, Address, memory);

                }

                                break;

                case INS_ASL_ABSX: {

                    Word Address = AddrAbsoluteX_5(Cycles, memory);
                    Byte Operand = ReadByte(Cycles, Address, memory);

                    Byte Result = ASL(Operand);

                    WriteByte(Result, Cycles, Address, memory);

                }

                                 break;

                case INS_LSR: {

                    A = LSR(A);

                }

                            break;

                case INS_LSR_ZP: {

                    Word Address = AddrZeroPage(Cycles, memory);
                    Byte Operand = ReadByte(Cycles, Address, memory);
                    Byte Result = LSR(Operand);

                    WriteByte(Result, Cycles, Address, memory);

                }

                               break;

                case INS_LSR_ZPX: {

                    Word Address = AddrZeroPageX(Cycles, memory);
                    Byte Operand = ReadByte(Cycles, Address, memory);
                    Byte Result = LSR(Operand);

                    WriteByte(Result, Cycles, Address, memory);

                }

                                break;

                case INS_LSR_ABS: {

                    Word Address = AddrAbsolute(Cycles, memory);
                    Byte Operand = ReadByte(Cycles, Address, memory);
                    Byte Result = LSR(Operand);

                    WriteByte(Result, Cycles, Address, memory);

                }

                                break;

                case INS_LSR_ABSX: {

                    Word Address = AddrAbsoluteX_5(Cycles, memory);
                    Byte Operand = ReadByte(Cycles, Address, memory);
                    Byte Result = LSR(Operand);

                    WriteByte(Result, Cycles, Address, memory);

                }

                                 break;

                case INS_ROL: {

                    A = ROL(A);

                }

                            break;

                case INS_ROL_ZP: {

                    Word Address = AddrZeroPage(Cycles, memory);
                    Byte Operand = ReadByte(Cycles, Address, memory);

                    Byte Result = ROL(Operand);

                    WriteByte(Operand, Cycles, Address, memory);

                }

                               break;

                case INS_ROL_ZPX: {

                    Word Address = AddrZeroPageX(Cycles, memory);
                    Byte Operand = ReadByte(Cycles, Address, memory);

                    Byte Result = ROL(Operand);

                    WriteByte(Operand, Cycles, Address, memory);

                }

                                break;

                case INS_ROL_ABS: {

                    Word Address = AddrAbsolute(Cycles, memory);
                    Byte Operand = ReadByte(Cycles, Address, memory);

                    Byte Result = ROL(Operand);

                    WriteByte(Operand, Cycles, Address, memory);

                }

                                break;

                case INS_ROR: {

                    A = ROR(A);

                }

                            break;

                case INS_ROR_ZP: {

                    Word Address = AddrZeroPage(Cycles, memory);
                    Byte Operand = ReadByte(Cycles, Address, memory);

                    Byte Result = ROR(Operand);


                    WriteByte(Operand, Cycles, Address, memory);


                }
                               break;

                case INS_ROR_ZPX: {

                    Word Address = AddrZeroPageX(Cycles, memory);
                    Byte Operand = ReadByte(Cycles, Address, memory);

                    Byte Result = ROR(Operand);


                    WriteByte(Operand, Cycles, Address, memory);


                }
                                break;

                case INS_ROR_ABS: {

                    Word Address = AddrAbsolute(Cycles, memory);
                    Byte Operand = ReadByte(Cycles, Address, memory);

                    Byte Result = ROR(Operand);


                    WriteByte(Operand, Cycles, Address, memory);

                }
                                break;

                case INS_ROR_ABSX: {

                    Word Address = AddrAbsoluteX_5(Cycles, memory);
                    Byte Operand = ReadByte(Cycles, Address, memory);

                    Byte Result = ROR(Operand);

                    WriteByte(Operand, Cycles, Address, memory);

                }

                                 break;

                case INS_BRK: {

                    PushPCPlusOneToStack(Cycles, memory);
                    constexpr bool SetIntDisableFlagAfter = true;

                    PushPSToStack();

                    constexpr Word InterruptVector = 0xFFFE;
                    PC = ReadWord(Cycles, InterruptVector, memory);
                    Flag.B = true;
                    Flag.I = true;

                }

                            break;

                case INS_RTI: {

                    PopPSFromStack();

                    PC = PopWordFromStack(Cycles, memory);

                }

                            break;

                case INS_ROL_ABSX: {

                    Word Address = AddrAbsoluteX_5(Cycles, memory);
                    Byte Operand = ReadByte(Cycles, Address, memory);

                    Byte Result = ROL(Operand);

                    WriteByte(Operand, Cycles, Address, memory);

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

                case INS_STX_ZPY: {

                    Word Address = AddrZeroPageY(Cycles, memory);
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

                    throw - 1;
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
        Word CPU::AddrZeroPage(s32& Cycles, const Memory& memory) {

            Byte ZeroPageAddress = FetchByte(Cycles, memory);

            return ZeroPageAddress;

        } // Function AddrZeroPage()
//      ============================

//      ====================================================
        Word CPU::AddrZeroPageX(s32& Cycles, const Memory& memory) {

            Byte ZeroPageAddress = FetchByte(Cycles, memory);
            ZeroPageAddress += X;
            Cycles--;

            return ZeroPageAddress;

        } // Function AddrZeroPageX()
//      ============================

//      ====================================================
        Word CPU::AddrZeroPageY(s32& Cycles, const Memory& memory) {

            Byte ZeroPageAddress = FetchByte(Cycles, memory);
            ZeroPageAddress += Y;
            Cycles--;

            return ZeroPageAddress;

        } // Function AddrZeroPageY()
//      =============================

//      ===================================================
        Word CPU::AddrAbsolute(s32& Cycles, const Memory& memory) {

            Word AbsAddress = FetchWord(Cycles, memory);

            return AbsAddress;

        } // Function AddrAbsolute()
//      ============================
// 
//      ====================================================
        Word CPU::AddrAbsoluteX(s32& Cycles, const Memory& memory) {

            Word AbsAddress = FetchWord(Cycles, memory);
            Word AbsAddressX = AbsAddress + X;

            bool CrossedPageBoundary = (AbsAddress ^ AbsAddressX) >> 8;

            if (CrossedPageBoundary) {

                Cycles--;

            }

            return AbsAddressX;

        } // Function AddrAbsoluteX()
//      ============================

//      ======================================================
        Word CPU::AddrAbsoluteX_5(s32& Cycles, const Memory& memory) {

            Word AbsAddress = FetchWord(Cycles, memory);
            Word AbsAddressX = AbsAddress + X;

            Cycles--;

            return AbsAddressX;

        } // Function AddrAbsoluteX_5()
//      ============================

//      ====================================================
        Word CPU::AddrAbsoluteY(s32& Cycles, const Memory& memory) {

            Word AbsAddress = FetchWord(Cycles, memory);
            Word AbsAddressY = AbsAddress + Y;

            bool CrossedPageBoundary = (AbsAddress ^ AbsAddressY) >> 8;

            if (CrossedPageBoundary) {
                Cycles--;
            }

            return AbsAddressY;

        } // Function AddrAbsoluteY()
//      =============================

//      ======================================================
        Word CPU::AddrAbsoluteY_5(s32& Cycles, const Memory& memory) {

            Word AbsAddress = FetchWord(Cycles, memory);
            Word AbsAddressY = AbsAddress + Y;

            Cycles--;

            return AbsAddressY;

        } // Function AddrAbsoluteY_5()
//      =============================

//      ====================================================
        Word CPU::AddrIndirectX(s32& Cycles, const Memory& memory) {

            Byte ZPAddress = FetchByte(Cycles, memory);
            ZPAddress += X;
            Cycles--;
            Word EffectiveAddr = ReadWord(Cycles, ZPAddress, memory);

            return EffectiveAddr;

        } // Function AddrIndirectX()
//      =============================

//      ====================================================
        Word CPU::AddrIndirectY(s32& Cycles, const Memory& memory) {

            Byte ZPAddress = FetchByte(Cycles, memory);

            Word EffectiveAddr = ReadWord(Cycles, ZPAddress, memory);
            Word EffectiveAddrY = EffectiveAddr + Y;

            bool CrossedPageBoundary = (EffectiveAddr ^ EffectiveAddrY) >> 8;

            if (CrossedPageBoundary) {
                Cycles--;
            }

            return EffectiveAddrY;

        } // Function AddrIndirectY()
//      =============================

//      ======================================================
        Word CPU::AddrIndirectY_6(s32& Cycles, const Memory& memory) {

            Byte ZPAddress = FetchByte(Cycles, memory);

            Word EffectiveAddr = ReadWord(Cycles, ZPAddress, memory);
            Word EffectiveAddrY = EffectiveAddr + Y;

            Cycles--;

            return EffectiveAddrY;

        } // Function AddrIndirectY_6()
//      ===============================

//      ============================================================
        Word CPU::LoadProgram(Byte* Program, u32 NumBytes, Memory& memory) {

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