.include "m32def.inc"

.def TMP     = R16
.def R_MODE  = R17
.def R_VAL1  = R18
.def R_VAL2  = R19
.def EE_ADDR = R24
.def EE_DATA = R25
.def R_BLANK = R23

.equ EE_MODE = 0
.equ EE_X    = 1
.equ EE_Y    = 2

.dseg
mode:   .byte 1
phase:  .byte 1
x_freq: .byte 1
y_val:  .byte 1

.cseg

.org 0x0000
    rjmp reset

.org INT0addr
    rjmp ISR_INT0

.org INT1addr
    rjmp ISR_INT1

.org OC1Aaddr
    rjmp ISR_T1_COMPA


reset:
    cli
    clr r1

    ; стек
    ldi TMP, HIGH(RAMEND)
    out SPH, TMP
    ldi TMP, LOW(RAMEND)
    out SPL, TMP

    ; PORTA, PORTB - выходы
    ser TMP
    out DDRA, TMP
    out DDRB, TMP

    ; PORTC - вход
    clr TMP
    out DDRC, TMP

    ; PORTD:
    ; PD0, PD1, PD4, PD5 - output
    ; PD2, PD3, PD7 - input
    ldi TMP, (1<<PD0)|(1<<PD1)|(1<<PD4)|(1<<PD5)
    out DDRD, TMP

    ; подтяжки выключены
    clr TMP
    out PORTA, TMP
    out PORTB, TMP
    out PORTC, TMP
    out PORTD, TMP

    ; начальные значения
    clr TMP
    sts mode, TMP

    clr TMP
    sts phase, TMP

    ldi TMP, 0x55
    sts y_val, TMP

    ; x читаем из EEPROM
    ldi EE_ADDR, EE_X
    rcall EEPROM_READ
    mov TMP, EE_ADDR
    cpi TMP, 3
    brlo X_OK
    clr TMP
X_OK:
    sts x_freq, TMP

    ; INT0/INT1 по фронту 0->1
    ldi TMP, (1<<ISC01)|(1<<ISC00)|(1<<ISC11)|(1<<ISC10)
    out MCUCR, TMP

    ; сброс флагов
    ldi TMP, (1<<INTF0)|(1<<INTF1)
    out GIFR, TMP

    ; разрешить INT0, INT1
    ldi TMP, (1<<INT0)|(1<<INT1)
    out GICR, TMP

    ; таймер
    rcall APPLY_X_TO_TIMER1

    ; начальный вывод
    rcall APPLY_OUTPUT

    sei

MAIN:
    ; если PD7 = 1 -> режим ввода y
    sbic PIND, PD7
    rcall READ_Y_FROM_PORTC

    rjmp MAIN


; ============================================================
; Чтение Y
; ============================================================
READ_Y_FROM_PORTC:
    push TMP
    push R_VAL1

    ; гасим PORTA/PORTB
    clr TMP
    out PORTA, TMP
    out PORTB, TMP
    rcall UPDATE_PORTD

READ_LOOP:
    ; если PD7 отпущена -> выйти
    sbic PIND, PD7
    rjmp PD7_STILL_PRESSED
    rjmp READ_DONE

PD7_STILL_PRESSED:
    in TMP, PINC

    ; если все кнопки PORTC отпущены -> выходим из чтения
    tst TMP
    breq WAIT_PD7_RELEASE

    ; сохраняем y
    sts y_val, TMP
    rjmp READ_LOOP

WAIT_PD7_RELEASE:
    ; ждем отпускания PD7, чтобы MAIN не вошел сюда снова сразу
    sbic PIND, PD7
    rjmp WAIT_PD7_RELEASE

READ_DONE:
    ; сохранить y в EEPROM
    lds EE_DATA, y_val
    ldi EE_ADDR, EE_Y
    rcall EEPROM_WRITE

    ; восстановить вывод
    rcall APPLY_OUTPUT

    pop R_VAL1
    pop TMP
    ret


; ============================================================
; TIMER1 COMPA
; ============================================================
ISR_T1_COMPA:
    push TMP
    in TMP, SREG
    push TMP
    push R_MODE

    ; если PD7 нажата -> не трогаем вывод
    sbic PIND, PD7
    rjmp T1_EXIT

    ; phase ^= 1
    lds TMP, phase
    ldi R_MODE, 1
    eor TMP, R_MODE
    sts phase, TMP

    rcall APPLY_OUTPUT

T1_EXIT:
    pop R_MODE
    pop TMP
    out SREG, TMP
    pop TMP
    reti


; ============================================================
; INT0 - смена режима
; ============================================================
ISR_INT0:
    push TMP
    in TMP, SREG
    push TMP
    push R_MODE

    lds R_MODE, mode
    ldi TMP, 1
    eor R_MODE, TMP
    sts mode, R_MODE

    rcall APPLY_OUTPUT

    lds EE_DATA, mode
    ldi EE_ADDR, EE_MODE
    rcall EEPROM_WRITE

WAIT_PD2_RELEASE:
    sbic PIND, PD2
    rjmp WAIT_PD2_RELEASE

    pop R_MODE
    pop TMP
    out SREG, TMP
    pop TMP
    reti


; ============================================================
; INT1 - смена частоты
; 0 -> 1 -> 2 -> 0
; ============================================================
ISR_INT1:
    push TMP
    in TMP, SREG
    push TMP
    push R_MODE

    ; x_freq = (x_freq + 1) mod 3
    lds R_MODE, x_freq
    inc R_MODE
    cpi R_MODE, 3
    brlo X_OK_ISR
    clr R_MODE
