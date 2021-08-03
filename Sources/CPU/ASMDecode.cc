#include "W65816.h"
#include "Bus.h"

#include <sstream>
#include <iomanip>
using std::setfill;
using std::setw;
using std::hex;
using std::stringstream;

void W65816::initializeAdrModeASMDecode()
{
    //string prefix = "$";

    auto absoluteAsm = [&](std::stringstream& stream, string prefix)
    {
        stream << " " << std::hex << prefix << setfill('0')<<setw(4)<< getParam(1,2);
    };

    //for(auto adrMode : {&Absolute,&AbsoluteWrite,&AbsoluteJMP,&AbsoluteJSR})
        //adrMode->setASMDecode(std::function<void(std::stringstream&)>(absoluteAsm)); //Or put the lambda directly here
         //adrMode->setASMDecode(bind(absoluteAsm,_1));//,prefix,this));

    auto setDecoder = [&](auto decoder,std::vector<AddressingMode*> adrModeList)//, auto adrModeList)
    {
        for(auto adrMode : adrModeList)
            adrMode->setASMDecoder(std::function<void(std::stringstream&,string)>(decoder));
    };

    setDecoder(absoluteAsm,{&Absolute,&AbsoluteWrite,&AbsoluteJMP,&AbsoluteJSR,&AbsoluteRMW});

    setDecoder([&](std::stringstream& stream, string prefix)
                {
                   stream << " " << std::hex << prefix << setfill('0')<<setw(6)<<getParam(1,3);
                },
               {&AbsoluteLong,&AbsoluteLongWrite,&AbsoluteLongJMP,&AbsoluteLongJSL});

    setDecoder([&](std::stringstream& stream, string prefix)
                {
                   stream << " " << std::hex << prefix << setfill('0')<<setw(6)<<getParam(1,3) << ",X";
                },
               {&AbsoluteXLong,&AbsoluteXLongWrite});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                   stream << " " << std::hex << prefix << setfill('0')<<setw(4)<<getParam(1,2) << ",X";
                },
               {&AbsoluteX,&AbsoluteXWrite,&AbsoluteXRMW});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                   stream << " " << std::hex << prefix << setfill('0')<<setw(4)<<getParam(1,2) << ",Y";
                },
               {&AbsoluteY,&AbsoluteYWrite});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                   stream << " (" << std::hex << prefix << setfill('0')<<setw(4)<<getParam(1,2) << ",X)";
                },
               {&AbsoluteXIndirectJMP,&AbsoluteXIndirectJSR});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                   stream << " [" << std::hex << prefix << setfill('0')<<setw(4)<<getParam(1,2) << "]";
                },
               {&AbsoluteIndirectJML});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                   stream << " (" << std::hex << prefix << setfill('0')<<setw(4)<<getParam(1,2) << ")";
                },
               {&AbsoluteIndirectJMP});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                   //stream << " A";
                },
               {&Accumulator});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                   stream << " " << std::hex << prefix << setfill('0')<<setw(2)<<getParam(1,1) << "";
                },
               {&Direct,&DirectWrite,&DirectRMW});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                   stream << " (" << std::hex << prefix << setfill('0')<<setw(2)<<getParam(1,1) << ",X)";
                },
               {&DirectXIndirect,&DirectXIndirectWrite});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                   stream << " (" << std::hex << prefix << setfill('0')<<setw(2)<<getParam(1,1) << ")";
                },
               {&DirectIndirect,&DirectIndirectWrite});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                   stream << " (" << std::hex << prefix << setfill('0')<<setw(2)<<getParam(1,1) << "),Y";
                },
               {&DirectIndirectY,&DirectIndirectYWrite});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                   stream << " [" << std::hex << prefix << setfill('0')<<setw(2)<<getParam(1,1) << "],Y";
                },
               {&DirectIndirectYLong,&DirectIndirectYLongWrite});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                   stream << " [" << std::hex << prefix << setfill('0')<<setw(2)<<getParam(1,1) << "]";
                },
               {&DirectIndirectLong,&DirectIndirectLongWrite});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                   stream << " " << std::hex << prefix << setfill('0')<<setw(2)<<getParam(1,1) << ",X";
                },
               {&DirectX,&DirectXWrite,&DirectXRMW});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                   stream << " " << std::hex << prefix << setfill('0')<<setw(2)<<getParam(1,1) << ",Y";
                },
               {&DirectY,&DirectYWrite});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                    int dataWidth = 1;
                    if(!p.mem8 || (isIndexRelated && !p.index8))
                    {
                        dataWidth = 2;
                    }
                    stream << " #" << std::hex << prefix << setfill('0')<<setw(dataWidth*2) << getParam(1,dataWidth) << "";
                },
               {&Immediate});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                   stream << " #" << std::hex << prefix << setfill('0')<<setw(2) << getParam(1,1) << "";
                },
               {&ImmediateSpecial});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                    uint16_t offset = (int8_t)getParam(1,1);
                    //bus->accessedAdr = uint16_t(pc.val()+offset+1);
                    //bus->isDataLoaded = true;
                    isBranchInstruction = true;
                    branchAddress = (uint32_t(pbr)<<16)| uint16_t(pc.val()+offset+1);
                    stream << " " << std::hex << prefix << setfill('0')<<setw(4)<<uint16_t(pc.val()+offset+1);// << "(+"<<prefix<<uint32_t(offset&0xFF)<<")";//TODO: display as signed decimal..
                },
               {&RelativeBranch});
    setDecoder([&](std::stringstream& stream, string prefix)
                {
                    uint16_t offset = getParam(1,2);
                    isBranchInstruction = true;
                    branchAddress = (uint32_t(pbr)<<16)| uint16_t(pc.val()+offset+1);
                    stream << " " << std::hex << prefix << setfill('0')<<setw(4)<<uint16_t(pc.val()+offset+1);// << "(+"<<prefix<<uint32_t(offset)<<")";//TODO: display as signed decimal..
                },
               {&RelativeBranchLong});
}

uint32_t W65816::getParam(int index, int length)
{
    uint32_t res = 0;

    for(int adr = pc.val()+index-1, offset = 0;length > 0; --length,++adr,offset+=8)
        res |= uint32_t(bus->privateRead(adr))<<offset;
    return res;
}
