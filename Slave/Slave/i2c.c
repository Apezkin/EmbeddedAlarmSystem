//
//https://www.electronicwings.com/avr-atmega/atmega1632-i2c
//

#include "i2c.h"

void I2C_slave_init(char slave_address) {
    TWAR = slave_address;
    TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
}

int8_t I2C_slave_listen() {
    while (1) {
        uint8_t status;
        while (!(TWCR & (1 << TWINT)));
        status = TWSR & 0xF8;
        if (status == 0x60 || status == 0x68)
            return SLAVE_READ;
        if (status == 0xA8 || status == 0xB0)
            return SLAVE_WRITE;
        if (status == 0x70 || status == 0x78)
            return SLAVE_COMMON;
        else
            continue;
    }
}

int8_t I2C_slave_transmit(uint8_t data) {
    uint8_t status;
    TWDR = data;
    TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)));
    status = TWSR & 0xF8;
    if (status == 0xA0)
    {
        TWCR |= (1 << TWINT);
        return MASTER_STOP;
    }
    if (status == 0xB8)
        return MASTER_ACK_RECEIVED;
    if (status == 0xC0)
    {
        TWCR |= (1 << TWINT);
        return MASTER_NACK_RECEIVED;
    }
    if (status == 0xC8)
        return MASTER_ACK_LAST_BYTE_RECEIVED;
    else
        return MASTER_ERROR;
}

int8_t I2C_slave_transmit_nack(char data) {
    uint8_t status;
    TWDR = data;
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
    status = TWSR & 0xF8;
    if (status == 0xA0)
    {
        TWCR |= (1 << TWINT);
        return MASTER_STOP;
    }
    if (status == 0xB8)
        return MASTER_ACK_RECEIVED;
    if (status == 0xC0)
    {
        TWCR |= (1 << TWINT);
        return MASTER_NACK_RECEIVED;
    }
    if (status == 0xC8)
        return MASTER_ACK_LAST_BYTE_RECEIVED;
    else
        return MASTER_ERROR;
}

char I2C_slave_receive() {
    uint8_t status;
    TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
    status = TWSR & 0xF8;
    if (status == 0x80 || status == 0x90)
        return TWDR;
    if (status == 0x88 || status == 0x98)
        return TWDR;
    if (status == 0xA0)
    {
        TWCR |= (1 << TWINT);
        return MASTER_STOP;
    } else
        return MASTER_ERROR;
}

void I2C_read_to_buffer(char *buffer, uint8_t count) {
    int i = 0;
    char k;
    while (1) {
        k = I2C_slave_receive();
        if (k == MASTER_STOP || k == MASTER_ERROR) {
            break;
        }
        if (i < count)
            buffer[i] = k;
        i++;
    }
}