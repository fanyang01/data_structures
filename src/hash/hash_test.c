#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define MAXSIZE 1<<18
int compare(const void *x, const void *y)
{
	const int *a = (int *)x;
	const int *b = (int *)y;
	if(*a > *b) return 1;
	else if(*a == *b) return 0;
	return -1;
}

long hash_key(const void *data)
{
	return (long)*(int *)data;
}

int main(void)
{
	hash *h;
	int i, tmp;
	struct hash_init h_init;

	h_init.size = MAXSIZE;
	h_init.data_size = sizeof(int);
	h_init.cmp_func = compare;
	h_init.hash_func = hash_key;
	h = hash_init(&h_init);
	if(!h) {
		fprintf(stderr, "failed to initialize hash\n");
		goto FAILED;
	}
	int data[MAXSIZE];
	int tested[MAXSIZE];

	memset(tested, 0, sizeof(int) * MAXSIZE);
	for(i = 0; i < MAXSIZE; i++) {
		data[i] = i;
		if(!hash_insert(h, &i)) {
			fprintf(stderr, "insert error\n");
			goto FAILED;
		}
	}

	for(i = MAXSIZE - 1; i >= 0; i--) {
		if(hash_search(h, &i) == NULL) {
			fprintf(stderr, "search error\n");
			goto FAILED;
		}
	}

	srand((long)time(0) % (1<<31));
	for(i = 0; i < MAXSIZE / 2; i++) {
		tmp = (int)rand() % MAXSIZE;
		bool res = hash_delete(h, &tmp);
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
		tested[tmp] = 1;
	}
	hash_free(h);
	puts("----------passed----------");
	exit(EXIT_SUCCESS);
FAILED:
	puts("!!!!!!!!!!failed!!!!!!!!!!");
	exit(EXIT_FAILURE);
}
