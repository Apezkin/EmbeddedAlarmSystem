
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

void USART_init(uint16_t ubrr);

int USART_Transmit(char data, FILE *stream);

int USART_Receive(FILE *stream);