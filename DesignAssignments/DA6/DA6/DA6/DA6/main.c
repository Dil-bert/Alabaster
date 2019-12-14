/*
 * DA6.c
 *
 * Created: 12/11/2019 6:29:29 PM
 * Author : Dilbert
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h>										/* Include standard library file */
#include <stdio.h>										/* Include standard library file */
#include <inttypes.h>
#include "MPU6050_def.h"							/* Include MPU6050 register define file */
#include "i2c_master.h"							/* Include I2C Master header file */
#include "uart.h"							/* Include USART header file */

#define MPU6050_WRITE 0xD0
#define MPU6050_READ 0xD1

float Acc_x,Acc_y,Acc_z,Gyro_x,Gyro_y,Gyro_z;

void init_uart(uint16_t baudrate){

	uint16_t UBRR_val = (F_CPU/16)/(baudrate-1);

	UBRR0H = UBRR_val >> 8;
	UBRR0L = UBRR_val;

	UCSR0B |= (1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0); // UART TX (Transmit - senden) einschalten
	UCSR0C |= (1<<USBS0) | (3<<UCSZ00); //Modus Asynchron 8N1 (8 Datenbits, No Parity, 1 Stopbit)
}

void uart_putc(unsigned char c){

	while(!(UCSR0A & (1<<UDRE0))); // wait until sending is possible
	UDR0 = c; // output character saved in c
}

void uart_puts(char *s){
	while(*s){
		uart_putc(*s);
		s++;
	}
}

void init_MPU6050()/* Gyro initialization function */
{
	_delay_ms(150);/* Power up time >100ms */
	I2C_Start_Wait(0xD0);/* Start with device write address */
	I2C_Write(SMPLRT_DIV);/* Write to sample rate register */
	I2C_Write(0x07);/* 1KHz sample rate */
	I2C_Stop();
	I2C_Start_Wait(0xD0);
	I2C_Write(PWR_MGMT_1);/* Write to power management register */
	I2C_Write(0x01);/* X axis gyroscope reference frequency */
	I2C_Stop();
	I2C_Start_Wait(0xD0);
	I2C_Write(CONFIG);/* Write to Configuration register */
	I2C_Write(0x00);/* Fs = 8KHz */
	I2C_Stop();
	I2C_Start_Wait(0xD0);
	I2C_Write(GYRO_CONFIG);/* Write to Gyro configuration register */
	I2C_Write(0x18);/* Full scale range +/-2000 degree/C */
	I2C_Stop();
	I2C_Start_Wait(0xD0);
	I2C_Write(INT_ENABLE);/* Write to interrupt enable register */
	I2C_Write(0x01);
	I2C_Stop();
	}
	
void MPU_Start_Loc(){
	I2C_Start_Wait(0xD0);/* I2C start with device write address */
	I2C_Write(ACCEL_XOUT_H);/* Write start location address from where to read */
	I2C_Repeated_Start(0xD1);/* I2C start with device read address */
	}
		
void Read_RawValue(void){
	_delay_ms(10);
	MPU_Start_Loc();/* Read Gyro values */
	Acc_x=(((int)I2C_Read_Ack()<<8)|(int)I2C_Read_Ack());//.. Read other registers
	Acc_y=(((int)I2C_Read_Ack()<<8)|(int)I2C_Read_Ack());
	Acc_z=(((int)I2C_Read_Ack()<<8)|(int)I2C_Read_Ack());
	Gyro_x=(((int)I2C_Read_Ack()<<8)|(int)I2C_Read_Ack());
	Gyro_y=(((int)I2C_Read_Ack()<<8)|(int)I2C_Read_Ack());
	Gyro_z=(((int)I2C_Read_Ack()<<8)|(int)I2C_Read_Nack());
	I2C_Stop();
	}


int main(void){
	char buffer[20], float_[10];
	float Xa,Ya,Za,Xg,Yg,Zg;
	
	I2C_Init();
	init_MPU6050();
	init_uart(9600);
	_delay_ms(150);
	while(1){
		_delay_ms(150);
		Read_RawValue();
		Xa = Acc_x/16384.0;								/* Divide raw value by sensitivity scale factor to get real values */
		Ya = Acc_y/16384.0;	
		Za = Acc_z/16384.0;	
		Xg = Gyro_x/16.4;	
		Yg = Gyro_y/16.4;	
		Zg = Gyro_z/16.4;

		dtostrf( Xa, 3, 2, float_ );					/* Take values in buffer to send all parameters over USART */
		sprintf(buffer,"%s, Xa \n ",float_);
		USART_SendString(buffer);

		dtostrf( Ya, 3, 2, float_ );					/* Take values in buffer to send all parameters over USART */
		sprintf(buffer,"%s, Ya \n",float_);
		USART_SendString(buffer);
		
		dtostrf( Za, 3, 2, float_ );					/* Take values in buffer to send all parameters over USART */
		sprintf(buffer,"%s, Za \n",float_);
		USART_SendString(buffer);
		
		dtostrf( Xg, 3, 2, float_ );					/* Take values in buffer to send all parameters over USART */
		sprintf(buffer,"%s, Xg \n",float_);
		USART_SendString(buffer);
		
		dtostrf( Yg, 3, 2, float_ );					/* Take values in buffer to send all parameters over USART */
		sprintf(buffer,"%s, Yg \n",float_);
		USART_SendString(buffer);
		
		dtostrf( Zg, 3, 2, float_ );					/* Take values in buffer to send all parameters over USART */
		sprintf(buffer,"%s, Zg \n",float_);
		USART_SendString(buffer);
		
		_delay_ms(1000);
	}
	
	return 0;
}
