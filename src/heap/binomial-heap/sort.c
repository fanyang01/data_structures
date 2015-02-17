#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <string.h>
#include "heap.h"

#define MAXSIZE (1<<19)

int func_qsort(const void *a, const void *b)
{
	int *x = (int *)a;
	int *y = (int *)b;

	if(*x > *y) return 1;
	else if(*x == *y) return 0;
	return -1;
}

int func_heap(const void *a, const void *b)
{
	int *x = (int *)a;
	int *y = (int *)b;

	if(*x < *y) return 1;
	else if(*x == *y) return 0;
	return -1;
}

int main(void)
{
	int a[MAXSIZE];
	int b[MAXSIZE];
	int i;
	clock_t start, end;

	heap *h = heap_init(sizeof(int), func_heap);
	if(!h) {
		fprintf(stderr, "failed to initialize heap\n");
		exit(EXIT_FAILURE);
	}
	srand((unsigned int)time(0));
	for(i = 0; i < MAXSIZE; i++) {
		a[i] = (int)(rand() % INT_MAX);
		b[i] = a[i];
	}

	start = clock();
	qsort(a, MAXSIZE, sizeof(int), func_qsort);
	end = clock();
	printf("qsort use %fs\n", (double)(end - start) / CLOCKS_PER_SEC);

	start = clock();
	for(i = 0; i < MAXSIZE; i++) {
		heap_insert(h, b + i);
	}
	for(i = 0; i < MAXSIZE; i++) {
		heap_pop(h, b + i);
	}
	end = clock();
	printf("heap  use %fs\n", (double)(end - start) / CLOCKS_PER_SEC);

	if(memcmp(a, b, sizeof(int) * MAXSIZE)) {
		puts("heap sort error");
	}
	return 0;
}
