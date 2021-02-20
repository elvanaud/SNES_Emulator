#include "SNES_APU.h"
#include "Bus.h"

SNES_APU::SNES_APU()
{
    //ctor
}

void SNES_APU::attachBus(Bus* p_bus)
{
    bus = p_bus;
}

void SNES_APU::tick()
{
    oa0 = ia0;
    if(ia1 == 0 && dataToProcess) //entry command
    {
        cout << "[APU] entry command triggered"<<endl;
        dataToProcess = false;
        oa0 = 0xAA;
        oa1 = 0xBB;
        ia0 = oa0;
    }
}

void SNES_APU::memoryMap(MemoryOperation op, uint32_t full_adr, uint8_t *data)
{
    uint8_t adr = full_adr & 0xFF;
    if(op == Read)//Read from the point of view of the main cpu, so this maps to out registers
    {
        switch(adr)
        {
            case 0x40:*data = oa0; break;
            case 0x41:*data = oa1; break;
            case 0x42:*data = oa2; break;
            case 0x43:*data = oa3; break;
        }
    }
    else
    {
        switch(adr)
        {
            case 0x40:ia0 = *data; dataToProcess = true; break;
            case 0x41:ia1 = *data; break;
            case 0x42:ia2 = *data; break;
            case 0x43:ia3 = *data; break;
        }
    }
}
