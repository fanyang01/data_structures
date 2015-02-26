#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAXSIZE (1<<19)
int func(const void *x, const void *y)
{
	const int *a = (int *)x;
	const int *b = (int *)y;
	if(*a > *b) return 1;
	else if(*a == *b) return 0;
	return -1;
}

int main(void)
{
	rb_tree *t;
	int i, tmp;
	clock_t start, end;

	srand((unsigned int)time(NULL));
	t = tree_init(sizeof(int), func);
	if(!t) {
		fprintf(stderr, "failed to initialize tree\n");
		goto FAILED;
	}
	char deleted[MAXSIZE];

	memset(deleted, 0, MAXSIZE);

	for(i = 0; i < MAXSIZE; i++) {
		if(tree_insert(t, &i) == NULL) {
			fprintf(stderr, "insert error\n");
			goto FAILED;
		}
	}

	for(i = 0; i < MAXSIZE; i++) {
		/* tmp = rand() % MAXSIZE; */
		if(tree_search(t, &i) == NULL) {
			fprintf(stderr, "search error\n");
			goto FAILED;
		}
	}

	for(i = 0; i < MAXSIZE / 2; i++) {
		tmp = rand() % MAXSIZE;
		void *res;
		res = tree_delete(t, &tmp);
		if(!res) {
			if(!deleted[tmp]) {
				fprintf(stderr, "delete error\n");
				goto FAILED;
			}
		} else {
			if(deleted[tmp]) {
				fprintf(stderr, "delete error\n");
				goto FAILED;
			}
			deleted[tmp] = 1;
		}
	}

	tree_free(t);
	puts("----------passed----------");
	exit(EXIT_SUCCESS);
FAILED:
	puts("!!!!!!!!!!failed!!!!!!!!!!");
	exit(EXIT_FAILURE);
}
