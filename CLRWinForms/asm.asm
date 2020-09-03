.data
    align 16
	RCPBOXWIDTH real4  0.0, 0.0, 0.0, 0.0
	ONES	    real4  1.0, 1.0, 1.0, 1.0
	rgb_ceil    real4  255.0, 255.0, 255.0, 255.0
	hsl_mul		real4  240.0, 240.0, 240.0, 0.0
	temphalve   real4  0.5, 0.0, 0.0, 0.0

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

	pixel_4_size dword 12
	pixel_size dword 3


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
	pxor xmm2, xmm2
	pxor xmm3, xmm3

	pinsrb xmm5, byte ptr [r8+rax], 0		; R in xmm5's 1st byte
	pinsrb xmm2, byte ptr [r8+rax+1], 4	    ; G in xmm2's 1st byte
	pinsrb xmm3, byte ptr [r8+rax+2], 8		; B in xmm3's 1st byte
	por xmm5, xmm2			; R G as xmm5's low words
    por xmm5, xmm3			; R G B xmm5's first 3 words

	addps xmm0, xmm5

	Finishedadd0:
		ret
AddOrgToXMM0 endp

AddOrgToXMM1 proc
	; Add the pixel that r10d and ecx point to, from bmpOrg to xmm1
	cmp ecx, 0
	jl OverflowLimiter1
	cmp r10d, 0
	jl OverflowLimiter1

	cmp ecx, [IMG_H]
	jge OverflowLimiter1
	cmp r10d, [IMG_W]
	jge OverflowLimiter1

	mov eax, ecx
	imul eax, [IMG_W]
	add eax, r10d

	mul [COUNT] 	

	pxor xmm5, xmm5
	pxor xmm2, xmm2
	pxor xmm3, xmm3

	pinsrb xmm5, byte ptr [r8+rax], 0		; R in xmm5's 1st byte
	pinsrb xmm2, byte ptr [r8+rax+1], 4	    ; G in xmm2's 1st byte
	pinsrb xmm3, byte ptr [r8+rax+2], 8		; B in xmm3's 1st byte
	por xmm5, xmm2			; R G as xmm5's low words
    por xmm5, xmm3			; R G B xmm5's first 3 words
	
	cvtdq2ps xmm5, xmm5		; dword ints to sp floats

	addps xmm1, xmm5
	jmp Finishedadd1

	OverflowLimiter1:
		pinsrd xmm1, [OVERFLOW], 0
		pshufd xmm1, xmm1, 0
		mulps xmm1, [ONES]
	Finishedadd1:
		ret
AddOrgToXMM1 endp

SubOrgFromXMM1 proc
	cmp ecx, 0
	jl UnderflowLimiter
	cmp r10d, 0
	jl UnderflowLimiter

	cmp ecx, [IMG_H]
	jge UnderflowLimiter
	cmp r10d, [IMG_W]
	jge UnderflowLimiter

	mov eax, ecx
	imul eax, [IMG_W]
	add eax, r10d

	mul [COUNT]

	pxor xmm5, xmm5
	pxor xmm2, xmm2
	pxor xmm3, xmm3

	pinsrb xmm5, byte ptr [r8+rax], 0		; R in xmm5's 1st byte
	pinsrb xmm2, byte ptr [r8+rax+1], 4	    ; G in xmm2's 1st byte
	pinsrb xmm3, byte ptr [r8+rax+2], 8		; B in xmm3's 1st byte
	por xmm5, xmm2			; R G as xmm5's low words
    por xmm5, xmm3			; R G B xmm5's first 3 words
	
	cvtdq2ps xmm5, xmm5		; dword ints to sp floats

	subps xmm1, xmm5
	jmp Finishedsub

	UnderflowLimiter:
		pxor xmm1, xmm1
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
	pxor xmm2, xmm2
	pxor xmm3, xmm3

	mov r11, HBLUR
	
	;comp[n] = HBlur[eax+n] , n = 0,1,2

	pinsrb xmm5, byte ptr [r11+rax], 0		; R in xmm5's 1st byte
	pinsrb xmm2, byte ptr [r11+rax+1], 4	    ; G in xmm2's 1st byte
	pinsrb xmm3, byte ptr [r11+rax+2], 8		; B in xmm3's 1st byte
	por xmm5, xmm2			; R G as xmm5's low words
    por xmm5, xmm3			; R G B xmm5's first 3 words
	
	addps xmm0, xmm5

	Finished_Vadd0:
		ret
