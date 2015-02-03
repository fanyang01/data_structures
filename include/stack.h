#ifndef _STACK_H
#define _STACK_H
#include <stdlib.h>
#include <stdbool.h>
#include "list.h"

typedef struct stack {
	struct list_head list;
	size_t unit_size;
} stack;

struct stack_element {
	struct list_node list;
	void *data;
};

stack *stack_init(size_t unit_size);
bool pop(stack *s, void *des);
bool push(stack *s, const void *data);
bool top(const stack *s, void *des);
void stack_free(stack *s);

#endif
