#ifndef Bus_H
#define Bus_H

class W65816;

class Bus
{
public:
    Bus(W65816 &c);

    void run();

private:
    W65816 &cpu;
};

#endif //Bus_H
