;
; DA2A_1_assemble.asm
;
; Created: 9/29/2019 8:16:28 PM
; Author : Dilbert
;
.org 0x00				; Start of program at reset interupt
	ldi r16, 0x08		;	Load register R16 with 0b'00001000
	ldi r17, 0x00		;	Load register R17 with 0x00
	out ddrb, r16		;	Set Port B Pin 3 as an output
	out portb, r17		;	Set all of Port B to Low
start:					; Start Location after each period
    ldi r18, 0x04		;	Load register R18 with 0x04 (This is the duty cycle percent) e.g. 40%
	ldi r19, 0x06		;	Load register R19 with 0x06 (This is the off time of the period) e.g. 60%
	out portb, r16		;	set Port B Pin 3 High
loop_one:				; Start the Loop for the duty cycle on time
	call delay			;	Call the Delay function, the delay runs for one tenth of the total period.
	dec r18				;	Decrement R18 
	brne loop_one		;	If R18 is not zero go back to Loop_one:
	out portb, r17		;	Set all of Port B to Low
loop_two:				; Start the Loop for the off time of the duty cycle
	call delay			;	Call the Delay function, the delay runs for one tenth of the total period.
	dec r19				;	Decrement R19
	brne loop_two		;	If R19 is not zero go back to Loop_two:
	rjmp start			;****End Of Main Prog***** Jump back up to start
;-----------------------------------------------------------------------------------------------------	
delay:					;************Begin Of Delay Function**************
	ldi r23, 0x0A		;	Load R23 With 10
d2:						; Loop d2
	ldi r24, 0x64		;	Load R22 With 100
d1:						; Loop d1
	ldi r25, 0x64		;	Load R21 With 100
d0:						; Loop d0
	nop					;	No operation
	nop					;	No operation
	nop					;	No operation
	nop					;	No operation
	nop					;	No operation
	nop					;	No operation
	nop					;	No operation
	dec r25				;	Decrement R25
	brne d0				;	Branch to d0 if R25 is not 0
	dec r24				;	Decrement R24
	brne d1				;	Branch to d1 if R24 is not 0
	dec r23				;	Decrement R23
	brne d2				;	Branch to d0 if R23 is not 0
	ret					;	Return to calling function
    rjmp start			;	Unused but would return to beginging of the cycle if ret was skiped for some reason
