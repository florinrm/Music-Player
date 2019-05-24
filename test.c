// test.c
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include "lcd.h"
#include "sd.h"
#include "spi.h"
#include "integer.h"
#include "pff.h"
#include "util.h"
//#include "sdcard/lab4.h"

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

#define SWITCH_1 500
#define SWITCH_2 1000
#define SWITCH_3 1500

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
#define LED_RED_A PA3

#define DDR_LED_BLUE_A DDRA
#define PORT_LED_BLUE_A PORTA
#define LED_BLUE_A PA0

#define DDR_LED_GREEN_A DDRA
#define PORT_LED_GREEN_A PORTA
#define LED_GREEN_A PA1

#define NOTES 40
#define CHANGE_NOTE 400

#define c    261
#define d    294
#define e    329
#define f    349
#define g    391
#define gS   415
#define a    440
#define aS   455
#define b    466
#define cH   523
#define cSH  554
#define dH   587
#define dSH  622
#define eH   659
#define fH   698
#define fSH  740
#define gH   784
#define gSH  830
#define aH   880


const char* messages[4][2] = {
	{"Imperial March", "Star Wars"},
	{"Papuci Gucci", "abi"},
	{"A Place With No Name", "Michael Jackson"},
	{"Lucid Dreams", "Juice WRLD"}
};

int nr_nota;
int durata;

int frecventa_nota[NUM_NOTE] = {
	262, 294, 330, 349, 392, 440, 494, 523
};

long notes[4][NOTES] = {
	{
		10234, 41201, 12314, 4512, 3120, 30121, 10234, 23010, 50120, 3201,
		10234, 41201, 12314, 4512, 12314, 4512, 3120, 3120, 30121, 10234,
		5325, 12541, 52112, 9591, 5321, 5385, 125, 591, 5911, 5295, 6539,
		3120, 30121, 10234, 23010, 512, 12314, 4512, 3120, 5295
	},
	{
		26048, 1366, 2991, 16268, 33936, 19432, 6376, 37763, 32945, 36536,
		27585, 38645, 24350, 30429, 4114, 8824, 34702, 48361, 48386, 40254,
		9089, 17164, 23320, 42645, 44533, 16087, 46990, 32315, 45445, 10008,
		17632, 34770, 18797, 1413, 28739, 3878, 12043, 47230, 22612, 19714
	},
	{
		25906, 4404, 18320, 16746, 41290, 21649, 8028, 22252, 3877, 13018,
		17058, 46869, 39732, 30882, 37733, 19408, 35400, 39376, 40351, 15588,
		49317, 44248, 14653, 49886, 1118, 40699, 13855, 9608, 38968, 33246,
		43142, 18261, 3773, 49880, 47756, 34423, 17124, 28116, 11274, 23991
	},
	{
		18965, 47503, 43855, 17892, 38681, 39268, 3924, 47822, 11350, 24532,
		28210, 21541, 48011, 45824, 22359, 20423, 4950, 44151, 27529, 41214,
		4657, 46302, 47966, 30377, 49679, 26705, 13450, 15419, 31275, 45752,
		48055, 40282, 4464, 1856, 6690, 38308, 41580, 40041, 15323, 44830
	}
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
	int perioada_nota = 1234567l / nota;
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

uint16_t surprise_notes[] = {a, a, a, f, cH, a, f, cH, a, eH, eH, eH, fH, cH, gS, f, cH, a, aH, a, a, aH, gSH, gH, fSH, fH, fSH, aS, dSH, dH, cSH, cH, b, cH, f, gS, f, a, cH, a, cH, eH, aH, a, a, aH, gSH, gH, fSH, fH, fSH, aS, dSH, dH, cSH, cH, b, cH, f, gS, f, cH, a, f, cH, a};

uint16_t durations[] = {500, 500, 500, 350, 150, 500, 350, 150, 650, 500, 500, 500, 350, 150, 500, 350, 150, 650, 500, 300, 150, 400, 200, 200, 125, 125, 250, 250, 400, 200, 200, 125, 125, 250, 125, 500, 375, 125, 500, 375, 125, 650, 500, 300, 150, 400, 200, 200, 125, 125, 250, 250, 400, 200, 200, 125, 125, 250, 250, 500, 375, 125, 500, 375, 125, 650};

uint8_t num_notes = sizeof(durations) / sizeof(durations[0]);


ISR(TIMER1_OVF_vect)
{
#if 0
    PORTD &= ~(1 << PD5);
#endif
}

ISR(TIMER1_COMPA_vect)
{
#if 0
    PORTD |= (1 << PD5);
#endif
}

volatile uint8_t prescaler_mode = 0;
ISR(PCINT1_vect)
{
    if ((PINB & (1 << PB2)) == 0) {
        prescaler_mode = (prescaler_mode + 1) % 5;
        TCCR1B &= 0b11111000;
        TCCR1B |= prescaler_mode + 1;
        _delay_ms(50);
    }
}

ISR(PCINT3_vect)
{
    if ((PIND & (1 << PD6)) == 0) {
        OCR1A = (OCR1A + (ICR1 / 8)) % ICR1;
        _delay_ms(50);
    }
}

void IO_init()
{
    /* LED-ul RGB si speaker-ul sunt puse ca iesire */
    DDRD |= (1 << PD4);
    /* LED-ul RGB este stins (punem pinii in starea HIGH) */

    /* butoanele sunt puse ca intrare, cu rezistenta de pull-up */

    /* intreruperile pentru butoane sunt activate */
    PCICR |= (1 << PCIE1) | (1 << PCIE3);
    PCMSK1 |= (1 << PCINT10);
    PCMSK3 |= (1 << PCINT30);
}

void timer0_init()
{
    TCCR0A |= (1 << WGM01) | (1 << WGM00) | (1 << COM0A1) | (1 << COM0A0);
    OCR0A = 0xC0;
    TCCR0B |= (1 << CS00);
}

void timer1_init()
{
#if 0
    /* timer1 este configurat cu modul Normal, top 0xFFFF si prescaler 256 */
    TCCR1B |= (1 << CS12);
#endif
    TCCR1B |= (1 << CS10);
#if 0
    TIMSK1 |= (1 << OCIE1A) | (1 << TOIE1);
#endif
    /* Task2 */
    TCCR1A |= (1 << COM1A1) | (1 << COM1A0);

#if 0
    /* Task1 */
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12) | (1 << WGM13);
#endif

#if 0
    /* 16MHz / 256 = 62500 Hz */
    ICR1 = 62500;
#endif
    ICR1 = 0xFFFF;

    /* Task6 */
    TCCR1A |= (1 << COM1B1) | (1 << WGM10) | (1 << WGM11);
    TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS12);
