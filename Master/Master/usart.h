#ifndef MASTER_USART_H
#define MASTER_USART_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>



/**
 @brief   Initializes USART.

 @param   ubrr frequency of the communication.
 @return  none
*/
void USART_init(uint16_t ubrr);

/**
 @brief   Transmits data to destination.

 @param   data byte to be transferred.
 @param   stream
 @return  none
*/
int USART_Transmit(char data, FILE *stream);

/**
 @brief   Reads data from destination.

 @param   stream
 @return  data from destination.
*/
int USART_Receive(FILE *stream);

#endif //MASTER_USART_H