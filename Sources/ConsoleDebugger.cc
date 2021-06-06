#include "ConsoleDebugger.h"

#include "Bus.h"

ConsoleDebugger::ConsoleDebugger(W65816 & p_cpu) : cpu(p_cpu)
{

}

void ConsoleDebugger::attachBus(Bus* b)
{
    bus = b;
}

vector<string> split(const string& s, char delimiter)
{
   vector<string> tokens;
   string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

string csvGet(string src,string field)
{
    auto parts = split(src,'\n');
    int pos =0; int p = 0;
    for(auto f : split(parts[0],';'))
    {
        if(f==field)
            pos = p;
        p++;
    }
    return split(parts[1],';')[pos];
}

bool ConsoleDebugger::executeSystem()
{
    if(cpu.VDA() && cpu.VPA())
    {
        uint32_t pc = cpu.getPC()-1;
        //cout<<"pc:"<<pc<<endl;
        for(uint32_t bp : program_breakpoints)
        {
            if(pc == bp)
            {
                cout<<"Breakpoint hit!"<<endl;
                stepMode = step = true;
                debugPrint = true;
                break;
            }
        }
    }

    return !stepMode || (stepMode && step);
}

void ConsoleDebugger::processEvent(sf::Event & event)
{
    uint32_t user_entry;
    bool delete_bp = false;
    int beforeLastElem = program_breakpoints.size()-1;
    //if(beforeLastElem<0)beforeLastElem=0;

    if(event.type == sf::Event::KeyPressed)
    {
        switch(event.key.code)
        {
        case sf::Keyboard::B:
            cout<<"Breakpoint at address:"<<endl;
            cin>>std::hex>>user_entry;

            //program_breakpoints.push_back(user_entry);
            //if(beforeLastElem<0)
            for(int i = 0; i < beforeLastElem;++i)
            {
                if(program_breakpoints[i] == user_entry)
                    delete_bp = true;
                if(delete_bp)
                {
                    program_breakpoints[i] = program_breakpoints[i+1];
                }
            }
            if(program_breakpoints[beforeLastElem]==user_entry || delete_bp)
                program_breakpoints.pop_back();
            else
                program_breakpoints.push_back(user_entry);

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
        case sf::Keyboard::W:
            cout << "Enter watch adress:";
            cin >> std::hex >>user_entry;
            watches.push_back(user_entry);
            stepMode = true;
            step = !step;
            break;
        default:
            ;
        }
    }
}

bool ConsoleDebugger::tick()
{
    if(cpu.VDA() && cpu.VPA()) //Sync = Opcode Fetch
    {
        step = false;
        if(debugPrint)
        {
            cout << "PC = " <<std::hex << cpu.getPC()-1 << "  ;  IR = " << (int)cpu.getIR() << "("<<cpu.getInst().getASM() << ")  ;  A = " << cpu.getAcc();
            cout << "  ;  Adr = " << cpu.getAdr() << "  ;  IDB = " << cpu.getIDB();
            cout << " ; X = "<< cpu.getX() << " ; Y = " << cpu.getY() << " ; D = " << cpu.getD() << endl;

            uint8_t p = cpu.getP();
            string status;
            if((p>>7)&1) status+="N";
            else status += "-"; //TODO: print flags in lower/upper case if set/unset
            if((p>>6)&1) status+="V";
            else status += "-";
            if((p>>1)&1) status+="Z";
            else status += "-";
            if((p>>0)&1) status+="C";
            else status += "-";
            cout << "Flags = " << status << " " << std::hex << int(p) << endl;
            //std::getchar();
            for(uint32_t watch : watches)
                cout << "Content of "<<std::hex<<watch<<" :"<<(int)bus->privateRead(watch)<<endl;
        }
    }
    return false;
}
