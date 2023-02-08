/**
 * @author ysy
 * VFD-DIN:4  
 *     CLK:5  
 *     CS:6  
 *     RESET:7  
 *     EN:8
 * DHT11-DATA:0
 * EC11-A:1
 *      B:2
 *      KEY:TX
 * BUZZER:RX
*/

#include <Arduino.h>
#include "VFD.h"
#include "font.h"
#include "ScollTime.h"
#include "Menu.h"
#include "timer.h"
#include "filesys.h"
#include "EC11.h"
#include <WiFi.h>
#include <esp_sntp.h>
#include <DHT.h>


#define BUZZER RX

#define ec11_isr_flag_clear() ec11_isr_flag=false

typedef struct{
  int hour=0;
  int min=0;
}alarm_time_t;

DHT dht(0, DHT11);
EC11 ec11;

bool ec11_isr_flag = false;
time_t now = 0;
char strftime_buf[20];
struct tm timeinfo = {0};
bit_data_t bit_data[6];
alarm_time_t alarm_time;
bool alarm_on = false;
uint8_t bright;

const char* ssid = "Defense_Home";
const char* psw = "Windhome@123";

//设置闹钟选项
void SetAlarm(){
  bool pos = 0;
  StartTimer();
  VFD.clear();
  VFD.write_one_char(2, 10);
  for(;;){
    if(timer_isr_flag){
      VFD.write_number(0, alarm_time.hour, 2);
      VFD.write_number(3, alarm_time.min, 2); 
      if(alarm_on){
        VFD.write_str(5, "ON");
      }
      else{
        VFD.write_str(5, "OFF");
      }
    }
    else{
      VFD.write_str(pos*3, "  ");
    }

    if(ec11_isr_flag){
      ec11_isr_flag_clear();
      if(pos){
        VFD.write_str(0, " SETOK! ");
        delay(1000);
        alarm_on = true;
        StopTimer();
        return;
      } 
      pos = 1;
    }
    uint8_t direction = ec11.read();
    if(direction == 0){
      if(!pos){
        alarm_time.hour == 23 ? alarm_time.hour = 0 : alarm_time.hour++;
      }
      else{
        alarm_time.min == 59 ? alarm_time.min = 0 : alarm_time.min++;
      }
      timer_isr_flag = 1;    
    }
    else if(direction == 1){
      if(!pos){
        alarm_time.hour == 0 ? alarm_time.hour = 23 : alarm_time.hour--;
      }
      else{
        alarm_time.min == 0 ? alarm_time.min = 59 : alarm_time.min--;
      }
      timer_isr_flag = 1;
    }
  }
  //闪烁当前调整的数字

}

//设置亮度选项
void SetBright(){
  VFD.clear();
  VFD.write_number(2, bright, 3);
  for(;;){
    uint8_t direction = ec11.read();
    if(direction == 0){
      bright == 240 ? : bright++;
      VFD.write_number(2, bright, 3);
      VFD.set_bright(bright);
    }
    else if(direction == 1){
      bright == 0 ? : bright--;
      VFD.write_number(2, bright, 3);
      VFD.set_bright(bright);
    }
    if(ec11_isr_flag){
      ec11_isr_flag_clear();
      break;
    }
  }
  StoreBright(bright);
}

//ntp对时选项
void SntpGetTime(){
  uint8_t i=0;
  VFD.clear();
  WiFi.begin(ssid, psw);
  VFD.write_one_char(i, 14);
  VFD.show();
  while (WiFi.status() != WL_CONNECTED) {
    VFD.write_one_char(++i, 14);
    VFD.show();
    delay(100);
  }
  //启动sntp
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "ntp2.aliyun.com");
  sntp_init();
  while(sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED){
    VFD.write_one_char(++i, 14);
    VFD.show();
    delay(700);
  }
  sntp_stop();
  setenv("TZ", "CST-8", 1);
  tzset();

  VFD.write_one_char(++i, 14);
  VFD.show();

  WiFi.disconnect(1, 0);
  VFD.write_str(0, "   OK   ");
  delay(1000);
}