X_OK_ISR:
    sts x_freq, R_MODE

    ; применить новую частоту таймера
    rcall APPLY_X_TO_TIMER1

    ; сразу обновить индикацию на PD0-PD1, PD4, PD5
    rcall UPDATE_PORTD

    ; сохранить x в EEPROM
    lds EE_DATA, x_freq
    ldi EE_ADDR, EE_X
    rcall EEPROM_WRITE

WAIT_PD3_RELEASE:
    ; ждем, пока кнопка PD3 будет отпущена
    sbic PIND, PD3
    rjmp WAIT_PD3_RELEASE

    pop R_MODE
    pop TMP
    out SREG, TMP
    pop TMP
    reti



; ============================================================
; Вывод на A/B
; mode=0:
;   phase=0 -> FF,FF
;   phase=1 -> 00,00
; mode=1:
;   phase=0 -> y,y
;   phase=1 -> -y*,-y*
; ============================================================
APPLY_OUTPUT:
    push TMP
    push R_MODE
    push R_VAL1
    push R_VAL2

    lds R_MODE, mode
    tst R_MODE
    brne MODE_1

MODE_0:
    lds TMP, phase
    tst TMP
    brne MODE0_STATE1

    ldi R_VAL1, 0xFF
    ldi R_VAL2, 0xFF
    rjmp WRITE_OUTPUT

MODE0_STATE1:
    clr R_VAL1
    clr R_VAL2
    rjmp WRITE_OUTPUT

MODE_1:
    lds TMP, phase
    tst TMP
    brne MODE1_STATE1

    lds R_VAL1, y_val
    mov R_VAL2, R_VAL1
    rjmp WRITE_OUTPUT

MODE1_STATE1:
    lds R_VAL1, y_val
    ;andi R_VAL1, 0x7F
    ;mov R_VAL2, R_VAL1
    ;ori R_VAL2, 0x80
    ;mov R_VAL1, R_VAL2
	ldi R_BLANK, 0xFF
	eor R_VAL1, R_BLANK
	mov R_VAL2, R_VAL1

WRITE_OUTPUT:
    out PORTA, R_VAL1
    out PORTB, R_VAL2
    rcall UPDATE_PORTD

    pop R_VAL2
    pop R_VAL1
    pop R_MODE
    pop TMP
    ret


; ============================================================
; UPDATE_PORTD
; PD0-PD1 = x_freq
; PD4     = mode
; PD5     = phase
; ============================================================
UPDATE_PORTD:
    push TMP
    push R_MODE

    clr TMP

    ; x_freq -> PD0..PD1
    lds R_MODE, x_freq
    cpi R_MODE, 0
    breq X_BITS_DONE          ; 00

    cpi R_MODE, 1
    breq X_IS_1

    ; x_freq = 2 -> 10
    ori TMP, (1<<PD1)
    rjmp X_BITS_DONE

X_IS_1:
    ; x_freq = 1 -> 01
    ori TMP, (1<<PD0)

X_BITS_DONE:
    ; mode -> PD4
    lds R_MODE, mode
    tst R_MODE
    breq NO_MODE_BIT
    ori TMP, (1<<PD4)
NO_MODE_BIT:

    ; phase -> PD5
    lds R_MODE, phase
    tst R_MODE
    breq NO_PHASE_BIT
    ori TMP, (1<<PD5)
NO_PHASE_BIT:

    out PORTD, TMP

    pop R_MODE
    pop TMP
    ret
; ============================================================
; Таймер1 CTC
; x_freq:
; 0 -> 0.25 Hz
; 1 -> 0.5  Hz
; 2 -> 1 Hz
; ============================================================
APPLY_X_TO_TIMER1:
    push TMP
    push R_MODE

    ; стоп таймера
    clr TMP
    out TCCR1A, TMP
    out TCCR1B, TMP

    ; сброс счетчика
    out TCNT1H, TMP
    out TCNT1L, TMP

    lds TMP, x_freq
    cpi TMP, 0
    breq SET_025HZ
    cpi TMP, 1
    breq SET_05HZ

SET_1HZ:
    ldi R_MODE, HIGH(3905)
    out OCR1AH, R_MODE
    ldi R_MODE, LOW(3905)
    out OCR1AL, R_MODE
    rjmp TIMER1_START

SET_05HZ:
    ldi R_MODE, HIGH(7811)
    out OCR1AH, R_MODE
    ldi R_MODE, LOW(7811)
    out OCR1AL, R_MODE
    rjmp TIMER1_START

SET_025HZ:
    ldi R_MODE, HIGH(15624)
    out OCR1AH, R_MODE
    ldi R_MODE, LOW(15624)
    out OCR1AL, R_MODE

TIMER1_START:
    ldi TMP, (1<<OCF1A)
    out TIFR, TMP

    ldi TMP, (1<<OCIE1A)
    out TIMSK, TMP

    ldi TMP, (1<<WGM12)|(1<<CS12)|(1<<CS10)
    out TCCR1B, TMP

    pop R_MODE
    pop TMP
    ret


; ============================================================
; EEPROM READ
; ============================================================
EEPROM_READ:
    push TMP

EE_R_WAIT:
    sbic EECR, EEWE
    rjmp EE_R_WAIT

    clr TMP
    out EEARH, TMP
    out EEARL, EE_ADDR

    sbi EECR, EERE
    in EE_ADDR, EEDR

    pop TMP
    ret


; ============================================================
; EEPROM WRITE
; ============================================================
EEPROM_WRITE:
    push TMP
    in TMP, SREG
    push TMP

    cli

EE_W_WAIT:
    sbic EECR, EEWE
    rjmp EE_W_WAIT

    clr TMP
    out EEARH, TMP
    out EEARL, EE_ADDR
    out EEDR, EE_DATA

    sbi EECR, EEMWE
    sbi EECR, EEWE

    pop TMP
    out SREG, TMP
    pop TMP
    ret