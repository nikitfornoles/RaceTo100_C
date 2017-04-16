#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

typedef struct {
	int *tileswtrap;
	int *tileswboost;
	int trapsize;
	int boostsize;
	int *trapvalue;
	int *boostvalue;
} board;

board b;

void printmsg1 () {
	printf ("The player who gets to the 100th tile first wins.\n");
}

void setBoost (int i) {
	b.boostsize = b.boostsize + 1;
	b.tileswboost = realloc(b.tileswboost, sizeof(int)*b.boostsize);
	b.tileswboost [b.boostsize-1] = i;
}

bool hasBoost (int i) {
	bool z = false;
	for (int j = 0; j < b.boostsize; j++) {
		if (i == b.tileswboost [j]) {
			z = true;
			break;
		}
	}
	return z;
}

void generateBoost () {
	int random;
	for (int i = 0; i < 10; i++) {
		random = rand() % 100 + 1;
		//no boosts on the topmost row
		if (random > 90)
			i = i-1;
		else if (hasBoost (random))
			i = i-1;
		else
			setBoost (random);
	}
}

void getBoost () {
	printf ("Tiles with boost:\t");
	for (int i = 0; i < b.boostsize; i++) {
		printf ("%d", b.tileswboost[i]);
		if (i != b.boostsize-1)
			printf (",\t");
	}
	printf ("\n"); 
}

void clearBoost () {
	b.boostsize = 0;
	b.tileswboost = calloc(0, sizeof(int));
}

void generateBoostValue () {
	int random;
	for (int i = 0; i < b.boostsize; i++) {
		int diff = 100-b.tileswboost[i];
		int half = diff * 0.5;
		random = rand() % half  + 5;
		b.boostvalue[i] = random;
	} 
}

void getBoostValue () {
	printf ("Boost Values:\t\t");
	for (int i = 0; i < b.boostsize; i++) {
		printf ("%d", b.boostvalue[i]);
		if (i != b.boostsize-1)
			printf (",\t");
	}
	printf ("\n"); 
}

void setTrap (int i) {
	b.trapsize = b.trapsize + 1;
	b.tileswtrap = realloc(b.tileswtrap, sizeof(int)*b.trapsize);
	b.tileswtrap [b.trapsize-1] = i;
}

bool hasTrap (int i) {
	bool z = false;
	for (int j = 0; j < b.trapsize; j++) {
		if (i == b.tileswtrap [j]) {
			z = true;
			break;
		}
	}
	return z;
}

void generateTrap () {
	int random;
	for (int i = 0; i < 10; i++) {
		random = rand()%100 + 1;
		//no traps on the bottom row
		if (random < 11)
			i = i-1;
		else if (random == 100)
			i = i-1;
		else if (hasBoost (random))
			i = i-1;
		else if (hasTrap (random))
			i = i-1;
		else
			setTrap (random);
	}
}

void getTrap () {
	printf ("Tiles with trap:\t");
	for (int i = 0; i < b.trapsize; i++) {
		printf ("%d", b.tileswtrap[i]);
		if (i != b.trapsize-1)
			printf (",\t");
	}
	printf ("\n"); 
}

void clearTrap () {
	b.tileswtrap = calloc(0, sizeof(int));
	b.trapsize = 0;
}

void generateTrapValue () {
	int random;
	for (int i = 0; i < b.trapsize; i++) {
		int half = (int) (b.tileswtrap[i] * 0.5);
		random = rand()%half  + 5;
		b.trapvalue[i] = random;
		b.trapvalue[i] = b.trapvalue[i] * -1;
	} 
}

void getTrapValue () {
	printf ("Trap Values:\t\t");
	for (int i = 0; i < b.trapsize; i++) {
		printf ("%d", b.trapvalue[i]);
		if (i != b.trapsize-1)
			printf (",\t");
	}
	printf ("\n"); 
}

int trapIndex (int pos) {
	int index;
	for (int i = 0; i < b.trapsize; i++) {
		if (pos == b.tileswtrap[i]) {
			index = i;
			break;
		}
	}
	return index;
}

int boostIndex (int pos) {
	int index;
	for (int i = 0; i < b.boostsize; i++) {
		if (pos == b.tileswboost[i]) {
			index = i;
			break;
		}
	}
	return index;
}

