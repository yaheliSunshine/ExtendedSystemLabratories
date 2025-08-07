STDOUT EQU 1
STDIN  EQU 0
EOF	EQU -1
section .rodata
	str: db "hello world",10,0
	nl:  db 10,0
section .data
	outfile: dd STDOUT ; int outfile=1
	infile: dd STDIN ; int infile = 0
	buff: db 0
section .text
global _start
global main
global encode
global infile_support
global outfile_support

_start:
	push ebp
	mov ebp, esp
	pushad
	
	call main
	
	popad
	mov esp, ebp
	pop ebp
	ret
	
main:
debug:

print_single_arg:			
	mov ecx,[ebp+8] ;[ebp+8] there is char**
	mov esi,2
here:	
	 mov     eax, 4    
	 mov     ebx, [outfile]
		 
	 mov	edx, 1
	 int 0x80
	 
	 inc ecx
	 cmp byte  [ecx],0 ;lets look if we are in the last byte in arg meaning '\0', this is one cell in the array 
	 je	next_arg
end:	 
	 jmp	here
	 
next_arg:
	 mov     eax, 4    
	 mov     ebx, [outfile]
	 mov     ecx,nl	 
	 mov	edx, 1
	 int 0x80
	inc esi
	mov ecx,[ebp+4*esi]
	cmp  ecx,0		;comparing if the last argument in array of char* is 0, this is one block of the array
	je support		
	jmp end

support:	 
call infile_support
call outfile_support
TA_1B:
	
	mov eax,3
	mov ebx,[infile]
	mov ecx, buff
	mov edx, 1
	int 0x80
	
	cmp eax,0 ;; !!!!!!why not working!!!!
	je finish
	
	call encode
	
	
	
	 ;mov     eax, 4    
	 ;mov     ebx, [outfile]
	 ;mov     ecx,nl	 
	 ;mov	edx, 1
	 ;int 0x80
	 
	mov     eax, 4    
	mov     ebx, [outfile]
	mov	ecx,buff	 
	mov	edx, 1
	 int 0x80
	jmp TA_1B
	
finish:
	mov ebx, 0
	mov eax, 1
	int 0x80	 
	

encode:
	mov ecx,buff
bigger_than_A:  
  	cmp byte [ecx], 'A'
  	;;cmp eax,0
  	jl the_same ; if the character is bigger than A we are we are going to check if it is smaller than z
  	
smaller_than_z:
	;;add eax,'A'
	cmp byte [ecx],'z'
	;;cmp eax,0  	
  	jg the_same  ;if the charcter is more than 'z' we are jumping to to the same, else we inc the letter 	

inc_letter:
inc byte [ecx]
;mov [ecx],eax
mov eax,1
ret

the_same:
mov eax,1
 ret 	


infile_support:
	mov ecx,[ebp+8] ;[ebp+8] there is char**
	mov esi,2
	
find_infile:
	
 	cmp byte [ecx], '-'
 	jnz next_arg_infile
 	inc ecx
 	cmp byte [ecx], 'i'
	jnz next_arg_infile
	inc ecx
	

open_infile:	
	mov eax,5
	mov ebx,ecx
	;add ebx,2
	;mov ebx,ebx
	mov ecx,2|64
	mov edx,0777
	int 0x80
	mov [infile],eax
	mov eax,1
	ret

next_arg_infile:
;cmp ecx,0
;jz end_infile_support
;inc esi
;mov ecx,[ebp+4*esi]
;jmp find_infile

inc esi
mov ecx,[ebp+4*esi]
cmp ecx,0
jz end_infile_support
jmp find_infile


end_infile_support:
ret


outfile_support:
	mov ecx,[ebp+8] ;[ebp+8] there is char**
	mov esi,2
	
find_outfile:
	
 	cmp byte [ecx], '-'
 	jnz next_arg_outfile
 	inc ecx
 	cmp byte [ecx], 'o'
	jnz next_arg_outfile
	inc ecx
	

open_outfile:	
	mov eax,5
	mov ebx,ecx
	;add ebx,2
	;mov ebx,ebx
	mov ecx,2|64
	mov edx,0777
	int 0x80
	mov [outfile],eax
	mov eax,1
	ret

next_arg_outfile:
;cmp ecx,0
;jz end_infile_support
;inc esi
;mov ecx,[ebp+4*esi]
;jmp find_infile

inc esi
mov ecx,[ebp+4*esi]
cmp ecx,0
jz end_outfile_support
jmp find_outfile


end_outfile_support:
ret  
   
      
