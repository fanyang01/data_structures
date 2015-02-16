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

static heap_tree *merge(heap *h, heap_tree *x, heap_tree *y);
static heap_tree *new_tree(heap *h, const void *data);
static void free_list(heap_tree *list);
static void copy(void *des, const void *src, size_t size);
static void swap(heap *h, heap_tree *x, heap_tree *p);
 
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
	h->list = NULL;
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
	free_list(h->list);
	free(h);
}

heap *heap_clean(heap *h)
{
	if(h) {
		free_list(h->list);
		h->size = 0;
		h->list = NULL;
	}
	return h;
}

heap *heap_merge(heap *x, heap *y)
{
	if(!x) return y;
	if(!y) return x;
	x->list = merge(x, x->list, y->list);
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
	heap_tree *highest = h->list;
	heap_tree *pos = h->list->siblings;

	while(pos) {
		if(h->compare(highest->data, pos->data) < 0) {
			highest = pos;
		}
		pos = pos->siblings;
	}

	if(highest->siblings) {
		highest->siblings->prev = highest->prev;
	}
	if(highest->prev) {
		highest->prev->siblings = highest->siblings;
	} else {
		h->list = highest->siblings;
	}
	copy(des, highest->data, h->data_size);
	h->list = merge(h, h->list, highest->childs);
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
	heap_tree *highest = h->list;
	heap_tree *pos = h->list->siblings;

	while(pos) {
		if(h->compare(highest->data, pos->data) < 0)
			highest = pos;
		pos = pos->siblings;
	}
	return highest->data;
}

heap_tree *heap_insert(heap *h, const void *data)
{
	if(!h || !data) return NULL;
	heap_tree *t = new_tree(h, data);
	if(!t) return NULL;
	h->list = merge(h, h->list, t);
	h->size++;
	return t;
}

#define IS_FIRST_CHILD(x) (!((x)->prev) || (x)->prev->childs == (x))

heap_tree *heap_inc_priority(heap *h, heap_tree *node, const void *data)
{
	if(!h || !node || !data) return NULL;
	if(h->compare(node->data, data) > 0) {
		heap_error("new value has lower priority");
		return NULL;
	}
	heap_tree *p, *x, *y;

	copy(node->data, data, h->data_size);
	x = y = node;

	/* move y to be the first child of x's parent */
	while(!IS_FIRST_CHILD(y)) {
		y = y->prev;
	}

	while(y->prev) {
		p = y->prev;
		if(h->compare(x->data, p->data) > 0) {
			/* swap x and p */
			swap(h, x, p);
			/* move y to be the first child of x's parent */
			y = x;
			while(!IS_FIRST_CHILD(y)) {
				y = y->prev;
			}
		} else {
			break;
		}
	}
	return x;
}

heap_tree *merge(heap *h, heap_tree *x, heap_tree *y)
{
	if(!y) return x;
	if(!x) {
		y->prev = NULL;
		return y;
	}

	if(x->rank > y->rank) {
		y = merge(h, x->siblings, y);
		x->siblings = NULL;
		if(y->rank < x->rank) {
			x->siblings = y;
			y->prev = x;
			return x;
		}
	}
	if(x->rank == y->rank) {
		heap_tree *rest = merge(h, x->siblings, y->siblings);
		if(h->compare(x->data, y->data) < 0) {
			heap_tree *tmp = x;
			x = y;
			y = tmp;
		}
		y->siblings = x->childs;
		if(x->childs) x->childs->prev = y;
		x->childs = y;
		/* let y->prev point to its parent */
		y->prev = x;
		x->rank++;
		x->siblings = rest;
		if(rest) rest->prev = x;
		return x;
	} else {
		return merge(h, y, x);
	}
}

void swap(heap *h, heap_tree *x, heap_tree *p)
{
	heap_tree *tmp;

	if(IS_FIRST_CHILD(p)) {
		if(p->prev) p->prev->childs = x;
		else h->list = x;
	} else {
		p->prev->siblings = x;
	}

	if(IS_FIRST_CHILD(x)) {
		p->childs = x->childs;
		x->childs = p;
		x->prev = p->prev;
		p->prev = x;
	} else {
		x->prev->siblings = p;
		tmp = x->prev;
		x->prev = p->prev;
		p->prev = tmp;

		tmp = x->childs;
		x->childs = p->childs;
		p->childs = tmp;
	}
	if(p->childs) p->childs->prev = p;
	x->childs->prev = x;

	/* swap siblings */
	tmp = x->siblings;
	x->siblings = p->siblings;
	p->siblings = tmp;
	if(p->siblings) p->siblings->prev = p;
	if(x->siblings) x->siblings->prev = x;
	/* swap rank */
	int tmp_i = x->rank;
	x->rank = p->rank;
	p->rank = tmp_i;
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
	t->siblings = t->childs = NULL;
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
		heap_tree *next = list->siblings;
		free(list);
		list = next;
	}
}

void copy(void *des, const void *src, size_t size)
{
	memcpy(des, src, size);
}
