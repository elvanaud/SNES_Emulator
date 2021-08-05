#include "W65816.h"

using std::bind;
using namespace std::placeholders;

void W65816::initializeAddressingModes()
{
	BlockMoveN.setStages({  {Stage(Stage::SIG_ALWAYS,fetchDec,&pc,&adr.high),Stage(Stage::SIG_ALWAYS,moveReg8,&adr.low,&dbr)},
							{Stage(Stage::SIG_ALWAYS,fetchIncLong,&adr.high,&x,&idb.low)},
							{Stage(Stage::SIG_ALWAYS,writeIncLong,&adr.low,&y,&idb.low)},
							{Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,decReg,&acc),Stage(Stage::SIG_ACC_ZERO,incReg,&pc)},
							{Stage(Stage::SIG_ACC_ZERO,incReg,&pc),Stage(Stage::SIG_ACC_NOT_ZERO,decReg,&pc)},
							{Stage(Stage::SIG_INST,dummyStage)}});
	BlockMoveN.setSignals({bind(incPC,this,1)});


	BlockMoveP.setStages({  {Stage(Stage::SIG_ALWAYS,fetchDec,&pc,&adr.high),Stage(Stage::SIG_ALWAYS,moveReg8,&adr.low,&dbr)},
							{Stage(Stage::SIG_ALWAYS,fetchDecLong,&adr.high,&x,&idb.low)},
							{Stage(Stage::SIG_ALWAYS,writeDecLong,&adr.low,&y,&idb.low)},
							{Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,decReg,&acc),Stage(Stage::SIG_ACC_ZERO,incReg,&pc)},
							{Stage(Stage::SIG_ACC_ZERO,incReg,&pc),Stage(Stage::SIG_ACC_NOT_ZERO,decReg,&pc)},
							{Stage(Stage::SIG_INST,dummyStage)}});
	BlockMoveP.setSignals({bind(incPC,this,1)});
}
