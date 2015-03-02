/*
 *
 *    |  | ---------------------------------> nil
 *    |  | ---------> |  | -----------------> nil
 *    |  | ---------> |  | ---------> |  | -> nil
 *    |  | -> |  | -> |  | ---------> |  | -> nil
 *    |  | -> |  | -> |  | -> |  | -> |  | -> nil
 *      ^               ^                       ^
 *      header          node                    with max key
 */

#ifndef _SKIPLIST_H
#define  _SKIPLIST_H

#include <stdlib.h>
#include <stdbool.h>

#define SKIP_MAX_LEVEL (20)
#define SKIP_POSSIBILITY (0.5)

typedef int (*cmp_func)(const void *, const void *);

typedef struct skip_node {
	struct skip_node **forwards;
	void *data;
} skip_node;

typedef struct skip_list {
	struct skip_node *header;
	struct skip_node *nil;
	int level;
	int max_level;
	size_t size;
	size_t data_size;
	cmp_func compare;
} skip_list;

extern skip_list *skip_init(size_t data_size, cmp_func f);
extern const void *skip_search(const skip_list *s, const void *data);
extern skip_node *skip_insert(skip_list *s, const void *data);
extern bool skip_delete(skip_list *s, void *data);
extern void skip_free(skip_list *s);

#endif
