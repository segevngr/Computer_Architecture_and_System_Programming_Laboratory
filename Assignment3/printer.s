extern drones
extern printf
global printer
extern target_x
extern target_y
extern N 

%macro printFloat 1
    pushad
    fld qword [%1]
    sub esp, 8
    fstp qword[esp]
    push float_format
    call printf
    add esp, 12
    popad
%endmacro


section .rodata
    float_format: db "%.2f", 0
    int_format: db "%d", 0
    print_underscore: db "_", 0
    print_comma: db ", ", 0
    print_newLine: db "", 10

section .data
    print_loop: dd 0
    drone_id: dd 0
    drone_offset: dd 0
    
section .bss

section .text
    align 16

printer:

    print_target:
    pushad
    printFloat target_x
    push print_comma
    call printf
    add esp, 4
    printFloat target_y
    push print_newLine
    call printf
    add esp, 4
    popad

    ;drones:            id | isActive | x | y | speed | angle | score  
    ;                    0       4      8   16   24       32      40     = size 44
    mov esi, 0                          ;esi = print loop counter
    print_drone_loop:
    mov edi, [drones]
    mov eax, esi                     
    mov ebx, 44
    mul ebx
    add edi, eax                         ;edi = pointer to drones array offset
    ;Print id:
    mov ebx, dword [edi]  
    mov [drone_id], ebx 
    push ebx
    push int_format 
    call printf
    add esp, 4
    push print_comma
    call printf
    add esp, 4
    ;Print X position:
    add edi, 8
    printFloat edi
    push print_comma
    call printf
    add esp, 4
    ;Print Y position:
    add edi, 8
    printFloat edi
    push print_comma
    call printf
    add esp, 4
    ;Print angle:
    add edi, 16
    printFloat edi
    push print_comma
    call printf
    add esp, 4
    ;Print speed:
    sub edi, 8
    printFloat edi
    push print_comma
    call printf
    add esp, 4
    ;Print score:   
    add edi, 16 
    push dword [edi]
    push int_format
    call printf
    add esp, 4   
    push print_newLine
    call printf
    add esp, 4
    inc esi
    deb1:
    cmp esi, dword [N]
    jl print_drone_loop

.finish:
        ret