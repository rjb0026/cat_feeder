
#ifndef _DS1307RTC_H_
#define _DS1307RTC_H_

#include <time.h>
#include "twi.h"

#ifndef RTC_ADDRESS
#define RTC_ADDRESS 0x68
#endif
#ifndef RTC_REGISTER_LEN
#define RTC_REGISTER_LEN 8
#endif

#define RTC_SEC_INDEX 0
#define RTC_MIN_INDEX 1
#define RTC_HOUR_INDEX 2
#define RTC_DOW_INDEX 3
#define RTC_DAY_INDEX 4
#define RTC_MON_INDEX 5
#define RTC_YEAR_INDEX 6

/// @brief An enumeration of the square wave output speeds on the SQW pin
enum Ds1307SquareWaveFreq
{
  Freq1Hz=0,
  Freq4kHz=1,
  Freq8kHz=2,
  Freq32kHz=3
};

/// @brief An enumeration of the square wave enable states
enum Ds1307SquareWaveEnable
{
  SqwOff=0,
  SqwOn=1
};

/// @brief An enumeration of the square wave logic level
enum Ds1307SquareWaveLogic
{
  SqwLogicLow=0,
  SqwLogicHigh=1
};

/// @brief Utility function to convert binary coded decimal to actual decimal
/// @param bcd_value is the value in bcd to convert to decimal
/// @returns decimal the integer value of the bcd input
uint8_t ds1307_bcd2dec(uint8_t bcd_value);

/// @brief Read current time from the RTC
///
/// The primary API for reading the entire time value from
/// the DS1307 real time clock.
/// @param current time is a pointer to the value of the current time to be updated
/// @returns 0 if there are no errors
uint8_t ds1307_read_rtc(struct tm *current_time);

/// @brief Configure the square wave output for the DS1307 module
/// @param enable turns on the square wave output when set to 1 and off when set to zero
/// @param logic_level specifies logic level high (1) when set to 1 and logic level low (0) when set to zero
/// @param freq specifies the desired square wave output frequency (0 = 1 Hz)
/// @returns 0 if configuration was successfull, returns 1 if a communcation error occured.
uint8_t ds1307_configure_square_wave(enum Ds1307SquareWaveEnable enable,
    enum Ds1307SquareWaveLogic logic_level,
    enum Ds1307SquareWaveFreq freq);

#endif
