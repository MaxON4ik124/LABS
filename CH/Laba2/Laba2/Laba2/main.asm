.def TMP = R20
.def ONE = R21
.def Y = R22

.equ F_CPU = 8000000
.equ PRESC = 1024

.equ EE_MODE = 0
.equ EE_X = 1
.equ EE_Y = 2

.dseg
mode: .byte 1   ; PD4 (0/1)
phase: .byte 1  ; PD5 (0/1)
x_freq: .byte 1  ; PD3 (0/1/2)
y_val: .byte 1   ; y
.cseg


.org $000
RJMP reset

.org INT0addr
RJMP ISR_INT0

.org INT1addr
RJMP ISR_INT1

.org OC1Aaddr
RJMP ISR_T1_COMPA




reset:
    CLI
    CLR R1
	LDI TMP, 0x01
	LDI ONE, 0x01
	MOV R0, TMP
	CLR TMP

    ; Инициализируем Стек
    LDI TMP, HIGH(RAMEND) ; Старшие разряды адреса
	OUT SPH, TMP
	LDI TMP, LOW(RAMEND) ; Младшие разряды адреса
	OUT SPL, TMP

    ; Выставляем порты
/*    SER TMP
    OUT DDRA, TMP   ; Вывод
    OUT DDRB, TMP   ; Вывод
    LDI TMP, 0x01
    OUT DDRC, TMP   ; Ввод
	SER TMP
	OUT PORTC, TMP
    LDI TMP, (1<<PD0)|(1<<PD1)|(1<<PD4)|(1<<PD5)    ;0b00110011 
    OUT DDRD, TMP
    
    LDI TMP, (1<<PD2)|(1<<PD3)|(1<<PD7)		;0b10001100
    OUT PORTD, TMP
	LDI R16, 0x00
	OUT PORTA, R16
	LDI R16, 0xFF
	out PORTB, R16*/
	LDI Y, 0x55
	SER TMP
	OUT DDRA, TMP
	OUT DDRB, TMP
	OUT DDRD, TMP
	LDI TMP, 0x1
	OUT DDRC, TMP





    ; Режим работы

    LDI R24, EE_MODE
    RCALL EEPROM_READ
    STS mode, R24

    ; Частота X
    LDI R24, EE_X
    RCALL EEPROM_READ
    STS x_freq, R24	

    ; Y
    LDI R24, EE_Y
    RCALL EEPROM_READ
    STS y_val, R24

    ; Начальные значения (Выставляем режим)

    LDS R16, mode
    CPI R16, 0xFF
    BRNE _chk_x
    CLR R16
    STS mode, R16

; Выставляем X
_chk_x:
    LDS R16, x_freq
    CPI R16, 0xFF
    BRNE _chk_y
    LDI R16, 2
    STS x_freq, R16
; Выставляем Y
_chk_y:
    LDS R16, y_val
    CPI R16, 0xFF
    BRNE _set_phase
    LDI R16, 0x55
    STS y_val, R16
; Выставляем ссостояние
_set_phase:
    CLR R16
    STS phase, R16
    RCALL UPDATE_PORTD
    RCALL APPLY_XCODE_TO_TIMER1

    LDI R16, (1<<ISC01)|(1<<ISC11)
    OUT MCUCR, R16

    LDI R16, (1<<INT0)|(1<<INT1)
    OUT GICR, R16

    SEI


MAIN:
    ; Проверяем, нажата ли PD7, если нажата, ждем отжатия
    IN R16, PIND
    SBRS R16, PD7
    RCALL READ_Y_FROM_PORTC
    RJMP MAIN

READ_Y_FROM_PORTC:
    IN  R17, PINC
    STS y_val, R17
    RET

ISR_T1_COMPA:
    PUSH R16
    PUSH R17
    PUSH R18
	PUSH R19

    ;IN R16, PIND
    ;SBRS R16, PD7
    ;RJMP _only_inds

    ; Переключение состояний

    LDS R16, phase
    EOR R16, ONE
    STS phase, R16

    LDS R18, mode
    TST R18
    BRNE _mode1

_mode0:
    ; Режим 0x00, 0xFF
    LDS R16, phase
    TST R16
    BREQ _out00
    LDI R17, 0xFF
	LDI R19, 0x00
    RJMP _do_out
_out00:
    LDI R17, 0x00
	LDI R19, 0xFF
    RJMP _do_out

