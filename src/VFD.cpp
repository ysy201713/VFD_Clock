/**
 * @file VFD.h
 * 此库为硬件SPI版本
*/

#include <Arduino.h>
#include "VFD.h"
#include <SPI.h>


void VFDClass::begin(gpio_num_t din, gpio_num_t clk, gpio_num_t cs, gpio_num_t rst, gpio_num_t en){
  _din = din;
  _clk = clk;
  _cs = cs;

  SPI.begin(_clk, -1, _din);

  pinMode(cs, OUTPUT);
  pinMode(rst, OUTPUT);
  pinMode(en, OUTPUT);

  gpio_set_level(en, 1);
  delay(1000);
  gpio_set_level(rst, 0);
  
  gpio_set_level(rst, 1);
}

// 写指令（库内部使用）
void VFDClass::write_data(uint8_t data){
  SPI.beginTransaction(SPISettings(190000, LSBFIRST, SPI_MODE2));
  SPI.write(data);
  SPI.endTransaction();
}

// 设置亮度0-255
void VFDClass::set_bright(uint8_t level){
  gpio_set_level(_cs, 0);
  write_data(0xe4);
  
  write_data(level);
  gpio_set_level(_cs, 1);
  
}

// 初始化
void VFDClass::init(){
  gpio_set_level(_cs, 0);
  write_data(0xe0);
  
  write_data(0x07);//6 digtal 0x05 // 8 digtal 0x07//16 digtal 0x0f
  gpio_set_level(_cs, 1);
  

  //set bright
  //设置亮度
  gpio_set_level(_cs, 0);
  write_data(0xe4);
  
  write_data(0x10);
  gpio_set_level(_cs, 1);
  
}

// 写指令
void VFDClass::write_cmd(uint8_t cmd){
  gpio_set_level(_cs, 0);
  write_data(cmd);
  gpio_set_level(_cs, 1);
}

// 开显示命令
void VFDClass::show(){
  gpio_set_level(_cs, 0);
  write_data(0xe8);     
  gpio_set_level(_cs, 1);
}

/**
 * @brief 在指定位置显示一个字符
 * @param x  显示的位置
 * @param chr 要显示的字符
*/ 
void VFDClass::write_one_char(unsigned char x, unsigned char chr){
  gpio_set_level(_cs, 0);  //开始传输
  write_data(0x20 + x); //地址寄存器起始位置
  write_data(chr + 0x30);//显示内容数据
  gpio_set_level(_cs, 1);
}

/******************************
  在指定位置打印字符串
  (仅适用于英文,标点,数字)
  x:位置;str:要显示的字符串
*******************************/
void VFDClass::write_str(unsigned char x,const char *str)
{
  gpio_set_level(_cs, 0);  //开始传输
  write_data(0x20 + x); //地址寄存器起始位置
  while (*str)
  {
    write_data(*str); //ascii与对应字符表转换
    str++;
  }
  gpio_set_level(_cs, 1);
  show();
}

// 需调用show
void VFDClass::write_user_font(unsigned char x, unsigned char y,unsigned char *s)
{
  gpio_set_level(_cs, 0); //开始传输 
    write_data(0x40+y);//地址寄存器起始位置
    for(uint8_t i=0;i<5;i++){
      write_data(s[i]);
    }

  gpio_set_level(_cs, 1); //停止传输

  gpio_set_level(_cs, 0);
  write_data(0x20+x);
  write_data(0x00+y);   
  gpio_set_level(_cs, 1);

  //show();
}

void VFDClass::write_number(uint8_t x, uint8_t num, uint8_t bits){
  char buffer[5] = {0};
  if(bits == 2){
    sprintf(buffer, "%.2d", num);
  }
  else{
    sprintf(buffer, "%.3d", num); 
  }
  write_str(x, buffer);
}

// 清屏
void VFDClass::clear(){
  write_str(0, "        ");
}

// 开启所有显示
void VFDClass::light_on(){
  write_cmd(0xE8);
}

// 关闭所有显示
void VFDClass::light_off(){
  write_cmd(0xEA);
}

void VFDClass::write_float(uint8_t x, float num){
  char buffer[7] = {0};
  sprintf(buffer, "%.3fV", num);
  write_str(x, buffer);
}

VFDClass VFD;