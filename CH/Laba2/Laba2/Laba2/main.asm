.def TMP = R20
.def ONE = R21


.equ F_CPU = 8000000
.equ PRESC = 1024

.equ EE_MODE = 0
.equ EE_Y = 1
.equ EE_X = 2

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



;------------------------------- Начало ---------------------------
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
    LDI TMP, 0x01   ; Выставляем вывод
    OUT DDRA, TMP   ; Вывод
    OUT DDRB, TMP   ; Вывод
    SER TMP         ; Выставляем вход
    OUT DDRC, TMP   ; Ввод
    LDI TMP, 0x30   ; 00110000
    OUT DDRD, TMP   ; Выставляем кастомку
	SER TMP

    OUT PORTA, TMP ; Изначальный режим (0xFF - 0x00) на порту А 0xFF
    OUT PORTB, TMP  ; 0x00
    OUT PORTC, TMP  ; Зануляем, так как ждем ввод
    LDI TMP, 0x8C   ;10001100
    OUT PORTD, TMP  ; Выставляем кастомку

    ; Инициализация значений
    LDI TMP, 0
    STS phase, TMP  ; Выставляем состояние - 0 (А:00 - B:FF) 

    ; Режим работы
    LDI R24, EE_MODE ; Грузим адрес для режима из EEPROM
    RCALL EEPROM_READ; Запись из EEPROM
    STS mode, R24    ; Запись в переменную

    ; Частота X 
    LDI R24, EE_X   
    RCALL EEPROM_READ
    STS x_freq, R24	

    ; Значение Y
    LDI R24, EE_Y
    RCALL EEPROM_READ
    STS y_val, R24

    ; Начальные значения (Выставляем режим)

    LDS R16, mode   ; Грузим режим из mode в R16
    CPI R16, 0xFF   ; Сравниваем с 0xFF (Если mode == 0xFF, загружаем mode = 0 (0x00 - 0xFF)) Следующая инструкция скипается
    BRNE _check_x     ; Если mode != 0xFF, пропускаем и идем проверять X, Y
    CLR R16         
    STS mode, R16   ; Грузим начальный режим


; Проверка X
_check_x:
    LDS R16, x_freq ; Грузим в R16 частоту
    CPI R16, 0xFF   ; Сравниваем с 0xFF, если R16 == 0xFF, выставляем начальное X = 1 Гц
    BRNE _check_y   ; Если X != 0xFF, пропускаем и проверяем Y
    LDI R16, 0
    STS x_freq, R16 ; Грузим начальную частоту

; Проверка Y
_check_y:
    LDS R16, y_val ; Грузим в R16 Y
    CPI R16, 0xFF  ; Сравниваем с 0xFF, если R16 == 0xFF, выставляем Y = 0x55
    BRNE _set_phase; Иначе, пропускаем и ставим режим = 0
    LDI R16, 0x55   ; Начальное Y = 0x55
    STS y_val, R16  ; Записываем Y в переменную
; Выставление состояния
_set_phase:
    CLR R16 ;R16 = 0
    STS phase, R16 ; Фаза = 0
    RCALL UPDATE_PORTD ; Обновляем Port D
    RCALL APPLY_XCODE_TO_TIMER1 

    LDI R16, (1<<ISC01)|(1<<ISC11)
    OUT MCUCR, R16

    LDI R16, (1<<INT0)|(1<<INT1)
    OUT GICR, R16

    SEI 

; ------------- Основная логика ----------------
MAIN:
    IN R16, PIND    ; Считываем нажатие с PORT D в R16
    SBRS R16, PD7   ; Если PD7 не нажат, не вызываем READ_Y_FROM_PORTC
    RCALL READ_Y_FROM_PORTC ; Если PD7 нажат, читаем Y
    RJMP MAIN

; Читаем Y
READ_Y_FROM_PORTC:
    CLR R16
    OUT PORTA, R16 ; Чистим порты A B когда читаем Y (Если не сделаем, то святой А.С.Макаров по шапке надает)
    OUT PORTB, R16
    IN  R17, PINC ; Читаем PORT C
    STS y_val, R17 ; Записываем в y_val все то что Саня понажимал
    RET

