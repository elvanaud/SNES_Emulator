#include "W65816.h"

void W65816::initializeOpcodes()
{
	decodingTable[0x44] = Instruction("MVP", BlockMoveP, dummyStage);
	decodingTable[0x54] = Instruction("MVN", BlockMoveN, dummyStage);
}
