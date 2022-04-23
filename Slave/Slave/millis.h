//
// Created by lut on 22.4.2022.
//

#ifndef SLAVE_MILLIS_H
#define SLAVE_MILLIS_H
#include <avr/io.h>
#include <avr/interrupt.h>



unsigned long int millis(void);

void Timer0_Init(void);


#endif //SLAVE_MILLIS_H
