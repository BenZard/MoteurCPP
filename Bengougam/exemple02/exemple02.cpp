//
// example02.cpp
//
// Analyse de l'impact de l'ecart (pas) entre deux mots
//

// --- Includes --------------------------------------------------------------

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include "Common/EsgiTimer.h"

#define DATATYPE    uint8_t
#define ITERATIONS  32
#define TAB_SIZE	sizeof(DATATYPE)*ITERATIONS*1024*1024
#define LOOP_LEN	1*1024*1024

// --- Functions -------------------------------------------------------------

int main(int argc, char* argv[])
{
	printf("example02.cpp");
	DATATYPE *tableau = new DATATYPE[TAB_SIZE];
	EsgiTimer benchmark;

	for (uint32_t loop = 0; loop <= ITERATIONS; ++loop)
	{
		srand(127);

		for (uint32_t index = 0; index < TAB_SIZE; ++index)
		{
			tableau[index] = rand()%255;
		}

		uint32_t somme = 0;
        
		DATATYPE *data = tableau;
		benchmark.Begin();
		for (uint32_t index = 0; index < LOOP_LEN; ++index, data += loop)
		{
			somme += *data;
		}
		benchmark.End();

		printf("[%d] somme = %d; duree de la boucle : %f millisecondes\n", loop, somme, benchmark.GetElapsedTime()*1000.0);
	}

	delete[] tableau;

	getchar();

	return 0;

}