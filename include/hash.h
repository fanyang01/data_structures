#ifndef _HASH_H
#define _HASH_H

#include <stdlib.h>
#include <stdbool.h>

#define HASH_INIT_SIZE (1<<12)

typedef long (*hash_func)(const void *);
typedef int (*cmp_func)(const void *, const void *);

typedef struct hash_list {
	struct hash_list *next;
	void *data;
} hash_list;

typedef struct hash_slot {
	hash_list *next;
} hash_slot;

typedef struct {
	hash_func hash;
	cmp_func compare;
	size_t data_size;
	size_t size;
	size_t nmembs;
	hash_slot *table;
} hash;

struct hash_init {
	size_t size;
	size_t data_size;
	cmp_func cmp_func;
	hash_func hash_func;
};

/* allocate and initialize a hash structure */
/* return a pointer to new-allocated hash structure */
hash *hash_init(struct hash_init *h_init);
/* insert data into hash table */
/* return true if insert successfully; */
/* return false if failed */
bool hash_insert(hash *h, const void *data);
/* delete data from hash table */
/* return true if delete successfully */
/* return false if errors happen */
bool hash_delete(hash *h, const void *data);
/* search data in hash table */
/* if found, return a pointer to data; */
/* if not found, return NULL */
void *hash_search(hash *h, const void *data);
/* free all space used by hash */
void hash_free(hash *h);

#endif
