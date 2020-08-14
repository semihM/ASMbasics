include pointstruct.inc

.data

	intvec1 dd 0,1,2,3,4,5,6,7
	intvec2 dd 4,5,6,7,8,9,10,11

	fvec1 real4 0.0, 1.0, 2.0, 3.0
	fvec2 real4 2.0, 3.0, 4.0, 5.0

	dvec1 real8 1.0, 2.0
	dvec2 real8 2.0, 3.0

	class_Vector struc
		x real4 ?
		y real4 ?
	class_Vector ends

.code 

; /////// Setting Exception Masks ///////////

COMMENT @ Make_All_Exceptions_Noisy

	stmxcsr mxcsr ; 32 bit variable
	and mxcsr, 0xffffe07f
	ldmxcsr mxcsr

@ Make_All_Exceptions_Noisy 

COMMENT @ Make_All_Exceptions_Quiet

	stmxcsr mxcsr ; 32 bit variable
	or mxcsr, 0x1F80
	ldmxcsr mxcsr

@ Make_All_Exceptions_Quiet

COMMENT @ Quiet/Noisy_Div_By_Zero
	
	stmxcsr mxcsr
	bts mxcsr, 9 ; btr to make it Noisy
	ldmxcsr

@ Quiet/Noisy_Div_By_Zero

COMMENT @ Check_Div_By_Zero
	divps xmmX, xmmY
	
	stmxcsr mxcsr
	and mxcsr, 4	    ; bit 3 is div by zero
	jz NoDivByZeroLabel ; label if no div by zero occured
	
	NoDivByZeroLabel: ;some code

@ Check_Div_By_Zero

COMMENT @ Check_Precision_Error
	
	
	divps xmmX, xmmY

	stmxcsr mxcsr
	and mxcsr, 32       ; bit 5 is precision
	jz NoPrecisionError ; Label if no precision error occured

	NoPrecisionError: ; some code
	
@ Check_Precision_Error

; /////////////// //////////////

addSIMDint proc
	movdqu xmm0, xmmword ptr [intvec1]
	movdqu xmm1, xmmword ptr [intvec2]

	paddd xmm0,xmm1

	ret
addSIMDint endp

mulSIMDfloat proc
	movaps xmm0, xmmword ptr [fvec1]
	movaps xmm1, xmmword ptr [fvec2]

	mulps xmm0,xmm1

	ret
mulSIMDfloat endp

divSIMDdouble proc
	movaps xmm0, xmmword ptr [dvec1]
	movaps xmm1, xmmword ptr [dvec2]

	divpd xmm0,xmm1

	ret
divSIMDdouble endp

Distance proc
	;double Distance(Point *p1 = rcx, Point *p2 = rdx)

	COMMENT @ CheesyWay @

	movapd xmm0, xmmword ptr [rcx]

	subpd xmm0, xmmword ptr [rdx]
	mulpd xmm0,xmm0

	haddpd xmm0,xmm0
	sqrtpd xmm0,xmm0

	ret

	;@ CheesyWay

	COMMENT @ ClassicWay @

	movsd xmm0, [rcx].Point.x
	movsd xmm1, [rcx].Point.y

	subsd xmm0, [rdx].Point.x
	subsd xmm1, [rdx].Point.y

	mulsd xmm0, xmm0
	mulsd xmm1, xmm1

	addsd xmm0, xmm1
	sqrtsd xmm0, xmm0
	
	ret

Distance endp

;/////////////////// class_Vector methods ///////////////////

??0class_Vector@@QEAA@MM@Z proc
	;class_Vector(float x, float y)
	movss dword ptr [rcx].class_Vector.x, xmm1
	movss dword ptr [rcx].class_Vector.y, xmm2

	mov rax, rcx ; return pointer to the new object in rax

	ret
??0class_Vector@@QEAA@MM@Z endp

??1class_Vector@@QEAA@XZ proc
	;~class_Vector

	ret
??1class_Vector@@QEAA@XZ endp

?GetX@class_Vector@@QEAAMXZ proc
	;class_Vector.GetX()
	movss xmm0, dword ptr [rcx].class_Vector.x
	ret
?GetX@class_Vector@@QEAAMXZ endp

?GetY@class_Vector@@QEAAMXZ proc
	;class_Vector.GetY()
	movss xmm0, dword ptr [rcx].class_Vector.y
	ret
?GetY@class_Vector@@QEAAMXZ endp

; /////////////////// ///////////////////

end