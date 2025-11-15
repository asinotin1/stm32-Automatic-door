#include <DS1307.h>
#include "stm32f1xx_hal.h"

extern I2C_HandleTypeDef hi2c1;
uint8_t B2D( uint8_t num)
{
  return ((num >> 4)*10 + (num & 0x0F));
}

uint8_t D2B(uint8_t num)
{
  return ((num / 10) << 4) | (num % 10);
}


void DS1307_Write(DS1307_RTC *rtc)
{
  uint8_t data[7];
  
  // Chuyển đổi Decimal sang BCD
  data[0] = D2B(rtc->sec) & 0x7F;   // Đảm bảo CH = 0 (clock running)
  data[1] = D2B(rtc->min);
  data[2] = D2B(rtc->hour) & 0x3F;  // Chế độ 24h
  data[3] = 0x01;                    // Day of week (1-7)
  data[4] = D2B(rtc->date);
  data[5] = D2B(rtc->month);
  data[6] = D2B(rtc->year);
  
  // Ghi 7 byte từ địa chỉ 0x00
  HAL_I2C_Mem_Write(&hi2c1, DS1307_ADDR, 0x00, 1, data, 7, HAL_MAX_DELAY);
}


void DS1307_Read(DS1307_RTC *rtc)
{
  uint8_t data[7];
  
  // Đọc 7 byte từ địa chỉ 0x00
  HAL_I2C_Mem_Read(&hi2c1, DS1307_ADDR, 0x00, 1, data, 7, HAL_MAX_DELAY);
  
  // Chuyển đổi BCD sang Decimal
  rtc->sec = B2D(data[0] & 0x7F);   // Bit 7 là CH (Clock Halt)
  rtc->min = B2D(data[1] & 0x7F);
  rtc->hour = B2D(data[2] & 0x3F);  // Bit 6 là chế độ 12/24h
  rtc->date = B2D(data[4] & 0x3F);
  rtc->month = B2D(data[5] & 0x1F);
  rtc->year = B2D(data[6]);
}

void DS1307_Start(void)
{
  uint8_t data;
  
  // Đọc byte đầu tiên
  HAL_I2C_Mem_Read(&hi2c1, DS1307_ADDR, 0x00, 1, &data, 1, HAL_MAX_DELAY);
  
  // Xóa bit CH (bit 7) để bật clock
  data &= 0x7F;
  
  // Ghi lại
  HAL_I2C_Mem_Write(&hi2c1, DS1307_ADDR, 0x00, 1, &data, 1, HAL_MAX_DELAY);
}

void DS1307_Stop(void)
{
  uint8_t data;
  
  // Đọc byte đầu tiên
  HAL_I2C_Mem_Read(&hi2c1, DS1307_ADDR, 0x00, 1, &data, 1, HAL_MAX_DELAY);
  
  // Set bit CH (bit 7) để dừng clock
  data |= 0x80;
  
  // Ghi lại
  HAL_I2C_Mem_Write(&hi2c1, DS1307_ADDR, 0x00, 1, &data, 1, HAL_MAX_DELAY);
}