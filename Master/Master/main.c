#define F_CPU 16000000UL
#define FOSC 16000000UL // Clock Speed
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "usart.h"
#include <stdlib.h>
#include <stdio.h>

enum State {Idle, HandleAlarm, Fail};
enum State g_STATE = Idle;

volatile unsigned char buffer[6];
FILE uart_output = FDEV_SETUP_STREAM(USART_Transmit, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, USART_Receive, _FDEV_SETUP_READ);

void readToBuffer(char *buffer, uint8_t buffer_length) {
    uint8_t twi_status;
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
    {
        ;
    }


    TWDR = 0b10101011; // load slave address and write command
    TWCR = (1 << TWINT) | (1 << TWEN);

    while (!(TWCR & ( 1<< TWINT)))
    {
        ;
    }

    twi_status = (TWSR & 0xF8);
    for(int8_t twi_data_index = 0; twi_data_index < buffer_length; twi_data_index++)
    {

        if((sizeof(buffer) - 1) == twi_data_index)
        {
            TWCR = (1 << TWINT) | (1 << TWEN);
            while (!(TWCR & (1 << TWINT)))
            {
                ;
            }
            buffer[twi_data_index] = TWDR;
        }
        else
        {
            // read data from slave and sent ACK to the slave
            // wait for the TWINT to set

            TWCR |=  (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
            //TWCR |= (1 << 7) | (1 << 6) | (1 << 2);

            while (!(TWCR & ( 1<< TWINT)))
            {
                ;
            }

            buffer[twi_data_index] = TWDR; // receive data from data register

        }
        twi_status = (TWSR & 0xF8);
    }

    TWCR = (1 << TWINT) | (1 << TWSTO) |(1 << TWEN);
}

int main(void) {
    TWSR = 0x00; // TWI status register prescaler value set to 1
    TWBR = 0x03; //0x03; // TWI bit rate register.
    TWCR |= (1 << TWEN); // enable TWI

    lcd_init(LCD_DISP_ON);
    USART_init(MYUBRR);
    stdout = &uart_output;
    stdin = &uart_input;

    // Initialize TWI

    // set SCL frequency to 400 kHz, using equation in p. 242 of ATmega2560 datasheet
    TWSR = 0x00; // TWI status register prescaler value set to 1
    TWBR = 0x03; //0x03; // TWI bit rate register.
    TWCR |= (1 << TWEN); // enable TWI
    // TWCR |= (1 << 2);

    while (1)
    {
        switch (g_STATE) {
            case Idle:
                break;
            case Fail:
                break;
            case HandleAlarm:
                break;
            default:
                g_STATE = Fail;
        }
        _delay_ms(100);

    }

    return 0;
}