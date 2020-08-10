// BUILD DEPENDENCIES MUST INCLUDE MASM

#include <iostream>

using namespace std;

struct paddedstruct
{
	char c;
	int i;
	short s;
	double d;
};

#pragma pack(1)
struct packedstruct
{
	// USE #pragma pack(1) to pack the struct without padding
	char c;
	int i;
	short s;
	double d;
};

class myclass
{
private:
	int i;
public:
	int geti()
	{
		return i;
	}

};

void printbinary(int i)
{
	for (int j = 15; j >= 0; j--)
	{
		cout << ((i >> j) & 1);
	}
	cout << endl;
}

extern "C" int testfun();
extern "C" int arithmetictest();
extern "C" int booltest();
extern "C" int flagtest();
extern "C" int conditional();
extern "C" int division();
extern "C" unsigned long long gcd(unsigned long long X, unsigned long long Y);

/* Parameter passing in Windows "C" Calling Convention

	parameter# \ type     int    float/double       pointer/obj/array

		  1st			  rcx	    xmm0                rcx
		  2nd			  rdx		xmm1				rdx
		  3rd			  r8		xmm2				r8
		  4th			  r9		xmm3				r9
		  more			  stack		stack				stack


	int and pointer returns are in:	rax
	float returns are in		  :	xmm0
*/

extern "C" void acctoclass(myclass & m);

extern "C" void addSIMDint();
extern "C" void mulSIMDfloat();
extern "C" void divSIMDdouble();

extern "C" void CallACPPProcedure();

extern "C" void PrintParameterMessage(int a, int b, int c, int d, int e, int f) 
{
	cout << "Parameters taken:" << f << endl;
}

extern "C" int FindMax(int* arr, int count);


int main()
{	
	//int i[5] = { 1,4,2,5,0 };

	/* FOR x86 in-line asm
	int i = 0;
	_asm {
		mov i, 23412
	}
	*/

	// cout << "result testfun: " << testfun() << endl;

	//arithmetictest();

	//printbinary(booltest());

	//flagtest();

	//conditional();

	//division();

	//cout << gcd(57 * 56, 57 * 78) << endl;

	//cout << sizeof(paddedstruct) << sizeof(packedstruct) << endl;

	/*
	myclass test;

	cout << test.geti() << endl;

	acctoclass(test);

	cout << test.geti() << endl;
	*/

	//addSIMDint();

	//mulSIMDfloat();

	//divSIMDdouble();

	//CallACPPProcedure();

	//FindMax(i, 5);

	cin.get();
	return 0;
}