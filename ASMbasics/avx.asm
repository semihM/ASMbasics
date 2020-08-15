.code

GetAVXSupport proc
	;bool GetAVXSupport()
	mov eax, 1 ; function no
	
	cpuid
	xor eax,eax

	bt ecx, 28 ; bit 28 to check
	adc eax, eax ; if CF = 1 which means its gonna be True

	ret
GetAVXSupport endp

end