//
// example03.cpp
//

// --- Includes --------------------------------------------------------------

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include "Common/EsgiTimer.h"

const uint32_t g_42 = 42;

// --- Classes ----------------------------------------------------------------

struct ObjectA
{	
	uint32_t value;
};

struct ObjectB
{
	uint32_t value;

	ObjectB() 
	{
		value = g_42;
	}
};

struct ObjectC
{	
	uint32_t value;

	ObjectC() 
	{
	}

	virtual ~ObjectC() 
	{
	}
};

// --- Functions -------------------------------------------------------------

int main(int argc, char* argv[])
{
	printf("cpp00.cpp");
	// objets crees dans la pile (stack) ---

	ObjectA A;
	ObjectB B;
	ObjectC C;

	C.value = A.value;
	if (B.value != A.value) {
		printf("A=%d B=%d\n", A.value, B.value);
	}

	// objects crees sur le tas (heap) ---

	ObjectA *pA = new ObjectA();
	ObjectB *pB = new ObjectB();
	ObjectC *pC = new ObjectC();

	pC->value = pA->value;
	if (pB->value != pA->value) {
		printf("A=%d B=%d\n", pA->value, pB->value);
	}

	delete pC;
	delete pB;
	delete pA;

	getchar();
	return 0;

}

