.code 

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

end