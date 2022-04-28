#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR (F_CPU/16/BAUD-1)

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "usart.h"
#include "i2c.h"
#include <stdlib.h>
#include <stdio.h>
#include "millis.h"

enum State {Idle, HandleAlarm, Fail, HandleKeypad, HandleMotion};
enum State g_STATE = Idle;

volatile unsigned char buffer[6];
FILE uart_output = FDEV_SETUP_STREAM(USART_Transmit, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, USART_Receive, _FDEV_SETUP_READ);


int main(void) {
    DDRE |= (1 << PE4); //output

    lcd_init(LCD_DISP_ON);
    USART_init(MYUBRR);
    I2C_Init();
    Timer2_Init();
    stdout = &uart_output;
    stdin = &uart_input;
    char key;

    char write_buffer[5] ="km";
    char password_buffer[10];
    uint8_t motionSensed = 0;

    lcd_puts("Hello world!");
    uint8_t twi_status;


    while (1)
    {
        switch (g_STATE) {
            case Idle:
                twi_status = I2C_Write_From_Buffer(write_buffer + 1, 1);
                if (twi_status == TWI_ERROR){
                    printf("error");
                    I2C_Stop();

                } else{
                    g_STATE = HandleMotion;
                }
                break;
            case Fail:
                break;
            case HandleAlarm:
                break;
            case HandleMotion:
                I2C_Start_Read(SLAVE_READ_ADDR);
                _delay_ms(5);
                motionSensed = I2C_Read_Ack();
                I2C_Read_Nack();
                I2C_Stop();

                if (motionSensed == 1){
                    g_STATE = HandleKeypad;
                    PORTE |= (1<<PE4);

                }
                else {
                    g_STATE = Idle;
                    PORTE &= ~(1<<PE4);

                }
                break;
            case HandleKeypad:
                _delay_ms(50);
                I2C_Write_From_Buffer(write_buffer, 1);
                _delay_ms(50);
                I2C_Start_Read(SLAVE_READ_ADDR);

                uint8_t i = 0;
                while(i < 4)
                {
                    key = I2C_Read_Ack();
                    if (key == 'x')
                        continue;
                    password_buffer[i] = key;
                    password_buffer[i+1] = '\0';
                    lcd_gotoxy(0,1);
                    lcd_puts(password_buffer);
                    i++;
                }
                I2C_Read_Nack();
                I2C_Stop();
                g_STATE = Idle;
                break;
            default:
                g_STATE = Fail;
        }
        _delay_ms(5);

    }

    return 0;
}