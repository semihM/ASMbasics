.code 

; ********** TRICKS **********
COMMENT	@ ClearRegisterToZero

pxor mmx, mmx
pandn mmx, mmx
pcmpgtb mmx,mmx ; OR pcmpgtw OR pcmpgtd

@ ClearRegisterToZerotricks

COMMENT	@ SetBitsOfRegisterOne

pcmpeqb mmx,mmx ; OR pcmpeqw OR pcmpeqd

@ SetBitsOfRegisterOne

COMMENT @ Compliment

pcmpeqb mmx,mmx ; mmx with all bits as 1
pxor mmy, mmx   ; flip bits of mmy using mmx

@ Compliment

COMMENT @ ComparisonOperations
; mmx : first vector, mmy : second vector, mmz : temporary vector

; != Not Equal to
	pcmpeqb mmx, mmy ; find equal to
	pcmpeqb mmz, mmz ; set mmz to all 1s
	pxor mmx, mmz    ; compliment mmx

; >= Greater than or Equal to
	movq mmz, mmx
	pcmpgtb mmx, mmy ; find > in mmx
	pcmpeqb mmz, mmy ; find = in mmx
	por mmx, mmz     ; OR them together

; < Less than
	movq mmz, mmx
	pcmpeqb mmz, mmy ; find =
	pcmpgtb mmx, mmy ; find >
	por mmx, mmz	 ; OR them together
	pcmpeqb mmz, mmz
	pandn mmx, mmz   ; NOT the result

; <= Less than or Equal to
	movq mmz, mmy	  
	pcmpgtb mmz, mmx ; Do the NOT of > by swapping operands
	movq mmx, mmz
@ ComparisonOperations

; ********** END OF TRICKS ************

CheckMMXCapability proc
	; bool CheckMMXCapability()
	push rbx ; save rbx
	pushfq	 ; push the flags

	pop rax	     ; pop them into rax
	mov rbx, rax ; save it to rbx for comparison later

	xor rax, 200000h ; toggle ID flag
	push rax		 ; push toggled flags

	popfq   ; pop them back into flags
	pushfq  ; push the flags

	pop rax		    ; pop them back into rax
	cmp rax, rbx    ; check if ID bit reset
	jz IsNotCapable 

	pop rbx    ; restore rbx
	mov eax, 1 ; cpu is capable of cpuid, function no 1

	cpuid ; call cpuid

	shr rdx, 23  ; shift the MMX bit to right-most position
	and rdx, 1   ; mask out the rest
	mov rax, rdx ; move it into rax
	ret			 ; and return it

	IsNotCapable:
		pop rbx      ; Not capable of CPUID and/or MMX
		xor rax, rax ; reset rax
		ret			 ; and return it

CheckMMXCapability endp

MMXCopy proc
	;void MMXCopy(char* rcx, char* rdx, int byteCount=r8d)
	; An even multiple of 8 bytes are copied with mmx registers and movq
	; Residual bytes copied one at a time with standard mov
	cmp r8d, 0   ; check the count
	jle Finished ; if not positive quit

	xor r9, r9	 ; set r9 to 0, our offset pointer
	mov eax, r8d ; save a copy of the count
	shr r8d, 3   ; divide count by 8

	cmp r8d, 0           ; make sure there's at least 8 bytes to copy
	je NothingLeftForMMX ; if there isn't, use standard mov

	MMXLoop:
		movq mm0, qword ptr [ rdx+r9 ]  ; Copy 8 bytes from *rdx ( source ) into mm0
		movq qword ptr [ rcx+r9 ], mm0 ; copy these from mm0 to *rcx ( destination )

		add r9, 8 ; move offset pointer 8 bytes

		dec r8d ; dec counter
		jnz MMXLoop

	NothingLeftForMMX:
		mov r8d, eax  ; original count back to r8d
		shr r8d, 3	  ; divide by 8
		shl r8d, 3	  ; multiple by 8 to check if there was any remainder

		sub eax, r8d ; check the remainder

		cmp eax, 0  ; if remainder was 0, mmx has copied everything
		jz Finished
	
	ResidualLoop:
		mov r8b, byte ptr [ rdx+r9 ] ; Copy next byte into r8b
		mov byte ptr [ rcx+r9 ], r8b ; Copy r8b to destination

		inc r9  ; move onto the next byte
		dec eax ; dec counter ( this time it's eax )
		jnz ResidualLoop

	Finished:
		emms	; Exit multimedia state, return to x87 FPU to floating point work
		ret

MMXCopy endp

Absolute proc
	; void Absolute(short *arr=rcx, int count=edx)

	cmp edx, 0   ; check if given count is not positive
	jle Finished

	pcmpeqw mm5, mm5  ; mm5 all ones
	pxor mm6, mm6	  ; mm6 all zeros

	pcmpeqw mm4,mm4   ; mm4 all ones 
	psrlw mm4, 15     ; shift right logical to get 4 words with 1 as highest bit

	mov eax, edx    ; save copy of edx
	shr edx, 2      ; divide edx by 4 since there are 4 words per iteration
	jz DealWithResiduals ; if edx becomes 0

	MainLoop:
		movq mm0, qword ptr [rcx] ; get the value
		movq mm1, mm0			  ; keep a copy

		pcmpgtw mm0, mm6		  ; get positive values
		movq mm7, mm0			  ; keep them in mm7
		pand mm0, mm1			  ; mask negatives, only posiives left in mm0
		pxor mm7, mm5		      ; compliment mm7, now it has negative values' bits as FFFF
		pand mm7, mm1			  ; mask positives, only negatives left in mm7
		pxor mm7, mm5			  ; one's compliment, flip all the bits
		paddw mm7, mm4			  ; need to add 1 to all because of the previous operation
		por mm0, mm7			  ; replace zeros in mm0 with new absolute values from mm7

		movq qword ptr [rcx], mm0  ; replace with the absolute value back
		
		add rcx, 8
		dec edx
		jnz MainLoop


	DealWithResiduals:
		and eax, 11b ; check for remainder of count/4 will
		jz Finished

	ResidualsLoop:
		mov dx, word ptr [rcx]

		mov r8w, dx
		neg r8w
		cmp dx, 0
		cmovl dx, r8w
		
		mov word ptr [rcx], dx

		add rcx, 2
		dec eax
		jnz ResidualsLoop

	Finished:
		emms ; Restore FPU state
		ret

Absolute endp

end