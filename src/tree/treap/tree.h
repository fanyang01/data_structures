#ifndef _TREAP_H
#define _TREAP_H

#include <stdlib.h>
#include <stdbool.h>

typedef int (*cmp_func)(const void *, const void *);

struct tree {
	struct tree *left;
	struct tree *right;
	int priority;
	void *data;
};

typedef struct {
	cmp_func compare;
	size_t unit_size;
	size_t size;
	struct tree *root;
} treap;

extern treap *tree_init(size_t unit_size, cmp_func f);
extern treap *tree_insert(treap *t, const void *data);
extern void *tree_search(treap *t, const void *data);
extern treap *tree_delete(treap *t, const void *data);
extern void tree_free(treap *t);

#endif
