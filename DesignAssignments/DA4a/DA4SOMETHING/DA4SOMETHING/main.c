/*
 * DA4A.c
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
volatile float ADC_in_Out = 0;
volatile int DC;

// Function definitions
void InitPort();

void InitADC();
void SetADCChannel(uint8_t ADCchannel);
void StartADC(void);
void DisableADC(void);

void USART0Init(void);
void InitTimer0(void);
void InitTimer1(void);
int USART0SendByte(char u8Data,FILE *stream);
void StartTimer0(void);
void ToggleTimer1(void);
void StopTimer(void);

ISR(ADC_vect);
ISR(PCINT1_vect);



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
	SetADCChannel(0);
	// Initialize timer0
	InitTimer0();
	InitTimer1(); // PWM timer
	// Start timer 0
	StartTimer0();
	
	// Start conversion
	StartADC();
	// Enable global interrupts
	sei();
    while (1){
		//_delay_ms(1000);
		printf("ADC_in_ = %.1f R\r\n",ADC_in_Out);
		//printf("DC = %.1d R\r\n",DC);
		DC = (ADC_in_Out/1023)*100;
		printf("DC = %.1d R\r\n",DC);
		DC = (DC * 0XFF)/2;
		OCR1AL = (DC);
    }
}

//-----------------------------------------------------------------------------
//	*********Port Initialization function*********
//-----------------------------------------------------------------------------
void InitPort(){
	// Set pin C0 as an input pin for ADC
	// Ensure pin C0 pull up resistor is off
	// Turn off Digital logic on pin C5
	DDRC  |= (0<<PINC0);
	PORTC |= (0<<PINC0);
	DIDR0 |= (1<<ADC0D);
	// Set pin C1 as an input for stop start button
	// Ensure pin C1 Pull up resistor is on
	// Enable PCINT Group1
	//DDRC  |= (0<<PINC1);
	PORTC |= (1<<PINC1);
	PCICR |= (1<<PCIE1);
	PCMSK1 |= (1<<PCINT9);
	
}

//-----------------------------------------------------------------------------
//	*********ADC Initialization function*********
//-----------------------------------------------------------------------------
void InitADC(){
	// Select Vref=Avcc and set (left = ADLAR = 1)(right = ADLAR = 0) justified result
	ADMUX |= (1<<REFS0)|(0<<ADLAR);
	// Set prescaller to 128, enable auto triggering, enable ADC interrupt
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

void InitTimer1(void){

	TCCR1A |=(1<<COM1A0)|(1<<COM1A1)|(1<<COM1B0)|(1<<COM1B1)|(1<<WGM10);	
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

void ToggleTimer1(void){
	// Set prescaller 8 and start timer
	TCCR1B ^= (1<<CS11);
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
	volatile uint8_t ADC_in_L = 0;
	volatile uint8_t ADC_in_H = 0;
	// clear timer compare match flag
	TIFR0 = (1<<OCF0A);
	// Toggle pin PD2 to track the end of ADC conversion
	PIND = (1<<PD2);
	ADC_in_L = ADCL;
	ADC_in_H = ADCH;
	ADC_in_Out = ((ADC_in_H << 8)|(ADC_in_L));
}

ISR(PCINT1_vect){
	//volatile uint8_t test = 0;
	//test = PINC1;
	if(!(PINC & (1 << PINC1))){
		ToggleTimer1();
	}
}