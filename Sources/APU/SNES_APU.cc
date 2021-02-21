#include "SNES_APU.h"
#include "Bus.h"
#include "SPC700.h"

SNES_APU::SNES_APU(SPC700 &p_spc):spc(p_spc)
{
    //spc.reset();
}

void SNES_APU::attachBus(Bus* p_bus)
{
    bus = p_bus;
}

uint8_t SNES_APU::read(uint16_t adr)
{
    uint8_t data = 0;
    memoryMap(Read, adr, &data);
    return data;
}

void SNES_APU::write(uint16_t adr, uint8_t data)
{
    memoryMap(Write, adr, &data);
}


void SNES_APU::tick()
{
    spc.tick();


    /*oa0 = ia0;
    if(ia1 == 0 && dataToProcess) //entry command
    {
        cout << "[APU] entry command triggered"<<endl;
        dataToProcess = false;
        oa0 = 0xAA;
        oa1 = 0xBB;
        ia0 = oa0;
    }*/
}

void SNES_APU::memoryMap(MemoryOperation op, uint32_t full_adr, uint8_t *data)
{
    uint16_t adr = full_adr; //Theoretically useless bit it's shorter to write

    doMemoryOperation(op,&ram[adr],data);
    if(control.bootROMEnabled() && op == Read && adr >= 0xFFC0)
    {
        *data = bootROM[adr-0xFFC0];
    }
    if(adr >= 0x00F0 && adr <= 0x00FF)//IO ports
    {
        switch(adr & 0xFF)
        {
        case 0xF0:
            break;
        case 0xF1:
            doMemoryOperation(op,&control.val,data);
            break;
        }
        return;
    }

    /*uint8_t adr = full_adr & 0xFF;
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
    }*/
}
