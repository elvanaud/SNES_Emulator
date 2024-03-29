#ifndef CONSOLE_DEBUGGER_H
#define CONSOLE_DEBUGGER_H

#include "Common.h"

#include "CPU/W65816.h"

#include <SFML/Graphics.hpp>

class Bus;

vector<string> split(const string& s, char delimiter);
string csvGet(string src,string field);

class ConsoleDebugger
{
public:
	ConsoleDebugger(W65816 & cpu);

	bool executeSystem();
	bool tick();
	void processEvent(sf::Event & event);
	void attachBus(Bus* b);

	void saveTrace(string path);

	bool checkEvents();
	void traceOrPrint();

	bool debugPrint = true;
	bool debugTrace = true;

	bool stepMode = true;

	int nbExecutedInstructions = 0;
private:
	W65816& cpu;
	Bus * bus;

	bool step = true;

	std::vector<uint32_t> watches;
	std::vector<uint32_t> program_breakpoints;

	string trace;

	bool firstTrace = true;
};

#endif // CONSOLE_DEBUGGER_H
