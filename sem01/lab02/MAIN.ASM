.386p

include MACROS.INC

descr struc    
    limit 	dw 0
    base_l 	dw 0
    base_m 	db 0
    attr_1	db 0
    attr_2	db 0
    base_h 	db 0
descr ends

intr struc 
    offs_l 	dw 0
    sel		dw 0
    cntr    db 0
    attr	db 0
    offs_h 	dw 0
intr ends

stack_seg segment  para stack 'STACK'
    stack_start	db	100h dup(?)
    stack_size = $-stack_start
stack_seg 	ENDS

data_seg segment para 'DATA'

    gdt_null  descr <0,0,0,0,0,0>
    gdt_code16 descr <rm_code_size-1, 0, 0, 98h, 0, 0>
    gdt_data16 descr <0FFFFh, 0, 0, 92h, 8F, 0>
    gdt_code32 descr <pm_code_size-1, 0, 0, 98h, 40h, 0>    
    gdt_data32 descr <data_size-1, 0, 0, 92h, 40h, 0>
    gdt_stack32 descr <stack_size-1, 0, 0, 96h, 40h, 0>
    gdt_screen32 descr <3999, 8000h, 0Bh, 92h, 40h, 0>

    gdt_size = $-gdt_null ; размер таблицы  GDT 
    gdtr	df 0

    sel_code16   equ    8   
    sel_data16    equ   16   
    sel_code32   equ   24
    sel_data32    equ   32
    sel_stack32    equ   40
    sel_videobuf equ   48
    
    IDT	label byte

	trap_f intr 12 dup (<0, sel_code32, 0, 10001111b, 0>) 
	trap_13 intr <0, sel_code32, 0, 10001111b, 0>
	trap_s intr 19 dup (<0, sel_code32, 0, 10001111b, 0>) 

    int08 intr <0, sel_code32, 0, 10001110b, 0>
    int09 intr	<0, sel_code32, 0, 10001110b, 0> 

    idt_size = $-IDT

    idtr df 0
    idtr_backup dw	3FFh, 0, 0

    mask_master	db 0
    mask_slave	db 0
        
	ascii   db 0, 0, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 45, 61, 0, 0
	        db 81, 87, 69, 82, 84, 89, 85, 73, 79, 80, 91, 93, 0, 0
            db 65, 83, 68, 70, 71, 72, 74, 75, 76, 59, 39, 96, 0
            db 92, 90, 88, 67, 86, 66, 78, 77, 44, 46, 47, 0, 0, 0, 32, 0

	ascii_low db 0, 0, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 45, 61, 0, 0
              db 113, 119, 101, 114, 116, 121, 117, 105, 111, 112, 91, 93, 0, 0
              db 97, 115, 100, 102, 103, 104, 106, 107, 108, 59, 39, 96, 0
              db 92, 122, 120, 99, 118, 98, 110, 109, 44, 46, 47, 0, 0, 0, 32, 0

    flag_enter_pr	    db 0				 
    cnt_time	        db 0
    color               db 0
    up_flag             db 0
    syml_pos            dd 80 * 8

    msg_in_rm   db 27, '[32;20mREAL MODE', 27, '[0m$'
    msg_move_pm db 27, '[32;20mAny key to enter PROTECTED MODE', 27, '[0m$'
    msg_out_pm  db 27, '[32;20mREAL MODE again! ', 27, '[0m$'

    data_size = $-gdt_null
data_seg ends

