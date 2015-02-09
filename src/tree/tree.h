#ifndef _TREE_H
#define _TREE_H

#include <stdlib.h>
#include <stdbool.h>

typedef int (*cmp_func)(void *, void *);

struct tree {
	struct tree *p;
	struct tree *left;
	struct tree *right;
	char color;
	void *data;
};

typedef struct {
	cmp_func compare;
	size_t unit_size;
	size_t size;
	struct tree *nil;
	struct tree *root;
} rb_tree;

extern rb_tree *tree_init(size_t unit_size, cmp_func f);
extern rb_tree *tree_insert(rb_tree *t, void *data);
extern void *tree_search(rb_tree *t, void *data);
extern rb_tree *tree_delete(rb_tree *t, void *data, void *des);
extern void tree_free(rb_tree *t);

#endif
