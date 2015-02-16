#ifndef _HEAP_H
#define _HEAP_H

#include <stdlib.h>
#include <stdbool.h>

typedef int (*cmp_func)(const void *, const void *);
typedef struct heap_tree {
	struct heap_tree *siblings;
	struct heap_tree *childs;
	struct heap_tree *prev;
	int rank;
	void *data;
} heap_tree;

typedef struct {
	size_t size;
	size_t data_size;
	cmp_func compare;
	heap_tree *list;
} heap;

typedef heap_tree *heap_handle;

extern heap *heap_init(size_t data_size, cmp_func f);
extern bool heap_is_empty(const heap *h);
extern void heap_free(heap *h);
extern heap *heap_clean(heap *h);
extern heap *heap_merge(heap *x, heap *y);
extern heap *heap_pop(heap *h, void *des);
extern heap_tree *heap_insert(heap *h, const void *data);
extern const void *heap_highest(const heap *h);
extern heap_tree *heap_inc_priority(heap *h,
		heap_tree *node, const void *data);

#endif
