/*
 * A demo/test program for dcarr.h
 *
 * Compiles cleanly with or without any of the following flags:
 *
 * gcc -Wall -pedantic -std=c99
 *
 * ------------------------------------------------------------------
 *
 * 2011 June 21
 *
 * The author disclaims copyright to this source code.
 */

#include <stdlib.h>
#include <stdio.h>
#include "dcarr.h"

dcarr_define_type(intarray_t, int);

/* qsort int comparison function */
int int_cmp(const void *a, const void *b) {
    return *(const int *)a - *(const int *)b;
}

void dumparray(intarray_t arr) {
	int i;
	printf("Printing array (length=%d, capacity=%d, offset=%d)\n",
	       dcarr_len(arr), arr.cap, arr.off);
	for (i=0; i<dcarr_len(arr); i++)
		printf(" %d", dcarr_elem(arr, i));
	printf("\n");
}

int main() {
	int i;
	intarray_t arr;

	printf("Initializing array.\n");
	dcarr_init(arr);

	printf("Pushing odd numbers and unshifting even numbers 0 to 14.\n");
	for (i = 0; i < 15; i++) {
		if (i % 2) dcarr_push(arr, int, i);
		else dcarr_unshift(arr, int, i);
	}

	printf("Inserting 100 at position 9.\n");
	dcarr_insert(arr, 9, int, 100);

	dumparray(arr);

	printf("Sorting.\n");
	dcarr_sort(arr, int, int_cmp);
	dumparray(arr);

	while(dcarr_len(arr) > 0) {
		if (dcarr_len(arr) % 3) {
			dcarr_shift(arr, int, i);
			printf("Shift %d. ", i);
		} else {
			dcarr_pop(arr, int, i);
			printf("Pop %d. ", i);
		}
		if (dcarr_len(arr) % 4 == 0) {
			printf("\n");
			dumparray(arr);
		}
	}

	/* free allocated memory */
	dcarr_destroy(arr);

	return 0;
}