pm_seg segment para public 'CODE' use32
    assume cs:pm_seg, ds:data_seg, ss:stack_seg

    dummy_exc proc
        iret
    dummy_exc endp

    exc13 proc
        pop eax
        iret
    exc13 endp
        
    int_time proc uses eax 
        mov edi, 80 * 2 + 2 * 30
        xor eax, eax

        mov ah, color
        rol ah, 4
        mov al, ' '
        stosw		

        cmp cnt_time, 5
        jne skip

        mov cnt_time, 11111111b
        inc ah
        cmp ah, 1111b
        jne save
        mov ah, 0000b

    save:
        mov color, ah
            
    skip:	
        inc cnt_time

        mov	al, 20h 
        out	20h, al
        iretd
    int_time endp

    int_keyboard proc uses eax ebx edx 
        ; получение скан-кода нажатой клавиши из порта клавиатуры
        in	al, 60h

        ; cравнение с кодом enter
        cmp	al, 1Ch
        jne	key_proc
        or flag_enter_pr, 1
        jmp allow_handle_keyboard

    key_proc:
        ; проверка по скан-коду, нажата или отжата клавиша
        cmp al, 80h
        ja allow_handle_keyboard

    check_back:
        ; cравнение с кодом backspace
        cmp al, 0Eh
        je backspace

    check_up:
        ; cравнение с кодом capslock
        cmp al, 3Ah
        jne print_value

    reverse_up_flag:
        xor up_flag, 1
        jmp allow_handle_keyboard

    backspace:
        mov ebx, syml_pos
        cmp ebx, 80 * 8
        je clear
        sub ebx, 2

    clear:
        mov al, ' '
        mov es:[ebx], al
        mov syml_pos, ebx
        jmp allow_handle_keyboard
        
    print_value:
        xor ah, ah
        xor ebx, ebx
        mov bx, ax

        cmp up_flag, 1
        je up
        mov dl, ascii_low[ebx]
        jmp print

    up:
        mov dl, ascii[ebx]

    print:
        mov ebx, syml_pos
        mov es:[ebx], dl

        add ebx, 2
        mov syml_pos, ebx

    allow_handle_keyboard: 
        in	al, 61h         ; сообщаем контроллеру о приёме скан кода:
        or	al, 80h         ; установкой старшего бита 
        out	61h, al         ; содержимого порта B
        and al, 7Fh         ; и последующим его сбросом
        out	61h, al

        mov	al, 20h         ; End of Interrupt ведущему контроллеру прерываний
        out	20h, al

        iretd
    int_keyboard endp

    count_memory proc uses ds eax ebx 
        mov ax, sel_data16
        mov ds, ax
        
        mov ebx, 100000h     ; пропускаем первый МБ памяти
        mov dl, 10101110b  

        mov	ecx, 0FFEFFFFEh

    iterate_through_memory:
        mov dh, ds:[ebx]

        mov ds:[ebx], dl
        cmp ds:[ebx], dl

        jnz print_memory_counter

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
        add ebx, 10h 
        mov ecx, 8   
        
    print_symbol: 
        mov dl, al
        and dl, 0Fh      
            
        cmp dl, 10
        jl add_zero_sym
        add dl, 'A' - '0' - 10 

    add_zero_sym:
        add dl, '0'       ; преобразуем число в цифру               
        mov es:[ebx], dl
        ror eax, 4        ; циклически сдвинуть вправо
        sub ebx, 2
        loop print_symbol

        ret
    print_eax endp


pm_start:
    ; в регистры сегмента загружаем селекторы
    mov	ax, sel_data32 
    mov	ds, ax
    mov	ax, sel_videobuf
    mov	es, ax
    mov	ax, sel_stack32
    mov	ss, ax
    mov	eax, stack_size
    mov	esp, eax

    sti ; разрешить прерывания, запрещенные в реальном режиме
    mem_str
    timer_str
    keyboard_str
    call count_memory
    mb_str

    ; Возвращение в реальный режим происходит по нажатию
    ; клавиши 'enter' - это будет обработано в коде обработчика прервания 
    ; чтобы программа не завершалась до этого момента, нужен бескончный цикл
proccess:
    test flag_enter_pr, 1
    jz	proccess

    ; запрещаем прерывания
    ; немаскируемые уже запрещены
    cli ; сброс флага прерывания IF = 0
    far_jump return_rm, sel_code16
		
    pm_code_size = $-pm_start 	
pm_seg ends

rm_seg segment para public 'CODE' use16
    assume cs:rm_seg, ds:data_seg, ss: stack_seg

