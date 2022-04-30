//
// Created by aksu-lut on 28.4.2022.
//

#include "millis.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned long ms;

ISR(TIMER2_COMPA_vect) {
        ms++;
}

unsigned long int millis(void) {
    return ms;
}
// Help from:
// https://github.com/bendebled/avr-atmega328p-millis/blob/master/millis.c
void Timer2_Init(void) {
    TCCR2A = _BV(WGM21); //Init Timer0, normal, prescalar = 1024
    TCCR2B = _BV(CS22);
    TIMSK2 = _BV(OCIE2A); //Set TOIE bit
    OCR2A = ((F_CPU / 64) / 1000);
}
