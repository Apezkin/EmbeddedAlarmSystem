//
//https://www.electronicwings.com/avr-atmega/atmega1632-i2c
//

#include "i2c.h"

void I2C_Init()
{
    TWSR = 0x00;
    TWBR = 0x03;
}

uint8_t I2C_Start_Write(char slave_write_addr)
{
    uint8_t twi_status;
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
    twi_status = TWSR & 0xF8;
    if (twi_status != 0x08)
        return TWI_START_FAILED;
    TWDR = slave_write_addr;
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
    twi_status = TWSR & 0xF8;
    if (twi_status == 0x18)
        return TWI_ACK_RECEIVED;
    if (twi_status == 0x20)
        return TWI_NACK_RECEIVED;

    return TWI_ERROR;
}

uint8_t I2C_Start_Read(char slave_read_addr)
{
    uint8_t twi_status;
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
    twi_status = TWSR & 0xF8;
    if (twi_status != 0x08)
        return TWI_START_FAILED;
    TWDR = slave_read_addr;
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
    twi_status = TWSR & 0xF8;
    if (twi_status == 0x40)
        return TWI_ACK_RECEIVED;
    if (twi_status == 0x48)
        return TWI_NACK_RECEIVED;

    return TWI_ERROR;
}

uint8_t I2C_Write(char data) {
    uint8_t tw_status;
    TWDR = data;
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
    tw_status = TWSR & 0xF8;
    if (tw_status == 0x28)
        return TWI_ACK_RECEIVED;
    if (tw_status == 0x30)
        return TWI_NACK_RECEIVED;

    return TWI_ERROR;
}

char I2C_Read_Ack() {
    TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

char I2C_Read_Nack() {
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

void I2C_Stop() {
    TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);
    while (TWCR & (1 << TWSTO));
}

int8_t I2C_command_write(char command) {
    uint8_t twi_status = I2C_Start_Write(SLAVE_WRITE_ADDR);
    if (twi_status == TWI_ERROR || twi_status == TWI_START_FAILED) {
        return TWI_ERROR;
    }
    twi_status = I2C_Write(command);
    if (twi_status != TWI_ACK_RECEIVED) {
        return TWI_ERROR;
    }
    I2C_Stop();
    return TWI_OK;
}
