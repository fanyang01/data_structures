#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "list.h"

static void list_error(const char *s);

int list_is_empty(struct list_head *list)
{
	return list->first == (struct list_node *)list;
}

void list_add(struct list_node *pos, struct list_node *new)
{
	new->next = pos->next;
	new->prev = &pos->next;
	pos->next->prev = &new->next;
	pos->next = new;
}

void list_add_prev(struct list_node *pos, struct list_node *new)
{
	list_add((struct list_node *)(pos->prev), new);
}

void list_add_head(struct list_head *list, struct list_node *new)
{
	list_add((struct list_node *)list, new);
}

void list_add_tail(struct list_head *list, struct list_node *new)
{
	list_add((struct list_node *)(list->tail), new);
}

void list_del(struct list_node *node)
{
	if(node) {
		*node->prev = node->next;
		node->next->prev = node->prev;
		node->prev = NULL;
		node->next = NULL;
		return;
	}
	list_error("delete a non-exit node");
}

struct list_node *list_find(struct list_head *list,
		int (*found) (struct list_node *))
{
	if (list_is_empty(list))
		return NULL;

	struct list_node *pos;
	list_for_each(pos, list) {
		if (found(pos))
			return pos;
	}
	return NULL;
}

void list_merge(struct list_head *x, struct list_head *y)
{
	if(list_is_empty(y)) return;
	y->first->prev = x->tail;
	((struct list_node *)(x->tail))->next = y->first;
	x->tail = y->tail;
	((struct list_node *)(y->tail))->next = (struct list_node *)x;
}

void list_merge_continue(struct list_head *x,
		struct list_head *y, struct list_node *pos)
{
	if(!pos) return;
	pos->prev = x->tail;
	((struct list_node *)(x->tail))->next = pos;
	x->tail = y->tail;
	((struct list_node *)(y->tail))->next = (struct list_node *)x;
}

struct list_node *list_tail(struct list_head *list)
{
	return (struct list_node *)list->tail;
}

struct list_node *list_head(struct list_head *list)
{
	return list->first;
}

struct list_node *list_next(struct list_node *node)
{
	return node->next;
}

struct list_node *list_prev(struct list_node *node)
{
	return (struct list_node *)node->prev;
}

void list_error(const char *s)
{
	fprintf(stderr, "list.c: %s\n", s);
}
