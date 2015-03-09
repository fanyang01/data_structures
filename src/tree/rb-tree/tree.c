#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define error(S) fprintf(stderr, "%s:%d:%s: %s\n", \
		__FILE__, __LINE__, __func__, S)
#define BLACK 0
#define RED   1

static struct tree *search(rb_tree *t, const void *data);
static void rb_insert_fixup(rb_tree *t, struct tree *x);
static void rb_delete_fixup(rb_tree *t, struct tree *x);
static void transplant(rb_tree *t, struct tree *to, struct tree *from);
static struct tree *minimum(rb_tree *t, struct tree *node);
static struct tree *new_node(rb_tree *t, const void *data);
static struct tree *left_rotate(rb_tree *t, struct tree *x);
static struct tree *right_rotate(rb_tree *t, struct tree *x);
static struct tree *double_right_rotate(rb_tree *t, struct tree *x);
static struct tree *double_left_rotate(rb_tree *t, struct tree *x);
static void free_node(rb_tree *t, struct tree *node);
static void copy(void *des, const void *src, size_t size);

rb_tree *tree_init(size_t data_size, cmp_func f)
{
	if(!f) {
		error("compare function missed");
		return NULL;
	}
	rb_tree *t = (rb_tree *)malloc(sizeof(rb_tree));
	if(!t) {
		error("failed to allocate memory");
		return NULL;
	}
	struct tree *nil = new_node(t, NULL);
	if(!nil) {
		error("failed to allocate memory");
		free(t);
		return NULL;
	};
	nil->p = nil;
	nil->left = nil;
	nil->right = nil;
	nil->color = BLACK;
	t->data_size = data_size;
	t->size = 0;
	t->compare = f;
	t->nil = nil;
	t->root = nil;
	return t;
}

rb_tree *tree_insert(rb_tree *t, const void *data)
{
	if(!t || !data) return NULL;
	struct tree *x = t->root;
	struct tree *p = t->nil;
	struct tree *node = new_node(t, data);
	if(!node) {
		error("failed to make new node");
		return NULL;
	}
	while(x != t->nil) {
		p = x;
		if(t->compare(node->data, x->data) < 0)
			x = x->left;
		else
			x = x->right;
	}
	node->p = p;
	if(p == t->nil) {
		t->root = node;
	} else if(t->compare(node->data, p->data) < 0) {
		p->left = node;
	} else {
		p->right = node;
	}
	node->left = t->nil;
	node->right = t->nil;
	node->color = RED;
	rb_insert_fixup(t, node);
	t->size++;
	return t;
}

void *tree_search(rb_tree *t, const void *data)
{
	struct tree *node = search(t, data);
	if(!node) return NULL;
	return node->data;
}

rb_tree *tree_delete(rb_tree *t, void *data)
{
	if(!t) return NULL;
	if(!data) return t;
	char prev_color;
	struct tree *x, *y;
	struct tree *node = search(t, data);
	if(!node) {
		/* error("not found"); */
		return NULL;
	}
	copy(data, node->data, t->data_size);

	y = node;
	prev_color = node->color;
	if(node->left == t->nil) {
		x = node->right;
		transplant(t, node, node->right);
	} else if(node->right == t->nil) {
		x = node->left;
		transplant(t, node, node->left);
	} else {
		y = minimum(t, node->right);
		prev_color = y->color;
		x = y->right;
		/*
		 * when y is the right child of node,
		 * no need to move y->right
		 */
		if(y->p == node) {
		/* !!! very important !!!
		 * for x can be t->nil,
		 * whose parent is uncertain
		 */
			x->p = y;
		} else {
			transplant(t, y, y->right);
			y->right = node->right;
			node->right->p = y;
		}
		y->left = node->left;
		node->left->p = y;
		transplant(t, node, y);
		y->color = node->color;
	}
	free(node->data);
	free(node);
	if(prev_color == BLACK)
		rb_delete_fixup(t, x);
	t->size--;
	return t;
}

void tree_free(rb_tree *t)
{
	if(!t) return;
	free_node(t, t->root);
	free(t->nil);
	free(t);
}

struct tree *search(rb_tree *t, const void *data)
{
	struct tree *x = t->root;
	int res;

