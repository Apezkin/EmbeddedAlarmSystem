//
// Created by lut on 21.4.2022.
//

#include "usart.h"

void USART_init(uint16_t ubrr) // unsigned int
{

    UBRR0H = (unsigned char) (ubrr >> 8);
    UBRR0L = (unsigned char) ubrr;

    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);

    UCSR0C |= (1 << USBS0) | (3 << UCSZ00);
}

int USART_Transmit(char data, FILE *stream) {

    while (!(UCSR0A & (1 << UDRE0))) 
	{ 
    }
    UDR0 = data;
	return 0;
}

int USART_Receive(FILE *stream) {

    while (!(UCSR0A & (1 << UDRE0))) 
	{ ;
    }
    return UDR0;
}