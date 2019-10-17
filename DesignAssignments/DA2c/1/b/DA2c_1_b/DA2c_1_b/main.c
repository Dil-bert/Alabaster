/*
 * DA2c_1_b.c
 *
 * Created: 10/12/2019 6:16:22 PM
 * Author : Dilbert
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	unsigned short count = 0; // Counting variable
	
	DDRC |=(0<<DDC3);	// Set PortC.3 as an input
	PORTC =(1<<DDC3);	// Set PortC.3 pull up High
	DDRB |=(1<<DDB2);	// Set PortB.2 as an output
	PORTB =(0<<DDB2);	// Set PortB.2 Low (LED off)
	
	TCCR0A = 0;			//Timer0, Normal mode, internal clk
	TCNT0 = 0X00;		//Setting the count register to 0[count goes from 0-255]
	TCCR0B |= 0X05;		//Setting the Prescaler to 1024 and starting the timer
    while (1) {
		if(!(PINC & (1 << 3))){					//If input PinC.3 == 0 
			PORTB ^=(1<<DDC2);					//Toggle PortB.2 (Turn on)
			while(count != 81){					//Run timer 81 times
				while((TIFR0 & 0X01) == 0){};	// run timer until overflow
				TCNT0 = 0X00;					//reset the timer count
				TIFR0 = 0X01;					//reset the tiemr over flow flag
				count = count + 1;				//increment the count of timer overflows
				}
			PORTB ^=(1<<DDB2);					//Toggle PortB.2 (Turn off)
			}
		count = 0;								//Reset the count of overflows
		}
}

