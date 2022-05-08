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
void timer2_init(void) {
    TCCR2A = (1 << WGM21); //Init Timer0, normal, prescalar = 1024
    TCCR2B = (1 << CS22);
    TIMSK2 = (1 << OCIE2A); //enable compA interrupt
    OCR2A = ((F_CPU / 64) / 1000);
}
