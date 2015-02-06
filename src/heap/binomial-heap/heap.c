#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"
#include "heap.h"

#define HEAD(x) (list_is_empty(x) ? NULL : \
		(heap_tree *)(list_head(x)))
#define TAIL(x) (list_is_empty(x) ? NULL : \
		(heap_tree *)(list_tail(x)))
#define NEXT(x, NODE) ((NODE) == TAIL(x) ? NULL : \
		(heap_tree *)list_next(&(NODE)->siblings))

static bool merge(heap *h, struct list_head *x, struct list_head *y);
static heap_tree *merge_tree(heap *h, struct list_head *x,
		struct list_head *y, heap_tree *tree_x, heap_tree *tree_y);
static heap_tree *merge_carry(heap *h, heap_tree *x, heap_tree *c);
static heap_tree *new_tree(heap *h, void *data);
static void free_list(struct list_head *list);
static void heap_error(const char *s);
static void copy(void *des, const void *src, size_t size);

heap *heap_init(size_t unit_size, cmp_func f)
{
	if(!f) {
		heap_error("heap_init: compare function missed");
		return NULL;
	}
	heap *h = (heap *)malloc(sizeof(heap));
	if(!h) {
		heap_error("heap_init: failed to allocate memory");
		return NULL;
	}
	h->unit_size = unit_size;
	h->size = 0;
	h->compare = f;
	INIT_LIST_HEAD(&h->list);
	return h;
}

bool heap_is_empty(heap *h)
{
	if(!h) return false;
	return h->size == 0;
}

void heap_free(heap *h)
{
	if(!h) return;
	free_list(&h->list);
	free(h);
}

heap *heap_clean(heap *h)
{
	if(h) free_list(&h->list);
	h->size = 0;
	INIT_LIST_HEAD(&h->list);
	return h;
}

heap *heap_merge(heap *x, heap *y)
{
	if(!x) return y;
	if(!y) return x;
	if(list_is_empty(&x->list) && list_is_empty(&y->list))
		return x;
	if(!merge(x, &x->list, &y->list))
		return NULL;
	x->size += y->size;
	return x;
}

heap *heap_pop(heap *h, void *des)
{
	if(!h || !des) return NULL;
	if(h->size == 0) {
		heap_error("heap_pop: heap is empty");
		return NULL;
	}
	heap_tree *highest = NULL;
	struct list_node *pos;

	list_for_each(pos, &h->list) {
		if(highest) {
			if(h->compare(highest->data,
				((heap_tree *)pos)->data) < 0) {
				highest = (heap_tree *)pos;
			}
		} else {
			highest = (heap_tree *)pos;
		}
	}

	list_del(&highest->siblings);
	copy(des, highest->data, h->unit_size);
	if(!merge(h, &h->list, &highest->childs)) {
		heap_error("heap_pop: merge failed");
		return NULL;
	}
	free(highest->data);
	free(highest);
	h->size--;
	return h;
}

heap *heap_insert(heap *h, void *data)
{
	if(!h || !data) return NULL;
	struct list_head list;
	INIT_LIST_HEAD(&list);
	heap_tree *t = new_tree(h, data);
	if(!t) return NULL;
	list_add_head(&list, &t->siblings);
	if(!merge(h, &h->list, &list)) {
		heap_error("heap_insert: merge failed");
		return NULL;
	}
	h->size++;
	return h;
}

bool merge(heap *h, struct list_head *x, struct list_head *y)
{
	if(!h || !x || !y) return false;
	if(list_is_empty(x)) {
		list_merge(x, y);
		return true;
	}
	if(list_is_empty(y)) return true;
	heap_tree *carry = merge_tree(h, x, y, HEAD(x), HEAD(y));
	if(carry) list_add_head(x, &carry->siblings);
	return true;
}

heap_tree *merge_tree(heap *h, struct list_head *x,
		struct list_head *y, heap_tree *tree_x, heap_tree *tree_y)
{
	if(!tree_y) return NULL;
	if(!tree_x) {
		heap_tree *next_y = NEXT(y, tree_y);
		list_del(&tree_y->siblings);
		list_add_tail(x, &tree_y->siblings);
		return merge_tree(h, x, y, tree_x, next_y);
	}
	heap_tree *carry;

	if(tree_x->rank > tree_y->rank) {
		carry = merge_tree(h, x, y, NEXT(x, tree_x), tree_y);
		if(!carry) {
			return NULL;
		} else if(carry->rank == tree_x->rank) {
			return merge_carry(h, tree_x, carry);
		} else {
			list_add(&tree_x->siblings, &carry->siblings);
			return NULL;
		}
	} else if(tree_x->rank < tree_y->rank) {
		carry = merge_tree(h, x, y, tree_x, NEXT(y, tree_y));
		if(!carry) {
			return NULL;
		} else if(carry->rank == tree_y->rank) {
			return merge_carry(h, tree_y, carry);
		} else {
			list_del(&tree_y->siblings);
			list_add_prev(&tree_x->siblings, &tree_y->siblings);
			list_add_prev(&tree_x->siblings, &carry->siblings);
			return NULL;
		}
	}
	carry = merge_tree(h, x, y, NEXT(x, tree_x), NEXT(y, tree_y));
	if(carry) {
		list_add(&tree_x->siblings, &carry->siblings);
	}
	list_del(&tree_y->siblings);
	return merge_carry(h, tree_x, tree_y);
}

heap_tree *merge_carry(heap *h, heap_tree *x, heap_tree *c)
{
	if(!x || !c) return NULL;
	if(x->rank != c->rank) {
		heap_error("two heap trees differ on rank");
		return NULL;
	}
	list_del(&x->siblings);
	if(h->compare(x->data, c->data) < 0) {
		heap_tree *tmp = x;
		x = c;
		c = tmp;
	}
	list_add_head(&x->childs, &c->siblings);
	x->rank++;
	return x;
}

heap_tree *new_tree(heap *h, void *data)
{
	heap_tree *t = (heap_tree *)
		malloc(sizeof(heap_tree));
	void *t_data = malloc(h->unit_size);
	if(!t || !t_data) {
		heap_error("new_tree: failed to allocate memory");
		return NULL;
	}
	t->data = t_data;
	t->rank = 0;
	INIT_LIST_HEAD(&t->childs);
	copy(t->data, data, h->unit_size);
	return t;
}

void free_list(struct list_head *list)
{
	if(!list) return;
	struct list_node *pos, *next;
	list_for_each_safe(pos, next, list) {
		free(((heap_tree *)pos)->data);
		free_list(&((heap_tree *)pos)->childs);
		free(pos);
	}
}

void heap_error(const char *s)
{
	fprintf(stderr, "heap.c: %s\n", s);
}

void copy(void *des, const void *src, size_t size)
{
	memcpy(des, src, size);
}
