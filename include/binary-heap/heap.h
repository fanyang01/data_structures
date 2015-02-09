#ifndef _HEAP_H
#define _HEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_HEAP_SIZE (1<<24)
#define MIN_HEAP_SIZE (1<<6)
#define MAX_UNIT_SIZE (1<<10)

typedef int (*cmp_func)(void *, void *);
typedef struct heap {
	size_t cap;
	size_t unit_size;
	size_t size;
	cmp_func compare;
	void *array;
} heap;

/* allocate and initialize a new heap */
/* return NULL when failed */
heap *heap_init(size_t unit_size, size_t cap, cmp_func f);
/* make a heap empty */
heap *heap_clean(heap *h);
/* free the space occupied by heap */
void heap_free(heap *h);
/* test if the heap is empty */
bool heap_is_empty(const heap *h);
/* test if the heap is full */
bool heap_is_full(const heap *h);
/* build heap from a given array */
heap *heap_build(void *array, size_t unit_size,
		size_t size, cmp_func f);
/* pop the element of highest priority */
heap *heap_pop(heap *h, void *des);
/* find the element of highest priority but without removing it */
const void *heap_highest(heap *h);
/* insert data into heap */
heap* heap_insert(heap *h, const void *data);
/* merge two heaps into the first one */
/* return NULL when failed or two heaps are NULL */
heap *heap_merge(heap *x, heap *y);

#endif
