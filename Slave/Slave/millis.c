//
// Created by lut on 22.4.2022.
//

#include "millis.h"
#include "main.h"

volatile unsigned long ms;

ISR(TIMER2_COMPA_vect) 
{
    ms++;
}

unsigned long int millis(void) 
{
    return ms;
}

// Help from:
// https://github.com/bendebled/avr-atmega328p-millis/blob/master/millis.c
void MILLIS_init(void) 
{
    TCCR2A = (1 << WGM21);
    TCCR2B = (1 << CS22);
    TIMSK2 = (1 << OCIE2A);
    OCR2A = ((F_CPU / 64) / 1000);
}