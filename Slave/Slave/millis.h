//
// Created by lut on 22.4.2022.
//

#ifndef SLAVE_MILLIS_H
#define SLAVE_MILLIS_H
#include <avr/io.h>
#include <avr/interrupt.h>



/**
 @brief   gets time from the start of the program.

 @param   none
 @return  time in ms
*/
unsigned long int millis(void);

/**
 @brief   Starts timer.

 Initializes timer 2 and enables compA interrupts.
 @param   none
 @return  none
*/
void MILLIS_init(void);


#endif //SLAVE_MILLIS_H
