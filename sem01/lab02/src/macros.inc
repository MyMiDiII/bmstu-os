mem_str macro 
    mov di, 0
    mov ah, 00001110b
    mov al, 'M'
    stosw
    mov al, 'e'
    stosw
    mov al, 'm'
    stosw
    mov al, 'o'
    stosw
    mov al, 'r'
    stosw
    mov al, 'y'
    stosw
    mov al, ':'
    stosw              
endm

timer_str macro
    mov di, 2 * 30
    mov ah, 00001110b
    mov al, 'T'
    stosw
    mov al, 'i'
    stosw
    mov al, 'm'
    stosw
    mov al, 'e'
    stosw
    mov al, 'r'
    stosw
    mov al, ':'
    stosw              
endm

keyboard_str macro
    mov di, 80 * 6
    mov ah, 00001110b
    mov al, 'K'
    stosw
    mov al, 'e'
    stosw
    mov al, 'y'
    stosw
    mov al, 'b'
    stosw
    mov al, 'o'
    stosw
    mov al, 'a'
    stosw
    mov al, 'r'
    stosw
    mov al, 'd'
    stosw
    mov al, ':'
    stosw              
endm

mb_str macro
    mov di, 80 * 2 + 16
    mov ah, 00000111b
    mov al, 'h'
    stosw
endm
