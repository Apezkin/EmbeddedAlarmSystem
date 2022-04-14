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


FILE uart_output = FDEV_SETUP_STREAM(USART_Transmit, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, USART_Receive, _FDEV_SETUP_READ);

int main(void)
{
    lcd_init(LCD_DISP_ON);
    USART_init(MYUBRR);
    stdout = &uart_output;
    stdin = &uart_input;
    
    int16_t test_number = 1023;
    char test_char_array[16]; 
    itoa(test_number, test_char_array, 10);

    lcd_clrscr();
    printf("moi");

    lcd_puts("Hello World!");
    
    lcd_gotoxy(0,1);
    lcd_puts(test_char_array);


    while(1)
    {

    }
    
    return 0;
}