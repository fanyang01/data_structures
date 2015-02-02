#include "list.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
	struct list_node list;
	int n;
} Node;

int main(void)
{
	int i;
	LIST_HEAD(list);
	Node *node;
	for(i = 0; i < 100; i++) {
		node = (Node *)malloc(sizeof(Node));
		node->n = i;
		list_add_head(&list, &node->list);
	}
	while(!list_is_empty(&list)) {
		printf("%d ", ((Node *)list_head(&list))->n);
		list_del(list_head(&list));
	}
	puts("");
	return 0;
}
