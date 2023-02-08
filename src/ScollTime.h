#include <VFD.h>

typedef struct{
    uint8_t numLst = 0;
    uint8_t numNow = 0;
    uint8_t buffer_now[5] = {0};
    uint8_t buffer_lst[5] = {0};
    bool scollFlag = 0;
}bit_data_t;

//更新每一位为当前时间
void updateTime(tm *tmnow, bit_data_t *bit_data){
  for(uint8_t i=0;i<6;i++){
    bit_data[i].numLst = bit_data[i].numNow;
  }    
  bit_data[0].numNow = tmnow->tm_hour / 10;
  bit_data[1].numNow = tmnow->tm_hour % 10;
  bit_data[2].numNow = tmnow->tm_min / 10;
  bit_data[3].numNow = tmnow->tm_min % 10;
  bit_data[4].numNow = tmnow->tm_sec / 10;
  bit_data[5].numNow = tmnow->tm_sec % 10;
}

//滚动一次
void scollTime(bit_data_t *bit_data){
    VFD.write_one_char(2, 10);
    VFD.write_one_char(5, 10);
    VFD.show();
    
    for(uint8_t i=0;i<6;i++){
        if(bit_data[i].numNow != bit_data[i].numLst) {
          bit_data[i].scollFlag = true;                        //更改滚动标志位
          memcpy(bit_data[i].buffer_lst, number[bit_data[i].numLst], sizeof(number[bit_data[i].numLst]));  //填充buffer_now
          memcpy(bit_data[i].buffer_now, number[bit_data[i].numNow], sizeof(number[bit_data[i].numNow]));
        }
        else{
          bit_data[i].scollFlag = false;
        }
    }
    for(uint8_t i=0;i<7;i++){
      for(uint8_t j=0;j<6;j++){
        if(bit_data[j].scollFlag){
          for(uint8_t k=0;k<5;k++){
            bit_data[j].buffer_lst[k] <<= 1;
            bit_data[j].buffer_lst[k] |= (bit_data[j].buffer_now[k] & 0x40) == 0x40;
            bit_data[j].buffer_now[k] <<= 1;  
          }
          VFD.write_user_font(j+j/2, j+j/2, bit_data[j].buffer_lst);
        }
        else{
          VFD.write_user_font(j+j/2, j+j/2, number[bit_data[j].numNow]);
        }
      }

      VFD.show();
      //delay(30);
      ets_delay_us(30000);
    }

}