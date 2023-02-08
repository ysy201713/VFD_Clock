#include <EC11.h>
#include <Arduino.h>

uint8_t EC11_B;
bool direction;
bool isr_flag=0;

void IRAM_ATTR ec11_isr(){
    if(digitalRead(EC11_B)){
        direction = 1;
    }
    else{
        direction = 0;
    }
    isr_flag = 1;
}

void EC11::begin(uint8_t A, uint8_t B){
    pinMode(A, INPUT_PULLUP);
    pinMode(B, INPUT_PULLUP);
    EC11_B = B;
    attachInterrupt(A, ec11_isr, RISING);
}

uint8_t EC11::read(){
    delay(30);
    if(isr_flag){
        isr_flag = 0;
        return direction;
    }
    return 2;
}

bool EC11::flag(){
    return isr_flag;
}