AddHBlurToXMM0 endp

AddHBlurToXMM1 proc
	; Add the pixel that r10d and ecx point to, from bmpOrg to xmm1
	; r10d<0 || ecx<0 || ecx>=IMG_W || r10d>=IMG_H
	cmp ecx, 0
	jl OverflowLimiter_Vadd1
	cmp r10d, 0
	jl OverflowLimiter_Vadd1

	cmp ecx, [IMG_W]
	jge OverflowLimiter_Vadd1
	cmp r10d, [IMG_H]
	jge OverflowLimiter_Vadd1

	;ecx = 3*(ecx + r10*IMG_W)
	mov eax, r10d
	imul eax, [IMG_W]
	add eax, ecx

	mul [COUNT] 	

	pxor xmm5, xmm5
	pxor xmm2, xmm2
	pxor xmm3, xmm3

	mov r11, HBLUR
	;comp[n] = HBlur[eax+n] , n = 0,1,2
	pinsrb xmm5, byte ptr [r11+rax], 0		; R in xmm5's 1st byte
	pinsrb xmm2, byte ptr [r11+rax+1], 4	    ; G in xmm2's 1st byte
	pinsrb xmm3, byte ptr [r11+rax+2], 8		; B in xmm3's 1st byte
	por xmm5, xmm2			; R G as xmm5's low words
    por xmm5, xmm3			; R G B xmm5's first 3 words
	
	cvtdq2ps xmm5, xmm5		; dword ints to sp floats

	addps xmm1, xmm5
	jmp Finished_Vadd1

	OverflowLimiter_Vadd1:
		pinsrd xmm1, [OVERFLOW], 0
		pshufd xmm1, xmm1, 0
		mulps xmm1, [ONES]
	Finished_Vadd1:
		ret
AddHBlurToXMM1 endp

SubHBlurFromXMM1 proc
	; r10d<0 || ecx<0 || ecx>=IMG_W || r10d>=IMG_H
	cmp ecx, 0
	jl UnderflowLimiter_Vsub
	cmp r10d, 0
	jl UnderflowLimiter_Vsub

	cmp ecx, [IMG_W]
	jge UnderflowLimiter_Vsub
	cmp r10d, [IMG_H]
	jge UnderflowLimiter_Vsub

	;ecx = 3*(ecx + r10*IMG_W)
	mov eax, r10d
	imul eax, [IMG_W]
	add eax, ecx

	mul [COUNT]

	pxor xmm5, xmm5
	pxor xmm2, xmm2
	pxor xmm3, xmm3

	mov r11, HBLUR
	
	;comp[n] = HBlur[eax+n] , n = 0,1,2
	pinsrb xmm5, byte ptr [r11+rax], 0		; R in xmm5's 1st byte
	pinsrb xmm2, byte ptr [r11+rax+1], 4	    ; G in xmm2's 1st byte
	pinsrb xmm3, byte ptr [r11+rax+2], 8		; B in xmm3's 1st byte
	por xmm5, xmm2			; R G as xmm5's low words
    por xmm5, xmm3			; R G B xmm5's first 3 words
	
	cvtdq2ps xmm5, xmm5		; dword ints to sp floats

	subps xmm1, xmm5
	jmp Finished_Vsub

	UnderflowLimiter_Vsub:
		pxor xmm1, xmm1
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

