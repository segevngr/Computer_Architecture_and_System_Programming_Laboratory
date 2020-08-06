%macro newLink 0
	pushad
	push 5                 ;TODO: CHANGED
	call malloc
	add esp, 4                      
	mov [eax], byte 0               ;data = 0
	mov [eax+1], dword 0            ;pointer = 0
	mov [currLink], eax
	popad
%endmacro

section .rodata                    	
        charFormat:             db "%c"
        intFormat:              db "%c"
        calcMsg:                db "calc: " , 0
        underflowErr:           db "Error: Insufficient Number of Arguments on Stack", 10, 0
        overflowErr:            db "Error: Operand Stack Overflow", 10, 0
        stackSize:              equ 5

section .bss
        opStack:	        resd stackSize
        stackTop:               resb 4                           
        currLink:               resb 4
        prevLink:               resb 4
        stackCount:             resb 1
        linkCount:              resb 1
        str:                    resb 80                    

section .text
     align 16
     global main
     extern printf
     extern fprintf
     extern fgets
     extern malloc
     extern free
     extern stderr
     extern stdin
     extern stdout 

main:
        push ebp              		
        mov ebp, esp         		
        pushad                   	
        mov [stackTop], dword opStack               
        mov [stackCount], byte 0                   

inputLoop:
        pushad
        push dword calcMsg                  
        call printf
        add esp, 4
        popad                      
        pushad
        push dword [stdin]
        push dword 80
        push dword str                        
        call fgets                             ;user input stored in str
        add esp, dword 12                      ;reset stack
        popad

        mov al, byte [str]
        cmp byte al, 112                 ; = 'p'
        je popAndPrint
        ;cmp byte str, 113               ; = 'q'
        ;je exit
        ;cmp byte str, 43                ; = '+'
        ;je plus
        ;cmp byte str, 100               ; = 'd'
        ;je duplicate
        ;cmp byte str, 38                ; = '&'
        ;je and
        ;cmp byte str, 24                ; = '|'
        ;je or
        ;cmp byte str, 110               ; = 'n'
        ;je numberOfDigits
        ;cmp byte str, 42                ; = '*'
        ;je multiple

handleNumInput:
        cmp [stackCount], byte stackSize         ;checking for overflow
        jge overflow

        mov edx,0                               
        mov edx, str                            ;edx = input number
        inc byte [stackCount]

        toASCII:
                cmp byte [edx], 10                      ;reached end of input               
                je setPrevLink
                cmp byte [edx], 57			;if 10 < num < 15 convert to hexaChar
                jg hexaChar	
                sub [edx], byte 48					
                inc edx
                jmp toASCII
                hexaChar:
                sub [edx], byte 55
                inc edx
                jmp toASCII

        setPrevLink:
                mov [prevLink], dword 0                 ;resetting previous Link to 0
        inputToLinks:
                newLink                                 ;macro for new link allocation
                mov ecx, [currLink]                     ;ecx = new link
                mov al, byte 0
                dec edx
                cmp edx, str
                mov al, byte [edx]
                mov [ecx], byte al                      ;insert input data to link
                mov eax, dword [prevLink]
                cmp eax, dword 0
                jne notFirst

                firstLink:
                mov ebx, [stackTop]
                mov [ebx], dword ecx                    ;set pointer from stack to new link
                add ebx, dword 4
                mov [stackTop], dword ebx               ; update SP
                jmp linkCreated

                notFirst:
                mov [eax+1], dword ecx

                linkCreated:
	        mov [prevLink], dword ecx              ;update previous link to current link
                cmp edx, str                           ;check if there's any input left
                jg inputToLinks
                jmp inputLoop

        overflow:
                pushad
                push dword overflowErr
                call printf
                add esp, 4
                pop ebp
                popad
                jmp inputLoop  

popAndPrint:
        mov [linkCount], byte 0               ;num of links we pushed to global stack
        mov ebx, dword [stackTop]             ;ebx holds the first link      
        push ebx                              ;Push first link to global stack
        inc byte [linkCount]                  ;linkCount++
        push_next_link:
                cmp dword [ebx+1], 0              ;if link pointer = 0 => its the last link
                je print
                mov ebx, dword [ebx+1]            ;update ebx to next link via prev links pointer
                push ebx                          ;push next link to global stack
                inc byte [linkCount]
                jmp push_next_link
        print:
                cmp [linkCount], byte 0
                je end_print
                pop ebx                                 ;ebx hodls the last link pushed to global stack
                pushad
                pushfd
                push ebx                                 ;print paramters: we push only bl because it holds the data section of ebx
                push intFormat
                call printf
                add esp, 8
                popfd
                popad
                dec byte [linkCount]
                jmp print
        end_print:
  
END:                                    ; END
        mov esp, ebp			; free function activation frame
        pop ebp				; restore Base Pointer previous value (to returnt to the activation frame of main(...))
        ret				; returns from assFunc(...) function