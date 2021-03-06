#include "stack.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define stack_error(E) fprintf(stderr, "%s:%d:%s: %s\n", \
		__FILE__, __LINE__, __func__, E)
static void copy(void *des, const void *src, size_t size);

stack *stack_init(size_t data_size)
{
	stack *new;

	if((new = (stack *)malloc(sizeof(stack))) == NULL) {
		stack_error("stack_init: failed to allocate memory");
		return NULL;
	}
	INIT_LIST_HEAD(&new->list);
	new->data_size = data_size;
	return new;
}

bool pop(stack *s, void *des)
{
	if(!s || !des) return false;
	if(list_is_empty(&s->list)) return false;
	struct stack_element *tmp;

	tmp = (struct stack_element *)list_head(&s->list);
	if(!tmp) return false;
	copy(des, tmp->data, s->data_size);
	list_del(list_head(&s->list));
	free(tmp);
	return true;
}

bool push(stack *s, const void *data)
{
	if(!s) return false;
	struct stack_element *new;
	void *element;

	element = malloc(sizeof(struct stack_element) + s->data_size);
	if(!element) {
		stack_error("push: failed to allocate memory");
		return false;
	}
	new = (struct stack_element *)element;
	new->data = element + sizeof(struct stack_element);

	copy(new->data, data, s->data_size);
	list_add_head(&s->list, &new->list);
	return true;
}

bool top(const stack *s, void *des)
{
	struct stack_element *tmp;

	if(!s) return false;

	tmp = (struct stack_element *)
		list_head((struct list_head *)(&s->list));
	if(!tmp) return false;
	copy(des, tmp->data, s->data_size);
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

static void copy(void *des, const void *src, size_t size)
{
	memcpy(des, src, size);
}
