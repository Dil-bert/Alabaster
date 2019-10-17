/*
 * DA2c_3_b.c
 *
 * Created: 10/16/2019 10:55:44 PM
 * Author : Dilbert
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned short county;
	
int main(void){						//-----------------------------------------
	DDRB = (1<<2);		// SET B2 AS OUTPUT
	PORTB = (0<<2);		// SET B2 OUTPUT LOW
	DDRC = (0<<3);		// SET C3 AS INPUT
	PORTC = (1<<3);		// SET C3 PULL UP RESISTOR HIGH
   	county = 0;			// GLOBAL COUNT VARIABLE = 0

    while (1) 
    {
		if(!(PINC & (1 << 3))){		// If pinC.3 goes low
			PORTB = (1<<DDB2);		// Toggle PortB.2 (Turn on)
			county = 0;				// Ensure Count variable is zero
		   	OCR0A = 0XFF;			// Set the compare register to 0xFF
   			TCCR0A = 0X02;			// Control RegisterA WGM01 = 1 (CTC mode)
   			TCCR0B = 0X05;			// Control RegisterB CS02 and CS00 = 1 (1024 PRESCALER)
   			TIMSK0 = (1<<OCIE0A);	// Set the Timer mask for compare interrupt flag
   			sei();
		}
		if(county == 81){		// Run timer 81 times
			PORTB =(0<<DDB2);	// Toggle PortB.2 low (Turn off)
			county = 0;			// Set Counter to zero.
		}
    }
}

ISR (TIMER0_COMPA_vect){	// Timer compare flag interrupt service request 
	county = county + 1;	// Increment the counter 1
	TIFR0 |= (1<<OCF0A);	// Toggle the compare flag
}