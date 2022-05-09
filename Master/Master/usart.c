#include "usart.h"

//inits UART 0.
void USART_init(uint16_t ubrr)
{
    UBRR0H = (unsigned char) (ubrr >> 8);
    UBRR0L = (unsigned char) ubrr;

    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
    UCSR0C |= (1 << USBS0) | (3 << UCSZ00);
}

int USART_Transmit(char data, FILE *stream)
{
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

int USART_Receive(FILE *stream)
{
    while(!(UCSR0A & (1 << UDRE0)));
    return UDR0;
}