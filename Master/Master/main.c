#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include <stdlib.h>

int
main(void)
{
    lcd_init(LCD_DISP_ON);
    
    int16_t test_number = 1023;
    char test_char_array[16]; 
    itoa(test_number, test_char_array, 10);

    lcd_clrscr();

    lcd_puts("Hello World!");
    
    lcd_gotoxy(0,1);
    lcd_puts(test_char_array);


    while(1)
    {

    }
    
    return 0;
}