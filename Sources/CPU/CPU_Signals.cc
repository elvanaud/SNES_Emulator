#include "W65816.h"

#include <iostream>
using std::cout;
using std::endl;

void W65816::incPC(unsigned int whatCycle)
{
    if(tcycle == whatCycle) ++pc;
    //cout << "incPc" << endl;
}

void W65816::opPrefetchInIDB()
{
    if(tcycle == 1) idb.low = adr.low;
    //cout << "opPrefetchInIDB" << endl;
}

void W65816::invalidPrefetch()
{
    if(tcycle == 1)forceInternalOperation = true;
    //cout << "invalidPrefetch" << endl;
}
