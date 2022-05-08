//
//https://www.electronicwings.com/avr-atmega/atmega1632-i2c
//

#include "i2c.h"

void I2C_init()
{
    TWSR = 0x00;
    TWBR = 0x03;
}

uint8_t I2C_write_start(char slave_write_addr)
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

uint8_t I2C_read_start(char slave_read_addr)
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

uint8_t I2C_write(char data) {
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

char I2C_read_ack() {
    TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

char I2C_read_nack() {
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

void I2C_stop() {
    TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);
    while (TWCR & (1 << TWSTO));
}

int8_t I2C_command_write(char command) {
    uint8_t twi_status = I2C_write_start(SLAVE_WRITE_ADDR);
    if (twi_status == TWI_ERROR || twi_status == TWI_START_FAILED) {
        return TWI_ERROR;
    }
    twi_status = I2C_write(command);
    if (twi_status != TWI_ACK_RECEIVED) {
        return TWI_ERROR;
    }
    I2C_stop();
    return TWI_OK;
}
