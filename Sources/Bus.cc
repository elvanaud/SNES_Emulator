#include "Bus.h"
#include "CPU/W65816.h"
#include "CartridgeHeader.h"

#include <SFML/Graphics.hpp>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <limits>
using std::cout;
using std::endl;

Bus::Bus(W65816 & c) : cpu(c), debugger(cpu)
{
    /*
    ram[0xFF]  = 0x38; //SEC
    ram[0x100] = 0x69; //ADC
    ram[0x101] = 19;
    ram[0x102] = 0x69; //ADC
    ram[0x103] = 55;
    ram[0x104] = 0x29; //AND
    ram[0x105] = 0x3F;
    ram[0x106] = 0x89; //BIT
    ram[0x107] = 0x00;
    ram[0x108] = 0x89; //BIT
    ram[0x109] = 0x02;
    ram[0x10A] = 0xC9; //CMP
    ram[0x10B] = 0x0A;
    ram[0x10C] = 0xA9; //LDA #42
    ram[0x10D] = 42;*/
}

void Bus::read(uint32_t adr)
{
    if (!cpu.VDA() && !cpu.VPA()) return;

    dmr = privateRead(adr);
}

void Bus::memoryMap(MemoryOperation op, uint32_t full_adr, uint8_t *data)
{
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
        }
    }
}

uint8_t Bus::privateRead(uint32_t full_adr)
{
    uint8_t data = 0;
    memoryMap(Read, full_adr, &data);

    return data;
}

void Bus::write(uint32_t full_adr, uint8_t data)
{
    if (!cpu.VDA() && !cpu.VPA()) return;

    //dmr = data;
    memoryMap(Write, full_adr, &dmr);
    //Writes in unmapped regions might indicate the presence of an unknown SRAM mapping on the cartridge
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

void Bus::run()
{
    sf::RenderWindow app(sf::VideoMode(300,300,32),"SNES Emulator");
    unsigned int clock = 6;
    unsigned int global_clock = 0;

    bool debugPrint = true;
    bool stepMode = false;
    bool step = false;

    uint16_t oldPC = cpu.getPC();
    while(app.isOpen())
    {
        if(!stepMode || (stepMode && step))
        {
            --clock;
            ++global_clock;
            /*if(debugger.isExecutionBlocked())
            {
                cout << "CPU Blocked" << endl;
                getchar();
                debugger.continueExec();
            }*/
            //cout << "TCycle = " << cpu.getTCycle() << endl;
            if(clock == 0)
            {
                cpu.tick();
                clock = 6;

                //debugger.tick();

                //cout << "PC = " <<std::hex << cpu.getPC() << "  ;  IR = " << (int)cpu.getIR() << "("<<cpu.getInst().getASM() << ")  ;  Acc = " << cpu.getAcc() << "  ;  Adr = " << cpu.getAdr();
                //cout << "  ;  IDB = " << cpu.getIDB() <<endl;
                /*uint8_t p = cpu.getP();
                string status;
                if((p>>7)&1) status+="N"; else status += "-";
                if((p>>6)&1) status+="V"; else status += "-";
                if((p>>1)&1) status+="Z"; else status += "-";
                if((p>>0)&1) status+="C"; else status += "-";*/
                //cout << "Flags = " << status << endl;
                //cout << "VDA = " << cpu.VDA() << "  ;  VPA = " << cpu.VPA() << endl;
                if(cpu.VDA() && cpu.VPA()) //Sync = Opcode Fetch
                {

                    //if(!stepMode || (stepMode && step))
                    {
                        step = false;
                        if(debugPrint)
                        {
                            cout << "PC = " <<std::hex << cpu.getPC() << "  ;  IR = " << (int)cpu.getIR() << "("<<cpu.getInst().getASM() << ")  ;  Acc = " << cpu.getAcc() << "  ;  Adr = " << cpu.getAdr();
                            cout << "  ;  IDB = " << cpu.getIDB() << " ; X = "<< cpu.getX() << " ; Y = " << cpu.getY() << endl;

                            uint8_t p = cpu.getP();
                            string status;
                            if((p>>7)&1) status+="N"; else status += "-";
                            if((p>>6)&1) status+="V"; else status += "-";
                            if((p>>1)&1) status+="Z"; else status += "-";
                            if((p>>0)&1) status+="C"; else status += "-";
                            cout << "Flags = " << status << endl;

                            /*if(cpu.getPC() == oldPC)
                            {
                                cout << "big pb or test passed (lol)";
                            }*/
                            oldPC = cpu.getPC();
                            //cout << std::hex << cpu.getPC() << endl;
                            //cout << std::hex << cpu.getPC() << "("<<cpu.getInst().getASM() << ")"<<endl;
                            if(oldPC == 0x0449)
                            {
                                cout << "Y="<<std::hex <<cpu.getY()//<<//endl;
                                     << " " << cpu.getInst().getASM() << endl;
                            }
                            //std::getchar();
                        }

                    }
                }
            }


            /*if(global_clock >= 1364*262) //Roughly one frame
            {
                global_clock = 0;
                sf::Event event;
                while(app.pollEvent(event))
                {
                    if(event.type == sf::Event::Closed) app.close();
                }

                app.clear(sf::Color::Red);
                app.display();
            }*/

        }
        //else
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
                    case sf::Keyboard::B:
                        cout<<"cnskdcnsckj";
                        break;
                    case sf::Keyboard::D:
                        debugPrint = !debugPrint;
                        break;
                    case sf::Keyboard::S:
                        stepMode = true;
                        step = !step;
                    default:
                        ;
                    }
                }
            }
        }
    }
}
