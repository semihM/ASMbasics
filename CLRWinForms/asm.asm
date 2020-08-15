.data



.code

ASMAdjustBrightness proc
	; extern "C" void ASMAdjustBrightness(unsigned char* bmpDataScan0,unsigned char* bmpOrg,short val,int imgsizeinbytes);
	; bmpDataScan0 : RCX , bmpOrg : RDX , val : R8W , imgsizeinbytes : R9D
	
	mov r10, 0 ; offset pointer to 0
	cmp r8w, 0 ; check if - or +
	jl Subtract

	mov r11w, 0ffffh ; set overflow value to 255

	MainLoopAdd:
		mov al, byte ptr [rdx + r10] ; read next byte from original
		add al, r8b ; add brightness
		cmovc ax, r11w ; set to 255 on overflow

		mov byte ptr [rcx + r10] , al ; store answer in Scan0

		inc r10 ; move to next byte
		dec r9d ; decrement the counter
		jnz MainLoopAdd ; jump if there's more
		
		ret
	
	Subtract:
		mov r11w, 0 ; move 0 into the overflow
		neg r8w ; reverse the sign to be positive
	
	MainLoopSub:
		mov al, byte ptr [rdx + r10] ; read next byte from original
		sub al, r8b ; subtract brightness
		cmovc ax, r11w ; set to 255 on overflow

		mov byte ptr [rcx + r10] , al ; store answer in Scan0

		inc r10 ; move to next byte
		dec r9d ; decrement the counter
		jnz MainLoopSub ; jump if there's more
	
	ret
ASMAdjustBrightness endp

ASMNegativeIMG proc
	; extern "C" void ASMNegativeIMG(unsigned char* bmpDataScan0,
	;								 unsigned char* bmpOrg,
	;							     int imgsizeinbytes);
	; bmpDataScan0 : RCX , bmpOrg : RDX , imgsizeinbytes : R8D, checked : r9
	
	mov r10, 0 ; offset pointer to 0
	cmp r9b, 0 ; check if negative is not checked

	jz Unchecked

	Checked:
		mov r11b, 0ffh;
		sub r11b, byte ptr [rdx + r10] ; negate then

		mov byte ptr [rcx + r10] , r11b ; store answer in Scan0

		inc r10 ; move to next byte
		dec R8d ; decrement the counter
		jnz Checked ; jump if there's more
		ret

	Unchecked:
		mov al, byte ptr [rdx + r10]
		mov byte ptr [rcx + r10] , al ; restore from original

		inc r10 ; move to next byte
		dec R8d ; decrement the counter
		jnz Unchecked ; jump if there's more
		ret

ASMNegativeIMG endp

end