#include "MemoryInterface.h"

MemoryInterface::MemoryInterface()
{
    //ctor
}

void MemoryInterface::doMemoryOperation(MemoryOperation op, uint8_t* dataAdr, uint8_t* data)
{
    if(op == Read)
        *data = *dataAdr;
    if(op == Write)
        *dataAdr = *data;
}
