/* File: main.cpp
 * Contains base main function and usually all the other stuff that avr does...
 */
/* Copyright (c) 2012-2013 Domen Ipavec (domen.ipavec@z-v.si)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
 #define F_CPU 8000000UL  // 8 MHz
#include <util/delay.h>

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/pgmspace.h>
//#include <avr/eeprom.h> 

#include <stdint.h>

#include "bitop.h"
#include "pwm.h"
#include "random32.h"
#include "exponential.h"

using namespace avr_cpp_lib;

#define ADMUX_SET 0b01100000

#define RED_START 0
#define YELLOW_START 0

pwm_channel pwm_data[25] = {
	// rdece ledice
	{&DDRB, &PORTB, PB3, RED_START}, // R4
	{&DDRB, &PORTB, PB5, RED_START}, // R5
	{&DDRB, &PORTB, PB6, RED_START}, // R1
	{&DDRD, &PORTD, PD0, RED_START}, // R6
	{&DDRD, &PORTD, PD2, RED_START}, // R2
	{&DDRD, &PORTD, PD4, RED_START}, // R7
	{&DDRD, &PORTD, PD6, RED_START}, // R8
	{&DDRA, &PORTA, PA6, RED_START}, // R12
	{&DDRC, &PORTC, PC6, RED_START}, // R11
	{&DDRC, &PORTC, PC4, RED_START}, // R3
	{&DDRC, &PORTC, PC2, RED_START}, // R10
	{&DDRC, &PORTC, PC0, RED_START}, // R9

	// rumene ledice
	{&DDRA, &PORTA, PA5, YELLOW_START}, // Y4
	{&DDRB, &PORTB, PB4, YELLOW_START}, // Y5
	{&DDRC, &PORTC, PC7, YELLOW_START}, // Y1
	{&DDRB, &PORTB, PB7, YELLOW_START}, // Y6
	{&DDRD, &PORTD, PD1, YELLOW_START}, // Y2
	{&DDRD, &PORTD, PD3, YELLOW_START}, // Y7
	{&DDRD, &PORTD, PD5, YELLOW_START}, // Y8
	{&DDRA, &PORTA, PA7, YELLOW_START}, // Y12
	{&DDRC, &PORTC, PC5, YELLOW_START}, // Y11
	{&DDRC, &PORTC, PC3, YELLOW_START}, // Y3
	{&DDRC, &PORTC, PC1, YELLOW_START}, // Y10
	{&DDRD, &PORTD, PD7, YELLOW_START}, // Y9

	PWM_CHANNEL_END
};
	
pwm_worker pwm(pwm_data);

void setLed(uint8_t x, uint8_t intensity, uint8_t color) {
	uint16_t tmp = color * intensity;
	tmp >>= 8;
	
	pwm_data[x].value = exponential(tmp);
	pwm_data[x+12].value = exponential(intensity - tmp);
}

#define INT_START 30

volatile uint8_t adc_channel = 0;
volatile uint8_t color = 100;
volatile uint8_t lightness = 100;
volatile uint8_t speed = 100;

void update_values(uint8_t x) {
	uint16_t light = lightness;
	light *= 3;
	light >>= 2;
	light += 30;
	uint8_t intensity = 5 + (lightness>>3) + get_random32(light);
	
	setLed(x, intensity, color);
}

int main() {
	// init
	
	
	// enable interrupts
	//sei();

	// adc settings
	ADMUX = ADMUX_SET | adc_channel;
	ADCSRA = 0b11101111;


	// count pwm cycle
	uint8_t i = 0;
	uint16_t c = 5120;

	sei();

	for (;;) {

		pwm.cycle(i);
		i++;
		
		
		c--;
		if (c < 13) {
			if (c == 0) {
				c = 2000 + speed*30 + get_random32(1000 + speed*40);
			} else {
				update_values(c-1);
			}
		}

	}
}

ISR(ADC_vect) {
	adc_channel++;
	if (adc_channel > 2) {
		adc_channel = 0;
	}
	ADMUX = ADMUX_SET | adc_channel;

	if (adc_channel == 0) {
		color = ADCH;
	} else if (adc_channel == 1) {
		lightness = ADCH;
	} else {
		speed = ADCH;
	}
}
