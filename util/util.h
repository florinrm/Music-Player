#include <avr/io.h>
#include <avr/interrupt.h>

void LCD_generate_bars();

void PWM_draw_bar(uint32_t value, uint32_t max, uint8_t lcd_address, uint8_t lcd_length);

void PWM_draw_bar_timer_1();

void printHertz(double value, uint8_t lcd_address);

void timer1_print_freq(uint8_t lcd_address);

void print_freq();

void UI_init();

void UI_draw();

