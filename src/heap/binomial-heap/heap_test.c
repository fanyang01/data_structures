#include "heap.h"
#include <stdio.h>
#include <stdlib.h>

int func(void *x, void *y)
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
		exit(EXIT_FAILURE);
	}
	for(i = 0; i < 50; i++)
		heap_insert(h, &i);
	for(i = 0; i < 40; i++) {
		heap_pop(h, &tmp);
		printf("%d ", tmp);
	}
	puts("");

	heap_clean(h);
	for(i = 0; i < 20; i++) {
		heap_insert(h, &i);
		tmp = i << 1;
		heap_insert(h_a, &tmp);
	}
	heap_merge(h, h_a);
	while(!heap_is_empty(h)) {
		heap_pop(h, &tmp);
		printf("%d ", tmp);
	}
	puts("");
	heap_free(h);
	exit(EXIT_SUCCESS);
}