#if 0
    /* duty cycle-ul este setat initial la 25% */
    OCR1A = 0xFFFF / 4;
    /* duty cycle-ul initial pentru speaker (OC1B) este 50% */
    OCR1B = 0xFFFF / 2;
#endif
    /* Task 1 */
    OCR1A = ICR1 / 4;
    OCR1B = ICR1 / 2;
}

void timer2_init()
{
    TCCR2A |= (1 << WGM21) | (1 << WGM20) | (1 << COM2A1) | (1 << COM2A0);
#if 0
    OCR2A = 0xC0;
#endif
    TCCR2B |= (1 << CS20);
}

uint8_t stare = 1;
void update_color()
{
    if (stare == 1) {
//        OCR0A += 0x01;
//        OCR1A -= 0x100;

//        if (OCR0A == 0xFF) {
//            stare = 2;
//        }
    } else if (stare == 2) {
        OCR1A += 0x100;
        OCR2A -= 0x01;

        if (OCR2A == 0) {
            stare = 3;
        }
    } else if (stare == 3) {
        OCR2A += 0x01;
        OCR0A -= 0x01;

        if (OCR0A == 0) {
            stare = 1;
        }
    }
}

void update_song()
{
    durata -= 25;
    if (durata < 25) {
        OCR1A = 15625 / surprise_notes[nr_nota];
        OCR1B = OCR1A/2;
        TCNT1 = 0;
        durata = durations[nr_nota];
        nr_nota = (nr_nota+1)%64;
    }
}

int main() {
    DDRD |= (1 << PD7);
    LCD_init();

	int nota = 0, firstB = 1, firstD = 1, firstA = 1;
	int ok = 0, current_colorB = RED, current_colorD = GREEN, current_colorA = BLUE;
	long switchColorB = 0, switchColorD = 0, switchColorA = 0;
	int isColorSwitchedB = 0, isColorSwitchedD = 0, isColorSwitchedA = 0;

	int i = 0, j = 0, increment_note = 0;
	int if_changed_j = 0, if_changed_i = 0;
	initLeds();
    for (;;) {
		if (if_changed_i) {
			if_changed_i = 0;
			LCD_clear_top_line();
			LCD_clear_bottom_line();
		}
        LCD_printAt(0x00, messages[i][0]);
		LCD_printAt(0x40, messages[i][1]);
		
		speaker_reda(notes[i][j]);
		++increment_note;
		if (increment_note == CHANGE_NOTE) {
			++j;
			if (j == NOTES) {
				j = 0;
				if_changed_i = 1;
				++i;
			}
			if (i == 4)
				i = 0;
			increment_note = 0;
		}
		// port B
		
		++switchColorB;
		if (switchColorB == SWITCH_1 && current_colorB == RED) {
			isColorSwitchedB = 1;
			current_colorB = GREEN;
		} else if (switchColorB == SWITCH_2 && current_colorB == GREEN) {
			isColorSwitchedB = 1;
			current_colorB = BLUE;
		} else if (switchColorB == SWITCH_3 && current_colorB == BLUE) {
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
			isColorSwitchedB = 0;
		} else if (current_colorB == GREEN && isColorSwitchedB) {
			PORT_LED_RED_B &= ~(1 << LED_RED_B);
			PORT_LED_GREEN_B |= (1 << LED_GREEN_B);
			isColorSwitchedB = 0;
		} else if (current_colorB == BLUE && isColorSwitchedB) {
			PORT_LED_GREEN_B &= ~(1 << LED_GREEN_B);
			PORT_LED_BLUE_B |= (1 << LED_BLUE_B);
			isColorSwitchedB = 0;
		} 

		// port D
		++switchColorD;
		if (switchColorD == SWITCH_1 && current_colorD == BLUE) {
			isColorSwitchedD = 1;
			current_colorD = RED;
		} else if (switchColorD == SWITCH_2 && current_colorD == RED) {
			isColorSwitchedD = 1;
			current_colorD = GREEN;
		} else if (switchColorD == SWITCH_3 && current_colorD == GREEN) {
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
		if (switchColorA == SWITCH_1 && current_colorA == GREEN) {
			isColorSwitchedA = 1;
			current_colorA = BLUE;
		} else if (switchColorA == SWITCH_2 && current_colorA == BLUE) {
			isColorSwitchedA = 1;
			current_colorA = RED;
		} else if (switchColorA == SWITCH_3 && current_colorA == RED) {
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
		//update_song();
		//task2and3();
		//_delay_ms(50);
		ok = 1 - ok;
    }
    return 0;
}