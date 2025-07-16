
#include "ds1307rtc.h"

static uint8_t i2c_buffer[RTC_REGISTER_LEN];
const static uint16_t year_2000_offset = 100;

uint8_t ds1307_bcd2dec(uint8_t bcd_value)
{
  int temp = ((uint8_t)(bcd_value & (uint8_t)0xF0) >> (uint8_t)0x04)*10;
  return (temp + (bcd_value & (uint8_t)0x0F));
}

uint8_t ds1307_read_rtc(struct tm *current_time)
{
  // Write 0x00 address to clock to reset register pointer
  i2c_buffer[0] = 0x00;
  int i2c_status = twi_write(RTC_ADDRESS, i2c_buffer, 1);
  if (i2c_status != TWI_OK)
  {
    return 1;
  }
  // Read register 0x00-0x07 values
  i2c_status = twi_read(RTC_ADDRESS,i2c_buffer,RTC_REGISTER_LEN);
  if (i2c_status != TWI_OK)
  {
    return 1;
  }

  // Parse register values to time
  current_time->tm_sec = ds1307_bcd2dec(i2c_buffer[RTC_SEC_INDEX]
                                        & 0x7F);
  current_time->tm_min = ds1307_bcd2dec(i2c_buffer[RTC_MIN_INDEX]);
  current_time->tm_hour = ds1307_bcd2dec(i2c_buffer[RTC_HOUR_INDEX]);
  // Week days need to be put in range [0-6]
  current_time->tm_wday = ds1307_bcd2dec(i2c_buffer[RTC_DOW_INDEX]) - 1;
  current_time->tm_mday = ds1307_bcd2dec(i2c_buffer[RTC_DAY_INDEX]);
  // Months need to be put in range [0-11]
  current_time->tm_mon = ds1307_bcd2dec(i2c_buffer[RTC_MON_INDEX]) - 1;
  current_time->tm_year = ds1307_bcd2dec(i2c_buffer[RTC_YEAR_INDEX]) + year_2000_offset;

  return 0;
}

uint8_t ds1307_configure_square_wave(enum Ds1307SquareWaveEnable enable,
    enum Ds1307SquareWaveLogic logic_level,
    enum Ds1307SquareWaveFreq freq)
{
  // Setup the data to write to the device
  // first is the address of the control registers on the clock
  i2c_buffer[0] = 0x07;
  // only if enable is on, setup the register value to be written based on inputs
  if (enable == SqwOn)
  {
    i2c_buffer[1] = ((uint8_t)logic_level << 7) | ((uint8_t)enable << 4) | (uint8_t)freq;
  }
  // otherwise, just write it to all zeros to make sure no output
  else
  {
    i2c_buffer[1] = 0;
  }
  // Write to the device. If there was an I2C error, return generic error status
  int i2c_status = twi_write(RTC_ADDRESS, i2c_buffer, 2);
  if (i2c_status != TWI_OK)
  {
    return 1;
  }
  return 0;
}
