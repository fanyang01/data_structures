#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

#define MAXSIZE (1<<19)

int main(void)
{
	int i, tmp;
	stack *s;

	if((s = stack_init(sizeof(int))) == NULL) {
		goto FAILED;
	}
	for(i = 0; i < MAXSIZE; i++) {
		push(s, &i);
	}
	for(i = MAXSIZE - 1; i >= 0; i--) {
		pop(s, &tmp);
		if(tmp != i) goto FAILED;
	}
	stack_free(s);
	puts("-----passed-----");

	return 0;
FAILED:
	fprintf(stderr, "!!!!!failed!!!!!\n");
	exit(EXIT_FAILURE);
}
