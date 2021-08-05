#include "W65816.h"

using std::bind;
using namespace std::placeholders;

void W65816::initializeAddressingModes()
{
	StackPEA.setStages({{Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high)},
						{Stage(Stage::SIG_ALWAYS,push,&adr.high)},
						{Stage(Stage::SIG_ALWAYS,push,&adr.low)},
						{Stage(Stage::SIG_INST,dummyStage)}});
	StackPEA.setSignals({bind(incPC,this,1)});


	StackPEI.setStages({{Stage(Stage::SIG_DL_NOT_ZERO,dummyFetchLast),Stage(Stage::SIG_DL_NOT_ZERO,halfAdd,&adr.low,&d.low),Stage(Stage::SIG_DL_NOT_ZERO,fixCarry,&adr.high,&ZERO)},
						{Stage(Stage::SIG_ALWAYS,fetchInc,&adr,&idb.low)},
						{Stage(Stage::SIG_ALWAYS,fetch,&adr,&idb.high)},
						{Stage(Stage::SIG_ALWAYS,push,&idb.high)},
						{Stage(Stage::SIG_ALWAYS,push,&idb.low)},
						{Stage(Stage::SIG_INST,dummyStage)}});
	StackPEI.setSignals({bind(incPC,this,1),bind(dhPrefetchInAdr,this)});


	StackPER.setStages({{Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high)},
						{Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&pc)},
						{Stage(Stage::SIG_ALWAYS,push,&adr.high)},
						{Stage(Stage::SIG_ALWAYS,push,&adr.low)},
						{Stage(Stage::SIG_INST,dummyStage)}});
	StackPER.setSignals({bind(incPC,this,1)});


	StackRTI.setStages({{Stage(Stage::SIG_ALWAYS,dummyFetchLast)},// Stage(Stage::SIG_ALWAYS,enableInterupts,true)}, //TODO: Re enable interupts where ? maybe no need to do it as it is saved in P
						{Stage(Stage::SIG_ALWAYS,popP)},
						{Stage(Stage::SIG_ALWAYS,pop,&pc.low)},
						{Stage(Stage::SIG_ALWAYS,pop,&pc.high)},
						{Stage(Stage::SIG_NATIVE_MODE,pop,&pbr)},
						{Stage(Stage::SIG_INST,dummyStage)}});
	StackRTI.setPredecodeSignals({bind(invalidPrefetch,this)});


	StackRTS.setStages({{Stage(Stage::SIG_ALWAYS,dummyFetchLast)},
						{Stage(Stage::SIG_ALWAYS,pop,&pc.low)},
						{Stage(Stage::SIG_ALWAYS,pop,&pc.high)},
						{Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,incReg,&pc)},
						{Stage(Stage::SIG_INST,dummyStage)}});
	StackRTS.setPredecodeSignals({bind(invalidPrefetch,this)});


	StackRTL.setStages({{Stage(Stage::SIG_ALWAYS,dummyFetchLast)},
						{Stage(Stage::SIG_ALWAYS,pop,&pc.low)},
						{Stage(Stage::SIG_ALWAYS,pop,&pc.high)},
						{Stage(Stage::SIG_ALWAYS,pop,&pbr),Stage(Stage::SIG_ALWAYS,incReg,&pc)},
						{Stage(Stage::SIG_INST,dummyStage)}});
	StackRTL.setPredecodeSignals({bind(invalidPrefetch,this)});


	StackRelative.setStages({   {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,moveReg8,&ZERO,&adr.high),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&s)},
								{Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
								{Stage(Stage::SIG_MODE16_ONLY,fetchLong,&ZERO,&adr,&idb.high)},
								{Stage(Stage::SIG_INST,dummyStage)}});
	StackRelative.setSignals({bind(incPC,this,1)});


	StackRelativeWrite.setStages({  {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,moveReg8,&ZERO,&adr.high),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&s)},
									{Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,writeIncLong,&ZERO,&adr,&idb.low)},
									{Stage(Stage::SIG_MODE16_ONLY,writeLong,&ZERO,&adr,&idb.high)},
									{Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
	StackRelativeWrite.setSignals({bind(incPC,this,1)});


	StackRelativeIndirectY.setStages({  {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,moveReg8,&ZERO,&adr.high),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&s)},
										{Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
										{Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&idb.high)},
										{Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,fullAdd,&idb,&y)},
										{Stage(Stage::SIG_ALWAYS,moveReg16,&idb,&adr),Stage(Stage::SIG_ALWAYS,fetchInc,&adr,&idb.low)},
										{Stage(Stage::SIG_MODE16_ONLY,fetch,&adr,&idb.high)},
										{Stage(Stage::SIG_INST,dummyStage)}});
	StackRelativeIndirectY.setSignals({bind(incPC,this,1)});


	StackRelativeIndirectYWrite.setStages({ {Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,moveReg8,&ZERO,&adr.high),Stage(Stage::SIG_ALWAYS,fullAdd,&adr,&s)},
											{Stage(Stage::SIG_ALWAYS,fetchIncLong,&ZERO,&adr,&idb.low)},
											{Stage(Stage::SIG_ALWAYS,fetchLong,&ZERO,&adr,&idb.high)},
											{Stage(Stage::SIG_ALWAYS,dummyFetchLast),Stage(Stage::SIG_ALWAYS,fullAdd,&idb,&y)},
											{Stage(Stage::SIG_ALWAYS,moveReg16,&idb,&adr),Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_ALWAYS,writeInc,&adr,&idb.low)},
											{Stage(Stage::SIG_MODE16_ONLY,write,&adr,&idb.high)},
											{Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
	StackRelativeIndirectYWrite.setSignals({bind(incPC,this,1)});

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
