//
// example03.cpp
//

// --- Includes --------------------------------------------------------------

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include "Common/EsgiTimer.h"

#define TAB_SIZE	512*1024

struct Object
{	
	uint32_t value0;
    uint8_t value1;
	uint32_t value2[2];
	uint8_t value3[3];
};

// --- Functions -------------------------------------------------------------

int main(int argc, char* argv[])
{
	printf("example03.cpp");
	Object *tableau = new Object[TAB_SIZE];
	EsgiTimer benchmark;	

	srand(127);

	for (uint32_t index = 0; index < TAB_SIZE; ++index)
	{
		tableau[index].value0 = rand()%255;
	}

	// boucle incrementale ---

	Object *data = tableau;
	benchmark.Begin();

	int somme = 0;

	for (uint32_t index = 0; index < TAB_SIZE; index++)
	{
		somme += data[index].value0;
	}

	benchmark.End();	

	printf("somme = %d; duree de la boucle incr : %Lf millisecondes\n", somme, benchmark.GetElapsedTime()*1000.0);

	// boucle a rebour ---

	data = tableau;
	benchmark.Begin();

	somme = 0;

	for (int32_t index = TAB_SIZE-1; index >= 0; index--)
	{
		somme += data[index].value0;
	}

	benchmark.End();	

	printf("somme = %d; duree de la boucle decr : %Lf millisecondes\n", somme, benchmark.GetElapsedTime()*1000.0);

	delete[] tableau;

	getchar();

	return 0;

}

