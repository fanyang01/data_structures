#ifndef _HEAP_H
#define _HEAP_H

#include <stdlib.h>
#include <stdbool.h>
#include "list.h"

typedef int (*cmp_func)(void *, void *);
typedef struct heap_tree {
	struct heap_tree *siblings;
	struct heap_tree *childs;
	int rank;
	void *data;
} heap_tree;

typedef struct {
	size_t size;
	size_t unit_size;
	cmp_func compare;
	heap_tree *list;
} heap;

heap *heap_init(size_t unit_size, cmp_func f);
bool heap_is_empty(heap *h);
void heap_free(heap *h);
heap *heap_clean(heap *h);
heap *heap_merge(heap *x, heap *y);
heap *heap_pop(heap *h, void *des);
heap *heap_insert(heap *h, void *data);

#endif
