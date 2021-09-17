#pragma once

    struct StatusFlags {

        Byte C : 1;      // Carry flag
        Byte Z : 1;      // Zero flag
        Byte I : 1;      // Interrupt Disable
        Byte D : 1;      // Decimal Mode
        Byte B : 1;      // Break
        Byte Unused : 1; // Unused
        Byte V : 1;      // Overflow
        Byte N : 1;      // Negative

    };