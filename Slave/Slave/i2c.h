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

char I2C_Slave_Receive();

void I2C_Read_To_Buffer(char *buffer, uint8_t count);
#endif //SLAVE_I2C_H
