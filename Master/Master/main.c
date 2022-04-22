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
#include <string.h>
#include <stdbool.h>


volatile unsigned char buffer[6];
FILE uart_output = FDEV_SETUP_STREAM(USART_Transmit, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, USART_Receive, _FDEV_SETUP_READ);

void readToBuffer(char *buffer) {
    int8_t twi_status;
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    while (!(TWCR & (1 << TWINT))) { ;
    }

    twi_status = (TWSR & 0xF8);


    TWDR = 0b10101011;

    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT))) {;}
    twi_status = (TWSR & 0xF8);

    for (int8_t twi_data_index = 0; twi_data_index < 6; twi_data_index++) {

        if ((strlen(buffer) - 1) == twi_data_index) {
            TWCR = (1 << TWINT) | (1 << TWEN);
            while (!(TWCR & (1 << TWINT))) { ;
            }
            buffer[twi_data_index] = TWDR;
        } else {
            TWCR |= (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
            while (!(TWCR & (1 << TWINT))) { ;}
            buffer[twi_data_index] = TWDR;
        }
        twi_status = (TWSR & 0xF8);
    }
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

int main(void) {
    TWSR = 0x00; // TWI status register prescaler value set to 1
    TWBR = 0x03; //0x03; // TWI bit rate register.
    TWCR |= (1 << TWEN); // enable TWI

    lcd_init(LCD_DISP_ON);
    USART_init(MYUBRR);
    stdout = &uart_output;
    stdin = &uart_input;
    char test_char_array[16]; // 16-bit array, assumes that the int given is 16-bits
    uint8_t twi_status = 0;

    // Joystick variables
    volatile uint16_t x_axis = 0;
    volatile uint16_t y_axis = 0;
    volatile bool b_switch = 0;
    volatile unsigned char joystick_data[6];

    // Initialize TWI

    // set SCL frequency to 400 kHz, using equation in p. 242 of ATmega2560 datasheet
    TWSR = 0x00; // TWI status register prescaler value set to 1
    TWBR = 0x03; //0x03; // TWI bit rate register.
    TWCR |= (1 << TWEN); // enable TWI
    // TWCR |= (1 << 2);

    while (1)
    {

        // Start transmission by sending START condition
        TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
        // TWCR = (1 << 7) | (1 << 5) | (1 << 2);

        // wait for the TWINT to set
        while (!(TWCR & (1 << TWINT)))
        {
            ;
        }


        TWDR = 0b10101011; // load slave address and write command
        // Slave address = 85 + read bit '1' as a LSB ---> 171

        // clear TWINT to start transmitting the slave address + write command
        TWCR = (1 << TWINT) | (1 << TWEN);
        // TWCR = (1 << 7) | (1 << 2);

        // wait for the TWINT to set
        while (!(TWCR & ( 1<< TWINT)))
        {
            ;
        }

        // read the status from TWI status register, 0xF8 is used to mask prescaler bits so that
        // only the status bits are read
        twi_status = (TWSR & 0xF8);

        itoa(twi_status, test_char_array, 16);

        // receive data from the slave
        for(int8_t twi_data_index = 0; twi_data_index < sizeof(joystick_data); twi_data_index++)
        {

            if((sizeof(joystick_data) -1) == twi_data_index)
            {
                // Read the last byte from the slave and return NOT ACK
                // "clear" TWINT to start transmitting the data
                TWCR = (1 << TWINT) | (1 << TWEN);
                // TWCR = (1 << 7) | (1 << 2);

                // wait for the TWINT to set
                while (!(TWCR & (1 << TWINT)))
                {
                    ;
                }

                joystick_data[twi_data_index] = TWDR;

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

                joystick_data[twi_data_index] = TWDR; // receive data from data register

            }

            // read the status from TWI status register, 0xF8 is used to mask prescaler bits so that
            // only the status bits are read
            twi_status = (TWSR & 0xF8);

        }
        printf(joystick_data);
        printf("\n");

        // stop transmission by sending STOP
        TWCR = (1 << TWINT) | (1 << TWSTO) |(1 << TWEN);
        //TWCR = (1 << 7) | (1 << 4) |(1 << 2);


        _delay_ms(1000);

    }

    return 0;
}