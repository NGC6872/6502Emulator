#include "Memory.h"

Memory::Memory()
{


}

Memory::~Memory()
{
}

//   		===================
            void Memory::Initialise() {

                for (u32 ii = 0; ii < MAX_MEM; ii++) {

                    Data[ii] = 0;

                }

            } // Function Initialise()
    //      ==========================
    // 
    //      ==================================== read one byte
            Byte Memory::operator[](u32 Address) const {

                return Data[Address];

            } // Function() (read one byte)

    //      =============================== read one byte
            Byte& Memory::operator[](u32 Address) {

                return Data[Address];

            } // Function (read one byte)
  