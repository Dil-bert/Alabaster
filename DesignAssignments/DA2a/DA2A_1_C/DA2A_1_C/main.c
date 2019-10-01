/*
 * DA2A_1_C.c
 *
 * Created: 9/29/2019 10:55:18 PM
 * Author : Dilbert
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRB |= (1 << 3);		// Set port B pin 3 as an output
	PORTB = (0 << 3);		// Set port B pin 3 output to zero
    while (1) 				// 	Infinate loop
    {
		PORTB |= (1 << 3);	// Set port B pin 3 output to High
		_delay_ms(250);		// Delay for .25 seconds
		PORTB = (0 << 3);	// Set port B pin 3 output to Low
		_delay_ms(375);		// Delay for .375 seconds
    }
}

