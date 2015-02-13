#ifndef _HEAP_H
#define _HEAP_H

#include <stdlib.h>
#include <stdbool.h>
#include "list.h"

typedef int (*cmp_func)(const void *, const void *);
typedef struct {
	struct list_node node;
	void *data;
	int degree;
	char mark;
	struct list_head childs;
} heap_node;

typedef struct {
	struct list_head root_list;
	size_t size;
	size_t data_size;
	cmp_func compare;
	heap_node *highest;
} heap;

extern heap *heap_init(size_t data_size, cmp_func f);
extern bool heap_is_empty(const heap *h);
extern void heap_free(heap *h);
extern heap *heap_clean(heap *h);
extern heap *heap_insert(heap *h, const void *data);
extern heap *heap_pop(heap *h, void *des);
extern heap *heap_merge(heap *x, heap *y);

#endif
