/*******************************************************************************
* File:         chatclient.h
* Author:       Shawn S Hillyer
* Date:         July 11, 2016
* Course:       OSU CSS 372: Project 1
*
* Description:  Include headers, constants, & common functions for chat client
*               
* Usage:        #include <"chatclient.h">
*               
* Cite:         Overall flow of a socket-based client/server pair of programs: 
                beej.us/guide/bgipc/output/html/multipage/unixsock.html  
*******************************************************************************/

#ifndef SSHILLYER_CHAT_H
#define SSHILLYER_CHAT_H

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


// Constants
#define MIN_PORT_NUMBER 1
#define MAX_PORT_NUMBER 65535
#define BUF_HANDLE 12
#define BUF_SIZE 513
#define BUF_MSG 500
// 500 MESSAGE + 10 handle + 1 prompt + 1 for space  + 1 null term

/*******************************************************************************
* void check_argument_count(int arg_c, int req, const char * message)
* : validates that arg_c == req and prints message if not
* argc: number of arguments expected
* req: number of arguments required
* message: usage message
*******************************************************************************/
void check_argument_count(int arg_c, int req, const char * message) {
	if (arg_c != req) {
		fprintf(stderr, "%s", message);
		exit(EXIT_FAILURE);
	}
}


/*******************************************************************************
* void perror_exit(char * message, int exit_value)
* prints message using perror() and calls exit with exit_value
* message: char * pointing to null terminated c-string
* exit_value: int representing the exit value to pass to exit
*******************************************************************************/
void perror_exit(char * message, int exit_value) {
	perror(message);
	exit(exit_value);
}


/*******************************************************************************
* int convert_string_to_int(const char * string)
* string: A null terminated string to be converted to an integer.
* Takes a string argument and returns best representation of it as a string
*******************************************************************************/
int convert_string_to_int(const char * string) {
	// parse port from command line argument and check result
	// Even though we are using the string version of the port, validate as an int
	errno = 0; // 0 out before evaluating the call to strtol
	int result = strtol(string, NULL, 10);
	if (errno == ERANGE) {
		fprintf(stderr, "The string '%s' converts to an integer outside the valid range.\n", string);
		exit(EXIT_FAILURE);
	}
	return result;
}


/*******************************************************************************
* void validate_port(int port, int err)
* confirm that port was parsed within valid range for ports
* Cite: man strtol, example section
*******************************************************************************/
void validate_port(int port, int err) {
	if ((errno == ERANGE && (port == LONG_MAX || port == LONG_MIN)) 
		|| (errno != 0 && port == 0) 
		|| (port > MAX_PORT_NUMBER || port < MIN_PORT_NUMBER)) 
	{
		// perror_exit("strtol", EXIT_FAILURE);
		fprintf(stderr, "strtol() cannot convert port, invalid format or out of range\n");
		// fprintf(stderr, "%s", message);
		exit(EXIT_FAILURE);
	}
}

/*******************************************************************************
* void strip_newline_from_string(char * string)
* string: A null-terminated c-string
* Strips newline, if one exists, from string & replace with null terminator
*******************************************************************************/
void strip_newline_from_string(char * string) {
	/* strcspn returns the length of all characters in a string that are not in
	the list of characters in the second argument. So this gives us the end
	and we place a null at that location in the string. */
	string[strcspn(string, "\r\n")] = 0; // replace LF, CR, CRLF< LFCR with null
}


/*******************************************************************************
* build_payload()
* 
*******************************************************************************/
char * build_payload(const char * handle, char * message) {
	int handle_len = strlen(handle);
	int message_len = strlen(message);
	char * payload = malloc(sizeof(char) * (handle_len + message_len) );
	memset(&payload[0], 0, sizeof(payload));
	int i, j;
	for (i = 0; i < handle_len; i++) {
		payload[i] = handle[i];
	}
	payload[i++] = '>';
	payload[i++] = ' ';

	for (j = 0; j < message_len; j++ ) {
		payload[i++] = message[j];
	}
	payload[i] = '\0';

	return payload;
}

/*******************************************************************************
* char * read_string_from_user(int max_len) {
* 
*******************************************************************************/
char * read_string_from_user(int max_len) {
	char * string; 
	string = malloc(sizeof(char) * max_len + 2); // extra space for \0 and \n
	memset(&string[0], 0, sizeof(string));

	// read input, trim off the \n replacing with a null
	fgets(string, max_len, stdin);
	string[strlen(string)-1] = '\0';
// >> DELETE THIS TRASH
	// printf("DEBUG: strlen(string) is: %u\n", strlen(string));
	// Flush the rest of the buffer?? 
	//( Not working as intended, user has to press enter twice after every input)
	// while(fgetc(stdin) != '\n');
// << END TRASH DELETE HERE
	return string;
}

/*******************************************************************************
* char * prompt_user_for_handle()
* 
*******************************************************************************/
char * prompt_user_for_handle() {
	char * handle;
	printf("Type in a handle, %d characters or less (extra truncated).\n", BUF_HANDLE - 2);
	printf("Press [enter] when done. (Excess characters in stdin buffer are sent as first message (BUG)\n");
	printf("Your handle?> ");
	handle = read_string_from_user(BUF_HANDLE);
	return handle;
}


/*******************************************************************************
* void safe_transmit_msg_on_socket(int message_length, int fd, char * buffer, int mode) {
* 
*******************************************************************************/
void safe_transmit_msg_on_socket(int fd, char * buffer, int message_length, int mode) {
	// mode == 1 : read | mode == 2 : write
	int bytes_remaining = message_length, bytes_transmitted = 0;
	while ( bytes_remaining > 0 ) {
		int i = message_length - bytes_remaining;
		if (mode == 1)
			bytes_transmitted = read(fd, buffer + i, bytes_remaining);
		else if (mode == 2)
			bytes_transmitted = write(fd, buffer, bytes_remaining);
		bytes_remaining -= bytes_transmitted;
	}
	if (bytes_transmitted == -1) {
		if (mode == 1)
			perror("read");
		else if (mode == 2)
			perror("write");	
		exit(EXIT_FAILURE);
	}
}


#endif