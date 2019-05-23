#include <avr/io.h>
#include <util/delay.h>

#include <stdio.h>

#include "lcd.h"


uint16_t prescaler_values[6] = {0,1,8,64,256,1024};


void LCD_generate_bars()
{
    int bars[6] = {0x00, 0x10, 0x18, 0x1C, 0x1e, 0x1f};

    LCD_writeInstr(LCD_INSTR_CGRAM);

    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 7; j++)
            LCD_writeData(bars[i]);
        LCD_writeData(0x1f);
    }
}

void PWM_draw_bar(uint32_t value, uint32_t max, uint8_t lcd_address, uint8_t lcd_length)
{
    uint32_t progress = (uint32_t)(value) * (lcd_length * 5) / max;
    int8_t full = progress / 5;

    LCD_writeInstr(LCD_INSTR_DDRAM + lcd_address);

    for(uint8_t pos = lcd_address; pos < lcd_address + lcd_length; pos++)
    {
        if(full > 0)
        {
            LCD_writeData(5);
            full--;
        }
        else if(full == 0)
        {
            LCD_writeData(progress % 5);
            pos++;
            while(pos++ < lcd_address + lcd_length)
                LCD_writeData(0);
            break;
        }
    }
}


void PWM_draw_bar_timer_1()
{
    uint32_t max = 0xFFFF;
    uint8_t wgm_bits = ((TCCR1A & (1 << WGM10)) >> WGM10)
        | (((TCCR1A & (1 << WGM11)) >> WGM11) << 1)
        | (((TCCR1B & (1 << WGM12)) >> WGM12) << 2)
        | (((TCCR1B & (1 << WGM13)) >> WGM13) << 3);

    if (wgm_bits == 0b0100)
    {
        max = OCR1A;
    } else if (wgm_bits)
    {
        max = ICR1;
    }

    PWM_draw_bar(OCR1A, max, 1, 4);
}


void printHertz(double value, uint8_t lcd_address)
{
    char msg[20];

    if(value > 1000000.0)
        sprintf(msg,"%2.2fMHz", value/1000000.0);
    else if(value > 1000.0)
        sprintf(msg,"%2.2fkHz", value/1000.0);
    else
        sprintf(msg,"%2.2f Hz", value);

    LCD_printAt(lcd_address, msg);
}


void timer1_print_freq(uint32_t max, uint8_t lcd_address)
{
    uint8_t crt_prescaler = TCCR1B & (7 << CS10);

    if(crt_prescaler == 0)
        LCD_printAt(lcd_address, "STOPPED");
    else
        printHertz(16000000/1.0/max/prescaler_values[crt_prescaler], lcd_address);
}


void print_freq()
{
    uint32_t max = 0xFFFF;
    uint8_t wgm_bits = ((TCCR1A & (1 << WGM10)) >> WGM10)
        | (((TCCR1A & (1 << WGM11)) >> WGM11) << 1)
        | (((TCCR1B & (1 << WGM12)) >> WGM12) << 2)
        | (((TCCR1B & (1 << WGM13)) >> WGM13) << 3);

    if (wgm_bits == 4)
    {
        max = OCR1A;
    } else if (wgm_bits == 14)
    {
        max = ICR1;
    }

    timer1_print_freq(max, 0x40);
}

void UI_init()
{
    LCD_init();
    LCD_generate_bars();
    /* activare backlight */
    DDRC |= (1 << PC7);
    PORTC |= (1 << PC7);
}

void UI_draw()
{
    LCD_writeInstr(LCD_INSTR_clearDisplay);
    LCD_printAt(0, "R");
    PWM_draw_bar_timer_1();
    LCD_printAt(5, "G");
    PWM_draw_bar(OCR2A, 0xFF, 6, 4);
    LCD_printAt(10, "B");
    PWM_draw_bar(OCR0A, 0xFF, 11, 4);
    print_freq();
}

