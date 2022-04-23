#define F_CPU 16000000UL
#define FOSC 16000000UL // Clock Speed
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <util/setbaud.h>
#include <string.h>
#include <stdbool.h>
#include "keypad.h"
#include "usart.h"
#include "millis.h"

enum state {Idle, ReadPassword, SendPassword, SendMotionDetected, Fault };
enum state g_STATE = Idle;
FILE uart_output = FDEV_SETUP_STREAM(USART_Transmit, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, USART_Receive, _FDEV_SETUP_READ);

volatile unsigned char g_joystick_data[]="12345";
void sendString() {

    for (uint8_t twi_index = 0; twi_index < 6; twi_index++) {
        uint8_t twi_status = 0;
        // wait for TWINT to set, meaning that we are waiting for a transmission
        while (!(TWCR & (1 << TWINT))) { ;
        }
        // get TWI status
        twi_status = (TWSR & 0xF8);

        // load data into TWDR
        TWDR = g_joystick_data[twi_index];

        // "clear" TWINT and generate acknowledgment ACK (TWEA)
        TWCR |= (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
        //TWCR |= (1 << 7) | (1 << 6) | (1 << 2);

        // wait for the the TWINT to set
        while (!(TWCR & (1 << TWINT))) { ;
        }

        // get TWI status
        twi_status = (TWSR & 0xF8);

        if (twi_status == 0xB8) {
            // data in TWDR was transmitted, ACK was received
            ;
        } else if (twi_status == 0xC8) {
            // last byte transmitted, ACK received
            ;
        } else if (twi_status == 0xC0) {
            // data was transmitted, not ACK was received
            // Clear interrupt flag
            TWCR |= (1 << TWINT);
        }
    }

}
int
main(void) {
    DDRB &= ~(1 << PB1); //input
    DDRB |= (1 << PB2); //output

    Timer0_Init();
    KEYPAD_Init();
    USART_init(MYUBRR);
    stdout = &uart_output;
    stdin = &uart_input;


    TWAR = 0b10101010; // same as 170 DEC

    TWCR |= (1 << TWEA) | (1 << TWEN);
    TWCR &= ~(1 << TWSTA) & ~(1 << TWSTO);

    int motionState = 0;
    char keypadKey;

    while (1)
    {
        switch (g_STATE) {
            case Idle:
                motionState = (PINB & (1 << PB1));
                if (motionState) {
                    g_STATE = SendMotionDetected;
                } else {
                }
                break;
            case ReadPassword:
                keypadKey = KEYPAD_GetKey(5000);
                break;
            case SendMotionDetected:
                printf("Motion detected");
                g_STATE = Idle;
                //sendString();
                break;
            case Fault:
                break;
            default:
                g_STATE = Fault;
                break;
        }
        _delay_ms(100);


    }

    return 0;
}


