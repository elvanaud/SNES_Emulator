#ifndef SNES_PPU_H
#define SNES_PPU_H

#include "Common.h"
#include <SFML/Graphics.hpp>

#include <array>

#include "MemoryInterface.h"

class Bus;

const unsigned int VRAM_WORD_SIZE = HALF_BANK_SIZE;
const unsigned int OAM_LOW_SIZE = 512;
const unsigned int OAM_HIGH_SIZE = 32;
const unsigned int CGRAM_SIZE = 512;

class SNES_PPU : MemoryInterface
{
public:
    SNES_PPU();
    void attachBus(Bus* b);
    virtual void memoryMap(MemoryOperation op, uint32_t full_adr, uint8_t* data);
    void tick();
    void setRenderWindow(sf::RenderWindow* p_renderWindow);
    uint16_t hcounter = -1; //auto increment when starting
    uint16_t vcounter = 0;
private:
    Bus* bus;
    void renderScreen();
    sf::RenderWindow * renderWindow;
    sf::Texture screenTexture;
    int idx = 0;
    uint8_t screenContent[350*250*4];

    uint16_t vram[VRAM_WORD_SIZE];
    uint8_t oamLow[OAM_LOW_SIZE]; //OAM is supposed to be 16bit words
    uint8_t oamHigh[OAM_HIGH_SIZE];
    uint8_t cgram[CGRAM_SIZE];

    

    bool forcedBlank = false;
    enum RenderState {HBLANK,VBLANK,RENDERING}; //Technically H-Blank still happens during VBlank
    RenderState renderState = RENDERING;

    void handleTimings();

    uint16_t translateAdr(uint16_t adr);
    void prefetchVRAM(uint16_t adr);
    void writeVHigh(uint16_t adr, uint8_t data);
    void writeVLow(uint16_t adr, uint8_t data);
    uint8_t readVHigh(uint16_t adr);
    uint8_t readVLow(uint16_t adr);
    void incrementVAddress(bool doIt);

    //OAM Access
    uint16_t oamAddress = 0; //10bit byte address
    uint8_t oamDataLow = 0;
    struct { //TODO: sprite priority stuff
        uint8_t lowVal = 0;
        uint8_t highVal = 0;
        int fullAdr() {return (((uint16_t(highVal)&1)<<8)|lowVal)<<1;}//This is a word address in oam
    } oamReloadAdr; //$2102/3

    //VRAM Access
    struct
    {
        uint8_t val = 0x0F;
        bool incrementHigh(){return (val>>7)&1;}
        int translation()   {return std::array<int,4>{0,8,9,10}[(val>>2)&0x03];}
        int incrementStep() {return std::array<int,4>{1,32,128,128}[val&0x03];}
    } vmain; //$2115

    uint16_t vAddress = 0; //$2116-7 ; Bit 15 ignored

    uint8_t vDataReadLow = 0; //$2139
    uint8_t vDataReadHigh = 0; //$213A

    //CGRAM Access
    uint8_t cgDataLow = 0;
    uint16_t cgAddress = 0; //9bit word address

    bool vblankInteruptEnabled = false;
};

#endif // SNES_PPU_H
