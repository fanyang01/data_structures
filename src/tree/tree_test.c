#include "tree.h"
#include <stdio.h>
#include <stdlib.h>

int func(void *x, void *y)
{
	int *a = (int *)x;
	int *b = (int *)y;
	if(*a > *b) return 1;
	else if(*a == *b) return 0;
	return -1;
}

int main(void)
{
	rb_tree *t;
	int i, tmp;

	t = tree_init(sizeof(int), func);
	if(!t) {
		fprintf(stderr, "failed to initialize tree\n");
		exit(EXIT_FAILURE);
	}
	for(i = 0; i < 50; i++)
		tree_insert(t, &i);
	for(i = 0; i < 40; i++) {
		tree_delete(t, &i, &tmp);
		printf("%d ", tmp);
	}
	puts("");
	tree_free(t);
	exit(EXIT_SUCCESS);
}
