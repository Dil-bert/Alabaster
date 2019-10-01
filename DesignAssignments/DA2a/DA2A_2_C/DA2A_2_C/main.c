/*
 * DA2A_2_C.c
 *
 * Created: 9/29/2019 11:23:02 PM
 * Author : Dilbert
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRB |= (1 << 2);			// Set port B pin 2 as output
	PORTB = (0 << 2);			// Set port B pin 2 Low
	DDRC |= (0 << 3);			// Set port C pin 3 as input
	PORTC = (1 << 3);			// Set port C pin 3 as normal high (Enable Pull up resistor)
	while (1)					// Infinite loop (polling)
	{
		if(!(PINC & (1 << 3))){	// IF Pin 3 on Port C is Low, enter statement, else loop
			PORTB |= (1 << 2);	// Turn on LED
			_delay_ms(1333);	// Delay for 1.333 sec
			PORTB = (0 << 2);	// Turn off LED
		}	
	}
}
