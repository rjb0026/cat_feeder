

#include "usart.h"

void usart_open(void)
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

void usart_close(void)
{
  // Disable interrupts
  UCSR0B &= ~(1 << RXCIE0);
  // Disable RX and TX pins
  UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0));
}

void usart_put_char(char c)
{
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0=c;
}

void usart_print_strn(const char *str, uint8_t size)
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
