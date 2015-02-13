/* list.h defines a low-level double-linked list ADT.
 *
 * operations on list:
 * add: add a node to list on given position;
 * delete: delete a node from list;
 * find: find a node according to given conditions;
 * traverse: traverse the list and do something to each node.
 *
 * A set of (macro-)functions are defined for operations above,
 * and it's suggested to following the guide to use them.
 */

#ifndef _LIST_H
#define _LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* structure list_node is the node in list.
 * it contains two field:
 * next, pointed to the next node in list;
 * prev, pointed to the 'next' field of previous node in list.
 */
struct list_node {
	struct list_node *next;
	struct list_node **prev;
};
/* structure list_head is the head of list.
 * it contains two field:
 * first, pointed to the first node in list;
 * tail, pointed to the 'next' field of the node at the tail of list.
 * it's similiar with structure list_node.
 */
struct list_head {
	struct list_node *first;
	struct list_node **tail;
};

/* macro LIST_HEAD declare and initialize a list_head structure,
 * with making its 'first' field pointed to itself,
 * and its 'tail' field pointed to its 'first' field.
 */
#define LIST_HEAD_INIT(name) { (struct list_node *)(&(name)), \
	(struct list_node **)(&(name)) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

/* macro INIT_LIST_HEAD initialize a list head to empty */
#define INIT_LIST_HEAD(list_head_ptr) ({ \
	(list_head_ptr)->first = (struct list_node *)(list_head_ptr); \
	(list_head_ptr)->tail = (struct list_node **)(list_head_ptr); \
	})

/* test if the list is empty */
int list_is_empty(struct list_head *list);
/* next node */
struct list_node *list_next(struct list_node *node);
/* previous node */
struct list_node *list_prev(struct list_node *node);
/* return a pointer to the node at the tail of list */
struct list_node *list_tail(struct list_head *list);
/* return a pointer to the node at the head of list */
struct list_node *list_head(struct list_head *list);
/* insert the new node to the position after pos */
void list_add(struct list_node *pos, struct list_node *new);
/* insert the new node to the position before pos */
void list_add_prev(struct list_node *pos, struct list_node *new);
/* insert the new node as the head of list */
void list_add_head(struct list_head *list, struct list_node *new);
/* insert the new node as the tail of list */
void list_add_tail(struct list_head *list, struct list_node *new);
/* delete the node from list */
void list_del(struct list_node *node);
/* find a node for which function found return a non-zero value */
struct list_node *list_find(struct list_head *list, int (*found)(struct list_node *));
/* append list y to list x */
void list_merge(struct list_head *x, struct list_head *y);
void list_merge_at(struct list_head *x,
		struct list_head *y, struct list_node *pos);

/*
macro list_for_each traverse a list from the head,
assign pos(of type struct list_node *) to the current node
*/
#define list_for_each(pos, list) \
	for(pos = (list)->first; pos != (struct list_node *)(list); \
			pos = pos->next)

/*
macro list_for_each_safe is a safe version of list_for_each,
avoiding the deletion of *pos
*/
#define list_for_each_safe(pos, next, list) \
	for(pos = (list)->first; next = pos->next, \
			pos != (struct list_node *)(list); \
			pos = next)

/*
macro list_for_each_continue traverse a list starting at cur_pos
*/
#define list_for_each_continue(pos, cur_pos, list) \
	for(pos = cur_pos; pos != (struct list_node *)(list); \
			pos = pos->next)

#endif
