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
	char * message;
	char * response;
	char * payload;
	const char * hostname = argv[1]; // ex: localhost
	const char * port_str = argv[2]; // the port as a string. TODO:  convert this from the port variable using int to string method?
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
		exit(2);  // TODO: UNCOMMENT THIS IF COMMENTED!!
	}


	// Prompt user for a handle. Truncates anything in excess of 10 characters, strips newline
	client_handle = prompt_user_for_handle();


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
		
		// If other host sends \quit message, break out and exit chatclient
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