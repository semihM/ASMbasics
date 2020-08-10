.data
vendorString db 12 dup(0), 0

.code

GetCPUIDSupport proc
	; bool GetCPUIDSupport()
	; Check if CPU has CPUID instruction by setting ID flag (bit 21) to 1
	push rbx	 ; Save caller's rbx
	pushfq		 ; push flags
	pop rax      ; pop it into rax
	mov rbx, rax ; save it to rbx

	xor rax, 200000h ; set ID flag ( bit 21 ) to 1
	push rax ; push toggled flags

	popfq		 ; pop toggled flags back into flags register
	pushfq		 ; push the flags again

	pop rax		 ; pop the flags into eax again
	cmp rax, rbx ; compare to the earlier version

	jz No_CPUID

	pop rbx		; restore caller's rbx
	mov rax, 1  ; CPUID is recognized
	ret			; return true, eax = 1

	No_CPUID:
		pop rbx    ; restore caller's rbx
		mov rax, 0 ; CPUID is NOT recognized
		ret        ; return false, eax = 0

GetCPUIDSupport endp

GetVendorString proc
	; char* GetVendorString()
	;CPUID_0000_0000_EBX:EDX:ECX
	push rbx ; save rbx
	
	mov eax, 0			  ; move function number 0 to eax
	cpuid				  ; call cpuid
	lea rax, vendorString ;load address of *vendorString into rax

	mov dword ptr [rax], ebx	 ; 1st 4 bytes are in ebx
	mov dword ptr [rax + 4], edx ; 2nd 4 bytes are in edx
	mov dword ptr [rax + 8], ecx ; 3rd 4 bytes are in ecx

	pop rbx		;restore caller's rbx
	ret			; return rax pointing to *vendorString

GetVendorString endp

GetLargestStandardFunction proc
	; int GetLargestStandardFunction
	; CPUID_0000_0000_EAX is the largest standard function my CPU knows
	push rbx	; save caller's rbx

	mov eax,0   ; move function 0 into eax
	cpuid		; call cpuid

	pop rbx		; restore rbx
	ret			; return largest standard function eax

GetLargestStandardFunction endp

GetLogicalProcessorCount proc
	;int GetLogicalProcessorCount()
	; CPUID_0000_0001_EBX[bits 23:16] 
	push rbx ; save caller's rbx

	mov eax, 1         ; function number is 1
	cpuid		       ; call cpuid
	and ebx, 00ff0000h ; mask out all the bits other than 16 to 23
	mov eax, ebx	   ; move the bits from 16 to 23 to eax

	pop rbx			   ; restore rbx
	shr eax, 16        ; shift the answer right so we can print it out
	ret 

GetLogicalProcessorCount endp

DetectFeatureEDX proc
	; bool DetectFeatureEDX(char bitNumber = cl)
	; CPUID_0000_0001_EDX[bit x] is set to 1 if feature X is enabled
	push rbx    ; save rbx
	mov r8b, cl ; save bit number
	
	mov eax, 1   ; function number 1 into eax
	cpuid		 ; call cpuid
	mov eax, edx ; move bit answers into eax from edx
	mov cl, r8b  ; move bit number back into cl, we need it in cl because of shr  

	shr eax, cl ; shift bit to 0th position of eax
	and eax, 1  ; mask out unwanted bits ( all but bit 0 )

	pop rbx		; restore rbx
	ret			; return 0 or 1 in al

DetectFeatureEDX endp

DetectFeatureECX proc
	; bool DetectFeatureEDX(char bitNumber = cl)
	; CPUID_0000_0001_EDX[bit x] is set to 1 if feature X is enabled
	push rbx    ; save rbx
	mov r8b, cl ; save bit number
	
	mov eax, 1   ; function number 1 into eax
	cpuid		 ; call cpuid
	mov eax, ecx ; move bit answers into eax from ecx
	mov cl, r8b  ; move bit number back into cl, we need it in cl because of shr  

	shr eax, cl ; shift bit to 0th position of eax
	and eax, 1  ; mask out unwanted bits ( all but bit 0 )

	pop rbx		; restore rbx
	ret			; return 0 or 1 in al

DetectFeatureECX endp

end
