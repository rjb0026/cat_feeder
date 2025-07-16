
#include "feed_switch.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* DEFINES */
#define FEED_MODE_LOW_TO_MED 256
#define FEED_MODE_MED_TO_HIGH 768

/* PRIVATE GLOBALS */
static enum ADCMode adc_mode;
static volatile enum FeedMode current_mode;

/* PUBLIC GLOBAL DEFINITIONS */
const char FeedLowString[] PROGMEM = "FeedLow\0";
const char FeedMedString[] PROGMEM = "FeedMed\0";
const char FeedHighString[] PROGMEM = "FeedHigh\0";
const char *FeedModeStrings[3] = {FeedLowString, FeedMedString, FeedHighString};

ISR(ADC_vect)
{
  // copy out the value from the result register
  uint16_t adc_value =  ADCL;
  adc_value |= (ADCH << 8);
  // Decode the current mode
  if (adc_value < FEED_MODE_LOW_TO_MED)
    current_mode = FeedLow;
  else if (adc_value >= FEED_MODE_LOW_TO_MED && adc_value < FEED_MODE_MED_TO_HIGH)
    current_mode = FeedMed;
  else
    current_mode = FeedHigh;
}

/// @brief Initialize the ADC driver interface
/// @param mode selects wether the ADC is polled vs run as a background task
static void adc_open(enum ADCMode mode)
{
  // Enable ADC and setup clock frequency to to F_CPU/8
  ADCSRA = (1 << ADEN) | (1 << ADPS1) | (1 << ADPS0);
  // Set to internal voltage reference source
  // ADMUX register: [REFS1; REFS0; ADLAR;  - ; MUX3; MUX2; MUX1; MUX0]
  // REFS1 = 0 & REFS0 = 0 is using external AREF pin
  // REFS1 = 0 & REFS0 = 1 is using internal AVcc with external capacitor at AREF
  // REFS1 = 1 & REFS0 = 1 is using internal 1.1V reference source
  ADMUX |= (1 << REFS0);
  // Set the current mode
  adc_mode = mode;
  switch (adc_mode)
  {
    case Polling:
    {
      // Make sure interrupt enable and auto trigger is turned off 
      ADCSRA &= ~((1<< ADATE) | (1 << ADIE));
      break;
    }
    case FreeRunning:
    {
      // ADCSRB defaults to free running mode
      // Set auto triggering and interrupts enabled
      ADCSRA |= (1 << ADATE) | (1 << ADIE);
      // Start ADC conversions
      ADCSRA |= (1 << ADSC);
      break;
    }
    default:
    {
      adc_mode = Polling;
      ADCSRA &= ~((1<< ADATE) | (1 << ADIE));
    }
  }
}

/// @brief Initialize the pins of an ADC channel
static void adc_open_channel(enum AnalogChannel channel)
{
  // Set the ADC pin to be an input
  DDRC &= ~(1 << channel);
  // Disable the internal pull-up for the ADC pin
  PORTC &= ~(1 << channel);
  // Disable the input buffer on the correct ADC pin
  if (channel < 6)
  {
    DIDR0 |= (1 << channel);
  }
  // Select the channel in the MUX register
  ADMUX = (ADMUX & 0xF0) | channel;
}

/// @brief Disable the ADC interface
static void adc_close(void)
{
  // Turn off ADC enable bit
  ADCSRA &= ~(1 << ADEN);
}

enum FeedMode feed_switch_read(void)
{
  // Polling mode
  if (adc_mode == Polling)
  {
    // Write zero to ADC power reduction
    PRR &= ~(1 << PRADC);
    // Start the ADC conversion
    ADCSRA |= (1 << ADSC);
    // Poll until the ADSC bit is cleared
    loop_until_bit_is_clear(ADCSRA, ADSC);
    // Read the ADC value and decode it
    uint16_t current_adc_value = ADCL;
    current_adc_value |= (ADCH << 8);
    // Decode the current mode
    if (current_adc_value  < FEED_MODE_LOW_TO_MED)
      current_mode = FeedLow;
    else if (current_adc_value >= FEED_MODE_LOW_TO_MED && current_adc_value < FEED_MODE_MED_TO_HIGH)
      current_mode = FeedMed;
    else
      current_mode = FeedHigh;
  }

  return current_mode;
}

void feed_switch_open(enum ADCMode mode, enum AnalogChannel channel)
{
  // Initialize channel for switch
  adc_open_channel(channel);
  // Initialize ADC interface
  adc_open(mode);
}

void feed_switch_close(void)
{
  // Close the ADC interface
  adc_close();
}

const char * feed_switch_get_mode_str(enum FeedMode mode)
{
  return FeedModeStrings[mode];
}
