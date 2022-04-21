#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "delay.h"
#include "keypad.h"
#include "stdutils.h"

#define F_CPU 16000000UL


int
main(void)
{	
	DDRB &= ~(1 << PB1); //input
	DDRB |= (1 << PB2); //output
	

	int motionState = 0;
	
	while (1)
	{
		motionState = (PINB & (1 << PB1));
		if (motionState) {
			PORTB |=  (1 << PB2);
		}
		
		_delay_ms(10000);
		PORTB &= ~(1 << PB2);
		_delay_ms(10000);
	}
	
	return 0;
}


