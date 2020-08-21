.data
    align 16
	RCPBOXWIDTH real4 0.0, 0.0, 0.0, 0.0
	ONES	    real4 0.0, 1.0, 1.0, 1.0
	
	HBLUR	  qword	 ?
	BLURWIDTH dword  ?

	IMG_H dd ?
	IMG_W dd ?

	COUNT dd 3

	basepointer qword ?

	R byte ?
	G byte ?
	B byte ?

	OVERFLOW dword 00FFh
	UNDERFLOW dword 0000h
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

	je Unchecked

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

UpdateR11 proc
	cmp r11, 300h
	cmovg r11d, [UNDERFLOW]
	cmp r11, 0FFh
	cmovg r11d, [OVERFLOW]
	ret
UpdateR11 endp
 
AddOrgToXMM0 proc
	; Add the pixel that r10d and ecx point to, from bmpOrg to xmm0
	cmp ecx, 0
	jl Finishedadd0
	cmp r10d, 0
	jl Finishedadd0

	cmp ecx, [IMG_H]
	jge Finishedadd0
	cmp r10d, [IMG_W]
	jge Finishedadd0

	mov eax, ecx
	imul eax, [IMG_W]
	add eax, r10d

	mul [COUNT]
	
	pxor xmm5, xmm5
	pxor xmm6, xmm6
	pxor xmm7, xmm7

	pinsrb xmm5, byte ptr [r8+rax], 0		; R in xmm5's 1st byte
	pinsrb xmm6, byte ptr [r8+rax+1], 4	    ; G in xmm6's 1st byte
	pinsrb xmm7, byte ptr [r8+rax+2], 8		; B in xmm7's 1st byte
	por xmm5, xmm6			; R G as xmm5's low words
    por xmm5, xmm7			; R G B xmm5's first 3 words

	addps xmm0, xmm5

	Finishedadd0:
		ret
AddOrgToXMM0 endp

AddOrgToXMM1 proc
	; Add the pixel that r10d and ecx point to, from bmpOrg to xmm1
	cmp ecx, 0
	jl Finishedadd1
	cmp r10d, 0
	jl Finishedadd1

	cmp ecx, [IMG_H]
	jge Finishedadd1
	cmp r10d, [IMG_W]
	jge Finishedadd1

	mov eax, ecx
	imul eax, [IMG_W]
	add eax, r10d

	mul [COUNT] 	

	pxor xmm5, xmm5
	pxor xmm6, xmm6
	pxor xmm7, xmm7

	pinsrb xmm5, byte ptr [r8+rax], 0		; R in xmm5's 1st byte
	pinsrb xmm6, byte ptr [r8+rax+1], 4	    ; G in xmm6's 1st byte
	pinsrb xmm7, byte ptr [r8+rax+2], 8		; B in xmm7's 1st byte
	por xmm5, xmm6			; R G as xmm5's low words
    por xmm5, xmm7			; R G B xmm5's first 3 words
	
	cvtdq2ps xmm5, xmm5		; dword ints to sp floats

	addps xmm1, xmm5

	Finishedadd1:
		ret
AddOrgToXMM1 endp

SubOrgFromXMM1 proc
	cmp ecx, 0
	jl Finishedsub
	cmp r10d, 0
	jl Finishedsub

	cmp ecx, [IMG_H]
	jge Finishedsub
	cmp r10d, [IMG_W]
	jge Finishedsub

	mov eax, ecx
	imul eax, [IMG_W]
	add eax, r10d

	mul [COUNT]

	pxor xmm5, xmm5
	pxor xmm6, xmm6
	pxor xmm7, xmm7

	pinsrb xmm5, byte ptr [r8+rax], 0		; R in xmm5's 1st byte
	pinsrb xmm6, byte ptr [r8+rax+1], 4	    ; G in xmm6's 1st byte
	pinsrb xmm7, byte ptr [r8+rax+2], 8		; B in xmm7's 1st byte
	por xmm5, xmm6			; R G as xmm5's low words
    por xmm5, xmm7			; R G B xmm5's first 3 words
	
	cvtdq2ps xmm5, xmm5		; dword ints to sp floats

	subps xmm1, xmm5

	Finishedsub:
		ret
SubOrgFromXMM1 endp

WriteXMM0ToHBlur proc
	cmp ecx, 0
	jl Finishedwrite
	cmp r10d, 0
	jl Finishedwrite

	cmp ecx, [IMG_H]
	jge Finishedwrite
	cmp r10d, [IMG_W]
	jge Finishedwrite

	mov eax, ecx
	imul eax, [IMG_W]
	add eax, r10d

	mul [COUNT]

	push rcx
	mov rcx, HBLUR

	; We use dword INT versions of XMM0 which are stored in XMM4
	;Red
	pextrd r11d	, xmm4, 0
	call UpdateR11
	mov byte ptr[rcx+rax], r11b

	;Green
	pextrd r11d	, xmm4, 1
	call UpdateR11
	mov byte ptr[rcx+rax+1], r11b

	;Blue
	pextrd r11d	, xmm4, 2
	call UpdateR11
	mov byte ptr[rcx+rax+2], r11b
	
	pop rcx

	Finishedwrite:
		ret
