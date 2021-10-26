; директива для работы с расширенным набором команд
.386p

; структура для описания дескрипторов сегментов
descr struc
    limit    dw 0 ; номер последнего байта сегмента
    base_l   dw 0 ; младшее слово базы сегмента
    base_m   db 0 ; средний байт базы сегмента
    attr_1   db 0 ; 0-3 тип, 4 сист/память, 5-6 DPL, 7 P
    attr_2   db 0 ; 0-3 граница, 6 D 16/32, 7 G Б/4КБ
    base_h   db 0 ; старший байт базы сегмента  
descr ends

; структура для описания дескрипторов прерываний
idescr struc
    offs_l dw 0 ; младшее слово смещения
    sel    dw 0 ; селектор
    cntr   db 0 ; счетчик
    attr   db 0 ; байт атрибутов (как attr_1)
    offs_h dw 0 ; старшее слово смещения
idescr ends

stack32 segment para stack 'STACK'
    stack_start db 100h dup(?)
    stack_size = $ - stack_start
stack32 ends

data32 segment para 'DATA'
; таблица глобальных дескрипторов GDT
    gdt_null    descr <>
    ;gdt_4gb     desct <0FFFFh,0,0,92h,0CFh,0>
    gdt_code32  descr <code32_size-1,0,0,98h,40h,0>
    gdt_data32  descr <data32_size-1,0,0,92h,40h,0>
    gdt_stack32 descr <stack_size-1,0,0,92h,40h,0>
    gdt_video   descr <3999,8000h,0Bh,92h,0,0>
    gdt_size = $ - gdt_null

; псевдодескриптор (6 байт)
    psdesct df 0
   
; селекторы
    data4gbs = 8
    code32s  = 16
    data32s  = 24
    stack32s = 32
    videos   = 40 

; таблица дескрипторов прерываний
    idt label byte
    ;idesct0_12   idesct 13 dup (<0,code32s,0,8Fh,0>)
    ;idesct13     idesct <0,code32s,0,8Fh,0>
    ;idesct14_31  idesct 18 dup (<0,code32s,0,8Fh,0>)
    ;int_time     idesct <0,code32s,0,Eh,0>
    ;int_keyboard idesct <0,code32s,0,Eh,0>
    idt_size = $ - idt

; псевдодекскриптор (6 байт) 
    ipsdesct df 0

; сообщения
    rm_msg     db 27,'[32;20mREAL MODE',27, '[0m', 13, 10, '$'
    to_pm_msg  db 27,'[32;20mAny key to enter PROTECTED MODE',27,'[0m'
               db 13, 10, '$'
    end_rm_msg db 27,'[32;20mREAL MODE again!', 27, '[0m', 13, 10, '$'

    code32_size = 10

    data32_size = $ - gdt_null 
data32 ends

code16 segment para public 'CODE' use16
    assume cs:code16, ds:data32, ss:stack32

begin:
; инициализация ds сегментом данных
    mov ax, data32
    mov ds, ax

; вывод сообщения о реальном режиме
    mov ah, 09h
    lea dx, rm_msg
    int 21h 

; вывод сообщения об ожидании клавиши
    mov ah, 09h
    lea dx, to_pm_msg
    int 21h

; ожидание нажатия клавиши
    mov ax, 10h
    int 16h

; вывод сообщения о возвращении в реальный режим
    mov ah, 09h
    lea dx, end_rm_msg
    int 21h

    mov ax, 4C00h
    int 21h
code16 ends

end begin
