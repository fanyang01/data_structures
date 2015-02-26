#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define error(S) fprintf(stderr, "%s:%d:%s: %s\n", \
		__FILE__, __LINE__, __func__, S)

static struct tree *search(treap *t, const void *data);
static void transplant(treap *t, struct tree *to, struct tree *from);
static void insert_fixup(treap *t, struct tree *node);
static struct tree *minimum(treap *t, struct tree *node);
static struct tree *new_node(treap *t, const void *data);
static struct tree *left_rotate(treap *t, struct tree *x);
static struct tree *right_rotate(treap *t, struct tree *x);
static struct tree *double_right_rotate(treap *t, struct tree *x);
static struct tree *double_left_rotate(treap *t, struct tree *x);
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
	struct tree *x = t->root;
	struct tree *p = NULL;
	struct tree *node = new_node(t, data);
	if(!node) {
		error("failed to make new node");
		return NULL;
	}
	while(x) {
		p = x;
		if(t->compare(node->data, x->data) < 0)
			x = x->left;
		else
			x = x->right;
	}
	node->p = p;
	if(p == NULL) {
		t->root = node;
	} else if(t->compare(node->data, p->data) < 0) {
		p->left = node;
	} else {
		p->right = node;
	}
	insert_fixup(t, node);
	t->size++;
	return t;
}

void *tree_search(treap *t, const void *data)
{
	struct tree *node = search(t, data);
	if(!node) return NULL;
	return node->data;
}

treap *tree_delete(treap *t, const void *data)
{
	if(!t) return NULL;
	if(!data) return t;
	struct tree *x, *y;
	struct tree *node = search(t, data);
	if(!node) {
		/* error("not found"); */
		return NULL;
	}

	y = node;
	if(node->left == NULL) {
		transplant(t, node, node->right);
	} else if(node->right == NULL) {
		transplant(t, node, node->left);
	} else {
		y = minimum(t, node->right);
		/*
		 * when y is the right child of node,
		 * no need to move y->right
		 */
		if(y->p != node) {
			transplant(t, y, y->right);
			y->right = node->right;
			node->right->p = y;
		}
		y->left = node->left;
		node->left->p = y;
		transplant(t, node, y);
	}
	free(node->data);
	free(node);
	t->size--;
	return t;
}

void tree_free(treap *t)
{
	if(!t) return;
	free_node(t, t->root);
	free(t);
}

void insert_fixup(treap *t, struct tree *node)
{
	while(node->p) {
		if(node->p->priority <= node->priority)
			break;
		if(node->p->left == node)
			right_rotate(t, node->p);
		else
			left_rotate(t, node->p);
	}
}

struct tree *search(treap *t, const void *data)
{
	struct tree *x = t->root;
	int res;

	while(x != NULL) {
		if((res = t->compare(data, x->data)) == 0) {
			return x;
		} else if(res < 0) {
			x = x->left;
		} else {
			x = x->right;
		}
	}
	return NULL;
}

void transplant(treap *t, struct tree *to, struct tree *from)
{
	if(to->p == NULL) {
		t->root = from;
	} else if(to == to->p->left) {
		to->p->left = from;
	} else {
		to->p->right = from;
	}
	if(from) from->p = to->p;
}

struct tree *minimum(treap *t, struct tree *node)
{
	while(node->left != NULL) {
		node = node->left;
	}
	return node;
}

struct tree *new_node(treap *t, const void *data)
{
	if(!t) return NULL;
	struct tree *node = (struct tree *)
		malloc(sizeof(struct tree));
	if(!node) {
		error("failed to allocate memory");
		return NULL;
	}
	if(data) {
		node->data = malloc(t->unit_size);
		if(!node->data) {
			error("failed to allocate memory");
			return NULL;
		}
		copy(node->data, data, t->unit_size);
	} else {
		node->data = NULL;
	}
	node->p = node->left = node->right = NULL;
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
struct tree *left_rotate(treap *t, struct tree *x)
{
	struct tree *y = x->right;
	transplant(t, x, y);
	x->right = y->left;
	if(y->left) y->left->p = x;
	y->left = x;
	x->p = y;
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
struct tree *right_rotate(treap *t, struct tree *x)
{
	struct tree *y = x->left;
	transplant(t, x, y);
	x->left = y->right;
	if(y->right) y->right->p = x;
	y->right = x;
	x->p = y;
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
struct tree *double_right_rotate(treap *t, struct tree *x)
{
	struct tree *y = x->left;
	struct tree *z = y->right;
	transplant(t, x, z);
	y->right = z->left;
	z->left->p = y;
	x->left = z->right;
	z->right->p = x;
	z->left = y;
	y->p = z;
	z->right = x;
	x->p = z;
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
struct tree *double_left_rotate(treap *t, struct tree *x)
{
	struct tree *y = x->right;
	struct tree *z = y->left;
	transplant(t, x, z);
	y->left = z->right;
	z->right->p = y;
	x->right = z->left;
	z->left->p = x;
	z->right = y;
	y->p = z;
	z->left = x;
	x->p = z;
	return z;
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
