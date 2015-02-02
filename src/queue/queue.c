#include "queue.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static void queue_error(const char *q);
static void copy(void *des, const void *src, size_t size);

queue *queue_init(void)
{
	queue *new;

	if((new = (queue *)malloc(sizeof(queue))) == NULL) {
		queue_error("failed to allocate memory");
		return NULL;
	}
	INIT_LIST_HEAD(&new->list);
	return new;
}

bool dequeue(queue *q, void *des, size_t size)
{
	if(!q || !des) return false;
	if(list_is_empty(&q->list)) return false;
	struct queue_element *tmp;

	tmp = (struct queue_element *)list_head(&q->list);
	if(!tmp) return false;
	copy(des, tmp->data, size);
	list_del(list_head(&q->list));
	free(tmp);
	return true;
}

bool enqueue(queue *q, const void *data, size_t size)
{
	if(!q) return false;
	struct queue_element *new;
	void *element;

	element = malloc(sizeof(struct queue_element) + size);
	if(!element) {
		queue_error("failed to allocate memory for new element");
		return false;
	}
	new = (struct queue_element *)element;
	new->data = element + sizeof(struct queue_element);

	copy(new->data, data, size);
	list_add_tail(&q->list, &new->list);
	return true;
}

bool queue_first(const queue *q, void *des, size_t size)
{
	struct queue_element *tmp;

	if(!q) return false;

	tmp = (struct queue_element *)
		list_head((struct list_head *)(&q->list));
	if(!tmp) return false;
	copy(des, tmp->data, size);
	return true;
}

void queue_free(queue *q)
{
	struct list_node *pos, *next;

	list_for_each_safe(pos, next, &q->list) {
		list_del(pos);
		free(pos);
	}
	free(q);
}

static void queue_error(const char *q)
{
	fprintf(stderr, "%q\n", q);
}

static void copy(void *des, const void *src, size_t size)
{
	memcpy(des, src, size);
}
