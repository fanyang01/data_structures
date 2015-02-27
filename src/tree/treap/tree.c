#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define error(S) fprintf(stderr, "%s:%d:%s: %s\n", \
		__FILE__, __LINE__, __func__, S)

static struct tree **search(const treap *t,
		struct tree **root, const void *data);
static struct tree *insert(treap *t,
		struct tree **root, struct tree *node);
static struct tree **minimum(const treap *t, struct tree **p);
static struct tree *new_node(const treap *t, const void *data);
static struct tree *left_rotate(treap *t, struct tree **x_p);
static struct tree *right_rotate(treap *t, struct tree **x_p);
static void free_node(treap *t, struct tree *node);
static void copy(void *des, const void *src, size_t size);
static int tree_rand(void);

treap *tree_init(size_t unit_size, cmp_func f)
{
	if(!f) {
		error("compare function missed");
		return NULL;
	}
	treap *t = (treap *)malloc(sizeof(treap));
	if(!t) {
		error("failed to allocate memory");
		return NULL;
	}
	t->unit_size = unit_size;
	t->size = 0;
	t->compare = f;
	t->root = NULL;
	return t;
}

treap *tree_insert(treap *t, const void *data)
{
	if(!t || !data) return NULL;

	struct tree *node = new_node(t, data);
	if(!node) {
		error("failed to make new node");
		return NULL;
	}

	insert(t, &t->root, node);
	t->size++;
	return t;
}

/* #define PRIORITY(x) ((x) ? (x)->priority : -1) */
#define PRIORITY(x) ((x)->priority)

struct tree *insert(treap *t, struct tree **root, struct tree *node)
{
	struct tree *r = *root;

	if(!r) {
		*root = node;
		return node;
	} else if(t->compare(node->data, r->data) < 0) {
		insert(t, &r->left, node);
		if(PRIORITY(r) < PRIORITY(r->left))
			right_rotate(t, root);
	} else {
		insert(t, &r->right, node);
		if(PRIORITY(r) < PRIORITY(r->right))
			left_rotate(t, root);
	}
	return *root;
}

void *tree_search(treap *t, const void *data)
{
	struct tree **node = search(t, &t->root, data);
	if(!node) return NULL;
	return (*node)->data;
}

treap *tree_delete(treap *t, const void *data)
{
	if(!t) return NULL;
	if(!data) return t;
	struct tree *x, *y;
	struct tree **x_p, **y_p;

	x_p = search(t, &t->root, data);
	if(!x_p) {
		/* error("not found"); */
		return NULL;
	}

	x = *x_p;
	if(x->left == NULL) {
		*x_p = x->right;
	} else if(x->right == NULL) {
		*x_p = x->left;
	} else {
		y_p = minimum(t, &x->right);
		y = *y_p;
		/*
		 * when y is the right child of x,
		 * no need to move y->right
		 */
		if(x->right != y) {
			*y_p = y->right;
			y->right = x->right;
		}
		y->left = x->left;
		*x_p = y;
	}
	free(x->data);
	free(x);
	t->size--;
	return t;
}

void tree_free(treap *t)
{
	if(!t) return;
	free_node(t, t->root);
	free(t);
}

struct tree **search(const treap *t,
		struct tree **root, const void *data)
{
	struct tree *x = *root;
	struct tree **p = root;
	int res;

	while(x != NULL) {
		if((res = t->compare(data, x->data)) == 0) {
			return p;
		} else if(res < 0) {
			p = &x->left;
			x = x->left;
		} else {
			p = &x->right;
			x = x->right;
		}
	}
	return NULL;
}

struct tree **minimum(const treap *t, struct tree **p)
{
	struct tree *node = *p;

	while(node->left != NULL) {
		p = &node->left;
		node = node->left;
	}
	return p;
}

struct tree *new_node(const treap *t, const void *data)
{
	if(!t) return NULL;

	struct tree *node = (struct tree *)
		malloc(sizeof(struct tree));
	if(!node) {
		error("failed to allocate memory");
		return NULL;
	}
	node->data = malloc(t->unit_size);
	if(!node->data) {
		error("failed to allocate memory");
		return NULL;
	}

	copy(node->data, data, t->unit_size);
	node->left = node->right = NULL;
	node->priority = tree_rand();
	return node;
}

/*
 *           x
 *          / \
 *         a   y
 *            / \
 *           b   c
 * ->
 *           y
 *          / \
 *         x   c
 *        / \
 *       a   b
 */
struct tree *left_rotate(treap *t, struct tree **x_p)
{
	struct tree *x = *x_p;
	struct tree *y = x->right;
	x->right = y->left;
	y->left = x;
	*x_p = y;
	return y;
}

/*
 *           x
 *          / \
 *         y   c
 *        / \
 *       a   b
 * ->
 *           y
 *          / \
 *         a   x
 *            / \
 *           b   c
 */
struct tree *right_rotate(treap *t, struct tree **x_p)
{
	struct tree *x = *x_p;
	struct tree *y = x->left;
	x->left = y->right;
	y->right = x;
	*x_p = y;
	return y;
}

void free_node(treap *t, struct tree *node)
{
	if(node == NULL) return;
	free_node(t, node->left);
	free_node(t, node->right);
	free(node->data);
	free(node);
}

void copy(void *des, const void *src, size_t size)
{
	memcpy(des, src, size);
}

int tree_rand(void)
{
	return rand();
}
