extern malloc:proc
extern free:proc

.data
	testMemory dq ?
	
.code

Allocate proc
	sub rsp, 10h ; shadow space enough for 1 param rounded up to nearest 16 bytes

	mov rcx, 1024*1024 ; 1MB is the amount of memory we want
	call malloc		   ; call malloc, rax will be a pointer to the memory or 0 (if there was no available memory)

	mov testMemory, rax ; save pointer to our testMemory

	add rsp, 10h
	ret
Allocate endp

Deallocate proc
	sub rsp, 10h		; shadow space for single param
	mov rcx, testMemory ; move memory to be removed to rcx
		
	call free
	add rsp, 10h
	ret

Deallocate endp

end