;
; DA2A_2_assemble.asm
;
; Created: 9/28/2019 9:31:49 PM
; Author : Dilbert
;

#define F_CPU 16000000UL;	Tell compiler CPU is running at 16MHz (un-needed for assembly?) 
.org 0x0				;	Start of program on reset interupt
	ldi r16,   0x04		;   Load R16 with 0b'00000100
	ldi r17,   0x00		;   Load R17 with 0x00
	ldi r18,   0x08		;	Load R18 with 0b'00001000
	out ddrb,  r16		;	Sets Port B Pin 2 as an output
	out portb, r17		;	Sets Port B Low / Turns off any pull up's
	out ddrc,  r17		;	Sets Port C to be an input, including c.3
	out portc, r18		;	Sets Port C Pin 3's Pull up resistor High
start:					; Start of prog runtime
    in r19, pinc		;	Read Port C into Register R19
	sbrs r19, 3			;	If Register R19's Pin 3 is High(Button not pushed) Skip
	rjmp led			;	Jump to LED Lable If Button pushed(Pin 3 Low)
	rjmp start			;	Jump to Start Lable
led:					; Start of LED Run
	out portb, r16		;	Set Port B pin 2 High
	call delay			;	Call the Delay function (1.3333 Seconds)
	out portb, r17		;	Set all of Port B low
	rjmp start			;************End Of Prog Main************
;------------------------------------------------------------------
delay:					;********Begin of Delay Function************
	ldi r23, 0x19		;	Load R23 with 25
d2:						; Loop d2
	ldi r24, 0xFF		;	Load R22 with 255
d1:						; Loop d1
	ldi r25, 0xFF		;	Load R21 with 255
d0:						; Loop d0
	nop					;	No operation
	nop					;	No operation
	nop					;	No operation
	nop					;	No operation
	nop					;	No operation
	nop					;	No operation
	nop					;	No operation
	nop					;	No operation
	nop					;	No operation
	nop					;	No operation
	dec r25				;	Decrement R25
	brne d0				;	Branch to d0 if R25 is not zero
	dec r24				;	Decrement R24
	brne d1				;	Branch to d1 if R24 is not zero
	dec r23				;	Decrement R23
	brne d2				;	Branch to d0
	ret					;	Return to Main Prog
