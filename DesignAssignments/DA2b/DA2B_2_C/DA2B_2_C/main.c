/*
 * DA2B_2_C.c
 *
 * Created: 10/3/2019 11:36:25 AM
 * Author : Dilbert
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//#define cli() asm volatile("cli"::)
//#define sei() asm volatile("sei"::)
int main(void)
{
	DDRB = 1<<2;		//	PB.2 set as output
	PORTD = 1<<2;		//	Enable Pull up resistor on PORTD.2
	EICRA = 0X2;		//	Set as a falling edge interrupt
	
	EIMSK = (1<<INT0);	//	THIS ENABLES THE INTERUPT "INT0" (note: INT0 is a global constant from <avr/interrupt.h>)
	sei();				//	set the global interrupt enable
    while (1)			// General program loop for what ever is needed.
    {
    }
}

	ISR (INT0_vect)			// Interrupt Sub Routine for external interrupt 0
	{
		PORTB = 1<<2;		// PORTB.2 output high
		_delay_ms(1333);	// Delay for 1.333 seconds
		PORTB = (0<<2);		// PORTB.2 output low
	}