void Settime(){
  VFD.clear();
  VFD.write_one_char(2, 10);
  VFD.write_one_char(5, 10);
  StartTimer();
  uint8_t pos = 0;
  for(;;){
    if(timer_isr_flag){
      VFD.write_number(0, timeinfo.tm_hour, 2);
      VFD.write_number(3, timeinfo.tm_min, 2);
      VFD.write_number(6, timeinfo.tm_sec, 2);
    }
    else{
      VFD.write_str(pos*3, "  ");
    }

    if(ec11_isr_flag){
      ec11_isr_flag_clear();
      if(pos == 2){
        StopTimer();
        time_t time_p = mktime(&timeinfo);
        struct timeval tv;
        tv.tv_sec = time_p;
        tv.tv_usec = 0;
        settimeofday(&tv, (struct timezone *)0);
        VFD.write_str(0, " SETOK! ");
        delay(1000);
        return;
      }
      pos++;
    }

    uint8_t direction = ec11.read();
    if(direction == 0){
      switch(pos){
        case 0:
        timeinfo.tm_hour == 23 ? timeinfo.tm_hour = 0 : timeinfo.tm_hour++;
        break;
        case 1:
        timeinfo.tm_min == 59 ? timeinfo.tm_min = 0 : timeinfo.tm_min++;
        break;
        case 2:
        timeinfo.tm_sec == 59 ? timeinfo.tm_sec = 0 : timeinfo.tm_sec++;
        break;
      }
      timer_isr_flag = 1;
    }
    else if(direction == 1){
      switch(pos){
        case 0:
        timeinfo.tm_hour == 0 ? timeinfo.tm_hour = 23 : timeinfo.tm_hour--;
        break;
        case 1:
        timeinfo.tm_min == 0 ? timeinfo.tm_min = 59 : timeinfo.tm_min--;
        break;
        case 2:
        timeinfo.tm_sec == 0 ? timeinfo.tm_sec = 59 : timeinfo.tm_sec--;
        break;
      }
      timer_isr_flag = 1;     
    }
  }
}

//显示主菜单
void ShowMainMenu(){
  uint8_t pos = 0;
  VFD.write_str(0, "  MENU  ");
  delay(1000);
  ec11_isr_flag_clear();
  VFD.write_str(0, menu[pos]);

  for(;;){
    uint8_t direction = ec11.read();
    if(direction == 0){
      pos == (sizeof(menu) / sizeof(menu[0]) - 1) ? : pos++;
      VFD.write_str(0, menu[pos]);
    }
    else if(direction == 1){
      pos == 0 ? : pos--;
      VFD.write_str(0, menu[pos]);
    }
    if(ec11_isr_flag){
      ec11_isr_flag_clear();
      switch(pos){
        case Bright:
          SetBright();
          break;
        case GetTime:
          SntpGetTime();
          break;
        case Alarm:
          SetAlarm();
          break;
        case SetTime:
          Settime();
        case Exit:
          return;
      }
      VFD.write_str(0, menu[pos]);
    }
  }
  
}


void RingAlarm(){
  bool stat = 0;
  for(;;){
    for(uint8_t i=0;i<3;i++){
    digitalWrite(BUZZER, 1);
    delay(100);
    digitalWrite(BUZZER, 0);
    delay(100);
    }
    if(stat){
      VFD.light_off();
    }
    else{
      VFD.light_on();
    }
    delay(1000);
    stat = !stat;
    if(ec11_isr_flag){
      ec11_isr_flag_clear();
      alarm_on = false;
      VFD.light_on();
      return;
    }  
  }

}

//检查中断标志
void CheckISR(){
  if(ec11_isr_flag){
    ShowMainMenu();
  }
}

//EC11按键中断服务
void IRAM_ATTR EC11_ISR(){
  ec11_isr_flag = true;
}


void setup(){
  //Serial.begin(9600);
  pinMode(BUZZER, OUTPUT);
  for(uint8_t i=0;i<2;i++){
    digitalWrite(BUZZER, 1);
    delay(60);
    digitalWrite(BUZZER, 0);
    delay(60);
  } 

  delay(1000);

  VFD.begin(GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7, GPIO_NUM_8);
  VFD.init();
  delay(1000);
  FS_Init();
  bright = ReadBright();  
  VFD.set_bright(bright);

  //配置按键为上拉
  pinMode(TX, INPUT_PULLUP);
  attachInterrupt(TX, EC11_ISR, RISING);
  ec11.begin(1, 2);  

  digitalWrite(BUZZER, 1);
  delay(60);
  digitalWrite(BUZZER, 0);
   
}

void loop(){
  static uint8_t pos = 1;

  if(ec11.flag()){
    VFD.clear();
    if(ec11.read() == 0){
      pos == 2 ? : pos++;
    }
    else{
      pos == 0 ? : pos--;
    }
  }

  switch(pos){
    case 0:
      VFD.write_float(0, (float)analogReadMilliVolts(3)/1000);
      delay(1000);
    break;
    case 1:
      time(&now);
      timeinfo = *localtime(&now);
      updateTime(&timeinfo, bit_data);
      scollTime(bit_data);    
      if(alarm_on){
        if((timeinfo.tm_hour == alarm_time.hour) && (timeinfo.tm_min == alarm_time.min)) RingAlarm();
      }   
      delay(100);
    break;
    case 2:
      VFD.write_user_font(0, 0, pattern[1]);
      VFD.write_user_font(4, 4, pattern[0]);
      VFD.show();
      VFD.write_str(7, "%");
      VFD.write_number(1, dht.readTemperature(), 2);
      VFD.write_number(5, dht.readHumidity(), 2);
      delay(2000);
    break;
  }

 
  CheckISR();

  
  // localtime_r(&now, &timeinfo);
  // VFD.write_user_font(0, 0, number[timeinfo.tm_sec / 10]);
  // VFD.write_user_font(1, 1, number[timeinfo.tm_sec % 10]);
  // delay(1000);

}


