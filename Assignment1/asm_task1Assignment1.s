section .data                    	; Definition of global variables
        format: db "%d" , 10, 0         ; Printing Format
section .text                    	; Code section
        extern c_checkValidity           
        extern printf                   
        global assFunc          	

assFunc:                        	   
        push ebp              		; Push base pointer
        mov ebp, esp         		; Sets stack pointer to ebp
        pushad                   	; push all registers to Stack
        mov ecx, dword [ebp+12]		; Puts y variable from c stack in ecx
        mov edx, dword [ebp+8]          ; Puts x variable from c stack in edx
                                                
	yourCode:			
        push dword ecx                  ; ecx = second argument
        push dword edx                  ; edx = first argument
        call c_checkValidity            ; calls c_checkValidity with the two arguments
        
        cmp byte eax, 49                ; c_checkValidity output saved in eax, then its being compared to '1' 
        je MINUS                        ; If equals jumps to MINUS
        add edx,ecx                     ; x+y => edx
        jmp PRINT                       ; Jumps to print
        
        MINUS:                          
        sub edx,ecx                     ; x - y => edx
        
        PRINT:                          
        push edx                        ; Push the calculated value to stack
        push dword format               ; Push print format
        call printf                     ; Calls c print funciton

        END:                            ; END
        mov esp, ebp			; Resets ebp to free activation frame
        pop ebp				; Pop ebp to return to main function
        ret				; Returns from assFunc
