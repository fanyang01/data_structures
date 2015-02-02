#include "stack.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static void stack_error(const char *s);
static void copy(void *des, const void *src, size_t size);

stack *stack_init(void)
{
	stack *new;

	if((new = (stack *)malloc(sizeof(stack))) == NULL) {
		stack_error("failed to allocate memory");
		return NULL;
	}
	INIT_LIST_HEAD(&new->list);
	return new;
}

bool pop(stack *s, void *des, size_t size)
{
	if(!s || !des) return false;
	if(list_is_empty(&s->list)) return false;
	struct stack_element *tmp;

	tmp = (struct stack_element *)list_head(&s->list);
	if(!tmp) return false;
	copy(des, tmp->data, size);
	list_del(list_head(&s->list));
	free(tmp);
	return true;
}

bool push(stack *s, const void *data, size_t size)
{
	if(!s) return false;
	struct stack_element *new;
	void *element;

	element = malloc(sizeof(struct stack_element) + size);
	if(!element) {
		stack_error("failed to allocate memory for new element");
		return false;
	}
	new = (struct stack_element *)element;
	new->data = element + sizeof(struct stack_element);

	copy(new->data, data, size);
	list_add_head(&s->list, &new->list);
	return true;
}

bool top(const stack *s, void *des, size_t size)
{
	struct stack_element *tmp;

	if(!s) return false;

	tmp = (struct stack_element *)
		list_head((struct list_head *)(&s->list));
	if(!tmp) return false;
	copy(des, tmp->data, size);
	return true;
}

void stack_free(stack *s)
{
	struct list_node *pos, *next;

	list_for_each_safe(pos, next, &s->list) {
		list_del(pos);
		free(pos);
	}
	free(s);
}

static void stack_error(const char *s)
{
	fprintf(stderr, "%s\n", s);
}

static void copy(void *des, const void *src, size_t size)
{
	memcpy(des, src, size);
}
