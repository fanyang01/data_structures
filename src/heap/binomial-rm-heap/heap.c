#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "heap.h"

/*
 *     16
 *     |
 *     8 -> 4 -> 2 -> 1
 *     |    |    |
 *     |    |    1
 *     |    |
 *     |    2 -> 1
 *     |    |
 *     |    1
 *     |
 *     4 -> 2 -> 1
 *     |    |
 *     |    1
 *     |
 *     2 -> 1
 *     |
 *     1
 */

#define heap_error(E) fprintf(stderr, "%s:%d:%s: %s\n", \
		__FILE__, __LINE__, __func__, E)

typedef struct list_head list_head;
typedef struct list_node list_node;

static heap_tree *merge(heap *h, heap_tree **x, heap_tree **y);
static heap_tree *new_tree(heap *h, const void *data);
static void free_list(heap_tree *list);
static void copy(void *des, const void *src, size_t size);

heap *heap_init(size_t data_size, cmp_func f)
{
	if(!f) {
		heap_error("compare function missed");
		return NULL;
	}
	heap *h = (heap *)malloc(sizeof(heap));
	if(!h) {
		heap_error("failed to allocate memory");
		return NULL;
	}
	h->data_size = data_size;
	h->size = 0;
	h->compare = f;
	h->root_list = NULL;
	return h;
}

bool heap_is_empty(const heap *h)
{
	if(!h) return false;
	return h->size == 0;
}

void heap_free(heap *h)
{
	if(!h) return;
	free_list(h->root_list);
	free(h);
}

heap *heap_clean(heap *h)
{
	if(h) {
		free_list(h->root_list);
		h->size = 0;
		h->root_list = NULL;
	}
	return h;
}

heap *heap_merge(heap *x, heap *y)
{
	if(!x) return y;
	if(!y) return x;
	heap_tree *res = merge(x, &x->root_list, &y->root_list);
	x->size += y->size;
	return x;
}

heap *heap_pop(heap *h, void *des)
{
	if(!h || !des) return NULL;
	if(h->size == 0) {
		heap_error("heap is empty");
		return NULL;
	}
	heap_tree *highest = h->root_list;
	heap_tree **highest_prev = &h->root_list;
	heap_tree *pos = h->root_list->next;
	heap_tree **prev = &h->root_list->next;

	while(pos) {
		if(h->compare(highest->data, pos->data) < 0) {
			highest = pos;
			highest_prev = prev;
		}
		prev = &pos->next;
		pos = pos->next;
	}

	*highest_prev = highest->next;
	copy(des, highest->data, h->data_size);
	h->root_list = merge(h, &h->root_list, &highest->childs);
	free(highest->data);
	free(highest);
	h->size--;
	return h;
}

const void *heap_highest(const heap *h)
{
	if(!h) return NULL;
	if(h->size == 0) {
		/* heap_error("heap is empty"); */
		return NULL;
	}
	heap_tree *highest = h->root_list;
	heap_tree *pos = h->root_list->next;

	while(pos) {
		if(h->compare(highest->data, pos->data) < 0)
			highest = pos;
		pos = pos->next;
	}
	return highest->data;
}

heap_tree *heap_insert(heap *h, const void *data)
{
	if(!h || !data) return NULL;
	heap_tree *t = new_tree(h, data);
	if(!t) return NULL;
	h->root_list = merge(h, &h->root_list, &t);
	h->size++;
	return t;
}

heap_tree *merge(heap *h, heap_tree **a, heap_tree **b)
{
	heap_tree *x = *a;
	heap_tree *y = *b;
	if(!x && !y) return NULL;
	if(!x) {
		y->prev = a;
		return y;
	}
	if(!y) {
		x->prev = a;
		return x;
	}

	if(x->rank > y->rank) {
		y = merge(h, &x->next, b);
		x->next = NULL;
		if(y->rank < x->rank) {
			x->next = y;
			x->prev = a;
			return x;
		}
	}
	if(x->rank == y->rank) {
		heap_tree *rest = merge(h, &x->next, &y->next);
		if(h->compare(x->data, y->data) < 0) {
			heap_tree *tmp = x;
			x = y;
			y = tmp;
		}
		y->next = x->childs;
		y->prev = &x->childs;
		if(x->childs) x->childs->prev = &y->next;
		x->childs = y;
		y->p = x;
		x->rank++;
		rest->prev = &x->next;
		x->next = rest;
		return x;
	} else {
		return merge(h, b, a);
	}
}

heap_tree *new_tree(heap *h, const void *data)
{
	heap_tree *t = (heap_tree *)
		malloc(sizeof(heap_tree));
	void *t_data = malloc(h->data_size);
	if(!t || !t_data) {
		heap_error("failed to allocate memory");
		free(t);
		free(t_data);
		return NULL;
	}
	t->data = t_data;
	t->rank = 0;
	t->next = t->childs = t->p = NULL;
	t->prev = NULL;
	copy(t->data, data, h->data_size);
	return t;
}

void free_list(heap_tree *list)
{
	if(!list) return;
	while(!list) {
		free(list->data);
		free_list(list->childs);
		heap_tree *next = list->next;
		free(list);
		list = next;
	}
}

void copy(void *des, const void *src, size_t size)
{
	memcpy(des, src, size);
}
