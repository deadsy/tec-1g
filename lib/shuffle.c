//-----------------------------------------------------------------------------
/*

Random Shuffle

*/
//-----------------------------------------------------------------------------

#include <stdlib.h>

#include "util.h"

//-----------------------------------------------------------------------------

static void swap(uint8_t *a, uint8_t *b) {
	uint8_t tmp = *a;
	*a = *b;
	*b = tmp;
}

// in-place shuffle an array of given size
void shuffle(uint8_t *array, uint8_t n) {
	// Start from the last element and swap down to the first
	for (uint8_t i = n - 1; i > 0; i--) {
		// Pick a random index from 0 to i
		uint8_t j = rand() % (i + 1);
		// Swap arr[i] with the element at random index j
		swap(&array[i], &array[j]);
	}
}

//-----------------------------------------------------------------------------
