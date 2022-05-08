//
// Created by lut on 29.4.2022.
//

#include "buzzer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TIMER1_COMPA_vect){
    //switch port on/off to make sound.
    PORTE ^= (1 << PE3);
    //for some reason has to enable interrupts after every interrupt. Otherwise deadlocks.
    sei();
}

void Buzzer_Init() {

    //clean clock
    TCCR1A = 0;
    TCCR1B = 0;

    TCCR1B |= 1 << WGM12 | 1 << CS10;
    TCCR1B = (TCCR1B & 0b11111000) | 0b001;

    // set frequency
    OCR1A = F_CPU / 32 / 2 / 64 - 1;
}

void Start_Buzzing() {
    //enable COMPA interrupt to start buzzing.
    TIMSK1 |= 1 << OCIE1A;
}

void Stop_Buzzing() {
    //disable COMPA interrupt to start buzzing.
    TIMSK1 &= ~(1 << OCIE1A);
    //set buzzer pin off.
    PORTE &= ~(1 << PE3);
}
