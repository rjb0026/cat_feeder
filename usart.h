/*
 * @author Robert Brothers
 * @date 4-16-2020
 */

#ifndef _CAT_FEEDER_USART_H_
#define _CAT_FEEDER_USART_H_

#include "avr/io.h"

#define USART_BAUD 9600
#define BAUD_PRESCALE ((((F_CPU / 16) + (USART_BAUD  / 2)) / (USART_BAUD))- 1)

void usart_open(void);

void usart_close(void);

void usart_put_char(char c);

void usart_print_strn(const char *str, uint8_t size);

#endif
