#ifndef _TREAP_H
#define _TREAP_H

#include <stdlib.h>
#include <stdbool.h>

typedef int (*cmp_func)(const void *, const void *);

struct tree {
	struct tree *left;
	struct tree *right;
	int height;
	void *data;
};

typedef struct {
	cmp_func compare;
	size_t unit_size;
	size_t size;
	struct tree *root;
} avl_tree;

extern avl_tree *tree_init(size_t unit_size, cmp_func f);
extern avl_tree *tree_insert(avl_tree *t, const void *data);
extern void *tree_search(avl_tree *t, const void *data);
extern avl_tree *tree_delete(avl_tree *t, const void *data);
extern void tree_free(avl_tree *t);

#endif
