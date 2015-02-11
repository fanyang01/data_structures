#include <stdio.h>
#include <stdlib.h>
#include "list.h"

#define MAXSIZE (1<<18)

typedef struct {
	struct list_node list;
	int n;
} Node;

int main(void)
{
	struct list_head list;
	INIT_LIST_HEAD(&list);
	int i, n;
	Node nodes[MAXSIZE];

	for(i = 0; i < MAXSIZE; i++) {
		nodes[i].n = i;
		list_add_head(&list, &nodes[i].list);
	}
	for(i = 0; i < MAXSIZE; i++) {
		if(((Node *)list_tail(&list))->n != i) {
			goto FAILED;
		}
		list_del(list_tail(&list));
	}

	for(i = 0; i < MAXSIZE; i++) {
		nodes[i].n = i;
		list_add_tail(&list, &nodes[i].list);
	}
	for(i = 0; i < MAXSIZE; i++) {
		if(((Node *)list_head(&list))->n != i) {
			goto FAILED;
		}
		list_del(list_head(&list));
	}

	for(i = 0; i < MAXSIZE; i++) {
		nodes[i].n = i;
		list_add_tail(&list, &nodes[i].list);
	}
	for(i = MAXSIZE - 1; i > 0; i--) {
		if(((Node *)list_tail(&list))->n != i) {
			goto FAILED;
		}
		list_del(list_tail(&list));
	}
	puts("----------passed----------");
	exit(EXIT_SUCCESS);
FAILED:
	puts("!!!!!!!!!!failed!!!!!!!!!!");
	exit(EXIT_FAILURE);
}
