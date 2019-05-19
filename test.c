// test.c
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

int main() {
    DDRD |= (1 << PD7);
    LCD_print("MUIE PSD");
    while(1) {
        PORTD ^= (1 << PD7);
        _delay_ms(500);
    }
    return 0;
}