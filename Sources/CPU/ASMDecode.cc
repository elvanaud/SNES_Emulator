#include "W65816.h"
#include "Bus.h"

#include <sstream>

void W65816::initializeAdrModeASMDecode()
{
    string prefix = "$";

    auto absoluteAsm = [&](std::stringstream& stream)
    {
        stream << std::hex << prefix << getParam(1,2);
    };

    //for(auto adrMode : {&Absolute,&AbsoluteWrite,&AbsoluteJMP,&AbsoluteJSR})
        //adrMode->setASMDecode(std::function<void(std::stringstream&)>(absoluteAsm)); //Or put the lambda directly here
         //adrMode->setASMDecode(bind(absoluteAsm,_1));//,prefix,this));

    auto setDecoder = [&](auto decoder,std::vector<AddressingMode*> adrModeList)//, auto adrModeList)
    {
        for(auto adrMode : adrModeList)
            adrMode->setASMDecoder(std::function<void(std::stringstream&)>(decoder));
    };

    setDecoder(absoluteAsm,{&Absolute,&AbsoluteWrite,&AbsoluteJMP,&AbsoluteJSR});
    setDecoder([&](std::stringstream& stream){stream << std::hex << prefix << getParam(1,3);},
               {&AbsoluteLong,&AbsoluteLongWrite,&AbsoluteLongJMP,&AbsoluteLongJSL});
}

uint32_t W65816::getParam(int index, int length)
{
    uint32_t res = 0;

    for(int adr = pc.val()+index-1, offset = 0;length > 0; --length,++adr,offset+=8)
        res |= uint32_t(bus->privateRead(adr))<<offset;
    return res;
}
