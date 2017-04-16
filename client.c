#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "board.h"
#include "player.h"

void die_with_error(char *error_msg){
	printf("%s", error_msg);
	exit(-1);
}

void print_position () {
		//Print players' position on board
		printf ("\033[32m\033[1m\nME\t[Y]: %d\033[0m\n", p2.position);
		printf ("%s\t[X]: %d\n", p1.name, p1.position);
		printf ("[M] for mixed piece in one tile");
		printboard(p1.position, p2.position);
}

int main(int argc, char *argv[]){
	int client_sock, port_no, n;
	struct sockaddr_in server_addr;

	struct hostent *server;
	struct  hostent {
		char    *h_name;        /* official name of host */
		char    **h_aliases;    /* alias list */
		int     h_addrtype;     /* host address type */
		int     h_length;       /* length of address */
		char    **h_addr_list;  /* list of addresses from name server */
		#define h_addr  h_addr_list[0]  /* address, for backward compatiblity */
	};

	if (argc < 3) {
		printf("Usage: %s hostname port_no",  argv[0]);
		exit(1);
	}

	// Create a socket using TCP
	client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (client_sock < 0) 
		die_with_error("Error: socket() Failed.");

	server = gethostbyname(argv[1]);
	if (server == NULL) {
		die_with_error("Error: No such host.");
	}

	// Establish a connection to server
	port_no = atoi(argv[2]);
	bzero((char *) &server_addr,  sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,(char *)&server_addr.sin_addr.s_addr, server->h_length);

	server_addr.sin_port = htons(port_no);
	if (connect(client_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) 
		die_with_error("Error: connect() Failed.");

	//connection successful
	printf("Welcome player2!\n");

	initialize_client();
	char sbuffer[256];
	char rbuffer[256];

	//receives tiles with boosts
	for (int i = 0; i < b.boostsize; i++) {
		bzero(rbuffer, 256);
		n = recv(client_sock, rbuffer, sizeof(rbuffer), 0);
		if (n < 0)
			die_with_error("Error: recv() Failed.");
		b.tileswboost[i] = atoi(rbuffer);
	}

	//receives tiles with traps
	for (int i = 0; i < b.trapsize; i++) {
		bzero(rbuffer, 256);
		n = recv(client_sock, rbuffer, sizeof(rbuffer), 0);
		if (n < 0)
			die_with_error("Error: recv() Failed.");
		b.tileswtrap[i] = atoi(rbuffer);
	}

	//receives boosts' values
	for (int i = 0; i < b.boostsize; i++) {
		bzero(rbuffer, 256);
		n = recv(client_sock, rbuffer, sizeof(rbuffer), 0);
		if (n < 0)
			die_with_error("Error: recv() Failed.");
		b.boostvalue[i] = atoi(rbuffer);
	}

	//receives traps' values
	for (int i = 0; i < b.trapsize; i++) {
		bzero(rbuffer, 256);
		n = recv(client_sock, rbuffer, sizeof(rbuffer), 0);
		if (n < 0)
			die_with_error("Error: recv() Failed.");
		b.trapvalue[i] = atoi(rbuffer);
	}

	//input name
	bzero(sbuffer, 256);
	printf("Enter your name: ");
	scanf ("%s", sbuffer);
	sprintf (p2.name, "%s", sbuffer);
	printf ("Welcome %s\n", p2.name);

	//Receives player1 name
	bzero(rbuffer, 256);
	n = recv(client_sock, rbuffer, 255, 0);
	if (n < 0) die_with_error("Error: recv() Failed.");
	sprintf (p1.name, "%s", rbuffer);

	//Sends name to player1
	n = send(client_sock, sbuffer, strlen(sbuffer), 0);
	if (n < 0)
		die_with_error("Error: send() Failed.");

	//Receives player1 position
	bzero(rbuffer, 256);
	n = recv(client_sock, rbuffer, sizeof(rbuffer), 0);
	if (n < 0)
		die_with_error("Error: recv() Failed.");
	p1.position = atoi(rbuffer);

	p2.position = 0;
	//Sends position to player1
	bzero(sbuffer, 256);
	sprintf (sbuffer, "%d", p2.position);
	n = send(client_sock, sbuffer, sizeof(sbuffer), 0);
	if (n < 0)
		die_with_error("Error: send() Failed.");

	printmsg1 ();
	printboard(p1.position, p2.position);
	sleep (1);
	system ("clear");

	while ((p1.position != 100) || (p2.position != 100)) {
		int random;
		int ans;

		//SERVER'S TURN
		printf("%s's turn\n", p1.name);

		//receives player 1 position
		bzero(rbuffer, 256);
		n = recv(client_sock, rbuffer, sizeof(rbuffer), 0);
		if (n < 0) 
			die_with_error("Error: recv() Failed.");
		p1.position = atoi (rbuffer);

		//print board and board information
		system ("clear");
		printInfo ();
		print_position ();

		if (p1.position == 100)
			break;

		//----------------------------------------------------------//

		//CLIENT'S TURN

		//roll the dice
		printf("Press 1 to roll the dice, press any NUMBER to skip: ");
		scanf ("%d", &ans);
		if (ans == 1) {
			random = rand()%6+1;
		}
		else
			random = 0;

		system ("clear");
		//print board information
		printInfo ();

		//get position
		printf("\nAdd %d to position.\n", random);
		p2.position = p2.position + random;
		p2.position = (p2.position > 100 ? 100-(p2.position-100) : p2.position);
		int addend = boostOrTrapValue (p2.position);
		if (addend != 0)
			p2.position = p2.position + addend;

		//send the updated position to player 1
		bzero(sbuffer, 256);
		sprintf (sbuffer, "%d", p2.position);
		n = send(client_sock, sbuffer, sizeof(sbuffer), 0);
		if (n < 0)
			die_with_error("Error: send() Failed.");

		//print position on board
		print_position ();
		if (p2.position == 100)
			break;
	}
	//announce result
	winner(p2.position);

	//deallocate memory
	fin ();
	
	//closing connection
	close(client_sock);
	return 0;
}
