#ifndef _QUEUE_H
#define _QUEUE_H
#include <stdlib.h>
#include <stdbool.h>
#include "list.h"

typedef struct queue {
	struct list_head list;
} queue;

struct queue_element {
	struct list_node list;
	void *data;
};

queue *queue_init(void);
bool dequeue(queue *s, void *des, size_t size);
bool enqueue(queue *s, const void *data, size_t size);
bool queue_first(const queue *s, void *des, size_t size);
void queue_free(queue *s);

#endif
