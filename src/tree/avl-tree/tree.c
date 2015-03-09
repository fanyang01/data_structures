#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* If lazy deletion is defined, ordinary BST deletion will be performed, */
/* only height infomation will be updated without any rotation. */
/* #define LAZY_DELETION 1 */

#define HEIGHT(x) ((x) ? (x)->height : -1)
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define DIFF(x) (HEIGHT(x->left) - HEIGHT(x->right))
#define UPDATE_HEIGHT(x) { \
	(x)->height = MAX(HEIGHT((x)->left), HEIGHT((x)->right)) + 1; \
}

#define error(S) fprintf(stderr, "%s:%d:%s: %s\n", \
		__FILE__, __LINE__, __func__, S)
static struct tree *balance(struct tree **root);
static struct tree *d_balance(struct tree **root);
static struct tree **search(const avl_tree *t,
		struct tree **root, const void *data);
static struct tree *insert(avl_tree *t,
		struct tree **root, struct tree *node);
static struct tree *delete(avl_tree *t,
		struct tree **root, const void *data);
static struct tree *delete_min(struct tree **p);
static struct tree *new_node(const avl_tree *t, const void *data);
static struct tree *left_rotate(struct tree **x_p);
static struct tree *right_rotate(struct tree **x_p);
static struct tree *double_right_rotate(struct tree **x_p);
static struct tree *double_left_rotate(struct tree **x_p);
static void free_node(avl_tree *t, struct tree *node);
static void copy(void *des, const void *src, size_t size);


avl_tree *tree_init(size_t data_size, cmp_func f)
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
	t->data_size = data_size;
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

struct tree *insert(avl_tree *t, struct tree **root, struct tree *node)
{
	struct tree *r = *root;

	if(!r) {
		*root = node;
		return node;
	}
	if(t->compare(node->data, r->data) < 0) {
		insert(t, &r->left, node);
	} else {
		insert(t, &r->right, node);
	}
	UPDATE_HEIGHT(r);
	balance(root);
	return *root;
}

void *tree_search(avl_tree *t, const void *data)
{
	struct tree **node = search(t, &t->root, data);
	if(!node) return NULL;
	return (*node)->data;
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

avl_tree *tree_delete(avl_tree *t, void *data)
{
	if(!t) return NULL;
	if(!data) return t;

	struct tree *node = delete(t, &t->root, data);
	if(!node) {
		return NULL;
	}
	copy(data, node->data, t->data_size);
	t->size--;
	return t;
}

struct tree *delete(avl_tree *t,
		struct tree **root, const void *data)
{
	struct tree *r = *root;
	if(!r) return NULL;
	struct tree *ret, *y;

	if(t->compare(data, r->data) < 0) {
		ret = delete(t, &r->left, data);
	} else if(t->compare(data, r->data) > 0) {
		ret = delete(t, &r->right, data);
	} else {
		ret = r;
		if(!r->right) {
			*root = r->left;
			return ret;
		} else if(!r->left) {
			*root = r->right;
			return ret;
		} else {
			y = delete_min(&r->right);
			y->right = r->right;
			y->left = r->left;
			*root = y;
		}
	}
	UPDATE_HEIGHT(*root);

#ifndef LAZY_DELETION
	d_balance(root);
#endif

	return ret;
}

struct tree *delete_min(struct tree **p)
{
	struct tree *x = *p;

	if(x->left) {
		x = delete_min(&x->left);
		UPDATE_HEIGHT(*p);

#ifndef LAZY_DELETION
	d_balance(p);
#endif

	} else {
		*p = x->right;
	}
	return x;
}

void tree_free(avl_tree *t)
{
	if(!t) return;
	free_node(t, t->root);
	free(t);
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
	node->data = malloc(t->data_size);
	if(!node->data) {
		error("failed to allocate memory");
		return NULL;
	}

	copy(node->data, data, t->data_size);
	node->left = node->right = NULL;
	node->height = 0;
	return node;
}

struct tree *balance(struct tree **root)
{
	struct tree *r = *root;
	int diff;

	diff = DIFF(r);
	if(diff == 2) {
		if(DIFF(r->left) == 1) {
			right_rotate(root);
		} else {
			double_right_rotate(root);
		}
	} else if(diff == -2) {
		if(DIFF(r->right) == -1) {
			left_rotate(root);
		} else {
			double_left_rotate(root);
		}
	}
	return *root;
}

/*
 * Different from insertion balance, in which if a node is
 * of balance factor 2 or -2, it's left/right child must be
 * of balance factor 1/-1, in deletion balance,
 * unbalanced nodes' childs can have balance factor 0.
 * It's easy to merge these two functions into one,
 * but this difference should be realized.
 */
struct tree *d_balance(struct tree **root)
{
	struct tree *r = *root;
	int diff;

	diff = DIFF(r);
	if(diff == 2) {
		if(DIFF(r->left) >= 0) {
			right_rotate(root);
		} else {
			double_right_rotate(root);
		}
	} else if(diff == -2) {
		if(DIFF(r->right) <= 0) {
			left_rotate(root);
		} else {
			double_left_rotate(root);
		}
	}
	return *root;
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
	UPDATE_HEIGHT(x);
	UPDATE_HEIGHT(y);
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
	UPDATE_HEIGHT(x);
	UPDATE_HEIGHT(y);
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
	UPDATE_HEIGHT(x);
	UPDATE_HEIGHT(y);
	UPDATE_HEIGHT(z);
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
	UPDATE_HEIGHT(x);
	UPDATE_HEIGHT(y);
	UPDATE_HEIGHT(z);
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
