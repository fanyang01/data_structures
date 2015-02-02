#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "list.h"

static struct list_node *temp_node_for_del;
static int func_del_safe(struct list_node *node);
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
	if (node) {
		*node->prev = node->next;
		node->next->prev = node->prev;
		return;
	}
	list_error("delete a non-exit node");
}

bool list_del_safe(struct list_head * list, struct list_node * node)
{
	temp_node_for_del = node;
	if (!list_find(list, func_del_safe))
		return false;
	list_del(node);
	return true;
}

static int func_del_safe(struct list_node *node)
{
	return node == temp_node_for_del;
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

struct list_node *list_tail(struct list_head *list)
{
	return (struct list_node *)list->tail;
}

struct list_node *list_head(struct list_head *list)
{
	return list->first;
}

void list_error(const char *s)
{
	fprintf(stderr, "list.c: %s\n", s);
}
