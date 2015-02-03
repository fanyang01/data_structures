#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int main(void)
{
	int i, tmp;
	queue *s;

	if((s = queue_init(sizeof(int))) == NULL) {
		fprintf(stderr, "failed to initialize queue\n");
		exit(EXIT_FAILURE);
	}
	for(i = 0; i < 100; i++) {
		push(s, &i);
	}
	while(pop(s, &tmp)) {
		printf("%d ", tmp);
	}
	puts("");
	queue_free(s);
	return 0;
}
