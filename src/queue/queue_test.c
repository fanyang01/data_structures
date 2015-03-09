#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

#define MAXSIZE (1<<19)

int main(void)
{
	int i, tmp;
	queue *s;

	if((s = queue_init(sizeof(int))) == NULL) {
		goto FAILED;
	}
	for(i = 0; i < MAXSIZE; i++) {
		enqueue(s, &i);
	}
	for(i = 0; i < MAXSIZE; i++) {
		dequeue(s, &tmp);
		if(tmp != i) goto FAILED;
	}
	queue_free(s);
	puts("-----passed-----");

	return 0;
FAILED:
	fprintf(stderr, "!!!!!failed!!!!!\n");
	exit(EXIT_FAILURE);
}
