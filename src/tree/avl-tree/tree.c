#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define error(S) fprintf(stderr, "%s:%d:%s: %s\n", \
		__FILE__, __LINE__, __func__, S)

static struct tree **search(const avl_tree *t,
		struct tree **root, const void *data);
static struct tree *insert(avl_tree *t,
		struct tree **root, struct tree *node);
static struct tree **minimum(const avl_tree *t, struct tree **p);
static struct tree *new_node(const avl_tree *t, const void *data);
static struct tree *left_rotate(struct tree **x_p);
static struct tree *right_rotate(struct tree **x_p);
static struct tree *double_right_rotate(struct tree **x_p);
static struct tree *double_left_rotate(struct tree **x_p);
static void free_node(avl_tree *t, struct tree *node);
static void copy(void *des, const void *src, size_t size);

avl_tree *tree_init(size_t unit_size, cmp_func f)
{
	if(!f) {
		error("compare function missed");
		return NULL;
	}
	avl_tree *t = (avl_tree *)malloc(sizeof(avl_tree));
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

avl_tree *tree_insert(avl_tree *t, const void *data)
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

#define HEIGHT(x) ((x) ? (x)->height : -1)
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define CORRECT_HEIGHT(x) { \
	x->height = MAX(HEIGHT(x->left), HEIGHT(x->right)) + 1; \
}

struct tree *insert(avl_tree *t, struct tree **root, struct tree *node)
{
	struct tree *r = *root;

	if(!r) {
		*root = node;
		return node;
	} else if(t->compare(node->data, r->data) < 0) {
		insert(t, &r->left, node);
		if(HEIGHT(r->left) - HEIGHT(r->right) == 2) {
			if(t->compare(node->data, r->left->data) < 0) {
				right_rotate(root);
			} else {
				double_right_rotate(root);
			}
		} else {
			CORRECT_HEIGHT(r);
		}
	} else {
		insert(t, &r->right, node);
		if(HEIGHT(r->right) - HEIGHT(r->left) == 2) {
			if(t->compare(node->data, r->right->data) > 0) {
				left_rotate(root);
			} else {
				double_left_rotate(root);
			}
		} else {
			CORRECT_HEIGHT(r);
		}
	}
	return *root;
}

void *tree_search(avl_tree *t, const void *data)
{
	struct tree **node = search(t, &t->root, data);
	if(!node) return NULL;
	return (*node)->data;
}

avl_tree *tree_delete(avl_tree *t, const void *data)
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

void tree_free(avl_tree *t)
{
	if(!t) return;
	free_node(t, t->root);
	free(t);
}

struct tree **search(const avl_tree *t,
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

struct tree **minimum(const avl_tree *t, struct tree **p)
{
	struct tree *node = *p;

	while(node->left != NULL) {
		p = &node->left;
		node = node->left;
	}
	return p;
}

struct tree *new_node(const avl_tree *t, const void *data)
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
	node->height = 0;
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
struct tree *left_rotate(struct tree **x_p)
{
	struct tree *x = *x_p;
	struct tree *y = x->right;
	x->right = y->left;
	y->left = x;
	*x_p = y;
	CORRECT_HEIGHT(x);
	CORRECT_HEIGHT(y);
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
struct tree *right_rotate(struct tree **x_p)
{
	struct tree *x = *x_p;
	struct tree *y = x->left;
	x->left = y->right;
	y->right = x;
	*x_p = y;
	CORRECT_HEIGHT(x);
	CORRECT_HEIGHT(y);
	return y;
}

/*
 *              x
 *             / \
 *            y   d
 *           / \
 *          a   z
 *             / \
 *            b   c
 *  ->
 *              z
 *            /   \
 *           y     x
 *          / \   / \
 *         a   b c   d
 */
struct tree *double_right_rotate(struct tree **x_p)
{
	struct tree *x = *x_p;
	struct tree *y = x->left;
	struct tree *z = y->right;

	*x_p = z;
	y->right = z->left;
	x->left = z->right;
	z->left = y;
	z->right = x;
	CORRECT_HEIGHT(x);
	CORRECT_HEIGHT(y);
	CORRECT_HEIGHT(z);
	return z;
}

/*
 *            x
 *           / \
 *          a   y
 *             / \
 *            z   d
 *           / \
 *          b   c
 * ->
 *            z
 *          /   \
 *         x     y
 *        / \   / \
 *       a   b c   d
 */
struct tree *double_left_rotate(struct tree **x_p)
{
	struct tree *x = *x_p;
	struct tree *y = x->right;
	struct tree *z = y->left;

	*x_p = z;
	y->left = z->right;
	x->right = z->left;
	z->right = y;
	z->left = x;
	CORRECT_HEIGHT(x);
	CORRECT_HEIGHT(y);
	CORRECT_HEIGHT(z);
	return z;
}

void free_node(avl_tree *t, struct tree *node)
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
