#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

int main(void)
{
	int i, tmp;
	stack *s;

	if((s = stack_init()) == NULL) {
		fprintf(stderr, "failed to initialize stack\n");
		exit(EXIT_FAILURE);
	}
	for(i = 0; i < 100; i++) {
		push(s, &i, sizeof(int));
	}
	while(pop(s, &tmp, sizeof(int))) {
		printf("%d ", tmp);
	}
	puts("");
	stack_free(s);
	return 0;
}
