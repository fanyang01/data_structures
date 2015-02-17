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

static heap_tree *merge(heap *h, heap_tree **a, heap_tree **b);
static heap_tree *new_tree(heap *h, const void *data);
static void free_list(heap_tree *list);
static void copy(void *des, const void *src, size_t size);
inline static void swap(heap_tree *x, heap_tree *p);

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
	x->list = merge(x, &x->list, &y->list);
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
	heap_tree *pos = h->list->next;

	while(pos) {
		if(h->compare(highest->data, pos->data) < 0) {
			highest = pos;
		}
		pos = pos->next;
	}

	if(highest->next) {
		highest->next->prev = highest->prev;
	}
	*highest->prev = highest->next;
	copy(des, highest->data, h->data_size);
	h->list = merge(h, &h->list, &highest->childs);
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
	heap_tree *pos = h->list->next;

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
	h->list = merge(h, &h->list, &t);
	h->size++;
	return t;
}

#define IS_FIRST_CHILD(x) ((x)->first)
#define SIBLING(x) ((heap_tree *)(((void *)(x)->prev) - (long)(&(((heap_tree *)0)->next))))
#define PARENT(x)  ((heap_tree *)(((void *)(x)->prev) - (long)(&(((heap_tree *)0)->childs))))

heap_tree *heap_inc_priority(heap *h, heap_tree *node, const void *data)
{
	if(!h || !node || !data) return NULL;
	if(h->compare(node->data, data) > 0) {
		heap_error("new value has lower priority");
		return NULL;
	}
	heap_tree *p, *x, *y;
	heap_tree **root_list = &h->list;

	copy(node->data, data, h->data_size);
	x = y = node;

	/* move y to be the first child of x's parent */
	while(!IS_FIRST_CHILD(y)) {
		y = SIBLING(y);
	}

	while(y->prev != root_list) {
		p = PARENT(y);
		if(h->compare(x->data, p->data) > 0) {
			/* swap x and p */
			swap(x, p);
			/* move y to be the first child of x's parent */
			y = x;
			while(!IS_FIRST_CHILD(y)) {
				y = SIBLING(y);
			}
		} else {
			break;
		}
	}
	return x;
}

heap_tree *merge(heap *h, heap_tree **a, heap_tree **b)
{
	heap_tree *x = *a;
	heap_tree *y = *b;
	if(!y) return x;
	if(!x) {
		y->prev = a;
		y->first = true;
		return y;
	}

	if(x->rank > y->rank) {
		y = merge(h, &x->next, b);
		x->next = NULL;
		/* x->first = true; */
		if(y->rank < x->rank) {
			x->next = y;
			y->prev = &x->next;
			y->first = false;
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
		if(x->childs) {
			x->childs->prev = &y->next;
			x->childs->first = false;
		}
		x->childs = y;
		/* x->first = true; */
		y->first = true;
		y->prev = &x->childs;
		x->rank++;
		x->next = rest;
		if(rest) {
			rest->prev = &x->next;
			rest->first = false;
		}
		return x;
	} else {
		return merge(h, b, a);
	}
}

void swap(heap_tree *x, heap_tree *p)
{
	*p->prev = x;
	if(IS_FIRST_CHILD(x)) {
		p->childs = x->childs;
		x->childs = p;

		x->prev = p->prev;
	} else {
		*x->prev = p;

		void *tmp = x->childs;
		x->childs = p->childs;
		p->childs = tmp;

		tmp = x->prev;
		x->prev = p->prev;
		p->prev = tmp;
	}
	x->childs->prev = &x->childs;
	if(p->childs) p->childs->prev = &p->childs;

	void *tmp = x->next;
	x->next = p->next;
	p->next = tmp;
	if(p->next) p->next->prev = &p->next;
	if(x->next) x->next->prev = &x->next;

	int tmp_i = x->rank;
	x->rank = p->rank;
	p->rank = tmp_i;

	bool tmp_b = x->first;
	x->first = p->first;
	p->first = tmp_b;
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
	t->next = t->childs = NULL;
	t->prev = NULL;
	t->first = false;
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
