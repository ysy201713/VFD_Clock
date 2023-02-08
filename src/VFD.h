#ifndef VFD_h
#define VFD_h

#include <stdint.h>

class VFDClass
{
private:
    /* data */
    gpio_num_t _din;
    gpio_num_t _clk;
    gpio_num_t _cs;
    gpio_num_t _rst;    

    void write_data(unsigned char);
public:
    void begin(gpio_num_t, gpio_num_t, gpio_num_t, gpio_num_t, gpio_num_t);
    void write_cmd(uint8_t);
    void show();
    void init();
    void write_one_char(unsigned char, unsigned char);
    void write_str(unsigned char, const char*);
    void write_user_font(unsigned char, unsigned char, unsigned char*);
    void set_bright(uint8_t);
    void write_number(uint8_t, uint8_t, uint8_t);
    void write_float(uint8_t, float);
    void clear();
    void light_on();
    void light_off();
};

extern VFDClass VFD;

#endif