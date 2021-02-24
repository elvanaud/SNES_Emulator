#ifndef CONSOLE_DEBUGGER_H
#define CONSOLE_DEBUGGER_H

#include "Common.h"

#include "CPU/W65816.h"
#include "CPU/CPU_Debugger.h"

#include <SFML/Graphics.hpp>

class Bus;

class ConsoleDebugger
{
public:
    ConsoleDebugger(W65816 & cpu);

    bool executeSystem();
    bool tick();
    void processEvent(sf::Event & event);
    void attachBus(Bus* b);
private:
    W65816& cpu;
    Bus * bus;

    bool debugPrint = true;
    bool stepMode = true;
    bool step = true;

    std::vector<uint32_t> watches;
};

#endif // CONSOLE_DEBUGGER_H
