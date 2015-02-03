#include "queue.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static void queue_error(const char *s);
static void copy(void *des, const void *src, size_t size);

queue *queue_init(size_t unit_size)
{
	queue *new;

	if((new = (queue *)malloc(sizeof(queue))) == NULL) {
		queue_error("queue_init: failed to allocate memory");
		return NULL;
	}
	INIT_LIST_HEAD(&new->list);
	new->unit_size = unit_size;
	return new;
}

bool pop(queue *s, void *des)
{
	if(!s || !des) return false;
	if(list_is_empty(&s->list)) return false;
	struct queue_element *tmp;

	tmp = (struct queue_element *)list_head(&s->list);
	if(!tmp) return false;
	copy(des, tmp->data, s->unit_size);
	list_del(list_head(&s->list));
	free(tmp);
	return true;
}

bool push(queue *s, const void *data)
{
	if(!s) return false;
	struct queue_element *new;
	void *element;

	element = malloc(sizeof(struct queue_element) + s->unit_size);
	if(!element) {
		queue_error("push: failed to allocate memory");
		return false;
	}
	new = (struct queue_element *)element;
	new->data = element + sizeof(struct queue_element);

	copy(new->data, data, s->unit_size);
	list_add_tail(&s->list, &new->list);
	return true;
}

bool top(const queue *s, void *des)
{
	struct queue_element *tmp;

	if(!s) return false;

	tmp = (struct queue_element *)
		list_head((struct list_head *)(&s->list));
	if(!tmp) return false;
	copy(des, tmp->data, s->unit_size);
	return true;
}

void queue_free(queue *s)
{
	struct list_node *pos, *next;

	list_for_each_safe(pos, next, &s->list) {
		list_del(pos);
		free(pos);
	}
	free(s);
}

static void queue_error(const char *s)
{
	fprintf(stderr, "%s\n", s);
}

static void copy(void *des, const void *src, size_t size)
{
	memcpy(des, src, size);
}
