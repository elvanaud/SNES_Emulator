#include "Bus.h"
#include "CPU/W65816.h"
#include "APU/SNES_APU.h"
#include "PPU/SNES_PPU.h"
#include "PPU/DmaHandler.h"
#include "CartridgeHeader.h"

#include <SFML/Graphics.hpp>

#include <cstdio>
#include <fstream>
#include <limits>

Bus::Bus(W65816 & c, SNES_APU &p_apu, SNES_PPU &p_ppu, DmaHandler &p_dma)
    : cpu(c), apu(p_apu),ppu(p_ppu),dmaHandler(p_dma), debugger(cpu)
{
    debugger.attachBus(this);
    ppu.attachBus(this);
    dmaHandler.attachBus(this);
}

void Bus::read(uint32_t adr)
{
    if (!cpu.VDA() && !cpu.VPA()) return;

    dmr = privateRead(adr);
}

void Bus::memoryMap(MemoryOperation op, uint32_t full_adr, uint8_t *data)
{
    if(cpu.VDA() && !cpu.VPA())
    {
        accessedAdr = full_adr;
        isDataLoaded = true;
    }

    uint8_t bank = (full_adr>>16)&0xFF;
    uint16_t adr = full_adr & 0xFFFF;

    if(bank == 0x7E || bank == 0x7F) //RAM Access
    {
        doMemoryOperation(op, &ram[bank-0x7E][adr], data);
        return; //Useless...
    }
    else if((bank >= 0x40 && bank <= 0x7D)||(bank >= 0xC0 && bank <= 0xFF)) //HiROM banks
    {
        cartridge.memoryMap(op,full_adr,data);
    }
    else if((bank >= 0x00 && bank <= 0x3F)||(bank >= 0x80 && bank <= 0xBF))
    {
        if(adr >= 0x6000) //LoROM bank and expansion region
        {
            cartridge.memoryMap(op,full_adr,data);
        }
        else //System area
        {
            if(adr < 0x2000) //First 8kB of RAM Mirror
            {
                doMemoryOperation(op, &ram[0][adr], data);
            }
            else if((adr >= 0x2100 && adr <= 0x213F) || adr == 0x4200)
            {
                ppu.memoryMap(op,full_adr,data);
            }
            else if(adr >= 0x2140 && adr <= 0x217F)
            {
                apu.mainBusIO(op,full_adr,data);
            }
            else if(adr == 0x2180)
            {
                doMemoryOperation(op,&ram[WMADDH][(uint16_t(WMADDM)<<8)|WMADDL],data);
                //Increment the address:
                uint32_t address = (uint32_t(WMADDH)<<16)|(uint32_t(WMADDM)<<8)|WMADDL;
                ++address;
                WMADDL = address;
                WMADDM = (address>>8)&0xFF;
                WMADDH = (address>>16)&0x01; //Clip to one bit
            }
            else if(adr == 0x2181)
            {
                doMemoryOperation(op,&WMADDL,data); //TODO: limit this operation in writing only (same for the following addresses)
            }
            else if(adr == 0x2182)
            {
                doMemoryOperation(op,&WMADDM,data);
            }
            else if(adr == 0x2183)
            {
                (*data) &= 0x01; //TODO: Keep this register one bit (maybe this can cause problems when reading this adr)
                doMemoryOperation(op,&WMADDH,data);
            }
            else if((adr >= 0x4300 && adr <= 0x437F)||(adr >= 0x420B && adr <= 0x420C))
            {
                dmaHandler.memoryMap(op,full_adr,data);
            }
        }
    }
}

uint8_t Bus::privateRead(uint32_t full_adr)
{
    uint8_t data = 0;
    memoryMap(Read, full_adr, &data); //TODO: find a way to differentiate this with regular read so that read trigger IO ports don't react to private reads

    return data;
}

void Bus::write(uint32_t full_adr, uint8_t data)
{
    if (!cpu.VDA() && !cpu.VPA()) return;

    dmr = data;
    memoryMap(Write, full_adr, &dmr);
    //Writes in unmapped regions might indicate the presence of an unknown SRAM mapping on the cartridge
}

void Bus::privateWrite(uint32_t full_adr, uint8_t data)
{
    memoryMap(Write, full_adr, &data);
}

uint8_t Bus::DMR()
{
    return dmr;
}

void Bus::copyInMemory(uint32_t adr, vector<uint8_t> const & buffer)
{
    for(auto value : buffer)
    {
        ram[0][adr] = value; //TODO: adapt that in the future
        ++adr;
    }
}

