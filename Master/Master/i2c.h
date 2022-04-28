//
// Created by aksu-lut on 28.4.2022.
//

#ifndef MASTER_I2C_H
#define MASTER_I2C_H
#include <avr/io.h>

#ifndef SLAVE_WRITE_ADDR
#define SLAVE_WRITE_ADDR 0x20
#endif
#ifndef SLAVE_READ_ADDR
#define SLAVE_READ_ADDR 0x21
#endif

#define TWI_START_FAILED 0
#define TWI_ACK_RECEIVED 1
#define TWI_NACK_RECEIVED 2
#define TWI_ERROR 3


uint8_t I2C_Start_Write(char slave_write_addr);
uint8_t I2C_Start_Read(char slave_read_addr);

void I2C_Init();

char I2C_Read_Ack();
char I2C_Read_Ack_Timeout(unsigned long timeout);
char I2C_Read_Nack();
void I2C_Stop();

uint8_t I2C_Write_From_Buffer(char *buffer, uint8_t count);
void I2C_Read_To_Buffer(char *buffer, uint8_t count);
#endif //MASTER_I2C_H
