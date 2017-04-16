#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
	int position;
	char name[100];
} player;

player p1, p2;

void winner (int pos) {
	if (pos == 100)
		puts ("You win!");
	else
		 puts ("You lose!");
}
