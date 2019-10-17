/*
 * DA2c_1_a.c
 *
 * Created: 10/16/2019 8:37:25 PM
 * Author : Dilbert
 */ 
 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	unsigned short count = 0;

	DDRB |=(1<<DDB2);	// Set PortB.2 as an output
	PORTB = (0<<DDB2);	// Set PortB.2 Low (LED off)
	TCCR0A = 0;			// Timer0, Normal mode, internal clk
	TCNT0 = 0X00;		// Setting the count register to 0[count goes from 0-255]
	TCCR0B |= 0X05;		// Setting the Prescaler to 1024 and starting the timer
	
    while (1) { 
		PORTB ^=(1<<DDB2);					// Toggle PortB.2 (Turn on)
		while(count != 15){					// Run timer 81 times
			while((TIFR0 & 0X01) == 0){};	// run timer until overflow
				TCNT0 = 0X00;				// reset the timer count
				TIFR0 = 0X01;				// reset the tiemr over flow flag
				count = count + 1;			// increment the count of timer overflows
			}
		count = 0;							// Reset the count of overflows
		PORTB ^=(1<<DDB2);					// Toggle PortB.2 (Turn off)
		while(count != 23){					// Run timer 81 times
			while((TIFR0 & 0X01) == 0){};	// run timer until overflow
			TCNT0 = 0X00;					// reset the timer count
			TIFR0 = 0X01;					// reset the tiemr over flow flag
			count = count + 1;				// increment the count of timer overflows
			}
		count = 0;							// Reset the count of overflows
	}
}