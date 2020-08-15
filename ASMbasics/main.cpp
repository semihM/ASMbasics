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

#pragma pack(push,1)
_declspec(align(16)) struct Point 
{ 
	double x; 
	double y; 

};
#pragma pack (pop)


#pragma pack(push,1)
class class_Vector
{
	float x, y;
public:
	class_Vector(float x, float y);
	~class_Vector();
	float GetX();
	float GetY();
};
#pragma pack (pop)


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


//////////// FROM THE BOOK ///////////////

extern "C" bool IsOdd(int num);

////////////  PRACTICAL x64 TUTORIAL STUFF  /////////////


extern "C" void CallACPPProcedure();

extern "C" void PrintParameterMessage(int a, int b, int c, int d, int e, int f) 
{
	cout << "Parameters taken:" << f << endl;
}

extern "C" int FindMax(int* arr, int count);

//////////////////  CPUID SSE  //////////////////
extern "C" bool GetCPUIDSupport();
extern "C" char* GetVendorString();
extern "C" int GetLargestStandardFunction();
extern "C" int GetLogicalProcessorCount();
extern "C" bool DetectFeatureEDX(char);
extern "C" bool DetectFeatureECX(char);

extern "C" bool CheckMMXCapability();
extern "C" void MMXCopy(char* a, char* b, int count);

extern "C" void Absolute(short* arr, int count);

extern "C" void Allocate();
extern "C" void Deallocate();

extern "C" double Distance(Point * p1, Point * p2);

//////////////////  CPUID AVX  //////////////////
extern "C" bool GetAVXSupport();

int main()
{	
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

	////////////  FROM THE BOOK  /////////////

	//cout << IsOdd(3) << "," << IsOdd(22) << endl;

	////////////  PRACTICAL x64 TUTORIAL STUFF  /////////////

	//CallACPPProcedure();

	//int i[5] = { 1,4,2,5,0 };
	//FindMax(i, 5);

	/*
	if (!GetCPUIDSupport()) 
	{
		cout << "CPU doesn't know CPUID" << endl;
	}
	else 
	{
		cout << "CPUID is known!" << endl;
		cout << endl << "Vendor String : " << GetVendorString() << endl;
		cout << endl << "Largest standard function: " << GetLargestStandardFunction() << endl;
		cout << endl << "Logical Processor Count: " << GetLogicalProcessorCount() << endl;

		cout << endl << "Supported Features from CPUID_0000_0001_EDX" << endl;
		for (int feat = 0; feat < 32; feat++) 
		{
			if (DetectFeatureEDX(feat)) cout << " " << feat;
		}
		cout << endl;

		cout << endl << "Supported Features from CPUID_0000_0001_ECX" << endl;
		for (int feat = 0; feat < 32; feat++)
		{
			if (DetectFeatureECX(feat)) cout << " " << feat;
		}
		cout << endl;
	}
	*/

	/*
	if (!CheckMMXCapability())
	{
		cout << "Not capable of working with MMX!" << endl;
	}
	else 
	{	
		char a[19] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		char b[19] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 };
		
		cout << "Old:" << endl;
		for (int i = 0; i < 19; i++) cout << (int)a[i] << " ";
		
		MMXCopy(a, b, 19);

		cout << endl << "New:" << endl;
		for (int i = 0; i < 19; i++) cout << (int)a[i] << " ";

	}
	*/

	/*
	short a[19] = { -1,-2,3,4,5,6,-7,-8,9,10,-11,12,-13,14,-15,16,17,-18,19 };

	cout << "Old:" << endl;
	for (int i = 0; i < 19; i++) cout << a[i] << " ";

	Absolute(a, 19);

	cout << endl << "New:" << endl;
	for (int i = 0; i < 19; i++) cout << a[i] << " ";
	*/
	
	/*
	Allocate();
	Deallocate();
	*/
	
	/*
	struct Point p1 = { 100,100 };
	struct Point p2 = { 200,200 };
	cout << "Dist: " << Distance(&p1, &p2) << endl;
	*/
	
	/*
	class_Vector v1(3.0f, 4.0f);
	cout << "x:" << v1.GetX() << ", " << "y:" << v1.GetY() << endl;
	*/
	
	/*
	if (GetAVXSupport()) cout << "AVX is supported!" << endl;
	else cout << "AVX is NOT supported :(" << endl;
	*/
	
	cout << endl << "Press enter to quit..." << endl;
	cin.get();
	return 0;
}