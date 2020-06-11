.data

intvec1 dd 0,1,2,3,4,5,6,7
intvec2 dd 4,5,6,7,8,9,10,11

fvec1 real4 0.0, 1.0, 2.0, 3.0
fvec2 real4 2.0, 3.0, 4.0, 5.0

dvec1 real8 1.0, 2.0
dvec2 real8 2.0, 3.0

.code 

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
end