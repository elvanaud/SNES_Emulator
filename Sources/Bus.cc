#include "Bus.h"
#include "CPU/W65816.h"

#include <SFML/Graphics.hpp>

#include <cstdio>
#include <iostream>
#include <fstream>
using std::cout;
using std::endl;

Bus::Bus(W65816 & c) : cpu(c), debugger(cpu)
{
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
    ram[0x10D] = 42;
}

void Bus::read(uint32_t adr)
{
    if (!cpu.VDA() && !cpu.VPA()) return;

    dmr = ram[adr]; //TODO: Might use privateRead in the future
}

uint8_t Bus::privateRead(uint32_t adr)
{
    return ram[adr];
}

void Bus::write(uint32_t adr, uint8_t data)
{
    if (!cpu.VDA() && !cpu.VPA()) return;

    dmr = data;
    ram[adr] = dmr;
}

uint8_t Bus::DMR()
{
    return dmr;
}

void Bus::copyInMemory(uint32_t adr, vector<uint8_t> const & buffer)
{
    for(auto value : buffer)
    {
        ram[adr] = value;
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

    input.read((char*)ram,0x10000);
}

void Bus::run()
{
    sf::RenderWindow app(sf::VideoMode(300,300,32),"SNES Emulator");
    unsigned int clock = 6;
    unsigned int global_clock = 0;

    bool debugPrint = false;
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
