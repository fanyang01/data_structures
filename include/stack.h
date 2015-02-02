#ifndef _STACK_H
#define _STACK_H
#include <stdlib.h>
#include <stdbool.h>
#include "list.h"

typedef struct stack {
	struct list_head list;
} stack;

struct stack_element {
	struct list_node list;
	void *data;
};

stack *stack_init(void);
bool pop(stack *s, void *des, size_t size);
bool push(stack *s, const void *data, size_t size);
bool top(const stack *s, void *des, size_t size);
void stack_free(stack *s);

#endif
