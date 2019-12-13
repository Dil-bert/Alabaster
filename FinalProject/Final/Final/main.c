/*
 * Final.c
 *
 * Created: 12/12/2019 3:34:59 PM
 * Author : Dilbert
 */ 


#define F_CPU 16000000UL
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>

#define USART_BAUDRATE 115200
#define UBRR_VALUE (int)round((((F_CPU/(USART_BAUDRATE*8UL)))-1))

// Global Variables
volatile uint8_t tempL = 0;
volatile uint8_t tempH = 0;
volatile float tempOut = 0;
char TEMP[50];
char WIFI[] = "";
char PASSWORD[] = "";


// Function definitions
void InitPort();


void USART0Init(void);
int USART0SendByte(char u8Data,FILE *stream);
void check_OK(void);
void UART_sendString(char * AT);
char * UART_ReciveString(void);

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
	
	sei();
	UART_sendString("AT\r\n");
	_delay_ms(20);
	// Check if ok returned
	check_OK();
	_delay_ms(20);
	// Select WIFI mode
	UART_sendString("AT+CWMODE=1\r\n");
	_delay_ms(20);
	// Connect to local WIFI (REPLACE "SSID" and "PASSWORD" with your ssid and password
	UART_sendString("AT+CWJAP=\"NoMore\",\"Ch@ng3l!ng\"\r\n");
	_delay_ms(40);
	// Enable connection
	UART_sendString("AT+CIPMUX=0\r\n");
	_delay_ms(40);
	
	while (1){
		// 15 Second Delay between displaying temp
		_delay_ms(15000);
		
		// Start a connection as client to Thingspeak
		UART_sendString("AT+CIPSTART=\"TCP\",\"184.106.153.149\",80\r\n");//"AT+CIPSTART=\"TCP\",\"184.106.153.149\",80\r\n"
		_delay_ms(40);
		// Specify the size of the data
		UART_sendString("AT+CIPSEND=51\r\n");
		_delay_ms(40);
		// Temp set up
		tempOut = ((tempOut * 0.488));
		// AT data send command set up
		snprintf(TEMP, sizeof(TEMP), "GET /update?key=XUHJJ4KV38XFPTT7&field1=%2f", tempOut);
		// Send temperature data
		UART_sendString(TEMP);
		_delay_ms(20);
		UART_sendString("\r\n\r\n");
	}
}

//-----------------------------------------------------------------------------
//	*********Port Initialization function*********
//-----------------------------------------------------------------------------
void InitPort(){
	// Set pin C4 as an input pin
	DDRC |= (0<<PINC4);
	// Ensure pin C4 pull up resistor is off
	PORTC = (0<<PINC4);
	// Turn off Digital logic on pin C4
	DIDR0 |= (1<<ADC4D);
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
	// Run double speed
	UCSR0A |= (1<<U2X0);
}



//-----------------------------------------------------------------------------
//	*********USART Send function*********
//-----------------------------------------------------------------------------
void UART_sendString(char * AT)
{
	volatile unsigned char len= 0;
	volatile unsigned char i;
	while(AT[len] != 0)
	{
		len++;
	}
	for(i= 0x00; i< len; i++){
		// Wait for the transmitter to finish
		while(!(UCSR0A & (1<<UDRE0)));
		UDR0 = AT[i];
	}
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


void check_OK(void){
	uint8_t len = 0;
	char *returned_str = UART_ReciveString();
	while(returned_str[0] == '\0'){
		returned_str = UART_ReciveString();
	}
	len = strlen(returned_str);
	if(len > 3){ // error
		while(1);
		UART_sendString(returned_str);
		_delay_ms(1000);
		UART_sendString("AT\r\n");
		_delay_ms(1000);
	}
	if(len < 3){
		UART_sendString(returned_str);
		_delay_ms(1000);
	}
}

char * UART_ReciveString(void){
	char *str = (char *) (sizeof(char)*20);
	volatile uint8_t i = 0;
	do{
		// Wait for the transmitter to finish
		while(!(UCSR0A & (1<<UDRE0)));
		str[i] = UDR0;
		i++;
	} while(!RXC0);
	return str;
}
