#include "Bus.h"
#include "CPU/W65816.h"

#include <SFML/Graphics.hpp>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <limits>
#include <sstream>
using std::cout;
using std::cin;
using std::endl;
using std::stringstream;

Bus::Bus(W65816 & c) : cpu(c), debugger(cpu)
{
    /*ram[0xFF]  = 0x38; //SEC
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

uint8_t Bus::privateRead(uint32_t adr)
{
    uint8_t bank = (adr>>16)&8;
    adr &= 0xFFFF;

    if(bank == 0x7E) return ram1[adr];
    if(bank == 0x7F) return ram2[adr];
    if(bank <= 0x3F)
    {
        if(adr < 0x8000)
        {
            //ram mirror and system area + expansion
        }
        else
        {
            //if bank == 0: header + vectors
            //WS1 LoROM
            adr -= 0x8000;
            if(lorom[bank] != nullptr)
                return lorom[bank][adr]; //TODO: check memType lo/hi rom
        }
    }
    if(bank >= 0x40 && bank <= 0x7D)
    {
        //WS1 HiROM
        if(memType == LoROM)
        {
            if(adr < 0x8000)
            {
                if(lorom[bank] != nullptr) return lorom[bank][adr];
            }
        }
    }
    if(bank >= 0x80 && bank < 0xC0)
    {
        if(adr < 0x8000)
        {
            //ram mirror and system area + expansion
        }
        else
        {
            //WS2 LoROM
            bank -= 0x80;
            adr -= 0x8000;
            if(lorom[bank] != nullptr) return lorom[bank][adr];
        }
    }
    if(bank >= 0xC0)
    {
        //WS2 HiROM
        if(memType == LoROM)
        {
            if(adr < 0x8000)
            {
                if(lorom[bank] != nullptr) return lorom[bank][adr];
            }
        }
    }

    return ram1[adr];
}

void Bus::write(uint32_t adr, uint8_t data)
{
    if (!cpu.VDA() && !cpu.VPA()) return;

    dmr = data;
    uint8_t bank = (adr>>16)&8;
    adr &= 0xFFFF;

    if(bank == 0x7E) ram1[adr] = dmr;
    if(bank == 0x7F) ram2[adr] = dmr;
    if(bank <= 0x3F)
    {
        if(adr < 0x8000)
        {
            //ram mirror and system area + expansion
        }
        else
        {
            //if bank == 0: header + vectors
            //WS1 LoROM
            adr -= 0x8000;
            if(lorom[bank] != nullptr)
            {
                //lorom[bank][adr] = dmr; //rom
            }
        }
    }
    if(bank >= 0x40 && bank <= 0x7D)
    {
        //WS1 HiROM
    }
    if(bank >= 0x80 && bank < 0xC0)
    {
        if(adr < 0x8000)
        {
            //ram mirror and system area + expansion
        }
        else
        {
            //WS2 LoROM
            bank -= 0x80;
            adr -= 0x8000;
            if(lorom[bank] != nullptr) ;//lorom[bank][adr] = dmr; //rom
        }
    }
    if(bank >= 0xC0)
    {
        //WS2 HiROM
    }
}

uint8_t Bus::DMR()
{
    return dmr;
}

void Bus::copyInMemory(uint32_t adr, vector<uint8_t> const & buffer)
{
    for(auto value : buffer)
    {
        write(adr,value);
        ++adr;
    }
}

void Bus::loadCartridge(std::string const & path)
{
    std::ifstream input(path, std::ios::binary);
    if(!input){
         std::cout << "ERREUR: Fichier ROM introuvable\n";
         exit(-1);
     }

    input.ignore(std::numeric_limits<std::streamsize>::max());
    std::streamsize romLength = input.gcount();
    input.clear();   //  Since ignore will have set eof.
    input.seekg(0, std::ios_base::beg);

    cout << "ROM Length: "<< romLength << endl;
    if((romLength & 0x3FF) == 0x200) //Useless header present
    {
        cout << "ignoring useless header\n";
        romLength -= 0x200;
        input.ignore(0x200);
    }

    if(romLength <= 64*32*1024) //LoROM
    {
        cout << "LoROM\n";
        memType = LoROM;

        bool allocateMem = true;
        for(int i = 0; i < 64; ++i)
        {
            lorom[i] = nullptr;
            if(allocateMem)
            {
                lorom[i] = new uint8_t[0x8000];
                input.read((char*)lorom[i],0x8000);
                if(!input) allocateMem = false;
            }

        }
    }
    else //HiROM
    {
        cout << "HiROM\n";
        memType = HiROM;
    }


    //input.read((char*)ram1,0x10000);
}

void Bus::run()
{
    sf::RenderWindow app(sf::VideoMode(300,300,32),"SNES Emulator");
    unsigned int clock = 6;
    unsigned int global_clock = 0;

    bool debugPrint = true;
    bool stepMode = true;
    bool step = false;

    bool memWatchEnabled = false;
    vector<uint32_t> memWatch;

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

                            if(memWatchEnabled)
                            {
                                cout << "Memory Watch\n";
                                for(uint32_t mem : memWatch)
                                {
                                    cout << mem << ": " << privateRead(mem) << endl;
                                }
                            }

                            if(cpu.getPC() == oldPC)
                            {
                                cout << "big pb or test passed (lol)";
                            }
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
                    default: break;
                    case sf::Keyboard::B:
                        cout<<"cnskdcnsckj";
                        break;
                    case sf::Keyboard::D:
                        debugPrint = !debugPrint;
                        break;
                    case sf::Keyboard::S:
                        stepMode = true;
                        step = !step;
                        break;
                    case sf::Keyboard::C:
                        stepMode = false;
                        break;
                    case sf::Keyboard::V:
                        cout << "Enter debug command: ";
                        string user_entry;
                        getline(cin,user_entry);
                        vector<string> res = split(user_entry, ' ');
                        cin.ignore();
                        if(res[0] == "watch")
                        {
                            memWatchEnabled = true;
                            stringstream ss;
                            ss << std::hex << res[1];
                            uint32_t adr;
                            ss >> adr;
                            memWatch.push_back(adr);
                        }
                        break;

                    }
                }
            }
        }
    }
}
