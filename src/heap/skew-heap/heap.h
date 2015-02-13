#ifndef _HEAP_H
#define _HEAP_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct heap_node {
	struct heap_node *left, *right;
	int npl; // null path length
	void *data;
} heap_node;

typedef int (*cmp_func)(void *, void *);

typedef struct heap {
	struct heap_node *root;
	cmp_func compare;
	size_t data_size;
	size_t size;
} heap;

heap *heap_init(size_t data_size, cmp_func f);
bool heap_is_empty(const heap *h);
heap *heap_merge(heap *x, heap *y);
heap *heap_insert(heap *h, void *data);
heap *heap_pop(heap *h, void *des);
const void *heap_highest(heap *h);
void heap_free(heap *h);
heap *heap_clean(heap *h);

#endif