WriteXMM0ToHBlur endp

AddHBlurToXMM0 proc
	; Add the pixel that r10d and ecx point to, from bmpOrg to xmm0
	;ecx<0 || ecx>= IMG_W || r10d<0 || r10d>= IMG_H

	cmp ecx, 0
	jl Finished_Vadd0
	cmp r10d, 0
	jl Finished_Vadd0

	cmp ecx, [IMG_W]
	jge Finished_Vadd0
	cmp r10d, [IMG_H]
	jge Finished_Vadd0

	;ecx = 3*(ecx + r10*IMG_W)
	mov eax, r10d		
	imul eax, [IMG_W]
	add eax, ecx
	mul [COUNT]
	
	pxor xmm5, xmm5
	pxor xmm6, xmm6
	pxor xmm7, xmm7

	mov r11, HBLUR
	
	;comp[n] = HBlur[eax+n] , n = 0,1,2

	pinsrb xmm5, byte ptr [r11+rax], 0		; R in xmm5's 1st byte
	pinsrb xmm6, byte ptr [r11+rax+1], 4	    ; G in xmm6's 1st byte
	pinsrb xmm7, byte ptr [r11+rax+2], 8		; B in xmm7's 1st byte
	por xmm5, xmm6			; R G as xmm5's low words
    por xmm5, xmm7			; R G B xmm5's first 3 words
	
	addps xmm0, xmm5

	Finished_Vadd0:
		ret
AddHBlurToXMM0 endp

AddHBlurToXMM1 proc
	; Add the pixel that r10d and ecx point to, from bmpOrg to xmm1
	; r10d<0 || ecx<0 || ecx>=IMG_W || r10d>=IMG_H
	cmp ecx, 0
	jl Finished_Vadd1
	cmp r10d, 0
	jl Finished_Vadd1

	cmp ecx, [IMG_W]
	jge Finished_Vadd1
	cmp r10d, [IMG_H]
	jge Finished_Vadd1

	;ecx = 3*(ecx + r10*IMG_W)
	mov eax, r10d
	imul eax, [IMG_W]
	add eax, ecx

	mul [COUNT] 	

	pxor xmm5, xmm5
	pxor xmm6, xmm6
	pxor xmm7, xmm7

	mov r11, HBLUR
	;comp[n] = HBlur[eax+n] , n = 0,1,2
	pinsrb xmm5, byte ptr [r11+rax], 0		; R in xmm5's 1st byte
	pinsrb xmm6, byte ptr [r11+rax+1], 4	    ; G in xmm6's 1st byte
	pinsrb xmm7, byte ptr [r11+rax+2], 8		; B in xmm7's 1st byte
	por xmm5, xmm6			; R G as xmm5's low words
    por xmm5, xmm7			; R G B xmm5's first 3 words
	
	cvtdq2ps xmm5, xmm5		; dword ints to sp floats

	addps xmm1, xmm5

	Finished_Vadd1:
		ret
AddHBlurToXMM1 endp

SubHBlurFromXMM1 proc
	; r10d<0 || ecx<0 || ecx>=IMG_W || r10d>=IMG_H
	cmp ecx, 0
	jl Finished_Vsub
	cmp r10d, 0
	jl Finished_Vsub

	cmp ecx, [IMG_W]
	jge Finished_Vsub
	cmp r10d, [IMG_H]

	jge Finished_Vsub
	;ecx = 3*(ecx + r10*IMG_W)
	mov eax, r10d
	imul eax, [IMG_W]
	add eax, ecx

	mul [COUNT]

	pxor xmm5, xmm5
	pxor xmm6, xmm6
	pxor xmm7, xmm7

	mov r11, HBLUR
	
	;comp[n] = HBlur[eax+n] , n = 0,1,2
	pinsrb xmm5, byte ptr [r11+rax], 0		; R in xmm5's 1st byte
	pinsrb xmm6, byte ptr [r11+rax+1], 4	    ; G in xmm6's 1st byte
	pinsrb xmm7, byte ptr [r11+rax+2], 8		; B in xmm7's 1st byte
	por xmm5, xmm6			; R G as xmm5's low words
    por xmm5, xmm7			; R G B xmm5's first 3 words
	
	cvtdq2ps xmm5, xmm5		; dword ints to sp floats

	subps xmm1, xmm5

	Finished_Vsub:
		ret
SubHBlurFromXMM1 endp

