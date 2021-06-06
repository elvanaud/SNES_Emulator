#include "CartridgeHeader.h"
#include <iostream>

CartridgeHeader::CartridgeHeader(){}

CartridgeHeader::CartridgeHeader(uint8_t bank[BANK_SIZE], uint32_t headerOffset)
{
    //Offset definitions
    const int oTitle = 0xC0;
    const int oLastByteOfTitle = 0xD4;
    const int oSpeedMapMode = 0xD5;
    const int oChipset = 0xD6;
    const int oROMSize = 0xD7;
    const int oRAMSize = 0xD8;
    const int oCountry = 0xD9;
    const int oDevIdCode = 0xDA;
    const int oROMVersion = 0xDB;
    const int oChecksumComplement = 0xDC;
    const int oChecksum = 0xDE;

    //Extended header
    const int oMakerCode = 0xB0;
    const int oGameCode = 0xB2;
    const int oExpFlashSize = 0xBC;
    const int oExpRAMSize = 0xBD;
    const int oSpecialVersion = 0xBE;

    const int oChipsetSubType = 0xBF;

    earlyExtendedHeader = bank[headerOffset+oLastByteOfTitle] == 0;
    extendedHeader = bank[headerOffset+oDevIdCode] == 0x33;

    for(int i = oTitle; i <= oLastByteOfTitle; ++i)
    {
        title += (char) bank[headerOffset+i];
    }

    speedMapMode = bank[headerOffset+oSpeedMapMode];
    fastROM = (speedMapMode >> 4)&1;

    switch(speedMapMode & 0x0F)
    {
        case 0: mapMode = LoROM; break;
        case 1: mapMode = HiROM; break;
        default: mapMode = unsupportedMapMode;
    }

    //Chipset Info
    chipset = bank[headerOffset+oChipset];
    chipsetSubType = bank[headerOffset+oChipsetSubType];
    switch(chipset & 0x0F)
    {
        case 0x00: break; //Default value
        case 0x01: ram = true; break;
        case 0x02: ram = battery = true; break;
        case 0x03: coprocessor = true; break;
        case 0x04: coprocessor = ram = true; break;
        case 0x05: coprocessor = ram = battery = true; break;
        case 0x06: coprocessor = battery = true; break;
        case 0x09:
        case 0x0A: coprocessor = ram = battery = true; unsupportedChipset = true; break;
        default:
            unsupportedChipset = true;
    }
    switch(chipset>>4)
    {
        case 0x00: break; //DSP (default) ?
        default:
            unsupportedChipset = true;
            coprocessor = unsupportedCoprocessor;
    }

    //ROM/RAM size
    romSize = (1 << bank[headerOffset+oROMSize])*1024;
    ramSize = (1 << bank[headerOffset+oRAMSize])*1024;
    if(ramSize == 1024) ramSize = 0; //A value of 0 indicates no ram instead of 1024 bytes

    //Country PAL/NTSC
    country = (Country)bank[headerOffset+oCountry];
    switch(country)
    {
    case Japan:
    case USA_Canada:
    case South_Korea:
    case Canada:
    case Brazil:
        tvStandard = NTSC;
    default:
        tvStandard = PAL;
    }

    //Codes and version
    devIDCode = bank[headerOffset+oDevIdCode];
    romVersion = bank[headerOffset+oROMVersion];

    //Checksum:
    valid = (bank[headerOffset+oChecksum] ^ 0xFF) == bank[headerOffset+oChecksumComplement];

    //Extended header
    makerCode += bank[headerOffset+oMakerCode+0];
    makerCode += bank[headerOffset+oMakerCode+1];

    for(int i = 0; i < 4; ++i)
        gameCode += bank[headerOffset+oGameCode+i];

    expFlashSize = (1 << bank[headerOffset+oExpFlashSize])*1024;
    expRAMSize = (1 << bank[headerOffset+oExpRAMSize])*1024;
    specialVersion = bank[headerOffset+oSpecialVersion];
}
