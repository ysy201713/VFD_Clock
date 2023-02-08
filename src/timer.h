#include <esp32-hal-timer.h>

hw_timer_t *timer = NULL;
bool timer_isr_flag = 0;

void IRAM_ATTR timer_isr(){
    timer_isr_flag = !timer_isr_flag;
}

void StartTimer(){
    timer = timerBegin(0, 16, true);
    timerAttachInterrupt(timer, timer_isr, true);
    timerAlarmWrite(timer, 2000000, true);
    timerAlarmEnable(timer);
}

void StopTimer(){
    timerStop(timer);
    timerEnd(timer);
}


