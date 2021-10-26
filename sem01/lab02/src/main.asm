; директива для работы с расширенным набором команд
.386p

include macros.inc

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
    gdt_4gb     descr <0FFFFh,0,0,92h,0CFh,0>
    gdt_code32  descr <code32_size-1,0,0,98h,40h,0>
    gdt_data32  descr <data32_size-1,0,0,92h,40h,0>
    gdt_stack32 descr <stack_size-1,0,0,92h,40h,0>
    gdt_video   descr <3999,8000h,0Bh,92h,0,0>
    gdt_size = $ - gdt_null

; псевдодескриптор (6 байт)
    psdescr df 0
   
; селекторы
    data4gbs = 8
    code32s  = 16
    data32s  = 24
    stack32s = 32
    videos   = 40 

; таблица дескрипторов прерываний
    idt label byte
    idescr0_12      idescr 13 dup (<dummy,code32s,0,8Fh,0>)
    idescr13        idescr <except13,code32s,0,8Fh,0>
    idescr14_31     idescr 18 dup (<dummy,code32s,0,8Fh,0>)
    idescr_time     idescr <int8h,code32s,0,0Eh,0>
    idescr_keyboard idescr <int9h,code32s,0,0Eh,0>
    idt_size = $ - idt

; псевдодекскриптор (6 байт) 
    ipsdescr df 0

; псевдодекскриптор таблицы прерываний реального режима
    ipsdescr16 dw 3FFh, 0, 0

; для сохранения масок контроллеров прерываний
    master db 0
    slave  db 0

; переменные для обработчика прерывания от таймера
    cnt   db 0 ; счетчик тиков
    color db 0 ; цвет фона

; переменные для обработчика прерывания от клавиатуры
    ascii    db 0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,0
                db 'q','w','e','r','t','y','u','i','o','p','[',']',0,0
                db 'a','s','d','f','g','h','j','k','l',';','"',0
                db '\\','z','x','c','v','b','n','m',',','.','/',0,0,0,' ',0, 0
                db 0,0,0,0,0,0,0,0,0,0,0,0

    enter_flag db 0
    syml_pos   dd 80 * 8
    up_flag    db 0


; сообщения
    rm_msg     db 27,'[32;20mREAL MODE',27, '[0m', 13, 10, '$'
    to_pm_msg  db 27,'[32;20mAny key to enter PROTECTED MODE',27,'[0m'
               db 13, 10, '$'
    end_rm_msg db 27,'[32;20mREAL MODE again!', 27, '[0m', 13, 10, '$'

    code32_size = 10

    data32_size = $ - gdt_null 
data32 ends

code32 segment para public 'CODE' use32
    assume cs:code32, ds:data32, ss:stack32
; заглушка для исключений
    dummy proc
        iret
    dummy endp

; заглушка для 13-ого исключения 
; при возникновении этого исключения в стек помещается
; код ошибки, который в обработчике мы снимаем
    except13 proc uses eax
        pop eax
        iret
    except13 endp

    int8h proc uses eax 
; установка положения курсора
        mov edi, 80 * 2 + 2 * 30

; изменение цвета фона 
        xor eax, eax
        mov ah, color
        rol ah, 4
        mov al, ' '
        stosw

; проверка прошло ли время одного цвета
        cmp cnt, 5
        jne same

; если прошло, отчет сначала и изменение цвета
        mov cnt, 11111111b
        inc ah
        mov color, ah

    same:
        inc cnt
; EOI
        mov	al, 20h 
        out	20h, al
        iretd
    int8h endp

    int9h proc uses eax ebx edx 
; получение скан-кода нажатой клавиши из порта клавиатуры
        in	al, 60h

; cравнение с кодом enter
        cmp	al, 1Ch
        jne	key_proc
        or enter_flag, 1
        jmp allow_handle_keyboard

    key_proc:
