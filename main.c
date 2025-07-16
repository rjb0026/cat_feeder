
#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "circular_buffer.h"
#include "feed_switch.h"
#include "twi.h"
#include "ds1307rtc.h"

// Defines and macros
#define USART_BAUD 9600
#define BAUD_PRESCALE ((((F_CPU / 16) + (USART_BAUD  / 2)) / (USART_BAUD))- 1)

#define BLINK_PIN PB1
#define ADC_LED_PIN PB0
#define ADC_PIN PC0
#define BUTTON_PIN PD2
#define BLINK_TIME 1000
#define UART_BUFFER_SIZE 128

// Volatile global flags
volatile struct
{
//  uint8_t adc_interrupt: 1;
  uint8_t print: 1;
  uint8_t button: 1;
} InterruptFlags;

// Helper functions
static void usart_setup(void)
{
  // Set baudrate prescaler
  UBRR0H = (BAUD_PRESCALE >> 8);
  UBRR0L = BAUD_PRESCALE;
  // Enable RX and TX pins
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);
  // Setings: 8 data bits, a single stop bit, no parity
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
  // Enable an interrupt on received serial data
  UCSR0B |= (1 << RXCIE0);
}

static void usart_put_char(char c)
{
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0=c;
}

static void usart_print_strn(const char *str, uint8_t size)
{
  for (uint8_t i=0; (i < size) && (*str != '\0');++i)
  {
    if (*str == '\n')
    {
      usart_put_char('\r');
    }
    usart_put_char(*str++);
  }
}

/* static void usart_print_strn_progmem(const char *str, uint8_t size) */
/* { */
/*   char c; */
/*   for (uint8_t i=0; (i < size) && (*str != '\0');++i) */
/*   { */
/*     c = pgm_read_byte(str); */
/*     ++str; */
/*     if (c == '\n') */
/*     { */
/*       usart_put_char('\r'); */
/*     } */
/*     usart_put_char(c); */
/*   } */
/* } */

/* void fill_usart_buffer_abc(cbuffer_handle_t buffer_handle) */
/* { */
/*   for (char data='a'; data <= 'z'; data++) */
/*   { */
/*     circular_buffer_write(buffer_handle, (uint8_t)data); */
/*   } */
/*   circular_buffer_write(buffer_handle, (uint8_t)'\r'); */
/*   circular_buffer_write(buffer_handle, (uint8_t)'\n'); */
/* } */

/* void print_usart_buffer(cbuffer_handle_t buffer_handle) */
/* { */
/*   while (circular_buffer_empty(buffer_handle) !=0) */
/*   { */
/*     char print_byte; */
/*     // Wait until the USART is ready to transmit */
/*     while ((UCSR0A & (1 << UDRE0)) == 0); */
/*     // Read the byte */
/*     if (circular_buffer_read(buffer_handle, (uint8_t*)&print_byte) == -1) */
/*       break; */
/*     // Send the byte to the uart output */
/*     UDR0 = print_byte; */
/*   } */
/* } */

// Stepper driving
void step_motor(uint16_t steps)
{
  // Pulse the step pin
  for (uint16_t step=0; step < steps; ++step)
  {
    // Turn step pin on
    PORTB |= (1 << BLINK_PIN);
    _delay_ms(1);
    // Turn step pin off
    PORTB &= ~(1 << BLINK_PIN);
    _delay_ms(1);
  }
}

// ISR Definitions
ISR(USART_RX_vect)
{
  // Read byte
  char receivedByte;
  receivedByte = UDR0;
  // Set flag
  InterruptFlags.print = 1;
}

ISR(INT0_vect)
{
  // Disable button interrupts
  EIMSK &= ~(1 << INT0);
  // Set button event flag
  InterruptFlags.button = 1; 
}

int main(void)
{
  // Set pin outputs
  DDRB |= (1 << BLINK_PIN) | (1 << ADC_LED_PIN);

  // Set Button pin to input
  DDRD &= ~(1 << BUTTON_PIN);

  // Setup button to trigger on rising edge of INT0
  EICRA |= (1 << ISC01) | (1 << ISC00);
  // Enable INT0 interrupt
  EIMSK = (1 << INT0);

  // setup USART 0 on RX pin 1 and TX pin 2
  usart_setup();

  // I2C Setup
  int twi_status = twi_init();
  if (twi_status != TWI_OK)
  {
    usart_print_strn("TWI ERROR\n",10);
  }
  // Setup data to hold the current time read from the RTC
  struct tm my_time;
  char time_print_buffer[34];
  
  // Allocate a byte buffer
  //uint8_t uart_byte_buffer[UART_BUFFER_SIZE];
  //struct circular_buffer_t uart_buffer = 
  //  circular_buffer_init(uart_byte_buffer, UART_BUFFER_SIZE);

  // Setup the feed switch
  feed_switch_open(Polling, A0); 
  // Setup some character buffer for ADC values
  char adc_buffer[32];

  // Print a startup message
  usart_print_strn("Prog Start:\n", 12);

  // Globally enable interrupts
  sei();

  while (1)
  {
    // print ADC value in string
    if (InterruptFlags.button == 1)
    {
      // Read time value
      if (ds1307_read_rtc(&my_time))
      {
        usart_print_strn("Failed to read from RTC\n",24);
      }
      else
      {
        int time_print_size = sprintf(time_print_buffer, "%02d:%02d:%02d %02d/%02d/%04d\n",
            my_time.tm_hour, my_time.tm_min, my_time.tm_sec, 
            my_time.tm_mon, my_time.tm_mday, my_time.tm_year);
        usart_print_strn(time_print_buffer, time_print_size);
      }
      // Handle button interrupt
      InterruptFlags.button = 0;
      int print_size = sprintf(adc_buffer, "%S\n", feed_switch_get_mode_str(feed_switch_read()));
      if (print_size <= 0)
      {
        usart_print_strn("Fail\n",6);
      }
      else
      {
        usart_print_strn(adc_buffer, print_size+1);
      }
      step_motor(200);
      // Reinable the button interrupt
      EIMSK |= (1 << INT0);
    }
    // Toggle LED blink pin 
    //PORTB ^= (1 << BLINK_PIN);
    //_delay_ms(BLINK_TIME);
    
    // Set into sleep mode or something
  }

  return 0;
}

