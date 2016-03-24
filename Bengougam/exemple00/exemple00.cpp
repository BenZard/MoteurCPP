//
// exemple00.cpp
//

// --- Includes --------------------------------------------------------------

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include "Common/EsgiTimer.h"

#define DATATYPE    uint32_t      // type des donnees du tableau
#define STEP        1
#define LOOP_LEN    4*1024*1024
#define TAB_SIZE	LOOP_LEN


// --- Functions -------------------------------------------------------------

int main(int argc, char* argv[])
{
	printf("example00.cpp");
	EsgiTimer benchmark;	

    DATATYPE *tableau = new DATATYPE[TAB_SIZE];

	srand(127);

	for (uint32_t index = 0; index < TAB_SIZE; ++index)
	{
		tableau[index] = rand()%255;
	}

    // la somme sert juste de pretexte
	uint32_t somme = 0;
    uint32_t iterations = 0;
    //
	const DATATYPE * data = (DATATYPE *)tableau;

	benchmark.Begin();
	for (uint32_t index = 0; index < LOOP_LEN; data++, ++index, ++iterations)
	{
		somme += *data;
	}
	benchmark.End();

	printf("somme = %d; (%d iters) duree de la boucle : %f millisecondes\n", somme, iterations, benchmark.GetElapsedTime()*1000.0);

    somme = 0;
    iterations = 0;
    data = (DATATYPE *)tableau;
    
	benchmark.Begin();
	for (uint32_t index = 0; index < LOOP_LEN; data+=STEP, index += STEP, ++iterations)
	{
		somme += *data;
	}
	benchmark.End();
    
	printf("somme = %d; (%d iters) duree de la boucle STEP=%d : %f millisecondes\n", somme, iterations, STEP, benchmark.GetElapsedTime()*1000.0);
    
    
	delete[] tableau;

	getchar();

	return 0;
}

