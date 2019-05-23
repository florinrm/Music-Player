// test.c
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "sdcard/sd.h"
#include "sdcard/spi.h"
#include "sdcard/integer.h"
#include "sdcard/pff.h"
#include "util.h"

#define NUM_NOTE		8

#define RED 0
#define GREEN 1
#define BLUE 2

#define DDR_LED_RED_B DDRB
#define PORT_LED_RED_B PORTB
#define LED_RED_B PB0

#define DDR_LED_BLUE_B DDRB
#define PORT_LED_BLUE_B PORTB
#define LED_BLUE_B PB1

#define DDR_LED_GREEN_B DDRB
#define PORT_LED_GREEN_B PORTB
#define LED_GREEN_B PB3

#define SWITCH_TO_GREEN_B 100000
#define SWITCH_TO_BLUE_B 200000
#define SWITCH_TO_RED_B 300000

#define DDR_LED_RED_D DDRD
#define PORT_LED_RED_D PORTD
#define LED_RED_D PD5

#define DDR_LED_BLUE_D DDRD
#define PORT_LED_BLUE_D PORTD
#define LED_BLUE_D PD6

#define DDR_LED_GREEN_D DDRD
#define PORT_LED_GREEN_D PORTD
#define LED_GREEN_D PD1

#define DDR_LED_RED_A DDRA
#define PORT_LED_RED_A PORTA
#define LED_RED_A PA2

#define DDR_LED_BLUE_A DDRA
#define PORT_LED_BLUE_A PORTA
#define LED_BLUE_A PA0

#define DDR_LED_GREEN_A DDRA
#define PORT_LED_GREEN_A PORTA
#define LED_GREEN_A PA1

#define NOTES 10

const char *message1 = "Imperial March";

int frecventa_nota[NUM_NOTE] = {
	262, 294, 330, 349, 392, 440, 494, 523
};

long notes[NOTES] = {
	10234, 41201, 12314, 4512, 3120, 30121, 10234, 23010, 50120, 3201
};

void initLeds() {
	DDR_LED_RED_B |= (1 << LED_RED_B);
	DDR_LED_GREEN_B |= (1 << LED_GREEN_B);
	DDR_LED_BLUE_B |= (1 << LED_BLUE_B);

	DDR_LED_RED_D |= (1 << LED_RED_D);
	DDR_LED_GREEN_D |= (1 << LED_GREEN_D);
	DDR_LED_BLUE_D |= (1 << LED_BLUE_D);

	DDR_LED_RED_A |= (1 << LED_RED_A);
	DDR_LED_GREEN_A |= (1 << LED_GREEN_A);
	DDR_LED_BLUE_A |= (1 << LED_BLUE_A);
}



