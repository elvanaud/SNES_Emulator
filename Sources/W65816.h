#ifndef W65816_H
#define W65816_H

class Bus;

class W65816
{
public:
    W65816();
    void attachBus(Bus * b);
    void tick();

private:
    Bus * bus;
};

#endif // W65816_H
