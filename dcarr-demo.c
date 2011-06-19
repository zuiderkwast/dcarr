/*
 * A demo/test program for dcarr.h
 *
 * Compiles cleanly with or without any of the following flags:
 *
 * gcc -Wall -pedantic -std=c99
 */

#include <stdlib.h>
#include <stdio.h>
#include "dcarr.h"

dcarr_define_type(intarray_t, int);

int main() {
	int i;
	intarray_t arr;

	printf("Initializing array with capacity 8.\n");
	dcarr_init(arr, int, 8);

	printf("Pushing odd numbers and unshifting even numbers 0 to 14.\n");
	for (i = 0; i < 15; i++) {
		if (i % 2) dcarr_push(arr, int, i);
		else dcarr_unshift(arr, int, i);
	}

	printf("Inserting 100 at position 9.\n");
	dcarr_insert(arr, 9, int, 100);

	printf("Printing array (length=%d, capacity=%d, offset=%d)\n",
	       dcarr_len(arr), arr.cap, arr.off);
	for (i=0; i<dcarr_len(arr); i++) {
		printf("Element at %2d : %d\n",
		       i, dcarr_elem(arr, i));
	}
	return 0;
}
