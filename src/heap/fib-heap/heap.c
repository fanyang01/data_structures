#include "heap.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE  1
#define MAX_DEGREE (1<<8)

#define heap_error(E) fprintf(stderr, "%s:%d:%s: %s\n", \
		__FILE__, __LINE__, __func__, E)

static void consolidate(heap *h);
static void cut(heap *h, heap_node *x, heap_node *p);
static void cascading_cut(heap *h, heap_node *x);
static void consolidate(heap *h);
static heap_node *new_node(heap *h, const void *data);
static void free_list(struct list_head *list);
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
	h->highest = NULL;
	INIT_LIST_HEAD(&h->root_list);
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
	free_list(&h->root_list);
	free(h);
}

heap *heap_clean(heap *h)
{
	if(h) {
		free_list(&h->root_list);
		h->size = 0;
		INIT_LIST_HEAD(&h->root_list);
	}
	return h;
}

heap_handle heap_insert(heap *h, const void *data)
{
	if(!h || !data) return NULL;
	heap_node *node = new_node(h, data);
	if(!node) {
		heap_error("make new node failed");
		return NULL;
	}
	list_add_tail(&h->root_list, &node->node);
	if(!h->highest ||
			h->compare(node->data, h->highest->data) > 0) {
		h->highest = node;
	}
	h->size++;
	return node;
}

heap *heap_pop(heap *h, void *des)
{
	if(!h || !des) return NULL;
	if(h->size == 0) return NULL;
	copy(des, h->highest->data, h->data_size);
	list_merge_at(&h->root_list,
			&h->highest->childs, &h->highest->node);
	heap_node *tmp = h->highest;
	h->highest = (heap_node *)list_next(&h->highest->node);
	list_del(&tmp->node);
	if(list_is_empty(&h->root_list))
		h->highest = NULL;
	else
		consolidate(h);
	h->size--;
	return h;
}

heap *heap_merge(heap *x, heap *y)
{
	if(!x) return y;
	if(!y) return x;
	list_merge(&x->root_list, &y->root_list);
	if(!x->highest || y->highest && x->compare(x->highest->data,
				y->highest->data) < 0) {
		x->highest = y->highest;
	}
	x->size += y->size;
	return x;
}

heap *heap_inc_priority(heap *h, heap_node *x, const void *data)
{
	if(!h || !x || !data) return NULL;
	if(h->compare(x->data, data) > 0) {
		heap_error("new value has lower priority");
		return NULL;
	}
	copy(x->data, data, h->data_size);
	heap_node *p = x->p;
	if(p && h->compare(p->data, x->data) < 0) {
		cut(h, x, p);
		cascading_cut(h, p);
	}
	if(h->compare(x->data, h->highest->data) > 0) {
		h->highest = x;
	}
	return h;
}

void cut(heap *h, heap_node *x, heap_node *p)
{
	list_del(&x->node);
	p->degree--;
	list_add_tail(&h->root_list, &x->node);
	x->p = NULL;
	x->mark = FALSE;
}

void cascading_cut(heap *h, heap_node *x)
{
	heap_node *p = x->p;
	if(p) {
		if(x->mark == FALSE) {
			x->mark = TRUE;
		} else {
			cut(h, x, p);
			cascading_cut(h, p);
		}
	}
}

void consolidate(heap *h)
{
	int i;
	heap_node *degree[MAX_DEGREE];

	memset(degree, 0, MAX_DEGREE * sizeof(heap_node *));
	struct list_node *pos, *next;
	list_for_each_safe(pos, next, &h->root_list) {
		heap_node *x = (heap_node *)pos;
		int d = x->degree;
		while(degree[d]) {
			heap_node *y = degree[d];
			if(h->compare(x->data, y->data) < 0) {
				heap_node *tmp = x;
				x = y;
				y = tmp;
			}
			/* link y to x and clear the mark of y */
			list_del(&y->node);
			list_add_head(&x->childs, &y->node);
			y->p = x;
			y->mark == FALSE;
			x->degree++;
			degree[d++] = NULL;
		}
		degree[d] = x;
	}
	INIT_LIST_HEAD(&h->root_list);
	h->highest = NULL;
	for(i = 0; i < MAX_DEGREE; i++) {
		if(degree[i]) {
			degree[i]->p = NULL;
			list_add_head(&h->root_list, &degree[i]->node);
			if(!h->highest || h->compare(h->highest->data,
						degree[i]->data) < 0) {
				h->highest = degree[i];
			}
		}
	}
}

heap_node *new_node(heap *h, const void *data)
{
	heap_node *node = (heap_node *)
		malloc(sizeof(heap_node));
	void *node_data = malloc(h->data_size);
	if(!node || !node_data) {
		heap_error("failed to allocate memory");
		free(node);
		free(node_data);
		return NULL;
	}
	node->data = node_data;
	node->degree = 0;
	INIT_LIST_HEAD(&node->childs);
	node->mark = FALSE;
	node->p = NULL;
	copy(node->data, data, h->data_size);
	return node;
}

void free_list(struct list_head *list)
{
	struct list_node *pos, *next;

	list_for_each_safe(pos, next, list) {
		heap_node *cur = (heap_node *)pos;
		free(cur->data);
		free_list(&cur->childs);
		free(cur);
	}
}

void copy(void *des, const void *src, size_t size)
{
	memcpy(des, src, size);
}