INSERT_TO_XMM macro baseptr, offsetptr, byteoffset
	pinsrb	xmm0, byte ptr[baseptr+offsetptr+byteoffset*3], byteoffset*4
	pinsrb	xmm1, byte ptr[baseptr+offsetptr+byteoffset*3+1], byteoffset*4
	pinsrb	xmm2, byte ptr[baseptr+offsetptr+byteoffset*3+2], byteoffset*4
endm

EXEC_INSERT proc 
	pxor xmm0,xmm0
	pxor xmm1,xmm1
	pxor xmm2,xmm2
	INSERT_TO_XMM rdx, r10, 0
	INSERT_TO_XMM rdx, r10, 1
	INSERT_TO_XMM rdx, r10, 2
	INSERT_TO_XMM rdx, r10, 3
	cvtdq2ps xmm0,xmm0	
	cvtdq2ps xmm1,xmm1
	cvtdq2ps xmm2,xmm2
	ret
EXEC_INSERT endp

macro_muldiv macro operation, source, mem

    pxor xmm4, xmm4
	pxor xmm5, xmm5

	pinsrd xmm4, source, 0
	cvtdq2ps xmm4, xmm4
	
	pinsrd xmm5, [mem], 0
	cvtdq2ps xmm5, xmm5 
	operation xmm4, xmm5
	roundps xmm4, xmm4, 1
	
	cvtps2dq xmm4, xmm4
	pextrd source, xmm4, 0

endm

