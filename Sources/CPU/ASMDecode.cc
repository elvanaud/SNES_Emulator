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
    const string prefix = "$";

    //for(auto adrMode : {&Absolute,&AbsoluteWrite,&AbsoluteJMP,&AbsoluteJSR})
        //adrMode->setASMDecode(std::function<void(std::stringstream&)>(absoluteAsm)); //Or put the lambda directly here
         //adrMode->setASMDecode(bind(absoluteAsm,_1));//,prefix,this));

    auto setDecoder = [&](auto decoder,std::vector<AddressingMode*> adrModeList)//, auto adrModeList)
    {
        for(auto adrMode : adrModeList)
            adrMode->setASMDecoder(std::function<void(stringstream&)>(decoder));
    };

    auto prepStream = [](stringstream& stream, int width, const string && prefix = "$") -> stringstream&
    {
        stream << " " << prefix << hex << setfill('0') << setw(width);
        return stream;
    };

    setDecoder([&](stringstream& stream)
                {
                    prepStream(stream,4) << getParam(1,2);
                },
                {&Absolute,&AbsoluteWrite,&AbsoluteRMW,&AbsoluteJMP,&AbsoluteJSR});
    setDecoder([&](stringstream& stream)
               {
                    prepStream(stream,6) << getParam(1,3);
               },
               {&AbsoluteLong,&AbsoluteLongWrite,&AbsoluteLongJMP,&AbsoluteLongJSL});
    setDecoder([&](stringstream& stream)
               {
                    prepStream(stream,4) << getParam(1,2) << ", X";
               },
               {&AbsoluteX,&AbsoluteXWrite,&AbsoluteXRMW});
    setDecoder([&](stringstream& stream)
               {
                    prepStream(stream,4) << getParam(1,2) << ", Y";
               },
               {&AbsoluteY,&AbsoluteYWrite});
    setDecoder([&](stringstream& stream)
               {
                    prepStream(stream,6) << getParam(1,3) << ", X";
               },
               {&AbsoluteXLong,&AbsoluteXLongWrite});
    setDecoder([&](stringstream& stream)
               {
                    stream << "("; prepStream(stream,4) << getParam(1,2) << ", X)";
               },
               {&AbsoluteXIndirectJMP,&AbsoluteXIndirectJSR});
    setDecoder([&](stringstream& stream)
               {
                    stream << "("; prepStream(stream,4) << getParam(1,2) << ")";
               },
               {&AbsoluteIndirectJMP});
    setDecoder([&](stringstream& stream)
               {
                    stream << "["; prepStream(stream,4) << getParam(1,2) << "]";
               },
               {&AbsoluteIndirectJML});
    setDecoder([&](stringstream& stream)
               {
                    prepStream(stream,2) << getParam(1,1);
               },
               {&Direct,&DirectWrite,&DirectRMW});
    setDecoder([&](stringstream& stream)
               {
                    stream << "("; prepStream(stream,2) << getParam(1,1) << ",X)";
               },
               {&DirectXIndirect,&DirectXIndirectWrite});
    setDecoder([&](stringstream& stream)
               {
                    stream << "("; prepStream(stream,2) << getParam(1,1) << ")";
               },
               {&DirectIndirect,&DirectIndirectWrite});
    setDecoder([&](stringstream& stream)
               {
                    stream << "("; prepStream(stream,2) << getParam(1,1) << "),Y";
               },
               {&DirectIndirectY,&DirectIndirectYWrite});
    setDecoder([&](stringstream& stream)
               {
                    stream << "["; prepStream(stream,2) << getParam(1,1) << "],Y";
               },
               {&DirectIndirectYLong,&DirectIndirectYLongWrite});
    setDecoder([&](stringstream& stream)
               {
                    stream << "["; prepStream(stream,2) << getParam(1,1) << "]";
               },
               {&DirectIndirectLong,&DirectIndirectLongWrite});
    setDecoder([&](stringstream& stream)
               {
                    prepStream(stream,2) << getParam(1,1) << ",X";
               },
               {&DirectX,&DirectXWrite,&DirectXRMW});
    setDecoder([&](stringstream& stream)
               {
                    prepStream(stream,2) << getParam(1,1) << ",Y";
               },
               {&DirectY,&DirectYWrite});
    setDecoder([&](stringstream& stream)
               {
                    int data_length = 2;
                    if(decodingTable[ir].isIndexRelated())
                    {
                        if(p.index8) data_length = 1;
                    }
                    else if(p.mem8) data_length = 1;
                    prepStream(stream,2*data_length, "#$") << getParam(1,data_length);
               },
               {&Immediate,&ImmediateSpecial});
    setDecoder([&](stringstream& stream)
               {
                    prepStream(stream,2) << getParam(1,1);
               },
               {&RelativeBranch});
    setDecoder([&](stringstream& stream)
               {
                    prepStream(stream,4) << getParam(1,2);
               },
               {&RelativeBranchLong});
    //todo: stack relative and block move
}

uint32_t W65816::getParam(int index, int length)
{
    uint32_t res = 0;

    for(int adr = pc.val()+index-1, offset = 0;length > 0; --length,++adr,offset+=8)
        res |= uint32_t(bus->privateRead(adr))<<offset;
    return res;
}
