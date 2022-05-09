//
// Created by lut on 29.4.2022.
//

#ifndef MASTER_BUZZER_H
#define MASTER_BUZZER_H

/**
 @brief   Inits Timer 1 for the buzzer.

 @param   none
 @return  none
*/
void buzzer_init();

/**
 @brief   Starts buzzing sound by enabling COMPA interrupt for timer.

 @param   none
 @return  none
*/
void Start_Buzzing();

/**
 @brief   Stops buzzing sound by disabling COMPA interrupt for timer.

 @param   none
 @return  none
*/
void Stop_Buzzing();

#endif //MASTER_BUZZER_H
