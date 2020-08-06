extern target_x
extern target_y
extern rnd_limit
extern generate_rnd
global createTarget

%macro setRndFloat 2
    mov [rnd_limit], dword %1                              
    call generate_rnd
    fstp qword [%2]
%endmacro   

section .bss
    
section .text

createTarget:
    setRndFloat 100, target_x
    setRndFloat 100, target_y