/*
 * DA3B.c
 *
 * Created: 10/24/2019 9:12:14 PM
 * Author : Dilbert
 */ 
#define F_CPU 16000000UL

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define USART_BAUDRATE 9600
#define UBRR_VALUE (((F_CPU/(USART_BAUDRATE*16UL)))-1)

// Global Variables 
volatile uint8_t tempL = 0;
volatile uint8_t tempH = 0;
volatile float tempOut = 0;

// Function definitions
void InitPort();

void InitADC();
void SetADCChannel(uint8_t ADCchannel);
void StartADC(void);
void DisableADC(void);

void USART0Init(void);
void InitTimer0(void);
int USART0SendByte(char u8Data,FILE *stream);
void StartTimer0(void);
void StopTimer(void);

ISR(ADC_vect);

// Set Stream Pointer
FILE usart0_str = FDEV_SETUP_STREAM(USART0SendByte, NULL, _FDEV_SETUP_WRITE);

//-----------------------------------------------------------------------------
//	**********MAIN**********
//-----------------------------------------------------------------------------
int main(void){
	// Initialize USART0
	USART0Init();
	// Initialize ports
	InitPort();
	// Assign our stream to standard I/O streams
	stdout=&usart0_str;
	// Initialize ADC
	InitADC();
	// Select ADC channel
	SetADCChannel(5);
	// Initialize timer0
	InitTimer0();
	// Start timer 0
	StartTimer0();
	// Start conversion
	StartADC();
	// Enable global interrupts
	sei();
    while (1){
		_delay_ms(1000);
		tempOut = ((tempH << 8)|(tempL));
		tempOut = ((tempOut * 0.4883));
		printf("Temp = %.1f C\r\n",tempOut);
    }
}

//-----------------------------------------------------------------------------
//	*********Port Initialization function*********
//-----------------------------------------------------------------------------
void InitPort(){
	// Set pin C5 as an input pin
	DDRC |= (0<<PINC5);
	// Ensure pin C5 pull up resistor is off
	PORTC = (0<<PINC5);
	// Turn off Digital logic on pin C5
	DIDR0 |= (1<<ADC5D);
}

//-----------------------------------------------------------------------------
//	*********ADC Initialization function*********
//-----------------------------------------------------------------------------
void InitADC(){
	// Select Vref=Avcc and set (left = ADLAR = 1)(right = ADLAR = 0) justified result
	ADMUX |=(1<<REFS0)|(0<<ADLAR);
	// Set prescaller to 32, enable auto triggering, enable ADC interrupt
	//  and enable ADC
	ADCSRA |=(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADATE)|(1<<ADIE)|(1<<ADEN);
	// Set ADC trigger source - Timer0 compare match A
	ADCSRB |=(1<<ADTS1)|(1<<ADTS0);
}

//-----------------------------------------------------------------------------
//	*********ADC PORT SELECT function*********
//-----------------------------------------------------------------------------
void SetADCChannel(uint8_t ADCchannel){
	// Select ADC channel with safety mask
	ADMUX = (ADMUX & 0xF0)|(ADCchannel & 0x0F);
}

//-----------------------------------------------------------------------------
//	*********ADC start function*********
//-----------------------------------------------------------------------------
void StartADC(void){
	ADCSRA |=(1<<ADSC);
}

//-----------------------------------------------------------------------------
//	*********ADC end function*********
//-----------------------------------------------------------------------------
void DisableADC(void){
	ADCSRA &= ~((1<<ADEN)|(1<<ADIE));
}

//-----------------------------------------------------------------------------
//	*********USART Initialization function*********
//-----------------------------------------------------------------------------
void USART0Init(void){
	// Set baud rate
	UBRR0H = (uint8_t)(UBRR_VALUE>>8);
	UBRR0L = (uint8_t)UBRR_VALUE;
	// Set frame format to 8 data bits no parity, 1 stop bit
	UCSR0C |= (1<<UCSZ01)|(UCSZ00);
	// Enable transmission and reception
	UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
}

//-----------------------------------------------------------------------------
//	*********Timer0 Initialization function*********
//-----------------------------------------------------------------------------
void InitTimer0(void){
	// Set Initial Timer value
	TCNT0 = 0;
	// Place TOP timer value to Output compare register
	OCR0A = 99;
	// Set CTC mode
	//  and make toggle PD6/OC0A pin on compare match
	TCCR0A |= (1<<COM0A0)|(1<<WGM01);
}

//-----------------------------------------------------------------------------
//	*********USART Send function*********
//-----------------------------------------------------------------------------
int USART0SendByte(char u8Data,FILE *stream){
	// Wait while previous byte is completed
	while(!(UCSR0A & (1<<UDRE0))){};
	// Transmit Data
	UDR0 = u8Data;
	return 0;
}

//-----------------------------------------------------------------------------
//	*********Timer start function*********
//-----------------------------------------------------------------------------
void StartTimer0(void){
	// Set prescaller 8 and start timer
	TCCR0B |= (1<<CS01);
}

//-----------------------------------------------------------------------------
//	*********Timer end function*********
//-----------------------------------------------------------------------------
void StopTimer(void){
	TCCR0B &= ~(1<<CS01);
	TIMSK0 &= ~(1<<OCIE0A);
}

//-----------------------------------------------------------------------------
//	*********ADC conversion complete ISR*********
//-----------------------------------------------------------------------------
ISR(ADC_vect){
	// clear timer compare match flag
	TIFR0 = (1<<OCF0A);
	// Toggle pin PD2 to track the end of ADC conversion
	PIND = (1<<PD2);
	tempL = ADCL;
	tempH = ADCH;
}