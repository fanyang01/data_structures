#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

int main(void)
{
	int i, tmp;
	stack *s;

	if((s = stack_init(sizeof(int))) == NULL) {
		fprintf(stderr, "failed to initialize stack\n");
		exit(EXIT_FAILURE);
	}
	for(i = 0; i < 100; i++) {
		push(s, &i);
	}
	while(pop(s, &tmp)) {
		printf("%d ", tmp);
	}
	puts("");
	stack_free(s);
	return 0;
}
