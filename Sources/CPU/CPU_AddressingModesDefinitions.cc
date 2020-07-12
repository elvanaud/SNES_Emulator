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
    //Implied.setSignals({bind(invalidPrefetch,this)});/*[](W65816* cpu){   if(cpu->invalidAddress) cpu->invalidAddress = false;
                                                //if(cpu->tcycle == 1)cpu->invalidAddress = true; },this)});*/
    Implied.setPredecodeSignals({bind(invalidPrefetch,this)});
}
