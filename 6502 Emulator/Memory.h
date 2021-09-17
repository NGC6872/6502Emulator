#pragma once
//#include "StatusFlags.h"

//  ===================
    using SByte = char;
    using Byte = unsigned char;
    using Word = unsigned short;
    using u32 = unsigned int;
    using s32 = signed   int;
//  =========================
class Memory
{

    public:

        Memory();
        ~Memory();

        void Initialise();

        Byte operator[](u32 Address) const;

        Byte& operator[](u32 Address);

	public:

            static constexpr u32 MAX_MEM = 1024 * 64;
            Byte Data[MAX_MEM];

};