void speaker_reda(int nota) {
	int perioada_nota = 1000000l / notes[nota];
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

	int nota = 0, firstB = 1, firstD = 1, firstA = 1;
	int ok = 0, current_colorB = RED, current_colorD = GREEN, current_colorA = BLUE;
	long switchColorB = 0, switchColorD = 0, switchColorA = 0;
	int isColorSwitchedB = 0, isColorSwitchedD = 0, isColorSwitchedA = 0;
	initLeds();
    for (;;) {
        LCD_printAt(0x00, message1);
		
		speaker_reda(nota++);
		if (nota == NOTES)
			nota = 0;

		// port B
		++switchColorB;
		if (switchColorB == 500 && current_colorB == RED) {
			isColorSwitchedB = 1;
			current_colorB = GREEN;
		} else if (switchColorB == 1000 && current_colorB == GREEN) {
			isColorSwitchedB = 1;
			current_colorB = BLUE;
		} else if (switchColorB == 1500 && current_colorB == BLUE) {
			switchColorB = 0;
			isColorSwitchedB = 1;
			current_colorB = RED;
		}
		
		if ((current_colorB == RED && isColorSwitchedB) || (current_colorB == RED && firstB)) {
			if (!firstB) {
				PORT_LED_BLUE_B &= ~(1 << LED_BLUE_B);
			}
			firstB = 0;
			PORT_LED_RED_B |= (1 << LED_RED_B);
			//current_colorB = GREEN;
			isColorSwitchedB = 0;
		} else if (current_colorB == GREEN && isColorSwitchedB) {
			PORT_LED_RED_B &= ~(1 << LED_RED_B);
			PORT_LED_GREEN_B |= (1 << LED_GREEN_B);
			//current_colorB = BLUE;
			isColorSwitchedB = 0;
		} else if (current_colorB == BLUE && isColorSwitchedB) {
			PORT_LED_GREEN_B &= ~(1 << LED_GREEN_B);
			PORT_LED_BLUE_B |= (1 << LED_BLUE_B);
			//current_colorB = RED;
			isColorSwitchedB = 0;
		}

		// port D
		++switchColorD;
		if (switchColorD == 500 && current_colorD == BLUE) {
			isColorSwitchedD = 1;
			current_colorD = RED;
		} else if (switchColorD == 1000 && current_colorD == RED) {
			isColorSwitchedD = 1;
			current_colorD = GREEN;
		} else if (switchColorD == 1500 && current_colorD == GREEN) {
			switchColorD = 0;
			isColorSwitchedD = 1;
			current_colorD = BLUE;
		}

		if (current_colorD == RED && isColorSwitchedD) {
			PORT_LED_BLUE_D &= ~(1 << LED_BLUE_D);
			PORT_LED_RED_D |= (1 << LED_RED_D);
			//current_colorB = GREEN;
			isColorSwitchedD = 0;
		} else if ((current_colorD == GREEN && isColorSwitchedD) || (current_colorD == GREEN && firstD)) {
			if (!firstD) {
				PORT_LED_RED_D &= ~(1 << LED_RED_D);			
			}
			firstD = 0;
			PORT_LED_GREEN_D |= (1 << LED_GREEN_D);
			//current_colorB = BLUE;
			isColorSwitchedD = 0;
		} else if (current_colorD == BLUE && isColorSwitchedD) {
			PORT_LED_GREEN_D &= ~(1 << LED_GREEN_D);
			PORT_LED_BLUE_D |= (1 << LED_BLUE_D);
			//current_colorB = RED;
			isColorSwitchedD = 0;
		}

		// port A
		++switchColorA;
		if (switchColorA == 500 && current_colorA == GREEN) {
			isColorSwitchedA = 1;
			current_colorA = BLUE;
		} else if (switchColorA == 1000 && current_colorA == BLUE) {
			isColorSwitchedA = 1;
			current_colorA = RED;
		} else if (switchColorA == 1500 && current_colorA == RED) {
			switchColorA = 0;
			isColorSwitchedA = 1;
			current_colorA = GREEN;
		}

		if (current_colorA == RED && isColorSwitchedA) {
			PORT_LED_BLUE_A &= ~(1 << LED_BLUE_A);
			PORT_LED_RED_A |= (1 << LED_RED_A);
			//current_colorB = GREEN;
			isColorSwitchedA = 0;
		} else if (current_colorA == GREEN && isColorSwitchedA) {
			PORT_LED_RED_A &= ~(1 << LED_RED_A);			
			PORT_LED_GREEN_A |= (1 << LED_GREEN_A);
			//current_colorB = BLUE;
			isColorSwitchedA = 0;
		} else if ((current_colorA == BLUE && isColorSwitchedA)  || (current_colorA == BLUE && firstA)) {
			if (!firstA) {
				PORT_LED_GREEN_A &= ~(1 << LED_GREEN_A);
			}
			firstA = 0;
			PORT_LED_BLUE_A |= (1 << LED_BLUE_A);
			//current_colorB = RED;
			isColorSwitchedA = 0;
			firstA = 0;
		}

		ok = 1 - ok;
    }
    return 0;
}