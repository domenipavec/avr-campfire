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
//#include <avr/interrupt.h>
//#include <avr/pgmspace.h>
//#include <avr/eeprom.h> 

#include <stdint.h>

#include "bitop.h"
#include "pwm.h"
#include "random32.h"

#define RED_START 0
#define YELLOW_START 0

avr_cpp_lib::pwm_channel pwm_data[30] = {
	// rdece ledice
	{&DDRB, &PORTB, PB0, RED_START}, // R4
	{&DDRB, &PORTB, PB2, RED_START}, // R5
	{&DDRB, &PORTB, PB3, RED_START}, // R1
	{&DDRB, &PORTB, PB5, RED_START}, // R6
	{&DDRB, &PORTB, PB7, RED_START}, // R2
	{&DDRD, &PORTD, PD1, RED_START}, // R7
	{&DDRD, &PORTD, PD3, RED_START}, // R8
	{&DDRA, &PORTA, PA1, RED_START}, // R12
	{&DDRA, &PORTA, PA4, RED_START}, // R11
	{&DDRA, &PORTA, PA6, RED_START}, // R3
	{&DDRC, &PORTC, PC7, RED_START}, // R10
	{&DDRC, &PORTC, PC5, RED_START}, // R9

	// rumene ledice
	{&DDRA, &PORTA, PA0, YELLOW_START}, // Y4
	{&DDRB, &PORTB, PB1, YELLOW_START}, // Y5
	{&DDRA, &PORTA, PA3, YELLOW_START}, // Y1
	{&DDRB, &PORTB, PB4, YELLOW_START}, // Y6
	{&DDRB, &PORTB, PB6, YELLOW_START}, // Y2
	{&DDRD, &PORTD, PD0, YELLOW_START}, // Y7
	{&DDRD, &PORTD, PD2, YELLOW_START}, // Y8
	{&DDRA, &PORTA, PA2, YELLOW_START}, // Y12
	{&DDRA, &PORTA, PA5, YELLOW_START}, // Y11
	{&DDRA, &PORTA, PA7, YELLOW_START}, // Y3
	{&DDRC, &PORTC, PC6, YELLOW_START}, // Y10
	{&DDRC, &PORTC, PC4, YELLOW_START}, // Y9

	PWM_CHANNEL_END
};
	
avr_cpp_lib::pwm_worker pwm(pwm_data);

void update_values() {
	//	for (uint8_t x = 0; x < 24; x++) {
	static uint8_t x = 0;
	static uint8_t intensity = 0;
	intensity++;
	if (intensity == 0) {
		x ++;
		if (x == 12) {
			x = 0;
		}
	}

	uint8_t color = 100;

	uint16_t tmp = color * intensity;
	tmp /= 255;

	pwm_data[x].value = tmp;
	pwm_data[x+12].value = intensity - tmp;
	//}
}

int main() {
	// init
	
	
	// enable interrupts
	//sei();


	// count pwm cycle
	uint8_t i = 0;
	uint16_t c = 100;

	update_values();

	for (;;) {

		pwm.cycle(i);
		i++;

		c--;
		if (c== 0) {
			update_values();
			//c = get_random32(5)*1000 + 5000;
			c = 100;
		}

	}
}
