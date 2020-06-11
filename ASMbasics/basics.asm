;;;Define variables

.data
	;varname_for_memory_adress data_type data_value

	myByte db 78
	myWord dw 656
	varbit db 3
	varqrq db 15
	var64add sqword 0

	padded struct		; {} {,5} {'a',,,5.35} for constructing
		c db ?			; ? is a placeholder, can be replaced for a default value
			db 3 dup(0) ; Padding
		i dd ?
		s dw ?
			db 6 dup(0) ; Padding
		d real8 ?
	padded ends


	myclass struct
		i dd ?
	myclass ends

	myunion union		; { 0ffffffffh } -> Only the first element will be set, so only ffh is set
		small db ?
		medium dw ?
		large dd ?
		huge dq ?
	myunion ends

	pp myunion { 0ffffffffh } ; create a myunion with value small 'ff'

.code

acctoclass proc 
	mov [rcx].myclass.i, 100

	ret
acctoclass endp

acctounion proc
	mov [rcx].myunion.large, 3525363

	ret
acctounion endp

testfun proc
	;nop is an instruction for padding

	mov al, byte ptr [myByte] ; "byte ptr [myByte]" is same as "myByte"


	;;CLEAR register after use
	push rbx
		mov bx, 20
		mov bh, 64
	pop rbx
	
	;;USE .data
	mov al, varbit
	mov ah, varqrq
	
	;;MOV AND LEA
	lea rax, varqrq
	mov byte ptr [rax], 9

testfun endp

arithmetictest proc
	;;ARITHMETIC	
	mov rax, 5
	mov rcx, 12
	add rax,rcx
	
	;Careful with 32bit immediate
	xor rcx,rcx ;Resets to 0
	add rcx, 2147483648 ; one's compliment of the number = negative of the number
	mov var64add ,rcx
	
	;Use this instead
	xor rcx,rcx
	mov rax, 2147483648
	add rcx, rax
	mov var64add, rcx
	
	;Increments == adds 1, careful with carry flags
	mov al,255
	inc al ; overflows, use "add al, 1" instead
	
	;Decrements
	mov al,123

	dec al ; can also use "sub al, 1" 
arithmetictest endp

booltest proc
	;;BOOLEAN OPERATIONS 
	mov eax, 1001100010011b ; 'b' at the end is for creating binary constants
	mov ecx, 1001000010010b

	;NOT Gate
	not eax

	;AND Gate
	and eax,ecx
	
	;NAND Gate
	and eax,ecx
	not eax

	;OR Gate
	or eax,ecx
	
	;NOR Gate
	or eax,ecx
	not eax

	;XOR Gate	
	xor eax,ecx 
	
	;NXOR
	xor eax,ecx
	not eax ; equals to

booltest endp

flagtest proc
	;;FLAGS ( PE: old odd-even stuff, AC: old decimal arithmetic stuff )
	;Carry flag: CY
	;stc: set carry flag, clc: clear carry flag
	mov al, 255
	add al, 1
	
	;Zero flag: ZR: 1 if result was zero else 0
	xor eax,eax
	add eax,5
	
	;Sign flag: PL: 1 if last result was negative else 0
	mov eax, 7
	mov ecx, 12
	sub eax,ecx
	add eax,ecx

	;Direction flag: UP: 1 if reading string forwards else 0
	; cld : clear direction flag, sld: set direction flag
	
	;Overflow flag: OV: overflow on the 2nd left bit, 1 if sign changes else 0
	mov al, 255
	shr al, 1
	add al, 1

	;;Push flags into a registery
	;pushfq
	;pop rax
	;; changle flags in rax
	;push rax
	;popfq
	;ret

flagtest endp

conditional proc
	;; ACTIONS
	; j for conditional jumps
	; cmov for conditional moves
	; set for conditional sets
	
	;; CODES 
	; O : Overflow, NO : Not Overflow -> JO,JNO etc.
	; B | C | NAE : Carry unsigned, NB | NC | AE: Not Carry Unsigned -> JB/JC/JAE etc.
	; E | Z : Zero, NE | NZ : Not Zero -> JE/JNE/JZ etc.
	; BE | NA : Below/ Equal unsigned, NBE | A : Above unsigned -> JNBE/JA/JNA etc.
	; S : Sign, NS : Not sign
	; P | PE : Parity even, NP | PO : Parity odd
	; L | NGE : Less signed, NL | GE : Not less signed
	; LE | NG : Not greater signed, NLE | G : Greater signed

	COMMENT @ jumpoverflow
	mov eax, -9
	add eax, -12
	jo Overflowed
	jno DidntOverflow

	Overflowed:
		mov rax, 1 ; Added for stepping in while debugging
	DidntOverflow:
		mov rax, 0

	@ jumpoverflow
	

	COMMENT @ looping
	mov rcx,10

	LoopHead:
		;Body of the loop
		dec rcx
		jnz LoopHead

	@ looping


	COMMENT @ carry
	push rbx

	mov ebx, 67
	mov eax, -1
	add eax, 1 ; Will cause cy=1
	clc ; cy=0
	cmovc eax,ebx ; move ebx to eax if cy=1

	mov eax, 5
	mov ecx, 5
	cmp eax,ecx ; compare
	sete bh ; set bh to 1 if eax and ecx are equal

	pop rbx

	@ carry


	COMMENT @ jumpabovebelow

	push rbx

	mov rax, 76
	mov rbx, 55

	cmp rax,rbx
	jb wasbelow
	ja wasabove

	wasbelow:
		pop rbx
		ret
	wasabove:
		pop rbx
		ret

	@ jumpabovebelow

	COMMENT @ cmovflag
	push rbx

	mov rbx,100
	mov rax,50
	mov rcx,90

	sub rax,rcx
	
	cmovs rax,rbx ; if rax-rcx was negative, set rbx to rax , cmovns for the positive 

	pop rbx
	
	@ cmovflag

	COMMENT @ setgreater
	push rbx
	
	mov eax, -9
	mov ecx, 100
	cmp eax, ecx

	setg bl ; if eax was greater than ecx set bl to 1

	pop rbx
	
	@ setgreater

	ret
conditional endp

division proc
	push rbx

	COMMENT @ div8bits
	mov al, 50 ; Divide 50
	mov bl, 10 ; by 10

	div bl ; al
	; result in al
	; remainder in ah

	@ div8bits

	COMMENT @ div16bits
	; dx:ax
	mov ax, 50
	xor dx,dx ; clear dx
	mov cx, 3
	
	div cx
	; result in ax
	; remainder in dx

	@div16bits

	COMMENT @ div32bits
	; edx:eax
	mov eax, 7871

	xor edx,edx
	
	mov ecx, 78

	div ecx
	;result in eax
	;remainder in edx

	@ div32bits

	COMMENT @ div64bits

	mov rax, 79871
	
	xor rdx,rdx

	mov rbx, 1238

	div rbx
	; result in rax
	; remainder in rdx

	@ div64bits

	pop rbx
	ret
division endp

gcd proc
	mov rax, 0 ; error value
	cmp rcx, 0
	je Finished
	cmp rdx, 0
	je Finished

	push rbx
	mov rbx,rdx ; free rdx to use div

	LoopHead:
		xor rdx, rdx ; clear
		mov rax, rcx ; move lower 64 bits 
		div rbx		 ; rdx will hold remainder
		mov rcx, rbx ; copy x to y
		mov rbx, rdx ; copy remainder to rbx

		cmp rdx, 0
		jne LoopHead
		
		mov rax, rcx

		pop rbx
	
	Finished:
		ret	

	
gcd endp



end

