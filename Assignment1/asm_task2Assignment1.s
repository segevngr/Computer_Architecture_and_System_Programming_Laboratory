section	.rodata						
	format_string: db "%s", 10, 0	; format = string

section .bss			
	an: resb 12					; enough to store integer in [-2,147,483,648 (-2^31) : 2,147,483,647 (2^31-1)]
	reveresed: resb 10			; will hold th desired output string in reveresed order

section .text
	global convertor
	extern printf

convertor:
	push ebp
	mov ebp, esp	
	pushad			

	mov ecx, dword [ebp+8]			; ecx = input string
	mov al,[ecx]					;al = first char in string
	sub al,48						;convert char to int digit
	movzx eax, al					;eax = al
	mov ebx,10						;ebx = 10, will be used to mutiply
	
	nextDigit:
	inc ecx						;iterate next char in the input string
	cmp byte [ecx],0			;Input stop condition 1: null terminated string
	je end						
	cmp byte [ecx],10			;Input stop condition 2: \n
	je end						
	mul ebx						;ebx*10 (get ready to add another digit)
	mov dl,[ecx]				;dl = next char in string
	sub dl,48					;convert char to int digit
	movzx edx,dl				;edx = dl
	add eax, edx				;adds the digit to the eax accumolator
	jmp nextDigit				;repeat process on the next input char

end:
	mov ebx, 16
	mov ecx, 0
	mov edx, 0

hexConvertor:		
	mov edx, 0
	div ebx
	cmp dl, 9			
	jle zeroToNine		
	add dl, 55			; If we read digits in range a to f
	jmp insertToReversed

	zeroToNine:
	add dl, 48			; If we read digits in range 0 to 9
	
	insertToReversed:	
	mov byte [reveresed + ecx], dl	;inserting a single char
	inc ecx								;ecx = index of the char to insert
	cmp eax, 0							;stop condition: when reaching the end of string we stop inserting chars
	jne hexConvertor

	;an initialization:
	dec ecx							;ecx = index of char in an
	mov byte [an + ecx + 2], 0		;insert null terminated to an
	mov byte [an + ecx + 1], 09		;insert \n to an
	mov ebx, 0						;ebx = index of char in reveresed
	mov edx, 0

	insertTo_an:
	cmp ecx, 0
	jl print							;Stop condition: when ecx < 0
	mov dl, byte [reveresed + ebx]		;we move the char from reversed to dl
	mov byte [an + ecx], dl				;we add the char to the relevant index in an
	inc ebx								;increment iteration index
	dec ecx								;decrement iteration index
	jmp insertTo_an

print:		
	push an					
	push format_string		
	call printf				;calls printf with string format
	add esp, 8				;clean up stack
	popad			
	mov esp, ebp	
	pop ebp
	ret