; проверка по скан-коду, нажата или отжата клавиша
        cmp al, 80h
        ja allow_handle_keyboard

    check_back:
; cравнение с кодом backspace
        cmp al, 0Eh
        jne check_up

; проверка на границу ввода
        mov ebx, syml_pos
        cmp ebx, 80 * 8
        je clear
        sub ebx, 2

; "стирание" предыдущего символа
    clear:
        mov al, ' '
        mov es:[ebx], al
        mov syml_pos, ebx
        jmp allow_handle_keyboard

    check_up:
; cравнение с кодом capslock
        cmp al, 3Ah
        jne choose_value

; установка флага прописных символов
    reverse_up_flag:
        xor up_flag, 1
        jmp allow_handle_keyboard

; вывод символа нажатой клавиши
    choose_value:
        xor ah, ah
        xor ebx, ebx
        mov bx, ax

        mov dl, ascii[ebx]
        cmp up_flag, 1
        jne print
        
        cmp dl, 'a'
        jb print
        cmp dl, 'z'
        ja print
        sub dl, 32

    print:
        mov ebx, syml_pos
        mov es:[ebx], dl

        add ebx, 2
        mov syml_pos, ebx

    allow_handle_keyboard:
        in	al, 61h
        or	al, 80h
        out	61h, al
        and al, 7Fh
        out	61h, al

        mov	al, 20h
        out	20h, al

        iretd
    int9h endp

    count_memory proc uses ds eax ebx 
;
        mov ax, data4gbs
        mov ds, ax
        
; пропуск первого МБ памяти
        mov ebx, 100000h
        mov dl, 10101110b  

; оставшееся число байт из 4ГБ ( )
        mov	ecx, 0FFEFFFFFh

    iterate_through_memory:
; сохранение байта из памяти в dh
        mov dh, ds:[ebx]

; попытка записать значение в память
        mov ds:[ebx], dl
        cmp ds:[ebx], dl
; если не записалось, подчет памяти окончен
        jnz print_memory_counter

; если записалось, продолжеение подсчета
; возвращение в память сохраненного значения
        mov	ds:[ebx], dh
        inc ebx
        loop iterate_through_memory

    print_memory_counter:
        ; перевод в МБ
        mov eax, ebx
        xor edx, edx

        mov ebx, 100000h
        div ebx

        mov ebx, 80 * 2 - 2
        call print_eax

        ret
    count_memory endp
            
; вывод значения eax в видеобуффер
    print_eax proc uses ecx ebx edx     
; начинаем вывод с последнего символа
        add ebx, 10h 
        mov ecx, 8   
        
    get_digit: 
        mov dl, al
; получение только бит последней цифры
        and dl, 0Fh      
            
        cmp dl, 10
        jl print_sym
        add dl, 'A' - 10 - '0' 

    print_sym:
; цифра -> символ
        add dl, '0'
        mov es:[ebx], dl
; переход к работе со следующей цифрой
        ror eax, 4
        sub ebx, 2
        loop get_digit

        ret
    print_eax endp

pm_begin:
; загрузка селекторов в сегментные регистры
    mov	ax, data32s
    mov	ds, ax
    mov	ax, videos
    mov	es, ax
    mov	ax, stack32s
    mov	ss, ax
    mov	eax, stack_size
    mov	esp, eax

; разрешение прерываний
    sti 

; вывод сообщений в защищенном режиме
    mem_str
    timer_str
    keyboard_str
    call count_memory
    mb_str

; ожидание нажатия enter для возвращение в реальный режим
proccess:
    test enter_flag, 1
    jz	proccess

; запрет прерываний (немаскируемые запрещены)
    cli

; возвращение в реальный режим
    mov eax, cr0
    and al, 0FEh
    mov cr0, eax

; дальний переход для загрузки CP и IP
    db 0EAh ; код команды far jmp
    dd offset real ; смещение
    dw code16 ; селектор

    code32_size = $ - pm_begin
code32 ends

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

