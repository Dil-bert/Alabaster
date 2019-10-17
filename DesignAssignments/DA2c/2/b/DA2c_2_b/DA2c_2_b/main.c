/*
 * DA2c_2_b.c
 *
 * Created: 10/15/2019 8:52:37 AM
 * Author : Dilbert
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned short county;		// Global Count Variable
int main(void)
{
	DDRB = 1<<2;			// Set PortB.2 as an output
	PORTB = 0<<2;			// Set PortB.2 Low (LED off)
	DDRC = 1<<3;			// Set PortC.3 as an input
	PORTC = 1<<3;			// Set PortC.3 Pull up high

	county = 0;				// Set global count to 0

	TCNT0 = 0X00;			// Set the Timer count register to zero
	TIMSK0 = (1<<TOIE0);	// Set the Timer Overflow interrupt enabled
	sei();					// Global interrupt enable
	while(1){
		if(county == 82){		// If Count = 82
			county = 0;			// Set global counter to 0
			PORTB = 0<<2;		// Set PortB.2 Low (LED off)
		}
		if(!(PINC & (1 << 3))){	// If PortC.3 Goes low (Button pressed)
			county = 0;			// Set global counter to 0
			PORTB = 1<<2;		// Set PortB.2 High (LED on)
			TCCR0A = 0X00;		// Set Timer Control Register.A = 0x00 (Normal Mode)
			TCCR0B = 0X05;		// Set Timer Control Register.B = 0x05 (Prescaler 1024)
		}
	};
}
ISR (TIMER0_OVF_vect){		// Timer Overflow interrupt Service Request 
	county = county + 1;	// Increment Count + 1
	TCNT0 = 00;				// Reset the timer count register to zero
}

