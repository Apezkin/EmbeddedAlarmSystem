//
// Created by aksu-lut on 28.4.2022.
//

#ifndef SLAVE_I2C_H
#define SLAVE_I2C_H
#include <avr/io.h>

#define SLAVE_READ 0
#define SLAVE_WRITE 1

void I2C_Slave_Init(char slave_address);


int8_t I2C_Slave_Listen();
int8_t I2C_Slave_Transmit(char data);
int8_t I2C_Slave_Transmit_Nack(char data);

char I2C_Slave_Receive();

#endif //SLAVE_I2C_H
