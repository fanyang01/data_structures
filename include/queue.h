#ifndef _QUEUE_H
#define _QUEUE_H
#include <stdlib.h>
#include <stdbool.h>
#include "list.h"

typedef struct queue {
	struct list_head list;
	size_t unit_size;
} queue;

struct queue_element {
	struct list_node list;
	void *data;
};

queue *queue_init(size_t unit_size);
bool dequeue(queue *s, void *des);
bool enqueue(queue *s, const void *data);
bool queue_head(const queue *s, void *des);
void queue_free(queue *s);

#endif
