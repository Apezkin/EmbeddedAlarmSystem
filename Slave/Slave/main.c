#define F_CPU 16000000UL
#define FOSC 16000000UL // Clock Speed
#define BAUD 9600
#define MYUBRR (FOSC/16/BAUD-1)
#define SLAVE_ADDRESS 0x20

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <util/setbaud.h>
#include <string.h>
#include <stdbool.h>
#include "keypad.h"
#include "usart.h"
#include "millis.h"
#include "i2c.h"

enum state {
    Idle, ReadKeypad, SendPassword, ReadMotionSensor, Fault
};
enum state g_STATE = Idle;
volatile uint8_t motionSensed = 0;
FILE uart_output = FDEV_SETUP_STREAM(USART_Transmit, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, USART_Receive, _FDEV_SETUP_READ);


enum state Get_State_From_char(char receivedChar)
{
    enum state state;
    switch (receivedChar) {
        case 'k': state = ReadKeypad;
            break;
        case 'm': state = ReadMotionSensor;
            break;
        default:
            state = Idle;
            break;
    }
    return state;
}

void check_Motion()
{
    uint8_t motionState = (PINB & (1 << PB1));
    if (motionState && motionSensed != 1)
    {
        motionSensed = 1;
        printf("motion detected");
    }
}

int
main(void) {
    DDRB &= ~(1 << PB1); //input
    DDRB |= (1 << PB2); //output

    Timer0_Init();
    KEYPAD_Init();
    USART_init(MYUBRR);
    I2C_Slave_Init(SLAVE_ADDRESS);

    char read_buff[20];

    stdout = &uart_output;
    stdin = &uart_input;

    int motionState = 0;
    char keypadKey;

    uint8_t count;
    int8_t Ack_status;

    while (1) {
        switch (g_STATE) {
            case Idle:
                switch (I2C_Slave_Listen()) {
                    case SLAVE_READ:
                        I2C_Read_To_Buffer(read_buff, 1);
                        printf("reading: %c\n", read_buff[0]);
                        g_STATE = Get_State_From_char(read_buff[0]);
                        break;
                    case SLAVE_WRITE:
                        break;
                }
                break;
            case ReadKeypad:
                printf("sending keys\n");
                do {
                    keypadKey = KEYPAD_GetKey(50);
                    Ack_status = I2C_Slave_Transmit(keypadKey);
                } while (Ack_status == 0);
                g_STATE = Idle;
                break;
            case ReadMotionSensor:
                printf("sending stuff\n");
                check_Motion();
                printf("sending stuff\n");
                do {
                    Ack_status = I2C_Slave_Transmit(motionSensed);
                    printf("sending stuff\n");
                } while (Ack_status == 0);
                motionSensed = 0;
                g_STATE = Idle;
                break;
            case Fault:
                g_STATE = Idle;
                break;
            default:
                g_STATE = Fault;
                break;
        }

    }
}


