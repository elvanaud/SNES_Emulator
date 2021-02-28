#include "SNES_PPU.h"
#include "Bus.h"

SNES_PPU::SNES_PPU()
{
}

void SNES_PPU::attachBus(Bus* b)
{
    bus = b;
}

void SNES_PPU::memoryMap(MemoryOperation op, uint32_t full_adr, uint8_t* data)
{
    uint16_t adr = full_adr;
    if(op == Read)
    {
        switch(adr)
        {
        //OAM Access
        case 0x2138:
            if(oamAddress >= 0x200)
            {
                *data = oamHigh[oamAddress&0x1F]; //&31 <=> %32 <=> mirroring
            }
            else
            {
                *data = oamLow[oamAddress]; //oamAddress&0x1FF superfluous
            }

            ++oamAddress;
            oamAddress &= 0x3FF; //Clip to 10 bits
            break;
        //VRAM Access
        case 0x2139:
            *data = vDataReadLow;
            vDataReadLow = readVLow(translateAdr(vAddress));
            incrementVAddress(!vmain.incrementHigh());
            break;
        case 0x213A:
            *data = vDataReadHigh;
            vDataReadHigh = readVHigh(translateAdr(vAddress));
            incrementVAddress(vmain.incrementHigh());
            break;
        //CGRAM Access
        case 0x213B:
            *data = cgram[cgAddress];
            ++cgAddress;
            cgAddress &= 0x1FF; //Clip to 9 bits
            break;
        }
    }
    else //Write
    {
        switch(adr)
        {
        //OAM Access
        case 0x2102: //TODO: ppu timing: reload this during v/f-blank
            oamReloadAdr.lowVal = *data;
            oamAddress = oamReloadAdr.fullAdr();
            break;
        case 0x2103:
            oamReloadAdr.highVal = *data;
            oamAddress = oamReloadAdr.fullAdr();
            break;
        case 0x2104:
            if((oamAddress&1) == 0)//even address
            {
                oamDataLow = *data;
            }
            else if(oamAddress <= 0x1FF)//odd address
            {
                oamLow[oamAddress-1] = oamDataLow;
                oamLow[oamAddress] = *data;
            }
            if(oamAddress >= 0x200)
            {
                oamHigh[oamAddress&0x1F] = *data;
            }
            ++oamAddress;
            oamAddress &= 0x3FF; //Clip to 10 bits
            break;
        //VRAM Access
        case 0x2115: vmain.val = *data; break;
        case 0x2116:
            vAddress &= 0xFF00;
            vAddress |= *data;

            prefetchVRAM(translateAdr(vAddress));
            break;
        case 0x2117:
            vAddress &= 0x00FF;
            vAddress |= (uint16_t(*data)<<8);
            vAddress &= ~(1<<15); //Ignore bit 15

            prefetchVRAM(translateAdr(vAddress));
            break;
        case 0x2118:
            writeVLow(translateAdr(vAddress),*data);
            incrementVAddress(!vmain.incrementHigh());
            break;
        case 0x2119:
            writeVHigh(translateAdr(vAddress),*data);
            incrementVAddress(vmain.incrementHigh());
            break;
        //CGRAM Access
        case 0x2121:
            cgAddress = *data;
            cgAddress <<= 1;
            break;
        case 0x2122:
            if((cgAddress&1)==0) //even address (low byte)
            {
                cgDataLow = *data;
            }
            else
            {
                cgram[cgAddress-1] = cgDataLow;
                cgram[cgAddress] = *data;
            }
            ++cgAddress;
            cgAddress &= 0x1FF; //Clip to 9 bits
            break;
        }
    }
}

void SNES_PPU::tick()
{

}

uint16_t SNES_PPU::translateAdr(uint16_t adr)
{
    int tSize = vmain.translation();
    uint16_t separationMask = (1<<tSize)-1;
    uint16_t translated = (adr&(~separationMask));//Keep upper bits unchanged
    uint16_t rotated = (adr&separationMask);
    uint16_t movedBits = rotated>>(tSize-3);
    rotated <<= 3;
    rotated |= movedBits;
    translated |= rotated;
    return translated;
}

void SNES_PPU::prefetchVRAM(uint16_t adr)
{
    uint16_t prefetch = vram[adr];
    vDataReadHigh = (prefetch>>8);
    vDataReadLow = prefetch;
}

void SNES_PPU::writeVLow(uint16_t adr, uint8_t data)
{
    uint16_t vData = vram[adr];
    vData &= 0xFF00;
    vData |= data;
    vram[adr] = vData;
}

void SNES_PPU::writeVHigh(uint16_t adr, uint8_t data)
{
    uint16_t vData = vram[adr];
    vData &= 0x00FF;
    vData |= (uint16_t(data)<<8);
    vram[adr] = vData;
}

uint8_t SNES_PPU::readVLow(uint16_t adr)
{
    return vram[adr]&0xFF;
}

uint8_t SNES_PPU::readVHigh(uint16_t adr)
{
    return vram[adr]>>8;
}

void SNES_PPU::incrementVAddress(bool doIt)
{
    if(!doIt) return;
    vAddress += vmain.incrementStep();
    vAddress &= ~(1<<15);
}
