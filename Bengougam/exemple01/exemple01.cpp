//
// exemple01.cpp
//

// --- Includes --------------------------------------------------------------

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include "Common/EsgiTimer.h"

#define FULL_PROCESS    1           // 0 = 1 octets / STEP ; 1 = la totalite
#define DATATYPE        uint8_t      // type des donnees du tableau
#define STEP            1           // ecart en octets entre deux lectures
#define INCREMENT       sizeof(DATATYPE)           //

#define LOOP_LEN    4*1024*1024
#if FULL_PROCESS
#define TAB_SIZE	STEP*LOOP_LEN
#else
#define TAB_SIZE	LOOP_LEN
#endif

// --- Functions -------------------------------------------------------------

int main(int argc, char* argv[])
{
	printf("example01.cpp");
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
	const uint8_t * data = (uint8_t *)tableau;

	benchmark.Begin();
	for (uint32_t index = 0; index < LOOP_LEN; data += INCREMENT, ++iterations)
	{
		somme += *data;
#if FULL_PROCESS
        ++index;
#else
        index += STEP;
#endif
	}
	benchmark.End();

	printf("somme = %d; (%d iters) duree de la boucle : %f millisecondes\n", somme, iterations, benchmark.GetElapsedTime()*1000.0);

	delete[] tableau;

	getchar();

	return 0;
}

