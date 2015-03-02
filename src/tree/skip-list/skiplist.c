#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "skiplist.h"

#define error(S) fprintf(stderr, "%s:%d:%s: %s\n", \
		__FILE__, __LINE__, __func__, S)

static skip_node *new_node(const skip_list *s,
		int level, const void *data);
static void free_node(skip_node *node);
static int rand_level(void);
static int compare(const skip_list *s,
		const skip_node *node, const void *data);
static void copy(void *des, const void *src, size_t size);

skip_list *skip_init(size_t data_size, cmp_func f)
{
	if(!f) {
		error("compare function missed");
		return NULL;
	}
	skip_list *s;
	skip_node *nil, *header;
	int i;

	s = (skip_list *)malloc(sizeof(skip_list));
	if(!s) {
		error("failed to allocate memory");
		return NULL;
	}

	nil = new_node(s, 0, NULL);
	header = new_node(s, SKIP_MAX_LEVEL, NULL);
	if(!nil || !header) {
		free(s);
		free(nil);
		free(header);
		error("failed to make new node");
		return NULL;
	}

	for(i = 0; i <= SKIP_MAX_LEVEL; i++)
		header->forwards[i] = nil;

	s->nil = nil;
	s->header = header;
	s->data_size = data_size;
	s->size = 0;
	s->level = 0;
	s->compare = f;
	return s;
}

const void *skip_search(const skip_list *s, const void *data)
{
	if(!s || !data) return NULL;

	int i;
	skip_node *x = s->header;

	for(i = s->level; i >= 0; i--) {
		while(compare(s, x->forwards[i], data) < 0)
			x = x->forwards[i];
	}
	/* now x->data < data <= x->forwards[0]->data */
	x = x->forwards[0];
	if(compare(s, x, data) == 0)
		return x->data;
	return NULL;
}

skip_node *skip_insert(skip_list *s, const void *data)
{
	if(!s || !data) return NULL;

	int i, level;
	skip_node *update[SKIP_MAX_LEVEL + 1];
	skip_node *x = s->header;


	for(i = s->level; i >= 0; i--) {
		while(compare(s, x->forwards[i], data) < 0)
			x = x->forwards[i];
		update[i] = x;
	}
	x = x->forwards[0];

	if(compare(s, x, data) == 0) {
		copy(x->data, data, s->data_size);
	} else {
		level = rand_level();
		if(level > s->level) {
			for(i = s->level + 1; i <= level; i++) {
				update[i] = s->header;
			}
			s->level = level;
		}

		x = new_node(s, level, data);
		if(!x) {
			error("failed to make new node");
			return NULL;
		}
		/* insert new node into skip list */
		/* update forward pointers before new node */
		for(i = 0; i <= level; i++) {
			x->forwards[i] = update[i]->forwards[i];
			update[i]->forwards[i] = x;
		}
	}
	return x;
}

bool skip_delete(skip_list *s, void *data)
{
	if(!s || !data) return false;

	int i;
	skip_node *update[SKIP_MAX_LEVEL + 1];
	skip_node *x = s->header;

	for(i = s->level; i >= 0; i--) {
		while(compare(s, x->forwards[i], data) < 0)
			x = x->forwards[i];
		update[i] = x;
	}
	x = x->forwards[0];

	if(compare(s, x, data) != 0)
		return false;
	/* x->data == data */
	copy(data, x->data, s->data_size);

	for(i = 0; i < s->level; i++) {
		if(update[i]->forwards[i] != x)
			break;
		update[i]->forwards[i] = x->forwards[i];
	}
	free_node(x);

	while(s->level > 0 && s->header->forwards[s->level] == s->nil) {
		s->header->forwards[s->level] = NULL;
		s->level--;
	}
	return true;
}

void skip_free(skip_list *s)
{
	if(!s) return;

	skip_node *x = s->header;
	skip_node *prev;
	while(x != s->nil) {
		prev = x;
		x = x->forwards[0];
		free_node(prev);
	}
	free_node(s->nil);
	free(s);
}

int rand_level(void)
{
	int l = 0;

	while(((double)rand() / RAND_MAX) < SKIP_POSSIBILITY &&
			l < SKIP_MAX_LEVEL) {
		l++;
	}
	return l;
}

skip_node *new_node(const skip_list *s, int level, const void *data)
{
	skip_node *node, **forwards;
	void *node_data;

	/* from 0 to level, need (level + 1) space */
	level++;
	node = (skip_node *)malloc(sizeof(skip_node));
	forwards = (skip_node **)malloc(level * sizeof(skip_node *));
	node_data = malloc(s->data_size);
	
	if(!node || !forwards || !node_data) {
		free(node);
		free(forwards);
		free(node_data);
		error("failed to allocate memory");
		return NULL;
	}

	copy(node_data, data, s->data_size);
	memset(forwards, 0, level * sizeof(skip_node *));

	node->forwards = forwards;
	node->data = node_data;
	return node;
}

void free_node(skip_node *node)
{
	if(!node) return;
	free(node->data);
	free(node->forwards);
	free(node);
}

int compare(const skip_list *s,
		const skip_node *node, const void *data)
{
	if(node == s->nil) return 1;
	else return s->compare(node->data, data);
}

void copy(void *des, const void *src, size_t size)
{
	memcpy(des, src, size);
}
