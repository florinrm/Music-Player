#ifndef NOTES_H
#define NOTES_H

#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>

#include <stdlib.h>
#include <stdio.h>

#include "lcd.h"
#include "util.h"


/* definitii note (Hz) */
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

uint16_t surprise_notes[] = {a, a, a, f, cH, a, f, cH, a, eH, eH, eH, fH, cH, gS, f, cH, a, aH, a, a, aH, gSH, gH, fSH, fH, fSH, aS, dSH, dH, cSH, cH, b, cH, f, gS, f, a, cH, a, cH, eH, aH, a, a, aH, gSH, gH, fSH, fH, fSH, aS, dSH, dH, cSH, cH, b, cH, f, gS, f, cH, a, f, cH, a};

uint16_t durations[] = {500, 500, 500, 350, 150, 500, 350, 150, 650, 500, 500, 500, 350, 150, 500, 350, 150, 650, 500, 300, 150, 400, 200, 200, 125, 125, 250, 250, 400, 200, 200, 125, 125, 250, 125, 500, 375, 125, 500, 375, 125, 650, 500, 300, 150, 400, 200, 200, 125, 125, 250, 250, 400, 200, 200, 125, 125, 250, 250, 500, 375, 125, 500, 375, 125, 650};

uint8_t num_notes = sizeof(durations) / sizeof(durations[0]);

int nr_nota;
int durata;

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
    DDRB |= (1 << PB3);
    DDRD |= (1 << PD4) | (1 << PD5) | (1 << PD7);
    /* LED-ul RGB este stins (punem pinii in starea HIGH) */
    PORTB |= (1 << PB3);
    PORTD |= (1 << PD5) | (1 << PD7);

    /* butoanele sunt puse ca intrare, cu rezistenta de pull-up */
    DDRB &= ~(1 << PB2);
    DDRD &= ~(1 << PD6);
    PORTB |= (1 << PB2);
    PORTD |= (1 << PD6);

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

#endif