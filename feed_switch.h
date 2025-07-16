/*
 * @file feed_switch.h
 * @brief An encapsulation of the feed switch mechanism. Drives a single ADC channel.
 * @author Robert Brothers
 * @date 4-13-2020
 */
#ifndef _CATFEEDER_FEED_SWITCH_H_
#define _CATFEEDER_FEED_SWITCH_H_

#include <stdint.h>
#include <avr/pgmspace.h>

/// @brief AnalogChannel is an enumeration of the available channels
enum AnalogChannel
{
  A0=0,
  A1,
  A2,
  A3,
  A4,
  A5,
  A6,
  A7,
  A8
};

/// @brief ADCMode is an enumeration of the operational modes of the ADC
///
/// in Polling mode, each read call will start an ADC conversion and block
/// until the conversion is complete
/// in FreeRunning mode, the ADC is continuously sampled in the background
/// and each read call will fetch the most recent state of the switch
enum ADCMode
{
  Polling = 0,
  FreeRunning
};

/// @brief FeedSwitch is an enumeration of the modes of
/// the feed selector switch
enum FeedMode
{
  FeedLow = 0,
  FeedMed = 1,
  FeedHigh = 2
};

/// @brief Flash stored string for FeedMode::FeedLow
extern const char FeedLowString[] PROGMEM;
/// @brief Flash stored string for FeedMode::FeedMed
extern const char FeedMedString[] PROGMEM;
/// @brief Flash stored string for FeedMode::FeedHigh
extern const char FeedHighString[] PROGMEM;
/// @brief An indexable list of the flash stored feed mode strings
extern const char *FeedModeStrings[3];

/// @brief open the feed mode select switch interface
/// @param mode selects wether the ADC is polled vs run as a background task
/// @param channel is the analog channel the switch output is connected to
void feed_switch_open(enum ADCMode mode, enum AnalogChannel channel);

/// @brief close the feed mode select switch interface 
void feed_switch_close(void);

/// @brief Get the current feed mode
/// @returns FeedMode - the current mode selection of the feed selector switch
enum FeedMode feed_switch_read(void);

/// @brief Get the flash stored string for an input FeedMode
/// @param mode is the desired feedmode
/// @returns FeedModeStr_P - a flash stored const char * corresponding to the input mode
const char * feed_switch_get_mode_str(enum FeedMode mode);

#endif