	while(x != t->nil) {
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

/*
 * 1.                B
 *                  / \
 *                 R   R  <- y
 *                / \
 *         x ->  R   R  <- (or x)
 * 2.
 *                   B
 *                 /   \
 *                R     B  <- y
 *                 \
 *            x ->  R
 * 3.
 *                   B
 *                 /   \
 *                R     B  <- y
 *               /
 *        x ->  R
 */
void rb_insert_fixup(rb_tree *t, struct tree *x)
{
	struct tree *y;

	while(x->p->color == RED) {
		if(x->p == x->p->p->left) {
			y = x->p->p->right;
			if(y->color == RED) { /* case 1 */
				x->p->color = BLACK;
				y->color = BLACK;
				x->p->p->color = RED;
				x = x->p->p;
			} else {
				if(x == x->p->right) { /* case 2 */
					x = x->p;
					left_rotate(t, x); /* convert to case 3 */
				}
				x->p->color = BLACK; /* case 3 */
				x->p->p->color = RED;
				right_rotate(t, x->p->p);
			}
		} else {
			/* symmetrical, exchange "left" and "right" */
			y = x->p->p->left;
			if(y->color == RED) { /* case 1 */
				x->p->color = BLACK;
				y->color = BLACK;
				x->p->p->color = RED;
				x = x->p->p;
			} else {
				if(x == x->p->left) { /* case 2 */
					x = x->p;
					left_rotate(t, x); /* convert to case 3 */
				}
				x->p->color = BLACK; /* case 3 */
				x->p->p->color = RED;
				left_rotate(t, x->p->p);
			}
		}
	}
	t->root->color = BLACK;
}

/*
 * when this function is called, x seems to have an additional
 * black color, so this function aims to shift the extra color.
 * 1.
 *                    B
 *                 /     \
 *          x ->  B       R  <- y
 *                       / \
 *                      B   B
 * 2.
 *                   R|B
 *                 /     \
 *          x ->  B       B  <- y
 *                      /   \
 *                     B     B
 * 3.
 *                   R|B
 *                 /     \
 *          x ->  B       B  <- y
 *                      /   \
 *                     R     B
 * 4.
 *                   R|B
 *                 /     \
 *          x ->  B       B  <- y
 *                          \
 *                           R
 */
void rb_delete_fixup(rb_tree *t, struct tree *x)
{
	struct tree *y;

	while(x != t->root && x->color == BLACK) {
		if(x == x->p->left) {
			y = x->p->right;
			/* case 1 */
			if(y->color == RED) {
				y->color = BLACK;
				x->p->color = RED;
				left_rotate(t, x->p);
				y = x->p->right;
			}
			/* case 2 */
			if(y->right->color == BLACK &&
					y->left->color == BLACK) {
				y->color = RED;
				x = x->p;
			} else {
			/* case 3 */
				if(y->right->color == BLACK) {
					y->left->color = BLACK;
					y->color = RED;
					right_rotate(t, y);
					y = x->p->right;
				}
			/* case 4 */
				y->color = x->p->color;
				x->p->color = BLACK;
				y->right->color = BLACK;
				left_rotate(t, x->p);
				x = t->root;
			}
		} else { /* symmetrical, exchange "left" and "right" */
			y = x->p->left;
			/* case 1 */
			if(y->color == RED) {
				y->color = BLACK;
				x->p->color = RED;
				right_rotate(t, x->p);
				y = x->p->left;
			}
			/* case 2 */
			if(y->left->color == BLACK &&
					y->right->color == BLACK) {
				y->color = RED;
				x = x->p;
			} else {
			/* case 3 */
				if(y->left->color == BLACK) {
					y->right->color = BLACK;
					y->color = RED;
					left_rotate(t, y);
					y = x->p->left;
				}
			/* case 4 */
				y->color = x->p->color;
				x->p->color = BLACK;
				y->left->color = BLACK;
				right_rotate(t, x->p);
				x = t->root;
			}
		}
	}
	x->color = BLACK;
}

void transplant(rb_tree *t, struct tree *to, struct tree *from)
{
	if(to->p == t->nil) {
		t->root = from;
	} else if(to == to->p->left) {
		to->p->left = from;
	} else {
		to->p->right = from;
	}
	from->p = to->p;
}

struct tree *minimum(rb_tree *t, struct tree *node)
{
	while(node->left != t->nil) {
		node = node->left;
	}
	return node;
}

struct tree *new_node(rb_tree *t, const void *data)
{
	if(!t) return NULL;
	struct tree *node = (struct tree *)
		malloc(sizeof(struct tree));
	if(!node) {
		error("failed to allocate memory");
		return NULL;
	}
	if(data) {
		node->data = malloc(t->data_size);
		if(!node->data) {
			error("failed to allocate memory");
			return NULL;
		}
		copy(node->data, data, t->data_size);
	} else {
		node->data = NULL;
	}
	node->p = node->left = node->right = NULL;
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
struct tree *left_rotate(rb_tree *t, struct tree *x)
{
	struct tree *y = x->right;
	x->right = y->left;
	if(y->left != t->nil) y->left->p = x;
	transplant(t, x, y);
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
struct tree *right_rotate(rb_tree *t, struct tree *x)
{
	struct tree *y = x->left;
	x->left = y->right;
	if(y->right != t->nil) y->right->p = x;
	transplant(t, x, y);
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
struct tree *double_right_rotate(rb_tree *t, struct tree *x)
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
struct tree *double_left_rotate(rb_tree *t, struct tree *x)
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

void free_node(rb_tree *t, struct tree *node)
{
	if(node == t->nil) return;
	free_node(t, node->left);
	free_node(t, node->right);
	free(node->data);
	free(node);
}

void copy(void *des, const void *src, size_t size)
{
	memcpy(des, src, size);
}
