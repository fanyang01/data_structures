#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *    +--+
 *    |  | --> node --> node --> NULL
 *    +--+
 *    |  | --> NULL
 *    +--+
 *    |  | --> node --> NULL
 *    +--+       ^
 *    |  |      data
 *    +--+
 *    |  |
 *    +--+
 *    |  |
 *    +--+
 *    |  |
 *    +--+
 *    |  |
 *    +--+
 *      ^
 *      table
 */

#ifndef MUL_FACTOR
#define MUL_FACTOR 0.6180339887
#endif

#define hash_error(E) fprintf(stderr, "%s:%d:%s: %s\n", \
		__FILE__, __LINE__, __func__, E)

static void free_node(hash_list *node);
static hash_list *new_node(hash *h, const void *data);
static void copy(void *des, const void *src, size_t size);

/* use multiplication method to hash key into some index */
int hash_gen_index(hash *h, int key)
{
	double f = MUL_FACTOR * (double)key;
	f -= (double)((int)f);
	return (int)((double)h->size * f);
}

/* allocate and initialize a hash structure */
/* return a pointer to new-allocated hash structure */
hash *hash_init(struct hash_init *h_init)
{
	if(!h_init) {
		hash_error("incorrect hash_init structure");
		return NULL;
	}
	hash *h = (hash *)malloc(sizeof(hash));
	hash_slot *table = (hash_slot *)
		malloc(h_init->size * sizeof(hash_slot));
	if(!h || !table) {
		hash_error("failed to allocate memory");
		free(h);
		free(table);
		return NULL;
	}
	memset(table, 0, HASH_INIT_SIZE * sizeof(hash_slot));
	h->hash = h_init->hash_func;
	h->compare = h_init->cmp_func;
	h->data_size = h_init->data_size;
	h->size = h_init->size;
	h->nmembs = 0;
	h->table = table;
	return h;
}

/* insert data into hash table */
/* return true if insert successfully; */
/* return false if failed */
bool hash_insert(hash *h, const void *data)
{
	if(!h) return false;
	int integer = h->hash(data);
	if(integer < 0) {
		hash_error("error in genarating index");
		return false;
	}
	int index = hash_gen_index(h, integer);
	hash_list *node = new_node(h, data);
	if(!node) {
		hash_error("make new node failed");
		return false;
	}
	/* tranverse the list to find data; */
	/* and if found, delete it */
	hash_list *x = h->table[index].next;
	hash_list **prev = &h->table[index].next;
	while(x != NULL) {
		if(h->compare(x->data, data) == 0) {
			*prev = x->next;
			free_node(x);
			break;
		}
		prev = &x->next;
		x = x->next;
	}
	/* add data to the head of list */
	node->next = h->table[index].next;
	h->table[index].next = node;
	h->nmembs++;
	return true;
}

/* delete data from hash table */
/* return true if delete successfully */
/* return false if errors happen */
bool hash_delete(hash *h, const void *data)
{
	if(!h) return false;
	int integer = h->hash(data);
	if(integer < 0) {
		hash_error("error in genarating index");
		return false;
	}
	int index = hash_gen_index(h, integer);

	/* tranverse the list to find and delete data */
	hash_list *x = h->table[index].next;
	hash_list **prev = &h->table[index].next;
	while(x != NULL) {
		if(h->compare(x->data, data) == 0) {
			*prev = x->next;
			free_node(x);
			return true;
		}
		prev = &x->next;
		x = x->next;
	}
	return false;
}

/* search data in hash table */
/* if found, return a pointer to data; */
/* if not found, return NULL */
void *hash_search(hash *h, const void *data)
{
	if(!h) return NULL;
	int integer = h->hash(data);
	if(integer < 0) {
		hash_error("error in genarating index");
		return false;
	}
	int index = hash_gen_index(h, integer);

	/* tranverse the list to find data */
	hash_list *x = h->table[index].next;
	while(x != NULL) {
		if(h->compare(x->data, data) == 0)
			return x->data;
		x = x->next;
	}
	return NULL;
}

void hash_free(hash *h)
{
	if(!h) return;
	int i;

	for(i = 0; i < h->size; i++) {
		hash_list *x = h->table[i].next;
		while(x != NULL) {
			hash_list *tmp = x;
			x = x->next;
			free_node(tmp);
		}
	}
	free(h->table);
	free(h);
}

hash_list *new_node(hash *h, const void *data)
{
	hash_list *node = (hash_list *)
		malloc(sizeof(hash_list));
	void *node_data = malloc(h->data_size);
	if(!node || !node_data) {
		hash_error("failed to allocate memory");
		free(node);
		free(node_data);
		return NULL;
	}
	copy(node_data, data, h->data_size);
	node->data = node_data;
	node->next = NULL;

	return node;
}

void free_node(hash_list *node)
{
	free(node->data);
	free(node);
}

void copy(void *des, const void *src, size_t size)
{
	memcpy(des, src, size);
}
