/*******************************************************************************
* File:         ftserv.c
* Author:       Shawn S Hillyer
* Date:         July 28, 2016
* Course:       OSU CSS 372: Project 2
*
* Description:  

*               
* Usage:        ftserv hostname port
*               
* Cite:         Overall flow of a socket-based client/server pair of programs: 
*               beej.us/guide/bgipc/output/html/multipage/unixsock.html  
*
* Note:         Portions of this program were re-used from assignment 4 that I
*               completed in CS 344.
*******************************************************************************/

#include "ftserv.h"


/*******************************************************************************
* main()
* Listens for connection request on port provided on command line
*******************************************************************************/
int main(int argc, char const *argv[]) {
	// allocate space to hold messages and various strings
	char * message;
	char * response;
	char * payload;
	const char * port_str = argv[1];
	const char * client_handle;
	char s[INET6_ADDRSTRLEN]; // used to store the connecting host

	// Other variables used - see page 23 beej
	socklen_t addr_size; 
	struct sockaddr_storage client_addr;

	// Verify Arguments are valid, print error message if not
	check_argument_count(argc, 2, "Usage: ftserv port\n");


	// Parse and validate port, save port as a string for loading address
	int port = convert_string_to_int(argv[1]);
	validate_port(port, errno);


	// Variables for sockets and the server address (See page 16 of beej guide)
	int sfd, control_sfd, data_sfd, status; 
	struct addrinfo hints, *servinfo, *clientinfo;
	
	
	// 0 out hints struct then init to connect to hostname via TCP
	// Cite: lecture slides, man getaddrinfo(3), and beej guide - random bits
	// Use the getaddrinfo() to fill out servinfo by passing in some 'hints'
	memset(&hints, 0, sizeof hints);  // clear out the hints struct for safety
	hints.ai_family = AF_INET; // AF_UNSPEC would be unspecified IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // Use TCP -- need 2-way communication
	hints.ai_flags = AI_PASSIVE; // fill in localhost ip

	// populate servinfo using the hints struct (Cite beej pg. 17 sample)
	if ( (status = getaddrinfo(NULL, port_str, &hints, &servinfo)) != 0) {
		perror_exit("getaddrinfo", EXIT_FAILURE);
	}


	// Now open a TCP socket stream; Cite: Slide 10 Unix Networking 2 (lecture)
	// Cite: Beej network guide for using hints structure, page 
	// Must be called after getaddrinfo() so that servinfo struct is populated
	if ((sfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
		perror_exit("socket", EXIT_FAILURE);
	}


	// Next, bind socket (sfd) to a port (See beej page 20)
	if (bind(sfd, servinfo->ai_addr, servinfo->ai_addrlen) == 01) {
		perror_exit("bind", EXIT_FAILURE);
	}

	
	
	printf("Server open on %d\n", port);

	// Inspired by beej guide page 29)
	while(1) {
		// Listen for connections (Beej's guide 22-23)
		if (listen(sfd, 5) == -1) {
			perror_exit("listen", EXIT_FAILURE);
		}

		// Accept the connection
		addr_size = sizeof client_addr;
		if (control_sfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size) == -1) {
			perror_exit("accept", EXIT_FAILURE);
		}
	

		// Figure out who connected and print it to screen (should be stored in )
		// populate servinfo using the hints struct (Cite beej pg. 15-26 sample)

// THIS BLOCK FROM PAGE 79 - LETTER FOR LETTER
socklen_t len;
struct sockaddr_storage addr;
char ipstr[INET6_ADDRSTRLEN];
int data_port; 
len = sizeof addr;
getpeername(control_sfd, (struct sockaddr*)&addr, &len);

// deal with both IPv4 and IPv6:
if (addr.ss_family == AF_INET) {
	struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	data_port = ntohs(control_sfd->sin_port);
	inet_ntop(AF_INET, &control_sfd->sin_addr, ipstr, sizeof ipstr);
} 
else { // AF_INET6
	struct sockaddr_in6 *control_sfd = (struct sockaddr_in6 *)&addr;
	data_port = ntohs(control_sfd->sin6_port);
	inet_ntop(AF_INET6, &control_sfd->sin6_addr, ipstr, sizeof ipstr);
}
//printf("Peer IP address: %s\n", ipstr);
//printf("Peer port : %d\n", data_port);
// END RIP


		printf("Connection from %s\n", ipstr);

		// Parse the incoming command

		// if command is not valid, send error message on control_sfd to client, close open socket P, continue while()

		// get/open a second connection, assign it to data_sfd, connecting back to clients ip and using <dataport> (which it sent over)

		// if client sent -l command, send dir() listing to client
		if(commandIsList) {
			printf("List directory requested on port %d\n", data_port);
			// send the info!
		}

		// if client sent -g <FILENAME>
		if(commandisGet) {
			printf("File \"%s\" requested on port %d\n", file_name, data_port);

			// validate filename then send file if it exists

			// If error opening/finding file, close connection Q (data_sfd) and send error message on connection P (control_sfd)

			// else send the file
		}
		
		// Close the data_sfd connection

		// close the control_sfd connection
				

		
		
	}







	// Main loop. Read message, send, listen for response.
	int again = 1; // "true"
	while (again) {
		// Print prompt and read user input
		printf("%s> ", client_handle);
		message = read_string_from_user(BUF_MSG);

		// If user's "message" is the '\quit' command, send the message as-is, then exit
		if (strcmp(message, "\\quit") == 0) {
			safe_transmit_msg_on_socket(sfd, message, strlen(message), 2);
			again = 0; // instead of these two lines, could just 'break'
			break;
		}
		// otherwise send the message
		else {
			payload = build_payload(client_handle, message);
			safe_transmit_msg_on_socket(sfd, payload, strlen(payload), 2);
		}

		// response is freed before the "chat loop" ends; not doing causes excess left in string
		response = malloc(sizeof(char) * BUF_MSG);
		int z;
		for (z = 0; z < BUF_MSG; z++) {
			response[z] = '\0';
		}

		// Receive response from server, print to screen if response not '\quit'
		int bytes_transmitted = read(sfd, response, BUF_SIZE);
		if (bytes_transmitted == -1) {
			perror("read");
			exit(EXIT_FAILURE);
		}
		
		// If other host sends \quit message, break out and exit ftserv
		if (strcmp(response, "\\quit") == 0) {
			printf("Server host has terminated session with \\quit command.\n");
			break;
		}
		// Otherwise just print the response.
		else {
			if(strlen(response) > 0) {
				printf("%s\n", response);
			}
		}

		// Free dynamic memory before looping again
		if (message) {
			free(message);
			message = NULL;
		}
		if (response) {
			memset(&response[0], 0, sizeof(response)); // just in case the array had values from before
			free(response);
			response = NULL;
		}
		if (payload) {
			free(payload);
			payload = NULL;
		}
	}

	// Free the dynamic allocated memory we used
	if (servinfo) 
		free(servinfo); // freeaddrinfo() ??
	return 0;

}
