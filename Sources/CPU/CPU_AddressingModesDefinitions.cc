#include "W65816.h"

using std::bind;
using namespace std::placeholders;

void W65816::initializeAddressingModes()
{
    Immediate.setStages({{Stage(Stage::SIG_MODE16_ONLY,fetchInc,&pc,&idb.high)},{Stage(Stage::SIG_INST,dummyStage)}});
    Immediate.setSignals({bind(incPC,this,1),bind(opPrefetchInIDB,this)});

    ImmediateSpecial.setStages({{Stage(Stage::SIG_INST,dummyStage)},{Stage(Stage::SIG_DUMMY_STAGE, dummyStage)}});
    ImmediateSpecial.setSignals({bind(incPC,this,2),bind(opPrefetchInIDB,this)});

    Implied.setStages({{Stage(Stage::SIG_INST,dummyStage)}});
    Implied.setPredecodeSignals({bind(invalidPrefetch,this)});

    ImpliedSpecial.setStages({{Stage(Stage::SIG_INST,dummyStage)},{Stage(Stage::SIG_DUMMY_STAGE, dummyStage)}});
    ImpliedSpecial.setPredecodeSignals({bind(invalidPrefetch,this)});

    Absolute.setStages({{Stage(Stage::SIG_ALWAYS, fetchInc,&pc,&adr.high)},{Stage(Stage::SIG_ALWAYS,fetchInc,&adr,&idb.low)},
                       {Stage(Stage::SIG_MODE16_ONLY,fetch,&adr,&idb.high)},{Stage(Stage::SIG_INST,dummyStage)}});
    Absolute.setSignals({bind(incPC,this,1)});

    AbsoluteWrite.setStages({{Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high),Stage(Stage::SIG_INST,dummyStage)},{Stage(Stage::SIG_ALWAYS,writeInc,&adr,&idb.low)},
                            {Stage(Stage::SIG_MODE16_ONLY,write,&adr,&idb.high)},{Stage(Stage::SIG_DUMMY_STAGE,dummyStage)}});
    AbsoluteWrite.setSignals({bind(incPC,this,1)});

    AbsoluteRMW.setStages({ {Stage(Stage::SIG_ALWAYS,fetchInc,&pc,&adr.high)},{Stage(Stage::SIG_ALWAYS,fetchInc,&adr,&idb.low)},
                            {Stage(Stage::SIG_MODE16_ONLY,fetch,&adr,&idb.high)},{Stage(Stage::SIG_INST,dummyStage),Stage(Stage::SIG_MODE8_ONLY,decReg,&adr)},
                            {Stage(Stage::SIG_MODE16_ONLY,writeDec,&adr,&idb.high)},{Stage(Stage::SIG_ALWAYS,write,&adr,&idb.low)},{Stage(Stage::SIG_DUMMY_STAGE, dummyStage)}});
    AbsoluteRMW.setSignals({bind(incPC,this,1)});
}
