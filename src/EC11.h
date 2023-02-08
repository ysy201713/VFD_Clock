#ifndef EC11_h
#define EC11_h

#include <stdint.h>

class EC11{
    public:
    void begin(uint8_t, uint8_t);
    uint8_t read();
    bool flag();
};


#endif