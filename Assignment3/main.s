extern printer, scheduler, drone, createTarget
extern printf
extern malloc
extern free
extern sscanf
global drones
global N, R, K, d, CORS, cors_len
global target_x, target_y, generate_rnd, rnd_limit
extern main


%macro sscanfMacro 4
        push %1         
        push %2         
        push %3         
        push %4        
        call sscanf
        add esp, 12
        pop %1 
%endmacro

%macro startFun 0
    push ebp
    mov ebp, esp
%endmacro

%macro endFun 0
    mov esp, ebp
    pop ebp
    ret
%endmacro

;Randomize float num in %1 limit, and stores it %2
%macro setRndFloat 2
    mov [rnd_limit], dword %1                              
    call generate_rnd
    fstp qword [%2]
%endmacro   

%macro mallocInReg 2
    mov ebx, %1
    push ebx
    call malloc
    mov [%2], eax
%endmacro

%macro insertStack 1
    pushad
    ;Create Stack:
    mov eax, STKi
    push eax
    call malloc
    add eax, STKi           ;make eax point to the end of the stack   
    mov [%1], eax
    ;Init COi esp:
    mov [SPMAIN], esp          ;Backup global esp
    lea esi, [%1]             ;esi = pointer to COi stack
    mov esp, [esi]  
    mov [%1], esp           ;point COi esp to COi func
    mov esp, [SPMAIN]          ;restore global esp
    popad        
%endmacro

section	.rodata					                             
	format_float:       db "%f",10 ,0
    format_decimal:     db "%d", 10, 0
    STKSIZE:            equ 16*1024

section .data
    N:              dd 0     ;num of drones
    R:              dd 0     ;scheduler cycles before elimination
    K:              dd 0     ;drone steps before print
    d:              dq 0     ;target destruction distance
    seed:           dd 0     ;seed
    drones:         dd 0
    tmp1:           dd 0
    target_x:       dq 0
    target_y:       dq 0
    cors_len:       dd 0
    CORS:           dd 0
    rnd_limit:      dd 0     ;Top limit of the random range
    max_size:       dd 65535 
    SPMAIN:         dd 0 
    curr_esp:       dd 0

section .bss  
    STKi:           resb STKSIZE

section .text
    align 16

main:

read_args:
        mov ebx, [esp+8]                
        mov ecx, dword [ebx+4]              
        sscanfMacro ebx, N, format_decimal, ecx
        mov ecx, dword [ebx+8]         
        sscanfMacro ebx, R, format_decimal, ecx
        mov ecx, dword [ebx+12]        
        sscanfMacro ebx, K, format_decimal, ecx
        mov ecx, dword [ebx+16]         
        sscanfMacro ebx, d, format_float, ecx     
        mov ecx, dword [ebx+20]        
        sscanfMacro ebx, seed, format_decimal, ecx

    ;For each drone:    id | isActive | x | y | speed | angle | score  
    ;                    0       4      8   16   24       32      40     = size 44

init:
    startFun
    call init_target
    call init_drones_arr
    call init_cors
    call startCo
    endFun

init_target:
    setRndFloat 100, target_x
    setRndFloat 100, target_y
                
init_drones_arr:
    mov eax, dword [N]
    mov ebx, 44
    mul ebx
    push eax
    call malloc
    mov [drones], eax
    mov esi, 0                          ;esi = loop counter\drone id
    init_single_drone:
    mov edi, [drones]                   ;edi = pointer to offset in drone array
    mov eax, esi
    mov ebx, 44
    mul ebx
    add edi, eax                        
    mov [edi], esi                      ;set drone id
    add edi, 4    
    mov [edi], dword 1                  ;set isActive field (1= active, 0 = eliminated)
    add edi, 4
    setRndFloat 100, edi                     ;Randomize X and set in array
    add edi, 8
    setRndFloat 100, edi                     ;Randomize Y and set in array
    add edi, 8    
    setRndFloat 100, edi                    ;Randomize Speed and set in array
    add edi, 8
    setRndFloat 360, edi                   ;Randomize Angle and set in array
    add edi, 8
    mov [edi], dword 0            ;set initial score to 0  
    inc esi
    cmp esi, [N]
    jl init_single_drone

init_cors:
    startFun
    mov ebx, [N]
    mov [cors_len], ebx
    add [cors_len], dword 3
    mov eax, [cors_len]
    mov ebx, 4
    mul ebx
    call malloc
    mov [CORS], eax
    mov esi, eax             ;esi = CORS array offset (|N|+3 size array)
    insert_scheduler:
    mallocInReg 8, esi
    mov [eax], dword scheduler
    add eax, 4
    insertStack eax
    insert_printer:
    add esi, 4
    mallocInReg 8, esi
    mov [eax], dword printer
    add eax, 4
    insertStack eax
    insert_target:
    add esi, 4
    mallocInReg 8, esi
    mov [eax], dword createTarget
    add eax, 4
    insertStack eax
    insert_drone:
    mov edi, 0               ;edi = drone index
    add esi, 4
    mallocInReg 8, esi
    mov [eax], dword drone
    add eax, 4
    inc edi
    cmp edi, dword [N]
    jl insert_drone  

startCo:
    pushad
    mov [SPMAIN], esp
    jmp do_resume

endCo:
    mov esp, [SPMAIN]
    popad

do_resume:
    mov esp, [CORS +4]          ;switch esp to scheduler stack
    popad
    popfd
    ret


generate_rnd:
    startFun                                               ; of random number
    pushad
    mov ecx, 16
    calc_random:
        mov ebx, dword [seed]
        mov eax, 1
        mov edi, 1
        and eax, ebx                                        
        shr ebx, 2                                          
        and edi, ebx                                       
        xor eax, edi                        
        mov edi, 1  
        shr  ebx, 1                               
        and edi, ebx                                    
        xor eax, edi                                     
        mov edi, 1
        shr ebx, 2                                  
        and edi, ebx                                       
        xor eax, edi                                       
        shl eax,  15
        shr dword[seed], 1
        xor dword [seed], eax
    loop calc_random, ecx
    calc_scale: 
        finit                                              
        cmp [rnd_limit], dword 120                              
        jne reg_scale
        jmp special_scale
        reg_scale:
            fild dword [rnd_limit]
            fild dword [max_size]                             
            fdiv
            fild dword [seed]                                
            fmul
            jmp finish_rnd
        special_scale:
            mov [tmp1], dword 60
            fild dword [rnd_limit]
            fild dword [max_size]                              
            fdiv
            fild dword [seed]                                 
            fmul
            fild dword [tmp1]                               
            fsub                                              
    finish_rnd:
    popad
    endFun