; очистка экрана
    mov ax, 3h
    int 10h

; подготовка к переходу в защищенный режим
; вычисление и загрузка в GDT линейных адресов сегментов
    xor eax, eax
    mov ax, code32
    shl eax, 4
    mov word ptr gdt_code32.base_l, ax
    shr eax, 16
    mov byte ptr gdt_code32.base_m, al
    mov byte ptr gdt_code32.base_h, ah
    
    xor eax, eax
    mov ax, data32
    shl eax, 4
    mov word ptr gdt_data32.base_l, ax
    shr eax, 16
    mov byte ptr gdt_data32.base_m, al
    mov byte ptr gdt_data32.base_h, ah

    xor eax, eax
    mov ax, stack32
    shl eax, 4
    mov word ptr gdt_stack32.base_l, ax
    shr eax, 16
    mov byte ptr gdt_stack32.base_m, al
    mov byte ptr gdt_stack32.base_h, ah

; подготовка псевдодескриптора и загрузка GDTR
    mov ax, data32
    shl eax, 4
    add eax, offset gdt_null
    mov dword ptr psdescr + 2, eax
    mov word  ptr psdescr, gdt_size - 1
    lgdt fword ptr psdescr

; подготовка превдодескриптора
    mov ax, data32
    shl eax, 4
    add eax, offset idt
    mov  dword ptr ipsdescr + 2, eax 
    mov  word ptr  ipsdescr, idt_size-1 

; сохранение масок контроллеров прерываний
; !!! описать
    in al, 21h
    mov master, al
    in al, 0A1h
    mov slave, al

; перепрограммирование ведущего контроллера
; !!! переписать комменты
    mov al, 11h  ; СКИ1 - два контроллера в компьютере -> Будет СКИ3
    out 20h, al                     
    mov al, 32   ; СКИ2 - базовый вектор 32 (был 8)
    out 21h, al                     
    mov al, 4    ; СКИ3 - ведомый подключен в уровню2 ведущего
    out 21h, al
    mov al, 1    ; СКИ4 - 8086, требуется EOI требуется
    out 21h, al

; маска для ведущего контроллера (разрешаем прерывания от таймера и клавиатуры)
    mov al, 0FCh
    out 21h, al

; маска для ведомого контроллера (запрещаем все прерывания)
    mov al, 0FFh
    out 0A1h, al 

; загрузка псевдодескриптора в IDTR
    lidt fword ptr ipsdescr

; открытие линии А20
	mov		AL, 0D1h
	out		64h, AL
	mov		AL, 0DFh
	out		60h, AL

; переход в защищенный режим
    mov eax, cr0
    or eax, 1
    mov cr0, eax

; дальний переход
    db 66h
    db 0EAh
    dd offset pm_begin
    dw code32s

; возращение в реальный режим
; !!! поменять комменты
real:
    ; обновляем все сегментные регистры
    mov ax, data32   
    mov ds, ax
    mov ax, code32
    mov es, ax
    mov ax, stack32   
    mov ss, ax
    mov ax, stack_size
    mov sp, ax

; возвращаем базовый вектор контроллера прерываний
    mov al, 11h
    out 20h, al
    mov al, 8
    out 21h, al
    mov al, 4
    out 21h, al
    mov al, 1
    out 21h, al

; восстанавливаем маски контроллеров прерываний
    mov al, master
    out 21h, al
    mov al, slave
    out 0A1h, al

; восстанавливаем IDTR прерываний (на 1ый кб)
    lidt fword ptr ipsdescr16

    sti      ; Резрешаем (аппаратные) прерывания 
	xor al, al
	out 70h, al
	
; работа в реальном режиме
    ;clear screen
    mov ax, 3
    int 10h

; вывод сообщения о возвращении в реальный режим
    mov ah, 09h
    lea dx, end_rm_msg
    int 21h

    mov ax, 4C00h
    int 21h
code16 ends

end begin
