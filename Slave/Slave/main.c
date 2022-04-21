#define F_CPU 16000000UL
#define FOSC 16000000UL // Clock Speed
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <util/setbaud.h>
#include "keypad.h"
#include "usart.h"

FILE uart_output = FDEV_SETUP_STREAM(USART_Transmit, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, USART_Receive, _FDEV_SETUP_READ);

int
main(void)
{	
	DDRB &= ~(1 << PB1); //input
	DDRB |= (1 << PB2); //output
    
    USART_init(MYUBRR);
    stdout = &uart_output;
    stdin = &uart_input;
	

	int motionState = 0;
    char keypadKey;
	
	while (1)
	{
        keypadKey = KEYPAD_GetKey();
        printf("aaa");
		motionState = (PINB & (1 << PB1));
		if (motionState) {
			PORTB |=  (1 << PB2);
		}
        
        if (keypadKey == '1') {
            PORTB |= (1 << PB2);
        }            
		
		_delay_ms(1000);
		PORTB &= ~(1 << PB2);
		_delay_ms(1000);
	}
	
	return 0;
}