; Обрабатываем прерывания (Таймер)
ISR_T1_COMPA:
    PUSH R16
    PUSH R17
    PUSH R18
	PUSH R19

    IN R16, PIND    ; Читаем PORTD
    SBRS R16, PD7   ; Нажата PD7, Cчитывается Y, не мешаем
    RJMP _exit_isr  ; Ливаем

    LDS R16, phase ; грузим состояние в R16
    EOR R16, ONE   ; XOR`им R16 ^ 1 (Короче говоря, инвертируем)
    STS phase, R16 ; Записываем в phase новое состояние

    LDS R18, mode  ; Грузим mode из R18
    TST R18        ; Сравниваем mode с 0, если не равен, то идем в mode1 иначе в mode0
    BRNE _mode1    ; Уходим

; Режим 0x00, 0xFF (R17 -> PORTA; R19 -> PORTB)
_mode0:
    LDS R16, phase ;Смотрим состояние
    TST R16        ;Если фаза == 0, то выводим 00-FF, иначе FF-00
    BREQ _out0F
    LDI R17, 0xFF  ;Вот тут ставим FF-00 в R17, R19
	LDI R19, 0x00
    RJMP _do_out   ;Мы все сделали, выводим
; Режим 0xFF, 0x00 (R17 -> PORTA; R19 -> PORTB)
_out0F:
    LDI R17, 0x00 ;Все тоже самое что и выше, но наоборот (Как с миллисекундами и секундами)
	LDI R19, 0xFF
    RJMP _do_out

; Режим y, -y   (R17 -> PORTA   R19 -> PORTB)
_mode1:
    LDS R16, phase      ; Также читаем фазу
    LDS R17, y_val		; Читаем значение У
	ANDI R17, 0x7F      ; Берем модулю
	MOV R19, R17        ; Копируем
    
    ORI R19, 0x80       ; Выставляем модуль
	TST R16             ; Если фаза == 0, выводим y ; -y иначе -y ; y
	BREQ _do_out
	MOV TMP, R19        ; Свап R17, R19
	MOV R19, R17
	MOV R17, TMP      

; Мы все выставили, отображаем то что навыставляли
_do_out:
    OUT PORTA, R17   ; R17 -> PORTA
    OUT PORTB, R19   ; R19 -> PORTB
    RCALL UPDATE_PORTD ; Обновляем PORTD
; Выходим из ISR, чистим стек
_exit_isr:
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
    LDS R16, mode ; Считывем режим
    EOR R16, ONE  ; Инвертируем
    STS mode, R16 ; Грузим в mode новый режим


    CLR R16       ; Чистим R16
    STS phase, R16; Грузим фазу (0) в R16
    RCALL UPDATE_PORTD; Обновляем PORTD

    ; Грузим режим в память
    LDS R25, mode
    LDI R24, EE_MODE    ; Грузим в EEPROM по адресу EE_MODE (0) из R24
    RCALL EEPROM_WRITE

    ; Грузим Х
    LDS R25, x_freq
    LDI R24, EE_X       ; Грузим в EEPROM по адресу EE_X (2) из R24
    RCALL EEPROM_WRITE

    ; Грузим Y
    LDS R25, y_val
    LDI R24, EE_Y       ; Грузим в EEPROM по адресу EE_Y (1) из R24
    RCALL EEPROM_WRITE

    POP R25
    POP R24
    POP R16
    RETI

; Прерывания на X

ISR_INT1:
    PUSH R16
    PUSH R24
    PUSH R25

    LDS R16, x_freq ;Читаем текущую частоту
    INC R16         ;Увеличиваем на 1
    CPI R16, 3      ;Берем по модую 3
    BRLO _x_ok      ;R16 < 3? Все хорошо, иначе R16 = 0
    CLR R16
_x_ok:
    STS x_freq, R16 ;Записываем R16 в x_freq
    RCALL APPLY_XCODE_TO_TIMER1; Применяем частоту
    RCALL UPDATE_PORTD         ; Обновляем PORTD

    LDS R25, x_freq     ; Записываем x_freq
    LDI R24, EE_X       ; Записываем адрес EEPROM для X
    RCALL EEPROM_WRITE  ; Все записали в регистры? Записываем в EEPROM

    POP R25
    POP R24
    POP R16
    RETI

UPDATE_PORTD:
    PUSH R16
    PUSH R17

    IN R16, PORTD  ; Считываем Порт D
    LDI R17, 0xCF   ; Сбрасываем PD4 PD5 (0b11001111)
    AND R16, R17    ; Побитовый AND R16, R17 (Чтобы во время нажатия не горели другие кнопки)


    LDS R17, mode   ; Грузим режим в R17
    TST R17         ; Проверяем что R17 == 0
    BREQ _no_mode   ; Если R17 == 0, mode не изменен
    ORI R16, (1<<PD4); R16 | 0001000  -> R16 - обновленный PORTD с измененным mode
_no_mode:
    LDS R17, phase ; Грузим состояние в R17
    TST R17        ; Проверяем что R17 == 0
    BREQ _no_phase ; Если R17 == 0, то фаза не изменена
    ORI R16, (1<<PD5); R16 | 0010000 -> R16 - обновленный PORTD с измененным phase
_no_phase:
    OUT PORTD, R16 ; Применяем изменения
    POP R17
    POP R16
    RET
; Подгоняем таймер под частоту X
APPLY_XCODE_TO_TIMER1:

    PUSH R16
    PUSH R17

    LDI R16, 0
    OUT TCCR1B, R16 ; Ставим для TCCR1B (Управление таймером) режим 0

    LDI R16, (1<<WGM12) ; Выставляем режим таймера WGM12 -> Вызов прерываний при достижении TCNT1 до OCR1AL
    OUT TCCR1B, R16     ; Выставили режим в TCCR1B

    LDS R16, x_freq     ; Загружаем частоту в R16
    CPI R16, 0          ; Если R16 == 0, Частота таймера - 0.25 Гц
    BREQ _ocr_025
    CPI R16, 1          ; R16 == 1, Частота - 0.5 Гц
    BREQ _ocr_05

; Выставление метки таймера для срабатывания (3 варианта которые зависят от частоты 1 Гц   0.5 Гц    0.25 Гц)
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
    OUT TCNT1H, R16                     ;
    OUT TCNT1L, R16                     ; Ставим метку таймера TCNT1 (Текущее значение таймера) на 0

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
        SBIC EECR, EEWE
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






