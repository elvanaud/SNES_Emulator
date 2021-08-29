#include "W65816.h"
#include "Bus.h"

#include <sstream>
#include <iomanip>
using std::setfill;
using std::setw;
using std::hex;
using std::stringstream;

uint32_t W65816::getParam(int index, int length)
{
	uint32_t res = 0;
	uint32_t bank = uint32_t(pbr)<<16;

	for(int adr = pc.val()+index-1, offset = 0;length > 0; --length,++adr,offset+=8)
		res |= uint32_t(bus->privateRead(bank|adr))<<offset;
	return res;
}

void W65816::processASM(ASM_AdrModeType type)
{
	//return;//should quit this method as soon as possible when not debugging (for performance)
	stringstream stream;
	string prefix = "$";
	
	int dataWidth = 1;
	uint16_t offset;
	
	switch(type)
	{
	case ASM_ABSOLUTE:
		stream << " " << std::hex << prefix << setfill('0')<<setw(4)<< getParam(1,2);
		break;
	case ASM_ABSOLUTE_LONG:
		stream << " " << std::hex << prefix << setfill('0')<<setw(6)<<getParam(1,3);
		break;
	case ASM_ABSOLUTE_X_LONG:
		stream << " " << std::hex << prefix << setfill('0')<<setw(6)<<getParam(1,3) << ",X";
		break;
	case ASM_ABSOLUTE_X:
		stream << " " << std::hex << prefix << setfill('0')<<setw(4)<<getParam(1,2) << ",X";
		break;
	case ASM_ABSOLUTE_Y:
		stream << " " << std::hex << prefix << setfill('0')<<setw(4)<<getParam(1,2) << ",Y";
		break;
	case ASM_ABSOLUTE_X_INDIRECT:
		stream << " (" << std::hex << prefix << setfill('0')<<setw(4)<<getParam(1,2) << ",X)";
		break;
	case ASM_ABSOLUTE_INDIRECT_JML:
		stream << " [" << std::hex << prefix << setfill('0')<<setw(4)<<getParam(1,2) << "]";
		break;
	case ASM_ABSOLUTE_INDIRECT_JMP:
		 stream << " (" << std::hex << prefix << setfill('0')<<setw(4)<<getParam(1,2) << ")";
		 break;
	case ASM_ACCUMULATOR:
		if(asmAccAddFinalA) stream << " a"; 
		break;
	case ASM_DIRECT:
		stream << " " << std::hex << prefix << setfill('0')<<setw(2)<<getParam(1,1) << "";
		break;
	case ASM_DIRECT_X_INDIRECT:
		stream << " (" << std::hex << prefix << setfill('0')<<setw(2)<<getParam(1,1) << ",X)";
		break;
	case ASM_DIRECT_INDIRECT:
		stream << " (" << std::hex << prefix << setfill('0')<<setw(2)<<getParam(1,1) << ")";
		break;
	case ASM_DIRECT_INDIRECT_Y:
		stream << " (" << std::hex << prefix << setfill('0')<<setw(2)<<getParam(1,1) << "),Y";
		break;
	case ASM_DIRECT_INDIRECT_Y_LONG:
		stream << " [" << std::hex << prefix << setfill('0')<<setw(2)<<getParam(1,1) << "],Y";
		break;
	case ASM_DIRECT_INDIRECT_LONG:
		stream << " [" << std::hex << prefix << setfill('0')<<setw(2)<<getParam(1,1) << "]";
		break;
	case ASM_DIRECT_X:
		stream << " " << std::hex << prefix << setfill('0')<<setw(2)<<getParam(1,1) << ",X";
		break;
	case ASM_DIRECT_Y:
		stream << " " << std::hex << prefix << setfill('0')<<setw(2)<<getParam(1,1) << ",Y";
		break;
	case ASM_IMMEDIATE:
		dataWidth = 1;
		if((!isIndexRelated && !p.mem8) || (isIndexRelated && !p.index8))
		{
			dataWidth = 2;
		}
		stream << " #" << std::hex << prefix << setfill('0')<<setw(dataWidth*2) << getParam(1,dataWidth) << "";
		break;
	case ASM_IMMEDIATE_SPECIAL:
		stream << " #" << std::hex << prefix << setfill('0')<<setw(2) << getParam(1,1) << "";
		break;
	case ASM_IMPLIED:
		; //empty
		break;
	case ASM_RELATIVE_BRANCH:
		offset = (int8_t)getParam(1,1);
		//bus->accessedAdr = uint16_t(pc.val()+offset+1);
		//bus->isDataLoaded = true;
		isBranchInstruction = true;
		branchAddress = (uint32_t(pbr)<<16)| uint16_t(pc.val()+offset+1);
		stream << " " << std::hex << prefix << setfill('0')<<setw(4)<<uint16_t(pc.val()+offset+1);// << "(+"<<prefix<<uint32_t(offset&0xFF)<<")";//TODO: display as signed decimal..
		break;
	case ASM_RELATIVE_BRANCH_LONG:
		offset = getParam(1,2);
		isBranchInstruction = true;
		branchAddress = (uint32_t(pbr)<<16)| uint16_t(pc.val()+offset+1);
		stream << " " << std::hex << prefix << setfill('0')<<setw(4)<<uint16_t(pc.val()+offset+1);// << "(+"<<prefix<<uint32_t(offset)<<")";//TODO: display as signed decimal..
		break;
	}
	
	asmLine = opcodeASM + stream.str();
}