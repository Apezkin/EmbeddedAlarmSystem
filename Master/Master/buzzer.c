//
// Created by lut on 29.4.2022.
//

#include "buzzer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TIMER1_COMPA_vect){
    PORTE ^= (1 << PE3);
    sei();
}

void Buzzer_Init() {

    TCCR1A = 0;
    TCCR1B = 0;
    //TCCR3A |= 1 << WGM30 | 1 << WGM31 | 1 << COM3A1;
    TCCR1B |= 1 << WGM12 | 1 << CS10; //CS00 was shifted into TCCROA#
    TCCR1B = (TCCR1B & 0b11111000) | 0b001;
    OCR1A = F_CPU / 32 / 2 / 64 - 1;

    //TIMSK3 |= 1 << OCIE3A;
}

void Start_Buzzing() {
    //PORTE |= (1 << PE3);
    TIMSK1 |= 1 << OCIE1A;
    //Buzzer_Init();
    //TCCR3B |= (1 << 0); // set prescaling to 1 (no prescaling)
}

void Stop_Buzzing() {
    TIMSK1 &= ~(1 << OCIE1A);
    PORTE &= ~(1 << PE3);
}
