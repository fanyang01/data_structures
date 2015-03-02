#include "skiplist.h"
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
	skip_list *s;
	int i, tmp;

	/* srand((unsigned int)time(NULL)); */
	s = skip_init(sizeof(int), func);
	if(!s) {
		fprintf(stderr, "failed to initialize skip\n");
		goto FAILED;
	}
	char tested[MAXSIZE];

	memset(tested, 0, MAXSIZE);
	for(i = 0; i < MAXSIZE; i++) {
		if(skip_insert(s, &i) == NULL) {
			fprintf(stderr, "insert error\n");
			goto FAILED;
		}
	}

	for(i = MAXSIZE - 1; i >= 0; i--) {
		if(skip_search(s, &i) == NULL) {
			fprintf(stderr, "search error\n");
			goto FAILED;
		}
	}

	for(i = 0; i < MAXSIZE / 2; i++) {
		tmp = (int)rand() % MAXSIZE;
		bool res;
		res = skip_delete(s, &tmp);
		if(!res) {
			if(!tested[tmp]) {
				fprintf(stderr, "delete error\n");
				goto FAILED;
			}
		} else {
			if(tested[tmp]) {
				fprintf(stderr, "delete error\n");
				goto FAILED;
			}
			tested[tmp] = 1;
		}
	}
	skip_free(s);
	puts("----------passed----------");
	exit(EXIT_SUCCESS);
FAILED:
	puts("!!!!!!!!!!failed!!!!!!!!!!");
	exit(EXIT_FAILURE);
}