start:
    mov ax, data_seg
    mov ds, ax

    mov ax, pm_seg
    mov es, ax

    print_str msg_in_rm  
    print_str msg_move_pm

    wait_key
    clear_screen 

    xor	eax, eax
    
    ; загружаем адреса сегментов
    mov	ax, rm_seg 
    load_gdt gdt_code16 ; в дескр сегмента

    mov ax, pm_seg
    load_gdt gdt_code32

    mov ax, data_seg
    load_gdt gdt_data32

    mov ax, stack_seg
    load_gdt gdt_stack32

    mov ax, data_seg  
    shl eax, 4   
    add	eax, offset gdt_null
    init_gdtr eax

    lea eax, es:dummy_exc
    load_idt trap_f
    
    lea eax, es:dummy_exc
    load_idt trap_s
    
    lea eax, es:exc13
    load_idt trap_13;

    ; загружаем дескриптор прерывания, нужно только смещение, тк селектор кода уже указан
    lea eax, es:int_time   ; в eax смещение 8 обработчика
    load_idt int08         ; прерывание таймера

    lea eax, es:int_keyboard
    load_idt int09; прерывание клавиатуры
    
    mov ax, data_seg
    shl eax, 4
    add	eax, offset IDT; в eax полный линейный адрес IDT
    init_idtr eax
    
    ; для возврата в защищенный:
    ; сохраним маски прерываний контроллеров
    in	al, 21h             ; получить набор масок (флагов) mask_master 21h - номер шины, in на неё даст нам набор масок (флагов)
    mov	mask_master, al     ; сохраняем в переменной mask_master (понадобится для возвращения в RM)
    in	al, 0A1h	        ; аналогично ведомого,  in даёт набор масок для ведомого
    mov	mask_slave, al

    ; перепрограммируем пик (контроллер)
    ; вектор прерывания = базовый вектор прерывания + № IRQ
    ; irq0 - системный таймер, 8 + 0 = 8ое исключение => system fault, паника системы
    ; необходимо перепрограмироваить пик на новый новый базовый вектор 32
    set_interrupt_base 32 

    ; Запретим все прерывания в ведущем контроллере, кроме IRQ0 (таймер) и IRQ1(клавиатура)
    ; установление новых масок
    mov	al, 0FCh
    out	21h, al
    ; запретим все прерывания в ведомом контроллере
    mov	al, 0FFh
    out	0A1h, al
    
    ; загрузим IDT
    lidt fword ptr idtr
    
    ; открытие линии A20  
    in	al, 92h						
    or	al, 2						
    out	92h, al						

    cli         ; отключить маскируемые прерывания
    in	al, 70h ; и немаскируемые прерывания
    or	al, 80h
    out	70h, al

    ; ПЕРЕХОД В ЗАЩИЩЕННЫЙ РЕЖИМ
    mov	eax, cr0
    or eax, 1
    mov	cr0, eax

    db	66h 
    far_jump pm_start, sel_code32

return_rm:
       
    ; ВОЗВРАЩАЕМСЯ В РЕАЛЬНЫЙ РЕЖИМ 
    mov	eax, cr0
    and	al, 0FEh 				; сбрасываем флаг защищенного режима
    mov	cr0, eax

    ; этот дальний переход необходим для модификации теневого регистра cs
    db	0EAh	; far jmp rm_seg:$+4
    dw	$+4	    ; *выполнить следующую после dw	rm_seg команду
    dw	rm_seg

    ; загружаем в сегментные регистры сегменты
    mov	eax, data_seg	
    mov	ds, ax          
    mov eax, pm_seg
    mov	es, ax
    mov	ax, stack_seg
    mov	ss, ax
    mov	ax, stack_size
    mov	sp, ax

    ; перепрограммируем контроллер
    ; теперь базовый вектор прерывания снова = 8 - смещение, 
    ; по которому вызываются стандартные обработчики прерываний в реалмоде
    set_interrupt_base 8 

    mov	al, mask_master ; восстанавить маски контроллеров прерываний
    out	21h, al
    mov	al, mask_slave
    out	0A1h, al

    ; загружаем таблицу дескриптров прерываний реального режима
    lidt	fword ptr idtr_backup

    in	al, 70h ; разрешить немаскируемые прерывания
    and	al, 7FH
    out	70h, al
    sti         ; и маскируемые

    clear_screen
    print_str msg_out_pm
        
    mov	ax, 4C00h
    int	21h

    rm_code_size = $-start 	; длина сегмента rm_seg
rm_seg	ends
end start
