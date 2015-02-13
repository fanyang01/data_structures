#include "heap.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define heap_error(E) fprintf(stderr, "%s:%d:%s: %s\n", \
		__FILE__, __LINE__, __func__, E)

static heap_node *new_node(const void *data);
static heap_node *merge(heap *h, heap_node *x, heap_node *y);
static heap_node *pop_highest(heap *h, heap_node *root, void *des);
static heap_node *insert(heap *h, heap_node *root, void *data);
static void copy(void *des, const void *src, size_t size);
static void free_tree(heap_node *t);

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
	h->root = NULL;
	h->size = 0;
	h->data_size = data_size;
	h->compare = f;
	return h;
}

bool heap_is_empty(const heap *h)
{
	if(!h) return false;
	return h->size == 0;
}

heap *heap_merge(heap *x, heap *y)
{
	if(!x) return y;
	if(!y) return x;
	if(x->data_size != y->data_size) {
		heap_error("heaps differ in unit size");
		return NULL;
	}
	x->root = merge(x, x->root, y->root);
	x->size += y->size;
	return x;
}

heap *heap_insert(heap *h, void *data)
{
	if(!h) return NULL;
	h->root = insert(h, h->root, data);
	if(!h->root) return NULL;
	h->size++;
	return h;
}

heap *heap_pop(heap *h, void *des)
{
	if(!h) return NULL;
	if(heap_is_empty(h)) return NULL;
	h->root = pop_highest(h, h->root, des);
	h->size--;
	return h;
}

const void *heap_highest(heap *h)
{
	if(!h || heap_is_empty(h)) return NULL;
	return h->root->data;
}

void heap_free(heap *h)
{
	if(!h) return;
	free_tree(h->root);
	free(h);
}

heap *heap_clean(heap *h)
{
	if(!h) return NULL;
	free_tree(h->root);
	h->root = NULL;
	h->size = 0;
}

// Static functions START
heap_node *init_node(heap_node *node)
{
	if(!node)
		node = (heap_node *)
			malloc(sizeof(heap_node));
	if(!node) return NULL;

	node->left = NULL;
	node->right = NULL;
	node->npl = 0;
	return node;
}

heap_node *merge(heap *h, heap_node *x, heap_node *y)
{
	if(!x) return y;
	if(!y) return x;
	heap_node *tmp;

	if(h->compare(y->data, x->data) > 0) {
		tmp = x;
		x = y;
		y = tmp;
	}
	x->right = merge(h, x->right, y);

	if(x->left) {
		if(x->left->npl < x->right->npl) {
			x->npl = x->left->npl + 1;
			tmp = x->left;
			x->left = x->right;
			x->right = tmp;
		} else {
			x->npl = x->right->npl + 1;
		}
	} else {
		x->left = x->right;
		x->right = NULL;
		x->npl = 0;
	}

	return x;
}

heap_node *pop_highest(heap *h, heap_node *root, void *des)
{
	if(!h || !root || !des) return NULL;
	heap_node *h_new;

	copy(des, root->data, h->data_size);
	h_new = merge(h, root->left, root->right);
	free(root->data);
	free(root);
	return h_new;
}

heap_node *insert(heap *h, heap_node *root, void *data)
{
	heap_node *new = (heap_node *)malloc(sizeof(heap_node));
	void *new_data = malloc(h->data_size);
	if(!new || !new_data) {
		heap_error("failed to allocate memory");
		return NULL;
	}
	new = init_node(new);
	new->data = new_data;
	copy(new->data, data, h->data_size);
	root = merge(h, root, new);
	if(!root) {
		heap_error("merge new node into heap failed");
		return NULL;
	}
	return root;
}

void free_tree(heap_node *t)
{
	if(!t) return;
	free_tree(t->left);
	free_tree(t->right);
	free(t->data);
	free(t);
}
void copy(void *des, const void *src, size_t size)
{
	memcpy(des, src, size);
}
