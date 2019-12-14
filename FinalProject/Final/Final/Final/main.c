/*
 * Final.c
 *
 * Created: 12/13/2019 10:22:35 AM
 * Author : Dilbert
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define USART_BAUDRATE 9600
#define UBRR_VALUE (int)round((((F_CPU/(USART_BAUDRATE*8UL)))-1))

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "i2cmaster.h"
#define Si7021 (0x40<<1)     // (0x68 << 1) I2C slave address
#define APDS9960_WRITE 0x72
#define APDS9960_READ 0x73

void USART0Init(void);
int USART0SendByte(char u8Data,FILE *stream);
void check_OK(void);
void UART_sendString(char * AT);
char * UART_ReciveString(void);
void InitPort();
// Set Stream Pointer
FILE usart0_str = FDEV_SETUP_STREAM(USART0SendByte, NULL, _FDEV_SETUP_WRITE);

char buffer[200];
unsigned char ret;        // return value
volatile float Temerature;
volatile float Humididty;
volatile float Clear;
volatile float Red;
volatile float Green;
volatile float Blue;
uint16_t raw;             // raw sensor value
uint16_t HT_Data, CHT_Data;          // Humidity raw value
uint16_t TMP_Data, CTMP_Data;          // Temperature raw value
uint16_t PTMP_Data, CPTMP_Data;          // Temperature raw value
int16_t raw_R = 0;
int16_t raw_G = 0;
int16_t raw_B = 0;
int16_t raw_C = 0;
char TEMP[50];

void Si7021_writereg(uint8_t reg, uint8_t val)
{
	i2c_start(Si7021+I2C_WRITE);
	i2c_write(reg);  // go to register e.g. 106 user control
	i2c_write(val);  // set value e.g. to 0100 0000 FIFO enable
	i2c_stop();      // set stop condition = release bus
}

uint16_t Si7021_readreg(uint8_t reg)
{
	i2c_start_wait(Si7021+I2C_WRITE);  // set device address and write mode
	i2c_write(reg);                     // ACCEL_XOUT
	i2c_write(0x81);                     // ACCEL_XOUT
	i2c_rep_start(Si7021+I2C_READ);    // set device address and read mode
	raw = i2c_readAck();                // read one intermediate byte
	raw = (raw<<8) | i2c_readNak();     // read last byte
	i2c_stop();
	return raw;
}

void Init_Si7021()
{
	i2c_init();     // init I2C interface
	_delay_ms(200);  // Wait for 200 ms.

	i2c_start(Si7021+I2C_WRITE);       // set device address and write mode
	if ( ret )
	{
		/* failed to issue start condition, possibly no device found */
		i2c_stop();
		//red_led(1);    // Turn on the red LED immediately to signal the read is not ok
		while(1) {;;}  // lock program here as sensor init failed
	}
	else
	{
	}
}

void init_APDS9960(void){
	_delay_ms(150);										/* Power up time >100ms */
	i2c_start(APDS9960_WRITE);
	i2c_write(0x80); // set pointer to ENABLE REGISTER
	i2c_write(0x03); // write 0x06 to ENABLE REGISTER
	i2c_stop();

	i2c_start(APDS9960_WRITE);
	i2c_write(0x81); // set pointer to ALS ADC Integration Time
	i2c_write(0xDB); // 219 =0XDB = 37 = 103 ms = 37889
	i2c_stop();

	i2c_start(APDS9960_WRITE);
	i2c_write(0x8F); // set pointer to CONTROL<AGAIN>
	i2c_write(0x01); // ALS and Color Gain Control = 0 = x1
	i2c_stop();
}

void getreading(void){

	i2c_start(APDS9960_WRITE);
	i2c_write(0x94); // set pointer to CDATA
	i2c_stop();

	i2c_start(APDS9960_READ);

	raw_C =  ((uint8_t)i2c_readAck());
	raw_C |= ((uint8_t)i2c_readAck())<<8;

	raw_R = ((uint8_t)i2c_readAck());
	raw_R |= ((uint8_t)i2c_readAck())<<8;

	raw_G = ((uint8_t)i2c_readAck());
	raw_G |= ((uint8_t)i2c_readAck())<<8;

	raw_B = ((uint8_t)i2c_readAck());
	raw_B |= ((uint8_t)i2c_readNak())<<8;

	i2c_stop();

}

int main(void){
	char buffer[200];///, float_[15];
	// Initialize USART0
	USART0Init();
	// Initialize ports
	////InitPort();
	// Assign our stream to standard I/O streams
	stdout=&usart0_str;
	Init_Si7021();    // Sensor init
	init_APDS9960();
	_delay_ms(200);     // Wait for 200 ms.
	sei();
	UART_sendString("AT\r\n");
	_delay_ms(20);
	// Check if ok returned
	//check_OK();
	_delay_ms(20);
	// Select WIFI mode
	UART_sendString("AT+CWMODE=1\r\n");
	_delay_ms(20);
	// Connect to local WIFI (REPLACE "SSID" and "PASSWORD" with your ssid and password
	UART_sendString("AT+CWJAP=\"SSID\",\"PASSWORD\"\r\n");
	_delay_ms(40);
	// Enable connection
	UART_sendString("AT+CIPMUX=0\r\n");
	_delay_ms(40);
	while(1){
		
		TMP_Data = Si7021_readreg(0xE3);   // read ch0 LSB byte
		HT_Data = Si7021_readreg(0xE5);   // read ch0 LSB byte
		PTMP_Data = Si7021_readreg(0xE0);   // read ch0 LSB byte
		Humididty = 125.0f * HT_Data / 65536.0f - 6.0f;
		Temerature = 175.25f * TMP_Data / 65536.0f - 46.85f;
		CPTMP_Data = 175.25f * PTMP_Data / 65536.0f - 46.85f;
		
		getreading();
		Clear = raw_C;
		Red = raw_R;
		Green = raw_G;
		Blue = raw_B;
		_delay_ms(40);
		// Start a connection as client to Thingspeak
		UART_sendString("AT+CIPSTART=\"TCP\",\"184.106.153.149\",80\r\n");//"AT+CIPSTART=\"TCP\",\"184.106.153.149\",80\r\n"
		_delay_ms(40);
		// Specify the size of the data
		UART_sendString("AT+CIPSEND=200\r\n");
		_delay_ms(40);
		// AT data send command set up
		snprintf(buffer, sizeof(buffer), "GET /update?key=LAC1YHQZJQKO3DHB&field1=%2f&field2=%2f&field3=%2f&field4=%2f&field5=%2f&field6=%2f", Temerature, Humididty, Clear, Red, Green, Blue);
		// Send temperature data
		UART_sendString(buffer);
		_delay_ms(20);
		UART_sendString("\r\n\r\n");
		// 15 Second Delay between displaying temp
		_delay_ms(15000);
	}
	
	return 0;
}

//-----------------------------------------------------------------------------
//	*********Port Initialization function*********
//-----------------------------------------------------------------------------
void InitPort(){

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
	//uint8_t len = 0;
	char *returned_str = UART_ReciveString();
	while(returned_str[0] == '\0'){
		returned_str = UART_ReciveString();
	//}
	//len = strlen(returned_str);
	//if(len > 3){ // error
	//	while(1);
	//	UART_sendString(returned_str);
	//	_delay_ms(1000);
	//	UART_sendString("AT\r\n");
	//	_delay_ms(1000);
	//}
	//if(len < 3){
	//	UART_sendString(returned_str);
	//	_delay_ms(1000);
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