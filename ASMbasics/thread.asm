.data
	extern CreateThread: proc ;Windows' thread create func

	mutex dw 0

.code

ThreadStart proc
	inc dword ptr [rcx]
	ret
ThreadStart endp

CreateThreadASM proc
	push rbp		;save caller's rbp
	mov rbp, rsp
	
	push 0 ; thread id
	push 0 ; creation flags, 0 for start immediately

	sub rsp, 20h ; shadow space for 4 quads

	mov r9, rcx  ; save *i to r9
	mov rcx, 0   ; security attributes
	mov rdx, 0   ; stack size, 0 to use same as caller's
	mov r8, ThreadStart	
	
	call CreateThread

	mov rsp, rbp
	pop rbp			;restore caller's rbp

	ret
CreateThreadASM endp


ThreadStartLockTest proc
	mov rax, 1000000

	LoopHead:
		lock inc dword ptr [rcx]  ; inc dword ptr [rcx] causes a race condition and i doesn't get to 2mil

		dec rax
		jnz LoopHead

	ret
ThreadStartLockTest endp

CreateThreadLockTest proc
	push rbp		;save caller's rbp
	mov rbp, rsp
	
	push 0 ; thread id
	push 0 ; creation flags, 0 for start immediately

	sub rsp, 20h ; shadow space for 4 quads

	mov r9, rcx  ; save *i to r9
	mov rcx, 0   ; security attributes
	mov rdx, 0   ; stack size, 0 to use same as caller's
	mov r8, ThreadStartLockTest	
	
	call CreateThread

	mov rsp, rbp
	pop rbp			;restore caller's rbp

	ret
CreateThreadLockTest endp


; MUTEX : lock and key, only 1 thread can own it at once
; SPINLOCK : Can be created via MUTEX, thread spins until MUTEX
;			 becomes available again
; CRITICAL SECTION: Can be guarded by a SPINLOCK, only 1 thread
;					is allowed in at a time, rest spins
; DEADLOCK: Occurs when the thread with MUTEX retires without
;		    releasing the MUTEX

ExecuteWork proc
	mov rax, 1000000

	;Spin lock
	SpinLoop:
		lock bts mutex, 0
		jc SpinLoop
	
	;Critical section !
	mov rdx, 1000

	InnerLoop:
		inc dword ptr [rcx]

		dec rdx
		jnz InnerLoop

	mov mutex, 0  ; Release the mutex
	
	sub rax, 1000
	jnz SpinLoop

	ret
ExecuteWork endp

CreateThreadSpinTest proc
	push rbp		;save caller's rbp
	mov rbp, rsp
	
	push 0 ; thread id
	push 0 ; creation flags, 0 for start immediately

	sub rsp, 20h ; shadow space for 4 quads

	mov r9, rcx  ; save *i to r9
	mov rcx, 0   ; security attributes
	mov rdx, 0   ; stack size, 0 to use same as caller's
	mov r8, ExecuteWork	
	
	call CreateThread

	mov rsp, rbp
	pop rbp			;restore caller's rbp

	ret
CreateThreadSpinTest endp


end