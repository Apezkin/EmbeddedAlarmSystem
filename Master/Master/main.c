#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR (F_CPU/16/BAUD-1)
#define ALARM_TIME 5000L

#define KEYPAD_READ_COMMAND 'k'
#define KEYPAD_KEY_TIMEOUT 'x'
#define KEYPAD_KEY_BACKSPACE 'C'
#define KEYPAD_KEY_ENTER '#'

#define PWD_KEY_STOP 1
#define PWD_KEY_INDEX_ERR -2
#define PWD_KEY_TIMEOUT -1
#define PWD_KEY_CONTINUE 0
#define PWD_MAX_LENGTH 9

#define MOTION_READ_COMMAND 'm'

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "usart.h"
#include "i2c.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include "millis.h"
#include "buzzer.h"

enum State {
    Idle, StartAlarm, StopAlarm, Fail, HandleKeypad, readMotion, WrongPassword, KeyPadTimeout, TooLongPassword
};
enum State g_STATE = Idle;

FILE uart_output = FDEV_SETUP_STREAM(USART_Transmit, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, USART_Receive, _FDEV_SETUP_READ);


int8_t password_handle_key(char *buffer, uint8_t *index);

void password_handle();


int main(void) {
    DDRE |= (1 << PE3);
    sei();

    lcd_init(LCD_DISP_ON);
    USART_init(MYUBRR);
    I2C_init();
    timer2_init();
    buzzer_init();
    stdout = &uart_output;
    stdin = &uart_input;
    lcd_clrscr();
    lcd_puts("Detecting...");

    uint8_t motion_sensed;
    uint8_t twi_status;


    while (1) {
        switch (g_STATE) {
            case Idle:
                if (I2C_command_write(MOTION_READ_COMMAND) != TWI_OK) {
                    printf("error\n");
                    g_STATE = Fail;
                } else {
                    g_STATE = readMotion;
                }
                break;
            case Fail:
                I2C_stop();
                I2C_init();
                g_STATE = Idle;
                break;
            case StartAlarm:
                Start_Buzzing();
                lcd_clrscr();
                lcd_puts("Alarm");
                g_STATE = HandleKeypad;
                break;
            case StopAlarm:
                Stop_Buzzing();
                lcd_clrscr();
                lcd_puts("Alarm stopped!");
                _delay_ms(1000);
                lcd_clrscr();
                lcd_puts("Detecting...");
                g_STATE = Idle;
                break;
            case readMotion:
                twi_status = I2C_read_start(SLAVE_READ_ADDR);
                if (twi_status != TWI_ACK_RECEIVED) {
                    g_STATE = Fail;
                    break;
                }
                motion_sensed = I2C_read_ack();
                if (motion_sensed == 'y') {
                    g_STATE = Fail;
                    break;
                }
                I2C_read_nack();
                I2C_stop();

                if (motion_sensed == 1) {
                    g_STATE = StartAlarm;
                } else {
                    g_STATE = Idle;
                }
                break;
            case HandleKeypad:
                password_handle();
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
                lcd_gotoxy(0, 1);
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
}

void lcd_second_handler(uint8_t *seconds, uint8_t x, uint8_t y, uint32_t alarm_now, uint32_t alarm_start) {
    uint8_t next_second = (ALARM_TIME - (alarm_now - alarm_start)) / 1000;
    char seconds_array[5];
    if (next_second != *seconds) {
        itoa(next_second, seconds_array, 9);
        seconds = next_second;
        lcd_gotoxy(x, y);
        lcd_puts(seconds_array);
        lcd_puts("   ");
    }
}

void password_handle() {
    uint64_t alarm_now;
    char password_buffer[10] = {'\0'};
    uint8_t seconds;
    char seconds_array[5];
    char password[] = "0000";
    uint8_t twi_status;
    int8_t key_read_status;
    uint32_t alarm_start = millis();
    seconds = ALARM_TIME / 1000;
    lcd_clrscr();
    lcd_puts("Alarm in: ");
    lcd_gotoxy(11, 0);
    itoa(seconds, seconds_array, 9);
    lcd_puts(seconds_array);

    if (I2C_command_write(KEYPAD_READ_COMMAND) != TWI_OK) {
        printf("Error writing keypad read command.\n");
        g_STATE = Fail;
    } else { g_STATE = readMotion; }

    twi_status = I2C_read_start(SLAVE_READ_ADDR);
    if (twi_status != TWI_ACK_RECEIVED) {
        printf("Error starting reading keys.\n");
        g_STATE = Fail;
        return;
    }

    uint8_t i = 0;
    while (1) {
        alarm_now = millis();
        lcd_second_handler(&seconds, 11, 0, alarm_now, alarm_start);
        if (alarm_now > alarm_start + ALARM_TIME) {
            I2C_read_nack();
            I2C_stop();
            g_STATE = KeyPadTimeout;
            return;
        }
        key_read_status = password_handle_key(password_buffer, &i);
        if (key_read_status == PWD_KEY_INDEX_ERR || key_read_status == PWD_KEY_STOP) {
            break;
        } else if (key_read_status != PWD_KEY_TIMEOUT) alarm_start = millis();

        lcd_gotoxy(0, 1);
        lcd_puts(password_buffer);
        lcd_puts("   ");
    }
    I2C_read_nack();
    I2C_stop();
    if (key_read_status == PWD_KEY_STOP) {
        if (strcmp(password_buffer, password) == 0) {
            g_STATE = StopAlarm;
        } else {
            g_STATE = WrongPassword;
        }
    } else {
        g_STATE = TooLongPassword;
    }
}

int8_t password_handle_key(char *buffer, uint8_t *index) {
    if (*index > PWD_MAX_LENGTH) { return PWD_KEY_INDEX_ERR; }
    char key;
    int8_t status;
    switch (key = I2C_read_ack()) {
        case KEYPAD_KEY_TIMEOUT:
            status = PWD_KEY_TIMEOUT;
            break;
        case KEYPAD_KEY_BACKSPACE:
            *index = *index - 1;
            buffer[*index] = '\0';
            status = PWD_KEY_CONTINUE;
            break;
        case KEYPAD_KEY_ENTER:
            status = PWD_KEY_STOP;
            break;
        default:
            buffer[*index] = key;
            buffer[*index + 1] = '\0';
            *index = *index + 1;
            status = PWD_KEY_CONTINUE;
            break;
    }
    return status;
}