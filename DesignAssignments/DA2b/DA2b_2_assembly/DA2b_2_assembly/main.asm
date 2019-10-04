;
; DA2b_2_assembly.asm
;
; Created: 10/2/2019 6:20:09 PM
; Author : Dilbert
;
.org 0x00					; Restart interupt insertion location
	jmp main				;	Jump to main on restart
.org 0x02					; INT0 Interupt insertion location
	jmp ext_int				;	Jump to Interupt Service Request Function
main:						; Main prog function	
	LDI R20, HIGH(RAMEND)	;	*Initalize stack*
    OUT SPH, R20			;	*Initalize stack*
	LDI R20, LOW(RAMEND)	;	*Initalize stack*
	OUT SPL, R20			;	*Initalize stack*
	SBI DDRB, 2				;	Set PORTB.2 as an output
	CBI DDRD, 2				;	Set PORTD.2 as an input
	SBI PORTD, 2			;	Set PORTD.2 as active Low(enable pull up resistor)
	LDI R20,1<<INT0			;	Load the INT0 Interupt pin location into R20
	OUT EIMSK,R20			;	Activate the INT0 interupt enable
	SEI						;	Activate the Global interupt enable
start:						; Start infinite loop
	rjmp start				;	Go to infinite loop start location
    rjmp main				; Back up jump, in case of skip (unnessesary)
;****************************End of main prog**********************************
;------------------------------------------------------------------------------
;****************************INT0 Interupt Service Request Function************
ext_int:					; ISR label
	LDI R20, 1<<INTF0		;	Set Interupt Flag location on R20
	OUT EIFR, R20			;	Set Interupt Flag
	IN R21, PORTB			;	load value of PORTB
	LDI R22,0X04			;	Load 0x04 into R22 = pin 2
	EOR R21,R22				;	EXOR PORTB.2 and 0000'0100; Load result into R21
	OUT PORTB,R21			;	Output the result in R21 to PORTB = PORTB.2(HIGH)
	CALL delay				;	Call Delay function
	CBI PORTB,2				;	Clear PORTB.2
	RETI					;	RETURN TO INSTRUCTION AFTER INTERUPT CALL
;****************************End Interupt Service Request Function*************
;------------------------------------------------------------------------------
;****************************Begin of Delay Function***************************
delay:
	ldi r23, 0x19			;	Load R23 with 25
d2:							; Loop d2
	ldi r24, 0xFF			;	Load R22 with 255
d1:							; Loop d1
	ldi r25, 0xFF			;	Load R21 with 255
d0:							; Loop d0
	nop						;	No operation
	nop						;	No operation
	nop						;	No operation
	nop						;	No operation
	nop						;	No operation
	nop						;	No operation
	nop						;	No operation
	nop						;	No operation
	nop						;	No operation
	nop						;	No operation
	dec r25					;	Decrement R25
	brne d0					;	Branch to d0 if R25 is not zero
	dec r24					;	Decrement R24
	brne d1					;	Branch to d1 if R24 is not zero
	dec r23					;	Decrement R23
	brne d2					;	Branch to d0
	ret						;	Return to Main Prog