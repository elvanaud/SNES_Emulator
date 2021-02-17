#ifndef CARTRIDGEHEADER_H
#define CARTRIDGEHEADER_H

#include "Bus.h"

#include <string>
using std::string;

struct CartridgeHeader
{
    CartridgeHeader(uint8_t bank[BANK_SIZE], uint32_t headerOffset);

    bool valid = false; //Only tells if checksum and complement match
    bool earlyExtendedHeader = false;
    bool extendedHeader = false;

    string title;
    uint8_t speedMapMode = 0;
    bool fastROM = false;

    enum MapMode {LoROM, HiROM, unsupportedMapMode}; //Mode 20 and Mode 21
    MapMode mapMode;

    //Chipset info:
    bool unsupportedChipset = false;
    uint8_t chipset = 0;
    uint8_t chipsetSubType = 0;
    bool ram = false;
    bool battery = false;
    bool coprocessor = false;
    enum CoprocessorType {unsupportedCoprocessor};
    CoprocessorType coprocessorType = unsupportedCoprocessor;

    //ROM/RAM Size
    int romSize = 0; //Value rounded up for 10,12,20,24Mbits chips
    int ramSize = 0;

    //Country and PAL/NTSC
    enum Country{   International = 0, Japan, USA_Canada, Europe_Oceania_Asia, Scandinavia,
                    Finland, Denmark, France, Holland, Spain, Germany_Austria_Switz, Italy,
                    China, Indonesia, South_Korea, Common_Unknown, Canada, Brazil, Australia, UnknownCountry};
    Country country = UnknownCountry;

    enum TVStandard{PAL, NTSC};
    TVStandard tvStandard = PAL;

    //Codes and versions
    uint8_t devIDCode = 0;
    uint8_t romVersion = 0;

    //Extended header
    string makerCode;
    string gameCode;

    int expFlashSize = 0;
    int expRAMSize = 0;

    uint8_t specialVersion = 0;
};

#endif // CARTRIDGEHEADER_H