void Bus::loadCartridge(std::string const & path)
{
    std::ifstream input(path, std::ios::binary);
    if(!input)
    {
        std::cout << "ERREUR: Fichier ROM introuvable\n";
        exit(-1);
    }

    //Compute size of ROM file:
    input.ignore( std::numeric_limits<std::streamsize>::max() );
    std::streamsize cartridgeSize = input.gcount();
    input.clear();   //  Since ignore will have set eof.

    int extraHeaderSize = cartridgeSize & 0x3FF;
    if(extraHeaderSize != 0)
    {
        cout << "Extra header present\n";
        if(extraHeaderSize != 0x200)
            cout << "Ill formed header; size="<<extraHeaderSize<<endl;
        cout << "Skipping extra header..."<<endl;
        cartridgeSize -= extraHeaderSize;
    }
    input.seekg(extraHeaderSize, std::ios_base::beg);

    uint8_t firstBank[BANK_SIZE];

    input.read((char*)firstBank,BANK_SIZE);

    input.seekg(extraHeaderSize, std::ios_base::beg); //Reset to beginning of file

    CartridgeHeader header(firstBank,0x007F00);
    cout << "Header loaded\n";
    //TODO: maybe flatten those ifs and add a counter of how many good conditions were obtained
    //We compute that score for each supported mappings/chipset
    //We select the highest score
    //This would probably be complicated for nothing...
    if(header.valid) //Maybe remove that condition to run homebrew games
    {
        cout << "Coherent checksum\n";
        if(header.romSize == cartridgeSize)
        {
            cout << "Expected rom size and Actual Rom Size match." <<endl;
            if(header.mapMode == CartridgeHeader::LoROM)
            {
                cout << "Valid LoROM Header" << endl;

            }
            else
            {
                cout << "Invalid LoROM Header - Expected LoRom but found: ";
                cout << header.mapMode << " from SpeedMapMode: "<<int(header.speedMapMode)<<endl;
            }
        }
        else
        {
            cout << "Rom size doesn't match.\nActual: "<< cartridgeSize <<" (Extra header removed)"<< endl;
            cout << "Expected: " << header.romSize << endl << "Difference: " << int(header.romSize-cartridgeSize)<<endl;
        }
    }
    cout << header.title<<endl;
    cout << "This game uses " << header.ramSize << " bytes of RAM ";
    if(header.battery) cout << "(with battery)";
    cout << endl;

    cartridge.load(input, header);
}

void Bus::triggerNMI()
{
    cpu.triggerNMI();
}

void Bus::run()
{
    sf::RenderWindow app(sf::VideoMode(400,400,32),"SNES Emulator");
    ppu.setRenderWindow(&app);
    unsigned int cpu_clock = 6;
    unsigned int ppu_clock = 4;
    unsigned int global_clock = 0;

    clock_t startTime = clock();
    debugger.debugTrace = false;
    debugger.debugPrint = false;
    debugger.stepMode = false;

    while(app.isOpen())
    {
        if(debugger.executeSystem())
        {
            --cpu_clock;
            --ppu_clock;
            ++global_clock;

            if(cpu_clock == 0)
            {
                cpu_clock = 6;

                if(!dmaEnabled)
                {
                    cpu.tick();
                    debugger.tick();
                }
            }
            if(ppu_clock == 0)
            {
                ppu.tick();
                ppu_clock = 4; //TODO: handle long dots and stuff
            }
            apu.tick();
            dmaHandler.tick();
        }
        /*if(global_clock >= 21148004)
        {
            app.close();
        }*/
        if(debugger.checkEvents())//todo:replace with ClockManager
        {
            sf::Event event;
            while(app.pollEvent(event))
            {
                if(event.type == sf::Event::Closed)
                    app.close();
                else if(event.type == sf::Event::KeyPressed)
                {
                    switch(event.key.code)
                    {
                    default: break;
                    case sf::Keyboard::B:
                    case sf::Keyboard::D:
                    case sf::Keyboard::S:
                    case sf::Keyboard::C:
                    case sf::Keyboard::W:
                        debugger.processEvent(event);
                        break;
                    }
                }
            }
        }
    }
    clock_t endTime = clock();
    double duration = ((double)(endTime-startTime)) / CLOCKS_PER_SEC;
    cout << "\nTime:" << duration << " Nb inst: " << debugger.nbExecutedInstructions << " Nb cycles:" << global_clock << endl;
    cout << "Freq: " << global_clock / duration << endl;

    debugger.saveTrace("../Logs/emul.log");//probably too much memory -> need to save it in multiple segments
}

void Bus::dmaEnable(bool enable)
{
    dmaEnabled = enable;
}
