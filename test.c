// test.c
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "sdcard/sd.h"


#define NUM_NOTE		8

#define DDR_LED_RED DDRB
#define PORT_LED_RED PORTB
#define LED_RED PB0

#define DDR_LED_BLUE DDRB
#define PORT_LED_BLUE PORTB
#define LED_BLUE PB1

#define DDR_LED_GREEN DDRB
#define PORT_LED_GREEN PORTB
#define LED_GREEN PB3

int frecventa_nota[NUM_NOTE] = {
	262, 294, 330, 349, 392, 440, 494, 523
};

void initLeds() {
	DDR_LED_RED |= (1 << LED_RED);
	DDR_LED_GREEN |= (1 << LED_GREEN);
	DDR_LED_BLUE |= (1 << LED_BLUE);
}

void speaker_reda(int nota) {
	int perioada_nota = 1000000l / frecventa_nota[nota];
	int i;

	/*
	 * Nu putem folosi variable ca argument pentru functia
	 * _delay_us(), asa ca folosim acest "workaround".
	 */
	PORTD |= (1 << PD4);
	for (i = 0; i < perioada_nota / 2; ++i)
		_delay_us(1);

	PORTD &= ~(1 << PD4);
	for (i = 0; i < perioada_nota / 2; ++i)
		_delay_us(1);
}

int main() {
    DDRD |= (1 << PD7);
    LCD_init();
    LCD_print("MUIE PSD");

	int nota = 3;
	int ok = 0;
    for (;;) {
        speaker_reda(nota);
		//if (nota == 8)
		//	nota = 0;
		if (!ok)
			LCD_print("MUIE PSD\n");
		else
			LCD_print("MUIE ALDE\n");
		ok = 1 - ok;
		_delay_us(100000);
    }
    return 0;
}