int boostOrTrapValue (int pos) {
	int value = 0;
	int index;
	if (hasBoost (pos)) {
		index = boostIndex(pos);
		value = b.boostvalue[index];
		printf ("\033[34m\033[1m");
		printf ("Tile %d contains a boost with a value of %d.\n", pos, value);
	}
	else if (hasTrap (pos)) {
		index = trapIndex(pos);
		value = b.trapvalue[index];
		printf ("\033[31m\033[1m");
		printf ("Tile %d contains a trap with a value of %d.\n", pos, value);
	}
	printf ("\033[0m");
	return value;
}

void initialize_server () {
	srand(time(NULL));
	b.trapsize = 0;
	b.tileswtrap = calloc(0, sizeof(int));
	b.boostsize = 0;
	b.tileswboost = calloc(0, sizeof(int));
	generateBoost ();
	generateTrap ();
	b.boostvalue = calloc(b.boostsize, sizeof(int));
	generateBoostValue ();
	b.trapvalue = calloc(b.trapsize, sizeof(int));
	generateTrapValue ();
}

void initialize_client () {
	srand(time(NULL));
	b.trapsize = 10;
	b.tileswtrap = calloc(b.trapsize, sizeof(int));
	b.boostsize = 10;
	b.tileswboost = calloc(b.boostsize, sizeof(int));
	b.boostvalue = calloc(b.boostsize, sizeof(int));
	b.trapvalue = calloc(b.trapsize, sizeof(int));
}

void printInfo () {
	printf ("  ==============================================\n");
	printf ("||\tTiles\tTrap\t||\tTiles\tBoost\t||\n");
	printf ("||\twith\tValue\t||\twith\tvalue\t||\n");
	printf ("||\ttrap\t\t||\tBoost\t\t||\n");
	printf ("  ==============================================\n");
	for (int i = 0; i < b.trapsize; i++) {
		printf ("||\t%d\t%d\t||\t%d\t%d\t||\n", b.tileswtrap[i], 
			b.trapvalue[i], b.tileswboost[i], b.boostvalue[i]);
	}
	printf ("  ==============================================\n");
}

void printboard(int pos1, int pos2){
	int w [10][10];
	int val;

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if (i%2 == 0)
				val = 100-((i*10)+j);
			else
				val = 100-((i+1)*10)+(j+1);
			w[i][j] = val;
		}
	}
	printf ("\n ");
	for (int k = 0; k < 10; k++) {
		printf ("_____ ");
	}
	printf ("\n");
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			printf ("|");
			printf (" ");
			if (w[i][j] < 100)
				printf (" ");
			if (w[i][j] < 10)
				printf (" ");
			if (w[i][j] == pos1) {
				if (pos1 == 100)
					printf (" ");
				if (pos1 != pos2)
					printf ("\033[33m\033[1mX"); //yellow bold
				else
					printf ("\033[33m\033[1mM"); //yellow bold
				if (pos1 == 100)
					printf (" ");
				else if (pos1 > 9)
					printf (" ");
			}
			else if (w[i][j] == pos2) {
				if (pos2 == 100)
					printf (" ");
				if (pos1 != pos2)
					printf ("\033[32m\033[1m\033[5mY"); //green bold
				else
					printf ("\033[33m\033[1mM"); //yellow bold
				if (pos2 == 100)
					printf (" ");
				else if (pos2 > 9)
					printf (" ");
			}
			else {
				if (hasBoost (w[i][j]))
					printf ("\033[34m\033[1m"); //blue bold
				else if (hasTrap (w[i][j]))
					printf ("\033[31m\033[1m"); //red bold
				printf ("%d", w[i][j]);
			}
			if (w[i][j] == 100)
				printf (" ");
			else if (w[i][j] < 100)
				printf (" ");

			printf ("\033[0m"); //reset color
			if (j == 9) {
				printf ("|");
				printf ("\n ");
				for (int k = 0; k < 10; k++)
					printf ("_____ ");
			}
		}
		printf ("\n");
	}
}

void fin () {
	free(b.tileswtrap);
	free(b.tileswboost);
	free(b.trapvalue);
	free(b.boostvalue);
}
