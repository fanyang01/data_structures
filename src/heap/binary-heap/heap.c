#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "heap.h"

#define offset(HEAP, NMEMBS) ((NMEMBS) * (HEAP->unit_size))
#define heap_error(E) fprintf(stderr, "%s:%d:%s: %s\n", \
		__FILE__, __LINE__, __func__, E)

static heap *shift_up(heap *h, int pos);
static heap *shift_down(heap *h, int pos);
static int find(heap *h, int index, void *data);
static int child_left(int parent);
static int child_right(int parent);
static int parent(int child);
static void copy(void *des, const void *src, size_t size);

/* allocate and initialize a new heap */
/* return NULL when failed */
heap *heap_init(size_t unit_size, size_t cap, cmp_func f)
{
	if(!f) {
		heap_error("compare function missed");
		return NULL;
	}
	if(unit_size <= 0 || unit_size > MAX_UNIT_SIZE) {
		heap_error("beyond max unit size");
		return NULL;
	}
	if(cap > MAX_HEAP_SIZE) {
		heap_error("beyond max heap size");
		return NULL;
	}
	if(cap < MIN_HEAP_SIZE) cap = MIN_HEAP_SIZE;
	heap *h = (heap *)malloc(sizeof(heap));
	void *array = malloc(cap * unit_size);
	if(!h || !array) {
		heap_error("failed to allocate memory");
		return NULL;
	}
	h->array = array;
	h->cap = cap;
	h->unit_size = unit_size;
	h->compare = f;
	heap_clean(h);
	return h;
}

/* make a heap empty */
heap *heap_clean(heap *h)
{
	if(!h) return NULL;
	h->size = 0;
	memset(h->array, 0, h->cap * h->unit_size);
	return h;
}

/* free the space occupied by heap */
void heap_free(heap *h)
{
	if(!h) return;
	free(h->array);
	free(h);
}

/* test if the heap is empty */
bool heap_is_empty(const heap *h)
{
	if(!h) return false;
	return h->size == 0;
}

/* test if the heap is full */
bool heap_is_full(const heap *h)
{
	if(!h) return false;
	return h->size == h->cap;
}

/* build heap from a given array */
heap *heap_build(void *array, size_t unit_size,
		size_t size, cmp_func f)
{
	if(!array) return NULL;
	if(!f) {
		heap_error("compare function missed");
		return NULL;
	}
	if(unit_size <= 0 || unit_size > MAX_UNIT_SIZE) {
		heap_error("beyond max unit size");
		return NULL;
	}
	if(size > MAX_HEAP_SIZE) {
		heap_error("beyond max heap size");
		return NULL;
	}

	int i;
	heap *h;

	h = (heap *)malloc(sizeof(heap));
	if(!h) {
		heap_error("failed to allocate memory");
		return NULL;
	}
	h->array = array;
	h->cap = size;
	h->size = size;
	h->unit_size = unit_size;
	for(i = parent(h->size - 1); i >= 0; i--)
		shift_down(h, i);
	return h;
}

/* pop the element of highest priority */
heap *heap_pop(heap *h, void *des)
{
	if(!h) return NULL;
	if(heap_is_empty(h)) return NULL;
	copy(des, h->array, h->unit_size);
	copy(h->array, h->array + offset(h, --h->size), h->unit_size);
	shift_down(h, 0);
	return h;
}

/* find the element of highest priority but without removing it */
const void *heap_highest(heap *h)
{
	if(!h) return NULL;
	return h->array;
}

/* insert data into heap */
heap* heap_insert(heap *h, const void *data)
{
	if(!h) return NULL;
	if(heap_is_full(h)) {
		heap_error("heap is full");
		return NULL;
	}
	copy(h->array + offset(h, h->size++), data, h->unit_size);
	shift_up(h, h->size - 1);
	return h;
}

/* merge two heaps into the first one */
/* return NULL when failed or two heaps are NULL */
heap *heap_merge(heap *x, heap *y)
{
	if(!x) return y;
	if(!y) return x;
	if(x->unit_size != y->unit_size) {
		heap_error("heaps differ on unit size");
		return NULL;
	}

	size_t size = x->size + y->size;
	int i;

	if(x->cap < size) {
		heap_error("beyond capcity");
		return NULL;
	}
	copy(x->array + offset(x, x->size), y->array, y->size * y->unit_size);
	x->size = size;
	for(i = parent(size - 1); i >= 0; i--)
		shift_down(x, i);
	heap_free(y);
	return x;
}

heap *heap_remove(heap *h, void *data)
{
	if(!h) return NULL;

	int i = find(h, 0, data);
	if(i < 0) {
		heap_error("not found");
		return NULL;
	}
	copy(h->array + offset(h, i),
			h->array + offset(h, --h->size), h->unit_size);
	shift_down(h, i);
	return h;
}

int find(heap *h, int index, void *data)
{
	int i, n;

	if(index >= h->size) return -1;
	if((n = h->compare(h->array + offset(h, index), data)) == 0)
		return index;
	else if(n < 0) return -1;
	i = find(h, child_left(index), data);
	if(i >= 0) return i;
	return find(h, child_right(index), data);
}

int child_left(int parent)
{
	return (parent << 1) + 1;
}

int child_right(int parent)
{
	return (parent << 1) + 2;
}

int parent(int child)
{
	return (child - 1) >> 1;
}

void copy(void *des, const void *src, size_t size)
{
	memcpy(des, src, size);
}

heap *shift_up(heap *h, int pos)
{
	char tmp[h->unit_size];

	if(!h) return NULL;
	copy(tmp, h->array + offset(h, pos), h->unit_size);
	while(pos && h->compare(tmp,
			h->array + offset(h, parent(pos))) > 0) {
		copy(h->array + offset(h, pos),
				h->array + offset(h, parent(pos)), h->unit_size);
		pos = parent(pos);
	}
	copy(h->array + offset(h, pos), tmp, h->unit_size);
	return h;
}

heap *shift_down(heap *h, int pos)
{
	int child;
	char tmp[h->unit_size];

	if(!h) return NULL;
	if(pos < 0 || pos >= h->size) return h;
	copy(tmp, h->array + offset(h, pos), h->unit_size);
	while((child = child_left(pos)) < h->size) {
		if(child < h->size - 1 && // pos has a right child
				h->compare(h->array + offset(h, child_right(pos)),
					h->array + offset(h, child_left(pos))) > 0)
			child = child_right(pos);
		if(h->compare(h->array + offset(h, child), tmp) > 0)
			copy(h->array + offset(h, pos),
					h->array + offset(h, child), h->unit_size);
		else break;
		pos = child;
	}
	copy(h->array + offset(h, pos), tmp, h->unit_size);
	return h;
}
