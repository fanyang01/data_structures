#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int main(void)
{
	int i, tmp;
	queue *q;

	if((q = queue_init()) == NULL) {
		fprintf(stderr, "failed to initialize queue\n");
		exit(EXIT_FAILURE);
	}
	for(i = 0; i < 100; i++) {
		enqueue(q, &i, sizeof(int));
	}
	while(dequeue(q, &tmp, sizeof(int))) {
		printf("%d ", tmp);
	}
	puts("");
	queue_free(q);
	return 0;
}
