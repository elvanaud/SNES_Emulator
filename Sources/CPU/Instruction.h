#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <vector>
using std::vector;
#include <string>
using std::string;

#include "Stage.h"
#include "AddressingMode.h"

class Instruction
{
public:
	Instruction();
	template<class F,class ... Args>
	Instruction(string opASM, AddressingMode &adrMd,F inst,Args ... args)
	{
		opcodeASM = opASM;
		stages = adrMd.gen(bind(inst,_1,args...));
		adrMode = adrMd;
	}

	vector<vector<Stage>> Stages();
	vector<std::function<void()>> Signals();
	vector<std::function<void()>> PredecodeSignals();
	AddressingMode AdrMode();

	bool isIndexRelated();
	void setIsIndexRelated(bool idxR);

	string getASM();
private:
	vector<vector<Stage>> stages;
	AddressingMode adrMode;
	string opcodeASM;
	bool indexRelated = false;
};

#endif // INSTRUCTION_H
