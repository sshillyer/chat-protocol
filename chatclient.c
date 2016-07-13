/*******************************************************************************
* File:         chatclient.h
* Author:       Shawn S Hillyer
* Date:         July 11, 2016
* Course:       OSU CSS 372: Project 1
*
* Description:  Connects to a chatserver running on a specified host and port #
*               After poviding a handle (ie, username or alias) the user can
*               type a message, press enter, and send the text to chatserver
*               Chatclient then waits for a response and will print the response
*               from the server if it is a message.
*               If the chatclient user types \quit as a message, chatclient will
*               send it as a message (without handle) to the server so that it
*               knows to cease communication. It will then close the socket
*               connection. 
*               
* Usage:        chatclient hostname port
*               
* Cite:         Overall flow of a socket-based client/server pair of programs: 
*               beej.us/guide/bgipc/output/html/multipage/unixsock.html  
*
* Note:         Portions of this program were re-used from assignment 4 that I
*               completed in CS 372.
*******************************************************************************/

#include "chatclient.h"


/*******************************************************************************
* main()
* Connects to a server and sends message
*******************************************************************************/
int main(int argc, char const *argv[]) {
	// allocate space to hold messages and various strings
	char message[BUF_SIZE];
	char resp[BUF_SIZE]; // might not really need this but it's "safer"
	const char * port_str = argv[2]; // the port as a string. TODO:  convert this from the port variable using int to string method?
	const char * hostname = argv[1]; // ex: localhost
	const char * client_handle;
	

	// Verify Arguments are valid
	check_argument_count(argc, 3, "Usage: chatclient hostname port\n");


	// Parse and validate port, save port as a string for loading address
	int port = convert_string_to_int(argv[2]);
	validate_port(port, errno);


	// Variables for sockets and the server address
	int sfd, status; 
	struct addrinfo hints, *servinfo;
	
	
	// 0 out hints struct then init to connect to hostname via TCP
	// Cite: lecture slides, man getaddrinfo(3), and beej guide - random bits
	// Use the getaddrinfo() to fill out servinfo by passing in some 'hints'
	memset(&hints, 0, sizeof hints);  // clear out the hints struct for safety
	hints.ai_family = AF_INET; 
	hints.ai_socktype = SOCK_STREAM; // Use TCP -- need 2-way communication
	hints.ai_flags = AI_PASSIVE; // fill in localhost ip

	// populate servinfo using the hints struct
	if ( (status = getaddrinfo(hostname, port_str, &hints, &servinfo)) != 0) {
		perror_exit("getaddrinfo", EXIT_FAILURE);
	}


	// Now open a TCP socket stream; Cite: Slide 10 Unix Networking 2 (lecture)
	// Cite: Beej network guide for using hints structure
	// Must be called after getaddrinfo() so that servinfo struct is populated
	if ((sfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
		perror_exit("socket", EXIT_FAILURE);
	}

	// Connect to server indicated by servinfo.ai_addr
	if(connect(sfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		fprintf(stderr, "chatclient: Could not contact chatserver process at %s:%d\n", hostname, port);
		exit(2);
	}


	// Prompt user for a handle. Truncates anything in excess of 10 characters, strips newline
	client_handle = prompt_user_for_handle();
	int client_handle_length = strlen(client_handle);
	// printf("%s>", client_handle);
	// printf("\nDEBUG  Client handle is %d long\n", client_handle_length);

	while (1) {
		long message_length = strlen(message);

		// Send Message
		safe_transmit_msg_on_socket(sfd, message, message_length, 2);

		// Receive response from the server and print to screen.
		safe_transmit_msg_on_socket(sfd, resp, message_length, 1);
		
	}

	// Free the dynamic allocated memory we used
	free(servinfo); // freeaddrinfo() ??

	return 0;
}