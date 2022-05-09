#define F_CPU 16000000UL
#define FOSC 16000000UL // Clock Speed
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)
#define SLAVE_ADDRESS 0x20

#define READ_KEYPAD_CHAR 'k'
#define READ_MOTION 'm'
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "keypad.h"
#include "usart.h"
#include "millis.h"
#include "i2c.h"

enum state {
    Idle, ReadKeypad, ReadMotionSensor, Fault
};
enum state g_STATE = Idle;
volatile uint8_t motionSensed = 0;
FILE uart_output = FDEV_SETUP_STREAM(USART_Transmit, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, USART_Receive, _FDEV_SETUP_READ);


/**
 @brief   Parses state from character
 @param   receivedChar character to parse
 @return  state enum. Default is idle.
*/
enum state state_parse(char receivedChar) {
    enum state state;
    switch (receivedChar) {
        case READ_KEYPAD_CHAR:
            state = ReadKeypad;
            break;
        case READ_MOTION:
            state = ReadMotionSensor;
            break;
        default:
            state = Idle;
            break;
    }
    return state;
}

/**
 @brief   check whether motion is detected.
 @param   none
 @return  none
*/
void motion_check() {
    uint8_t motionState = (PINB & (1 << PB1));
    if (motionState && motionSensed != 1) {
        motionSensed = 1;
    }
}

/**
 @brief   Handles motion detection and master communication.
 @param   none
 @return  none
*/
void handleMotion() {
    uint8_t twi_status;
    switch (I2C_slave_listen()) {
        case SLAVE_WRITE:
            motion_check();
            do {
                twi_status = I2C_slave_transmit(motionSensed);
            } while (twi_status == MASTER_ACK_RECEIVED);
            motionSensed = 0;
            g_STATE = Idle;
            break;
        case SLAVE_READ:
        default:
            g_STATE = Fault;
            break;
    }

}

int main(void) {
    DDRB &= ~(1 << PB1); //input
    DDRB |= (1 << PB2); //output

    sei();

    MILLIS_init();
    KEYPAD_init();
    USART_init(MYUBRR);
    I2C_slave_init(SLAVE_ADDRESS);

    char read_buff[20];

    stdout = &uart_output;
    stdin = &uart_input;

    uint8_t keypadKey;

    int8_t Ack_status;

    while (1) {
        switch (g_STATE) {
            case Idle:
                switch (I2C_slave_listen()) {
                    case SLAVE_READ:
                        I2C_read_to_buffer(read_buff, 1);
                        g_STATE = state_parse(read_buff[0]);
                        break;
                    case SLAVE_WRITE:
                        I2C_slave_transmit_nack('y');
                        break;
                }
                break;
            case ReadKeypad:
                switch (I2C_slave_listen()) {
                    case SLAVE_WRITE:
                        do {
                            keypadKey = KEYPAD_GetKey(50);
                            Ack_status = I2C_slave_transmit(keypadKey);
                        } while (Ack_status == MASTER_ACK_RECEIVED);
                        g_STATE = Idle;
                        break;
                    case SLAVE_READ:
                        g_STATE = Fault;
                }
                break;
            case ReadMotionSensor:
                handleMotion();
                break;
            case Fault:
                g_STATE = Idle;
                printf("handling fault\n");
                break;
            default:
                g_STATE = Fault;
                break;
        }
        _delay_ms(10);

    }
}


