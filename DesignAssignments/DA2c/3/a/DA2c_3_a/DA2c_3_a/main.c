/*
 * DA2c_3_a.c
 *
 * Created: 10/16/2019 10:01:39 PM
 * Author : Dilbert
 */ 

#include <avr/io.h>

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned short county;
	
int main(void){
   	county = 0;				// Global count variable set to zero
	DDRB = (0<<2);			// Set PortB.2 as an output
   	PORTB = (1<<2);			// Set PortB.2 high (Turn on LED)
	OCR0A = 0XFF;			// Set the output compare register to 0xFF
   	TCCR0A = 0X02;			// Set the Timer compare register.B WGM01 = 1 (CTC mode)
   	TCCR0B = 0X05;			// Set the Timer compare register.A CS02 & CS00 = 1 (Prescaler 1024)
   	TIMSK0 = (1<<OCIE0A);	// Set the Timer mask to ouptut the compare flag interrupt
   	sei();					// Global interrupt enable
    while (1) 
    {
		if(county == 15){	// If the count goes to 15 enter (after 40% of period)
			PORTB = (0<<2);	// Set PortB.2 Low (turn off LED)
		}
		if(county == 38){	// If the count goes to 38 enter (after completion of period)
			PORTB = (1<<2);	// Set PortB.2 High (turn on LED)
			county = 0;		// Set count to zero
		}
    }
}

ISR (TIMER0_COMPA_vect){	// Compare interrupt service request function
	county = county + 1;	// Count increment + 1
	TIFR0 |= (1<<OCF0A);	// Reset the compare flag interrupt register
}