WriteHBlurToScan0 proc
	; ecx<0 || r10d<0 || ecx>=IMG_W || r10d>=IMG_H
	cmp ecx, 0
	jl Finished_Vwrite
	cmp r10d, 0
	jl Finished_Vwrite

	cmp ecx, [IMG_W]
	jge Finished_Vwrite
	cmp r10d, [IMG_H]
	jge Finished_Vwrite

	;ecx = ecx + r10d*IMG_W 
	;ecx = ecx*3
	mov eax, r10d
	imul eax, [IMG_W]
	add eax, ecx

	mul [COUNT]

	; We use dword INT versions of XMM0 which are stored in XMM4
	;Red
	pextrd r11d	, xmm4, 0
	call UpdateR11
	mov byte ptr[r9+rax], r11b

	;Green
	pextrd r11d	, xmm4, 1
	call UpdateR11
	mov byte ptr[r9+rax+1], r11b

	;Blue
	pextrd r11d	, xmm4, 2
	call UpdateR11
	mov byte ptr[r9+rax+2], r11b

	Finished_Vwrite:
		ret
WriteHBlurToScan0 endp

ASMBlurIMG proc
	;extern "C" void ASMBlurIMG(int height=rcx,
	;						    int width=rdx,
	;							unsigned char* bmpOrg=r8,
	;							unsigned char* bmpDataScan0=r9,
	;							unsigned char* hblur, int blurWidth)

	push rbp
	mov rbp, rsp

	mov [IMG_H], ecx
	mov [IMG_W], edx

	mov rax, qword ptr[rbp+48]
	mov HBLUR, rax


	mov eax, dword ptr[rbp+56]
	mov BLURWIDTH, eax

	; Calculate 1.0f/(blurWidth * 2.0f + 1.0f)
	cvtsi2ss xmm0, eax	
	shufps xmm0, xmm0, 0	; broadcast to all 4 floats
	addps xmm0, xmm0		
	addps xmm0, xmmword ptr [ONES]
	rcpps xmm0,	xmm0
	movaps [RCPBOXWIDTH], xmm0

	xor ecx, ecx
	HBlur_YLoop:
		;x is r10d
		;y is ecx
		xorps xmm0, xmm0
		
		mov r10d, [BLURWIDTH]
		inc r10d
		neg r10d

		HBlur_XLoop_1:
			;avg in xmm0
			;tmp in xmm1
			call AddOrgToXMM0

			inc r10d
			cmp r10d, [BLURWIDTH]
			jl HBlur_XLoop_1
		
		cvtdq2ps xmm0, xmm0
		mulps xmm0, xmmword ptr [RCPBOXWIDTH]

		xor r10, r10
		HBlur_XLoop_2:
			xorps xmm1, xmm1

			sub r10d, [BLURWIDTH]
			dec r10d

		    call SubOrgFromXMM1

			add r10d, [BLURWIDTH]
			inc r10d
			add r10d, [BLURWIDTH]
			
			call AddOrgToXMM1

			sub r10d, [BLURWIDTH]

			mulps xmm1, xmmword ptr [RCPBOXWIDTH]

			addps xmm0, xmm1
			
			roundps  xmm4, xmm0, 1 
			cvtps2dq xmm4, xmm4		;xmm4 to floor round floats, so we can keep xmm0 floats
			call WriteXMM0ToHBlur

			inc r10d
			cmp r10d, [IMG_W]
			jl HBlur_XLoop_2
		
		
		inc ecx
		cmp ecx, [IMG_H]
		jl HBlur_YLoop

	xor r10d, r10d
	xor ecx, ecx
	VBlur_YLoop:
		;x is r10d
		;y is ecx
		xorps xmm0, xmm0
		
		mov r10d, [BLURWIDTH]
		inc r10d
		neg r10d

		VBlur_XLoop_1:
			;avg sum in xmm0
			;ecx = 3*(ecx + r10*IMG_W)
			;comp[n] = HBlur[ecx+n] , n = 0,1,2
			call AddHBlurToXMM0
			
			inc r10d
			cmp r10d, [BLURWIDTH]
			jl VBlur_XLoop_1

		cvtdq2ps xmm0, xmm0		; xmm0 dword ints to SP floats
		mulps xmm0, xmmword ptr [RCPBOXWIDTH]

		xor r10, r10
		VBlur_XLoop_2:
			xorps xmm1, xmm1

			sub r10d, [BLURWIDTH]
			dec r10d

		    call SubHBlurFromXMM1

			add r10d, [BLURWIDTH]
			inc r10d
			add r10d, [BLURWIDTH]
			
			call AddHBlurToXMM1

			sub r10d, [BLURWIDTH]

			mulps xmm1, xmmword ptr [RCPBOXWIDTH]

			addps xmm0, xmm1

			roundps  xmm4, xmm0, 1 
			cvtps2dq xmm4, xmm4		;xmm4 to floor round floats, so we can keep xmm0 floats
			call WriteHBlurToScan0

			inc r10d
			cmp r10d, [IMG_H]
			jl VBlur_XLoop_2
		
		
		inc ecx
		cmp ecx, [IMG_W]
		jl VBlur_YLoop
	
	mov rsp, rbp
	pop rbp

	ret
ASMBlurIMG endp
	
end