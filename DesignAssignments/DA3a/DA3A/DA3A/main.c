#define BAUD 9600
#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void USART_send(char data); // Used to send integer to terminal
void USART_putstring(char* StringPtr); // Used to take in every character in the string and sends it to the terminal
void USART_init(void); // Initializes the analog to digital functions, as well as OVF interrupt
void USART_tx_int(int x); // Convert a random integer to a string

char stringtype[] = "String: "; // Declaring the string value on screen
char inttype[] = "Integer: "; // Declaring the integer value on screen
char floattype[] = "Floating Point: "; // Declaring the floating point value on screen
char Space[] = "\n"; // Used to create the next line

char String[] = "DA3A"; //String[] is the variable to output into terminal
char outs[20]; // Allocating memory space to contain the float value
char outints[40]; // Allocating memory space to contain the random int value
volatile float adc_temp = 69.6969696969; // Sets the float value
volatile int r; // Global Variable for the random int (So I don't have to pass it)

int main(void)
{
	USART_init(); // Initializes the analog to digital functions as well as OVF interrupt
	srand(5); // Random number seed
	while(1)
	{
		// main loop
		r = rand();	// making and setting a random int
	}
}

ISR (TIMER1_OVF_vect)
{
	USART_putstring(Space); // creates next line
	USART_putstring(stringtype); // LABEL PRINT "String: "
	USART_putstring(String); // prints string "DA3A"
	USART_putstring(Space); // creates next line
	USART_putstring(inttype); // LABEL PRINT "Integer: "
	snprintf(outints, sizeof(outints), "%d", r); // the int characters are stored in outints
	USART_putstring(outints); // transmits outints to UART
	USART_putstring(Space); // creates next line
	USART_putstring(floattype); // LABEL PRINT "FLoating Point: "
	snprintf(outs, sizeof(outs), "%f\r\n", adc_temp); // the floating point characters are stored in outs
	USART_putstring(outs); // transmits outs to UART
	USART_putstring(Space); // creates next line
	TCNT1 = 49911; // Reset timer
}

void USART_init( void )
{
	UBRR0H = 0;
	UBRR0L = F_CPU/16/BAUD - 1; // Used for the BAUD prescaler
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
	TCCR1B |= 5; //(1 << CS12) | (1 << CS10); // Sets prescaler to 1024
	TIMSK1 = (1 << TOIE1); // Enables overflow flag
	TCNT1 = 49911; // 1 second delay = (0xFFFF) - TCNT = 65535 - 15624 = 49911
	sei();
}

void USART_send(char data)
{
	while (!(UCSR0A & (1 << UDRE0))); // Until UDRE0 goes high, it will keep looping
	UDR0 = data; // UDR0 register grabs the value given from the parameter
	
}

void USART_putstring(char *StringPtr)
{
	while ((*StringPtr != '\0')){ // Until it reaches the end of the line, it will keep looping
		while (!(UCSR0A & (1 << UDRE0))); // Until UDRE0 goes high, it will keep looping
		UDR0 = *StringPtr; // UDR0 register grabs the value given from the parameter
		StringPtr++; // but it does it by every character as shown here
	}
}

// ***** my Original transmit function (much more complex than needed) *****

/*void USART_tx_int(int x){
	char buffer[64];
	int ret = snprintf(buffer, sizeof buffer, "%d", x);
	USART_send(buffer);
	if(ret < 0){
		return EXIT_FAILURE;
	}
	if(ret >= sizeof buffer){
		//Result was truncated - resize the buffer and retry
		USART_send("\r\nResult was truncated - resize the buffer and retry\r\n");
	}
}
*/