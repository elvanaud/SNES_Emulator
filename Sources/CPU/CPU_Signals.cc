#include "W65816.h"

#include <iostream>
using std::cout;
using std::endl;

void W65816::incPC(unsigned int whatCycle)
{
    //if(tcycle == whatCycle) 
    ++pc;
    //cout << "incPc" << endl;
}

void W65816::opPrefetchInIDB()
{
    //if(tcycle == 1) 
    idb.low = adr.low;
    //cout << "opPrefetchInIDB" << endl;
}

void W65816::accPrefetchInIDB()
{
    //if(tcycle == 1) 
    idb.set(acc.val());
}

void W65816::dhPrefetchInAdr()
{
    //if(tcycle == 1) 
    adr.high = d.high;
}

void W65816::invalidPrefetch()
{
    //if(tcycle == 1) //This if is optional...
    forceInternalOperation = true;
    //cout << "invalidPrefetch" << endl;
}

void W65816::branchInstruction()
{
    thisIsABranch = true;
}