ASMGreyscale proc
; extern "C" void ASMNegativeIMG(unsigned char* scan0,
	;								 unsigned char* org,
	;							     int size);
	; 		scan0 : RCX , org : RDX , size : R8D
	;
	;		float total;
	;		unsigned char greyed;
	;		float org[3];
	;		for (int i = 0; i < imgSize; i+=3)
	;		{	
	;			org[0] = (float)original[i];
	;			org[1] = (float)original[i+1];
	;			org[2] = (float)original[i+2];
	;			total = org[0]+ org[1]+ org[2];

	;			if (total == 0){continue;}
	;			greyed = (unsigned char)( ((org[0] / total) * org[0]) + ((org[1] / total) * org[1]) + ((org[2] / total) * org[2]));
	;			bmp[i] = greyed;
	;			bmp[i+1] = greyed;
	;			bmp[i+2] = greyed;
		
	xor r10,r10	
	pxor xmm3,xmm3

	mov r9, r8 ; copy of size

	macro_muldiv divps, r9d, pixel_4_size
	macro_muldiv mulps, r9d, pixel_4_size

	cmp r9, r8
	je XMMOnly   ;size divisible by 4
	
	sub r8, r9

	macro_muldiv divps, r9d, pixel_4_size
	macro_muldiv divps, r8d, pixel_size

	MainLoop:
		; rcx scan0
		; r10 offset
		; r9 counter
		call EXEC_INSERT	; insert 4 pixels' rgb values to xmm0-1-2 regs as dwords

		addps xmm3, xmm2	; rgb sums for each pixel in dwords
		addps xmm3, xmm1
		addps xmm3, xmm0

		mulps xmm0, xmm0
		mulps xmm1, xmm1
		mulps xmm2, xmm2

		rcpps xmm3, xmm3

		addps xmm0, xmm1
		addps xmm0, xmm2

		mulps xmm0, xmm3	; grey scale values in xmm0

		cvtps2dq xmm0, xmm0

		pextrb byte ptr[rcx+r10], xmm0, 0
		pextrb byte ptr[rcx+r10+3], xmm0, 4
		pextrb byte ptr[rcx+r10+6], xmm0, 8
		pextrb byte ptr[rcx+r10+9], xmm0, 12

		
		pextrb byte ptr[rcx+r10+1], xmm0, 0
		pextrb byte ptr[rcx+r10+4], xmm0, 4
		pextrb byte ptr[rcx+r10+7], xmm0, 8
		pextrb byte ptr[rcx+r10+10], xmm0, 12
		
		pextrb byte ptr[rcx+r10+2], xmm0, 0
		pextrb byte ptr[rcx+r10+5], xmm0, 4
		pextrb byte ptr[rcx+r10+8], xmm0, 8
		pextrb byte ptr[rcx+r10+11], xmm0, 12

		add r10, 12
		dec r9d
		jnz MainLoop

	ResidualLoop:
		pxor xmm0,xmm0
		pxor xmm1,xmm1
		pxor xmm2,xmm2

		INSERT_TO_XMM rdx, r10, 0

		cvtdq2ps xmm0,xmm0	
		cvtdq2ps xmm1,xmm1
		cvtdq2ps xmm2,xmm2

		addps xmm3, xmm2	; rgb sums for each pixel in dwords
		addps xmm3, xmm1
		addps xmm3, xmm0

		mulps xmm0, xmm0
		mulps xmm1, xmm1
		mulps xmm2, xmm2

		rcpps xmm3, xmm3

		addps xmm0, xmm1
		addps xmm0, xmm2

		mulps xmm0, xmm3	; grey scale values in xmm0

		cvtps2dq xmm0, xmm0

		pextrb byte ptr[rcx+r10], xmm0, 0
		pextrb byte ptr[rcx+r10+1], xmm0, 0
		pextrb byte ptr[rcx+r10+2], xmm0, 0

		add r10, 3
		dec r8d
		jnz ResidualLoop
		ret

	XMMOnly:
		macro_muldiv divps, r9d, pixel_4_size
	XMMOnlyLoop:
		; rcx scan0
		; r10 offset
		; r9 counter
		call EXEC_INSERT	; insert 4 pixels' rgb values to xmm0-1-2 regs as dwords

		addps xmm3, xmm2	; rgb sums for each pixel in dwords
		addps xmm3, xmm1
		addps xmm3, xmm0

		mulps xmm0, xmm0
		mulps xmm1, xmm1
		mulps xmm2, xmm2

		rcpps xmm3, xmm3

		addps xmm0, xmm1
		addps xmm0, xmm2

		mulps xmm0, xmm3	; grey scale values in xmm0

		cvtps2dq xmm0, xmm0

		pextrb byte ptr[rcx+r10], xmm0, 0
		pextrb byte ptr[rcx+r10+3], xmm0, 4
		pextrb byte ptr[rcx+r10+6], xmm0, 8
		pextrb byte ptr[rcx+r10+9], xmm0, 12

		
		pextrb byte ptr[rcx+r10+1], xmm0, 0
		pextrb byte ptr[rcx+r10+4], xmm0, 4
		pextrb byte ptr[rcx+r10+7], xmm0, 8
		pextrb byte ptr[rcx+r10+10], xmm0, 12
		
		pextrb byte ptr[rcx+r10+2], xmm0, 0
		pextrb byte ptr[rcx+r10+5], xmm0, 4
		pextrb byte ptr[rcx+r10+8], xmm0, 8
		pextrb byte ptr[rcx+r10+11], xmm0, 12

		add r10, 12
		dec r9d
		jnz XMMOnlyLoop
		ret

ASMGreyscale endp

ASMrgb2hsl proc
	; extern "C" void ASMrgb2hsl(float r:xmm0,
	;						  float g:xmm1,
	;						  float b:xmm2,
	;						  unsigned char* hsl:r9);

	;	double rN, gN, bN, max, min, delta, h, s ,l;
	;	rN = r / 255; gN = g / 255; bN = b / 255;
			
	;	max = rN + (gN - rN) * (gN > rN);
	;	max = max + (bN - max) * (bN > max);
	;	min = rN + (gN - rN) * (gN < rN); 
	;	min = min + (bN - min) * (bN < min);

	;	l = (max + min) * 0.5;

	;	if(max == min)
	;	{
	;		hsl[0] = 0;
	;		hsl[1] = 0;
	;		hsl[2] = (unsigned char)(l * 240.0);
	;	}
	;	else 
	;	{
	;		delta = max - min;
	;		s = l > 0.5 ? delta / (2.0 - max - min) : delta / (max + min);
				
	;		if (max == rN)
	;			h = (gN - bN) / delta + (gN < bN ? 6.0 : 0);
	;		else if (max == gN)
	;			h = (bN - rN) / delta + 2.0;
	;		else if (max == bN)
	;			h = (rN - gN) / delta + 4.0;
	;		else
	;			h = 0;
			
	;		h /= 6.0;
	;		hsl[0] = (unsigned char)(h*240.0);
	;		hsl[1] = (unsigned char)(s*240.0);
	;		hsl[2] = (unsigned char)(l*240.0);
	
	; Store RGB in xmm0
	pslldq xmm1, 4
	pslldq xmm2, 8
	por xmm1, xmm2
	por xmm0, xmm1

	; Clear old regs
	pxor xmm1,xmm1
	pxor xmm2,xmm2

	; Normalise
	divps xmm0, xmmword ptr [rgb_ceil]
	
	pshufd xmm1, xmm0 , 11111101b ; xmm0 : _-b-g-r , xmm1: _-_-_-g
	pshufd xmm2, xmm0 , 11111101b ; xmm0 : _-b-g-r , xmm2: _-_-_-g
	minps xmm1, xmm0			  
	maxps xmm2, xmm0
	pshufd xmm3, xmm0 , 11111110b ; xmm0 : _-b-g-r , xmm3: _-_-_-b
	pshufd xmm4, xmm0 , 11111110b ; xmm0 : _-b-g-r , xmm4: _-_-_-b
	minps xmm1, xmm3  ; min in xmm1 first dword
	maxps xmm2, xmm4  ; max in xmm2 first dword
	
	xor r10,r10
	pxor xmm5,xmm5

	pextrd r10d, xmm2, 0
	pinsrd xmm5, r10d, 0	; max copy in xmm5 first dword
	subps xmm5, xmm1	; delta in xmm5 first dword

	pslldq xmm5, 8
	pslldq xmm2, 12
	psrldq xmm2, 8

	por xmm2, xmm5
	por xmm1, xmm2		; delta, max, min in xmm1

	; normalisez values in xmm0
	; max+min, delta, max, min  as xmm1 dwords

	pxor xmm2, xmm2  ; use xmm2 for hsl calculations

	pshufd xmm2, xmm1, 11111101b ; max as lowest dword of xmm2
	addps xmm2, xmm1			; max + min in xmm2's lowest dword

	pextrd r10d, xmm2, 0		
	pinsrd xmm1, r10d, 3		; store copy of max+min for future

	mulps xmm2, xmmword ptr [temphalve] 
	
	pslldq xmm2, 8  ; l value in 3rd dword of xmm2

	; check delta
	pextrd r10d, xmm1, 2
	cmp r10d, 0
	je DeltaZero

	pshufd xmm2, xmm2, 11000110b ; l as lowest dword 
	mov r8d, 3f000000h ; 0.5 as ieee754 sp fp
	pinsrd xmm3, r8d, 0
	
	comiss xmm2, xmm3  ; compare l with 0.5
	jc LessThanHalf	   ; jump if l<0.5
	; set s

	pshufd xmm2, xmm2, 11000110b ; put l back to 3rd dword
	mov r8d, 40000000h  ;2.0 as sp fp
	pinsrd xmm3, r8d, 3 ;2.0 in xmm3's highest dword

	subps xmm3, xmm1	; 2 - max - min in xmm3's highest dword
	rcpps xmm3, xmm3    ; get reciprocal
	psrldq xmm3, 4		; shift right a dword to align with delta
	mulps xmm3, xmm1	; delta*(1/(2-min-max)) in as 3rd dword in xmm3
	pextrd r8d, xmm3, 2
	pinsrd xmm2, r8d, 1 ; xmm2 with 0-l-s-0 dwords
	jmp H_set ; last one is h

	LessThanHalf:
		;set s
		pshufd xmm2, xmm2, 11000110b ; put l back to 3rd dword
		rcpps xmm3, xmm1 ; reciprocals of max+min and the unused rest in xmm3
		psrldq xmm3, 4	; shift right a dword to align with delta
		mulps xmm3, xmm1	; delta*(1/(min+max)) in as 3rd dword in xmm3
		pextrd r8d, xmm3, 2
		pinsrd xmm2, r8d, 1 ; xmm2 with 0-l-s-0 dwords

	H_set:
		; xmm3 ' s lowest dword whill be h
	    pxor xmm3, xmm3
		pxor xmm4, xmm4

		pextrd r8d, xmm1, 1 ; max in r8d
		pextrd eax, xmm0, 0 ; rN in eax
		pextrd ecx, xmm0, 1 ; gN in ecx
		pextrd edx, xmm0, 2 ; bN in edx
		
		cmp r8d, eax
		je max_rN
		cmp r8d, ecx
		je max_gN
		cmp r8d, edx
		je max_bN

		jmp end_hsl
		
		max_rN:
			rcpps xmm1, xmm1  ; delta's reciprocal is needed
			psrldq xmm1, 8	  ; 1/delta as first dword
			pinsrd xmm3, ecx, 0
			pinsrd xmm4, edx, 0
			comiss xmm3, xmm4
			jc add6				; jump to add 6 if ecx<edx
			subps xmm3, xmm4
			mulps xmm3, xmm1
			jmp end_hsl
			
			add6:
				subps xmm3, xmm4
				mulps xmm3, xmm1

				mov r8d, 40C00000h ; 6.0 as sp fp
				pinsrd xmm3, r8d , 1
				haddps xmm3, xmm3  ; add 6.0 to h
				jmp end_hsl
				
		max_gN:
			rcpps xmm1, xmm1  ; delta's reciprocal is needed
			psrldq xmm1, 8	  ; 1/delta as first dword
			pinsrd xmm3, edx, 0
			pinsrd xmm4, eax, 0
			subps xmm3, xmm4
			mulps xmm3, xmm1

			mov r8d, 40000000h ; 2.0 as sp fp
			pinsrd xmm3, r8d , 1
			haddps xmm3, xmm3
			jmp end_hsl
		
		max_bN:
			rcpps xmm1, xmm1  ; delta's reciprocal is needed
			psrldq xmm1, 8	  ; 1/delta as first dword
			pinsrd xmm3, eax, 0
			pinsrd xmm4, ecx, 0
			subps xmm3, xmm4
			mulps xmm3, xmm1

			mov r8d, 40800000h ; 4.0 as sp fp
			pinsrd xmm3, r8d , 1
			haddps xmm3, xmm3
			jmp end_hsl
		
		end_hsl:
			mov r8d, 3E2AAAABh ; 1/6 as sp fp
			pinsrd xmm4, r8d, 0
			mulps xmm3, xmm4
			addps xmm2, xmm3
			mulps xmm2, xmmword ptr [hsl_mul]

			roundps xmm2, xmm2, 0
			cvtps2dq xmm2, xmm2

			pextrb byte ptr[r9], xmm2, 0
			pextrb byte ptr[r9+1], xmm2, 4
			pextrb byte ptr[r9+2], xmm2, 8
		
			ret

	DeltaZero:
		; h=0, s=0, l= (unsigned char)(l*240.0)
		mulps xmm2, xmmword ptr [hsl_mul]
		roundps	xmm2, xmm2, 0
		cvtps2dq xmm2, xmm2
		pextrb byte ptr[r9], xmm2, 0
		pextrb byte ptr[r9+1], xmm2, 4
		pextrb byte ptr[r9+2], xmm2, 8
		ret

ASMrgb2hsl endp

end