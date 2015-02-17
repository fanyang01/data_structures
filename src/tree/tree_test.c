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

	srand((unsigned int)time(NULL));
	t = tree_init(sizeof(int), func);
	if(!t) {
		fprintf(stderr, "failed to initialize tree\n");
		goto FAILED;
	}
	int data[MAXSIZE];
	char tested[MAXSIZE];

	memset(tested, 0, sizeof(int) * MAXSIZE);
	for(i = 0; i < MAXSIZE; i++) {
		data[i] = i;
		if(tree_insert(t, &i) == NULL) {
			fprintf(stderr, "insert error\n");
			goto FAILED;
		}
	}

	for(i = MAXSIZE - 1; i >= 0; i--) {
		if(tree_search(t, &i) == NULL) {
			fprintf(stderr, "search error\n");
			goto FAILED;
		}
	}

	for(i = 0; i < MAXSIZE / 2; i++) {
		tmp = (int)rand() % MAXSIZE;
		void *res;
		res = tree_delete(t, &tmp);
		if(!res) {
			if(!tested[tmp]) {
				fprintf(stderr, "search error\n");
				goto FAILED;
			}
		} else {
			if(tested[tmp]) {
				fprintf(stderr, "search error\n");
				goto FAILED;
			}
			tested[tmp] = 1;
		}
	}
	tree_free(t);
	puts("----------passed----------");
	exit(EXIT_SUCCESS);
FAILED:
	puts("!!!!!!!!!!failed!!!!!!!!!!");
	exit(EXIT_FAILURE);
}
