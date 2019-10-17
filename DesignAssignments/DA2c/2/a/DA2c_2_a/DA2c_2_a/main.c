/*
 * DA2c_2_a.c
 *
 * Created: 10/16/2019 8:57:56 PM
 * Author : Dilbert
 */ 
 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned short county;		// Global counter variable
int main(void)
{
	county = 0;				// Set count to zero
	DDRB = 1<<2;			// Set PortB.2 as an output
	PORTB = 1<<2;			// Set PortB.2 high (Turn on LED)
	
	TCNT0 = 0X00;			// Clear the timer count register
	TIMSK0 = (1<<TOIE0);	// Turn on the timer overflow interrupt 
	TCCR0A = 0X00;			// Set the Timer control register.A = 0x00 (Normal mode)
	TCCR0B = 0X05;			// Set the Timer control register.B = 0x00 (Prescaler 1024)
	sei();					// Global interrupts Enable
	while(1){
		if(county == 15){	// If count equals 15 (40% of period)
			PORTB = 0<<2;	// Set PortB.2 Low (Turn off LED)
		}
		if(county == 38){	// If count equals 38 (end of period)
			PORTB = 1<<2;	// Set PortB.2 High (Turn on LED)
			county = 0;		// REset Counter to 0
		}
	};
}

ISR (TIMER0_OVF_vect){		// Overflow Interrupt service request function
	county = county + 1;	// Count increment Plus one
	TCNT0 = 00;				// Reset the timer count register to 0x00
}
