global printing
global print_args2
global main
global x_multi_struct
global y_struct
global get_multi
global getMinMax
extern calloc
extern fgets
extern puts
extern printf
extern stdin
extern sscanf
extern strncpy
global rand_num
global IRcheck

section .rodata
	str1: db "I am alive",10,0
	str2: db "%x",0
	format: db "%02hhx",0
	frmtd: db "%d",0
section .data
	x_multi_struct: db 5,
	x_num: db 0xaa, 1,2,0x44,0x4f
	x_max_size: equ 600
	
	y_struct: db 6,
	y_num: db 0xaa, 1,2,3,0x44,0x4f
	
	 buffer: times 600 db 0 ; Buffer to hold the input string
    	 bufsize: equ 600       ; Buffer size
    	 current: db 0
    	 
    	 state: dd 0x001a
    	 mask: dd 0x1000
section .text
main:    
	push ebp
	mov ebp, esp
	
	call IRcheck
	
	push x_multi_struct
        push y_struct
        call add_multi
        add esp,8
	
	;call rand_num
	
finish:	
	mov esp, ebp
	pop ebp
	ret
;--------------------------------------------------------------------------------
IRcheck:
	push ebp                 
        mov ebp, esp
        pushad 
        
        mov ecx, [ebp+12] ;;ecx = av[1]
       ;mov ecx,[ecx]
        
       
        cmp byte[ecx],'-'
        jne done_IRCHECK
     	push str1
     	call printf
     	add esp,4
        inc ecx
        
        cmp byte[ecx],'I'
        jne R_CHECK
        pushad
        push x_multi_struct 
        call get_multi
        add esp,4
        push y_struct 
        call get_multi	
	add esp,4
	popad
R_CHECK:
	cmp byte [ecx],'R'
	jne done_IRCHECK
	call rand_num        
        
done_IRCHECK:
	popad
	mov esp, ebp 
        pop ebp 
        ret
;--------------------------------------------------------------------------------	
rand_num:

        push ebp                 
        mov ebp, esp     
        
        mov bx, word[state]
        mov cx, word[mask]
        
        mov ax,bx
        shr bx,1
  
       
        pushad
        push 0
        push bx
        push str2
        call printf
        add esp, 10
        popad
    
        mov esp, ebp 
        pop ebp 
        ret

;------------------------------------------------------------------------------
add_multi:
        push ebp                 
        mov ebp, esp   
        pushad
	
        mov eax, [ebp+8] 
        mov ebx, [ebp+12]
	
	push eax
	push ebx
	call getMinMax
	add esp,8
		
	mov edi,eax ;; edi = long array
	mov esi,ebx ;esi = short array
	
        mov eax, dword[eax] ;in eax there is length of max array
        
        
        add eax, 1
        push eax
        push 1
        call calloc ;; allocating space for new array size of maxlength+1
        add esp,8
        
      
       ; mov ebx,dword[edi]
        ;inc ebx ;;maxlength+1
      	mov edx,1
        ;inc ebx

	        
add:	
	cmp edx,7
	je prnt	
	mov cl, byte[edi+edx] ;;using ecx
	;jc carry_support
after_cry_sprt:
	
	mov bl,byte[esi+edx]  ;;using ebx
	mov byte[eax+edx-1],0
	add byte[eax+edx-1], cl  ;;using eax
	add byte[eax+edx-1],bl
	;jc carry_support
	
	
	;add eax,ebx
	;push 1
	;push eax
	;push current
	;call strncpy
	;add esp,12
	
	inc edx
	jmp add

carry_support:
inc cl
jmp after_cry_sprt	
	

prnt:
	
	pushad
	push eax
	call print_multi
	add esp,4
	popad	
	
	;push eax
        ;push format
	;call printf	
	;add esp,8
	;jmp done_add
	
	;mov edi, dword[ebx]
	;cmp byte [edx],0
	;je  done_add 
	 
	     
        ;adc [eax],[ecx]
        ;adc [eax],[ebx]
        ;dec dword [edx]
        
        
        ;jmp add
        
        
        
done_add:        
       popad
        mov esp, ebp 
        pop ebp 
        ret
        	
;________________________________________________________________________

	
;------------------------------------------------------------------------------
getMinMax:
	push ebp                 
        mov ebp, esp 
        
        mov eax, x_multi_struct
        mov ebx, y_struct
        mov edi, [eax]
        mov esi, [ebx]

        cmp edi,esi
        jle replace_structs

        mov esp, ebp 
        pop ebp 
        ret
      
replace_structs: 
	mov ecx, ebx
        mov ebx, eax
        mov eax, ecx

        mov esp, ebp 
        pop ebp 
        ret
	
;------------------------------------------------------------------------------
get_multi:
	push ebp
	mov ebp, esp
	pushad
	push dword [stdin]
	push dword bufsize
	push dword buffer
	call fgets	
	add esp,12
	
converting:
	mov esi,0
	mov edi,0
	
getting_to_nl:		
	mov ecx ,buffer
	add ecx,esi    ;;ecx = buffer+esi

	cmp byte [ecx],10
	je done2
	push 2
	;push buffer
	push ecx
	push current
	call strncpy
	add esp,12
	
	mov ebx,current
	shl ebx,2
	
	mov edx, [ebp+8]
	add edx,edi	;;edx = x_multi_struct+edi
	push edx  
	;push x_multi_struct
	push str2
	push current
	call sscanf
	add esp,12
	
	add esi,2
	add edi,1
	jmp getting_to_nl
	
	;push x_multi_struct
	;push format
	;call printf
	;add esp,8
	
	 
	
done2:	
	mov esp, ebp
	pop ebp
	ret
		
;------------------------------------------------------------------------------
print_multi:
	push ebp
	mov ebp, esp
	pushad

	mov esi,0	
	mov edx,[ebp+8];; x_len in char** 
	mov edi,[edx] ;;x_len in char*
	movzx eax, byte [edx] 

print:
	mov ebx,[edx+eax]
	cmp eax,-1
	je done
	call printing 
	dec eax
	jmp print


printing:
	pushad
	push ebx
	push format
	call printf
	add esp,4
	add esp,4
	popad
	ret

done:
	popad
	mov esp, ebp
	pop ebp
	ret
	
