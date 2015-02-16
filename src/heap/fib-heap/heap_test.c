#include "heap.h"
#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE (1<<20)

int func(const void *x, const void *y)
{
	int *a = (int *)x;
	int *b = (int *)y;
	if(*a < *b) return 1;
	else if(*a == *b) return 0;
	return -1;
}

int main(void)
{
	heap *h, *h_a;
	int i, tmp;

	h = heap_init(sizeof(int), func);
	h_a = heap_init(sizeof(int), func);
	if(!h || !h_a) {
		fprintf(stderr, "failed to initialize heap\n");
		goto FAILED;
	}
	for(i = 0; i < MAXSIZE; i++)
		heap_insert(h, &i);
	for(i = 0; i < MAXSIZE; i++) {
		heap_pop(h, &tmp);
		if(i != tmp) goto FAILED;
	}

	heap_handle handle[MAXSIZE/2];
	for(i = MAXSIZE / 2; i < MAXSIZE; i++)
		handle[i - MAXSIZE / 2] = heap_insert(h, &i);
	for(i = MAXSIZE / 2 - 1; i >= 0; i--)
		heap_inc_priority(h, handle[i], &i);

	for(i = MAXSIZE / 2; i < MAXSIZE; i++)
		heap_insert(h_a, &i);

	heap_merge(h, h_a);
	for(i = 0; i < MAXSIZE; i++) {
		heap_pop(h, &tmp);
		if(i != tmp) goto FAILED;
	}
	heap_free(h);
	puts("----------passed----------");
	exit(EXIT_SUCCESS);
FAILED:
	puts("!!!!!!!!!!failed!!!!!!!!!!");
	exit(EXIT_FAILURE);
}