; Режим y, -y
_mode1:
    LDS R16, phase
    LDS R17, y_val		;y
	ANDI R17, 0x7F
	MOV R19, R17
    
    ORI R19, 0x80       ;-y
	TST R16
	BREQ _do_out
	MOV TMP, R19
	MOV R19, R17
	MOV R17, TMP      

_do_out:
	;COM R17
	;COM R19
    OUT PORTA, R17
    OUT PORTB, R19
    RCALL UPDATE_PORTD
	POP R19
    POP R18
    POP R17
    POP R16
    RETI

; Прерывания при смене режима
ISR_INT0:
    PUSH R16
    PUSH R24
    PUSH R25
    LDS R16, mode
    EOR R16, ONE
    STS mode, R16

    ; Сброс состояния при смене режима

    CLR R16
    STS phase, R16
    RCALL UPDATE_PORTD

    ; Грузим режим в память
    LDS R25, mode
    LDI R24, EE_MODE
    RCALL EEPROM_WRITE

    ; Грузим Х
    LDS R25, x_freq
    LDI R24, EE_X
    RCALL EEPROM_WRITE

    ; Грузим Y
    LDS R25, y_val
    LDI R24, EE_Y
    RCALL EEPROM_WRITE

    POP R25
    POP R24
    POP R16
    RETI

; Вторые прерывания на x

ISR_INT1:
    PUSH R16
    PUSH R24
    PUSH R25

    LDS R16, x_freq
    INC R16
    CPI R16, 3
    BRLO _x_ok
    CLR R16
_x_ok:
    STS x_freq, R16
    RCALL APPLY_XCODE_TO_TIMER1
    RCALL UPDATE_PORTD

    LDS R25, x_freq
    LDI R24, EE_X
    RCALL EEPROM_WRITE

    POP R25
    POP R24
    POP R16
    RETI

UPDATE_PORTD:
    PUSH R16
    PUSH R17

    IN R16, PORTD
    LDI R17, ~((1<<PD0)|(1<<PD1)|(1<<PD4)|(1<<PD5))
    AND R16, R17

    ; Выставляем режим

    LDS R17, mode
    TST R17
    BREQ _no_modebit
    ORI R16, (1<<PD4)
_no_modebit:
    ; Выставляем состояние
    LDS R17, phase
    TST R17
    BREQ _no_phasebit
    ORI R16, (1<<PD5)
_no_phasebit:
    ; Меняем Частоту
    LDS R17, x_freq
    ANDI R17, 0x03
    OR R16, R17

    OUT PORTD, R16

    POP R17
    POP R16
    RET
APPLY_XCODE_TO_TIMER1:
; Выставляем частоту
    PUSH R16
    PUSH R17

    LDI R16, 0
    OUT TCCR1B, R16

    LDI R16, (1<<WGM12)
    OUT TCCR1B, R16

    LDS R16, x_freq
    CPI R16, 0
    BREQ _ocr_025
    CPI R16, 1
    BREQ _ocr_05
_ocr_1:
    LDI R16, LOW(3905)
    OUT OCR1AL, R16
    LDI R16, HIGH(3905)
    OUT OCR1AH, R16
    RJMP _timer_start
_ocr_05:
    LDI R16, LOW(7811)
    OUT OCR1AL, R16
    LDI R16, HIGH(7811)
    OUT OCR1AH, R16
    RJMP _timer_start
_ocr_025:
    LDI R16, LOW(15624)
    OUT OCR1AL, R16
    LDI R16, HIGH(15624)
    OUT OCR1AH, R16

_timer_start:
    CLR R16
    OUT TCNT1H, R16
    OUT TCNT1L, R16

    LDI R16, (1<<OCIE1A)
    OUT TIMSK, R16

    IN R16, TCCR1B
    ORI R16, (1<<CS12)|(1<<CS10)
    OUT TCCR1B, R16

    POP R17
    POP R16
    ret

EEPROM_READ:
    PUSH R16

_wait_er:
    sbic EECR, EEWE
    RJMP _wait_er

    OUT EEARL, R24
    SBI EECR, EERE
    IN R24, EEDR

    POP R16
    RET

EEPROM_WRITE:
    PUSH R16

_wait_ew:
    SBIC EECR, EEWE
    RJMP _wait_ew
    OUT EEARL, R24

    OUT EEDR, R25

    SBI EECR, EEMWE
    SBI EECR, EEWE

    POP R16
    RET






