#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR (F_CPU/16/BAUD-1)
#define ALARM_TIME 15000L

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "usart.h"
#include "i2c.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "millis.h"

enum State {
    Idle, StartAlarm, StopAlarm, Fail, HandleKeypad, HandleMotion, WrongPassword, KeyPadTimeout, TooLongPassword
};
enum State g_STATE = Idle;

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
    uint64_t alarm_start;
    uint64_t alarm_now;

    char password_buffer[10];
    uint8_t motionSensed = 0;
    uint8_t seconds;
    uint8_t next_second;
    char seconds_array[5];
    char password[] = "0000";

    lcd_puts("Hello world!");
    uint8_t twi_status;


    while (1) {
        switch (g_STATE) {
            case Idle:
                twi_status = I2C_Start_Write(SLAVE_WRITE_ADDR);
                if (twi_status == TWI_ERROR || twi_status == TWI_START_FAILED) {
                    printf("error\n");
                    g_STATE = Fail;
                    break;
                }
                twi_status = I2C_Write('m');
                if (twi_status != TWI_ACK_RECEIVED) {
                    printf("error\n");
                    g_STATE = Fail;
                    break;
                }
                I2C_Stop();
                g_STATE = HandleMotion;

                break;
            case Fail:
                I2C_Stop();
                I2C_Init();
                g_STATE = Idle;
                break;
            case StartAlarm:
                PORTE |= (1 << PE4);
                g_STATE = HandleKeypad;
                break;
            case StopAlarm:
                PORTE &= ~(1 << PE4);
                lcd_clrscr();
                lcd_puts("Alarm stopped!");
                _delay_ms(1000);
                g_STATE = Idle;
                break;
            case HandleMotion:
                twi_status = I2C_Start_Read(SLAVE_READ_ADDR);
                if (twi_status != TWI_ACK_RECEIVED) {
                    g_STATE = Fail;
                    break;
                }
                motionSensed = I2C_Read_Ack();
                if (motionSensed == 'y') {
                    g_STATE = Fail;
                    break;
                }
                I2C_Read_Nack();
                I2C_Stop();

                if (motionSensed == 1)
                    g_STATE = StartAlarm;
                else
                    g_STATE = Idle;
                break;
            case HandleKeypad:
                alarm_start = millis();
                seconds = ALARM_TIME / 1000;
                lcd_clrscr();
                lcd_puts("Alarm in: ");
                lcd_gotoxy(11, 0);
                itoa(seconds, seconds_array, 9);
                lcd_puts(seconds_array);

                twi_status = I2C_Start_Write(SLAVE_WRITE_ADDR);
                if (twi_status != TWI_ACK_RECEIVED) {
                    printf("error\n");
                    g_STATE = Fail;
                    break;
                }
                twi_status = I2C_Write('k');
                if (twi_status != TWI_ACK_RECEIVED) {
                    printf("error\n");
                    g_STATE = Fail;
                    break;
                }
                I2C_Stop();
                twi_status = I2C_Start_Read(SLAVE_READ_ADDR);
                if (twi_status != TWI_ACK_RECEIVED) {
                    printf("Error starting reading keys.\n");
                    g_STATE = Idle;
                    I2C_Stop();
                    continue;
                }

                uint8_t i = 0;
                while (1) {
                    key = I2C_Read_Ack();
                    alarm_now = millis();
                    next_second = (ALARM_TIME - (alarm_now - alarm_start)) / 1000;
                    if (alarm_now > alarm_start + ALARM_TIME){
                        I2C_Read_Nack();
                        I2C_Stop();
                        g_STATE = KeyPadTimeout;
                        break;
                    }
                    if (next_second != seconds) {
                        itoa(next_second, seconds_array, 9);
                        seconds = next_second;
                        lcd_gotoxy(11, 0);
                        lcd_puts(seconds_array);
                        lcd_puts("   ");
                    }
                    if (key == 'x')
                        continue;
                    alarm_start = millis();
                    if (key == 'C') {
                        i--;
                        password_buffer[i] = '\0';
                    } else if (key == '#') {
                        I2C_Read_Nack();
                        I2C_Stop();
                        if (strcmp(password_buffer, password) == 0) {
                            g_STATE = StopAlarm;

                        } else {
                            g_STATE = WrongPassword;
                        }
                        break;
                    } else {
                        if (i > 9) {
                            g_STATE = TooLongPassword;
                            I2C_Read_Nack();
                            I2C_Stop();
                            break;
                        }
                        password_buffer[i] = key;
                        password_buffer[i + 1] = '\0';
                        i++;
                    }
                    lcd_gotoxy(0, 1);
                    lcd_puts(password_buffer);
                    lcd_puts("   ");
                }
                break;
            case WrongPassword:
                lcd_clrscr();
                lcd_puts("Wrong password!");
                _delay_ms(1000);
                g_STATE = HandleKeypad;
                break;
            case TooLongPassword:
                lcd_clrscr();
                lcd_puts("Too long");
                lcd_gotoxy(0,1);
                lcd_puts("password!");
                _delay_ms(1000);
                g_STATE = HandleKeypad;
                break;
            case KeyPadTimeout:
                lcd_clrscr();
                lcd_puts("Too slow!");
                _delay_ms(1000);
                g_STATE = HandleKeypad;
                break;

            default:
                g_STATE = Fail;
        }
        _delay_ms(10);

    }

    return 0;
}