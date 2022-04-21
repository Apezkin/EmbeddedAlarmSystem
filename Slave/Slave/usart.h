//
// Created by lut on 21.4.2022.
//

#ifndef SLAVE_USART_H
#define SLAVE_USART_H
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

void USART_init(uint16_t ubrr);

void USART_Transmit(unsigned char data, FILE *stream);

char USART_Receive(FILE *stream);
#endif //SLAVE_USART_H
