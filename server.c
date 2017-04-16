#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "board.h"
#include "player.h"

void die_with_error(char *error_msg){
	printf("%s", error_msg);
	exit(-1);
}

void print_position () {
	//Print players' position on board
	printf ("\n\033[33m\033[1mME\t[X]: %d\033[0m\n", p1.position);
	printf ("%s\t[Y]: %d\n", p2.name, p2.position);
	printf ("[M] for mixed piece in one tile");
	printboard(p1.position, p2.position);
}

int main(int argc, char *argv[]) {
	int server_sock, client_sock, port_no, client_size, n;

	struct sockaddr_in server_addr, client_addr;
	struct sockaddr_in {
		short   sin_family; /* must be AF_INET */
		u_short sin_port;
		struct  in_addr sin_addr;
		char    sin_zero[8]; /* Not used, must be zero */
	};

	if (argc < 2) {
		printf("Usage: %s port_no", argv[0]);
		exit(1);
	}

	// Create a socket for incoming connections
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock < 0)
		die_with_error("Error: socket() Failed.");

	// Bind socket to a port
	bzero((char *) &server_addr, sizeof(server_addr));
	port_no = atoi(argv[1]);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_no);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
		die_with_error("Error: bind() Failed.");

	// Mark the socket so it will listen for incoming connections
	listen(server_sock,5);
	printf("Wating for Player2!\n");

	// Accept new connection
	client_size = sizeof(client_addr);
	client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &client_size);
	if (client_sock < 0)
		die_with_error("Error: accept() Failed.");

	//Client successfully connected
	initialize_server();

	char sbuffer[256];
	char rbuffer [256];

	//sends tiles with boosts
	for (int i = 0; i < b.boostsize; i++) {
		bzero(sbuffer, 256);
		int a = b.tileswboost [i];
		sprintf (sbuffer, "%d", a);

		n = send(client_sock, sbuffer, sizeof(sbuffer), 0);
		if (n < 0)
			die_with_error("Error: send() Failed.");
	}

	//sends tiles with traps
	for (int i = 0; i < b.trapsize; i++) {
		bzero(sbuffer, 256);
		int a = b.tileswtrap [i];
		sprintf (sbuffer, "%d", a);

		n = send(client_sock, sbuffer, sizeof(sbuffer), 0);
		if (n < 0)
			die_with_error("Error: send() Failed.");
	}

	//sends boosts' values
	for (int i = 0; i < b.boostsize; i++) {
		bzero(sbuffer, 256);
		int a = b.boostvalue [i];
		sprintf (sbuffer, "%d", a);

		n = send(client_sock, sbuffer, sizeof(sbuffer), 0);
		if (n < 0)
			die_with_error("Error: send() Failed.");
	}

	//sends traps' values
	for (int i = 0; i < b.trapsize; i++) {
		bzero(sbuffer, 256);
		int a = b.trapvalue [i];
		sprintf (sbuffer, "%d", a);

		n = send(client_sock, sbuffer, sizeof(sbuffer), 0);
		if (n < 0)
			die_with_error("Error: send() Failed.");
	}

	//input name
	bzero(sbuffer, 256);
	printf("Enter your name: ");
	scanf ("%s", sbuffer);
	sprintf (p1.name, "%s", sbuffer);
	printf ("Welcome %s\n", p1.name);

	//Sends name to player2
	n = send(client_sock, sbuffer, strlen(sbuffer), 0);
	if (n < 0)
		die_with_error("Error: send() Failed.");

	//Receives player2 name
	bzero(rbuffer, 256);
	n = recv(client_sock, rbuffer, 255, 0);
	if (n < 0) die_with_error("Error: recv() Failed.");
	sprintf (p2.name, "%s", rbuffer);

	p1.position = 0;
	//Sends position to player2
	bzero(sbuffer, 256);
	sprintf (sbuffer, "%d", p1.position);
	n = send(client_sock, sbuffer, sizeof(sbuffer), 0);
	if (n < 0)
		die_with_error("Error: send() Failed.");

	//Receives player2 position
	bzero(rbuffer, 256);
	n = recv(client_sock, rbuffer, sizeof(rbuffer), 0);
	if (n < 0)
		die_with_error("Error: recv() Failed.");
	p2.position = atoi(rbuffer);

	printmsg1 ();
	printboard(p1.position, p2.position);
	sleep (1);
	system ("clear");

	while (p1.position != 100 || p2.position != 100) {
		int random;
		int ans;

		//SERVER'S TURN

		//roll the dice
		printf("Press 1 to roll the dice, press any NUMBER to skip: ");
		scanf ("%d", &ans);
		if (ans == 1) {
			//get 2 values for random, so client won't have the same value for random
			random = rand()%6+1;
			random = rand()%6+1;
		}
		else
			random = 0;

		system ("clear");
		//print board information
		printInfo ();

		//get position
		printf("\nAdd %d to position.\n", random);
		p1.position = p1.position + random;
		p1.position = (p1.position > 100 ? 100-(p1.position-100) : p1.position);
		int addend = boostOrTrapValue (p1.position);
		if (addend != 0)
			p1.position = p1.position + addend;

		//send the updated position to player 2
		bzero(sbuffer, 256);
		sprintf (sbuffer, "%d", p1.position);
		n = send(client_sock, sbuffer, sizeof(sbuffer), 0);
		if (n < 0)
			die_with_error("Error: send() Failed.");

		//print position on board
		print_position ();
		if (p1.position == 100)
			break;

		//----------------------------------------------------------//

		//CLIENT'S TURN
		printf("%s's turn\n", p2.name);
		//receive position of player 2
		bzero(rbuffer, 256);
		n = recv(client_sock, rbuffer, sizeof(rbuffer), 0);
		if (n < 0) 
			die_with_error("Error: recv() Failed.");
		p2.position = atoi (rbuffer);

		//print board and board information
		system ("clear");
		printInfo ();
		print_position ();

		if (p2.position == 100)
			break;
	}
	//announce result
	winner(p1.position);

	//deallocate memory
	fin ();

	//closing connection
	close(client_sock);
	close(server_sock);
	return 0;
}
