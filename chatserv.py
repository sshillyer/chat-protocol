################################################################################
# File:         chatserv.py
# Author:       Shawn S Hillyer
# Date:         July 11, 2016
# Course:       OSU CSS 372: Project 1
#
# Description:  Listens for incoming requests on a port designated at command
#               line from chatclient process.
#               After TCP connectoin established, listens for message then
#               alternates sending and receiving messages to and from the 
#               recipient. Once a connection is terminated, chatserv listens
#               for additional incoming requests to establish a connection
#               If chatclient or chatserver types \quit, that host process will
#               send it as a message (without handle) to the server so that it
#               knows to cease communication. It will then close the socket
#               connection. 
#               
# Usage:        chatserver port
#               
# Cite:         Textbook page 167 for basic tcp python code
#
################################################################################

from socket import *
import sys

serverPort = int(str(sys.argv[1]))
serverHandle = "Host A"
serverSocket = socket(AF_INET, SOCK_STREAM)
serverSocket.bind(('', serverPort))
serverSocket.listen(1)

print('Server is listening on port ' + str(serverPort))

# Main server loop: listen for incoming connection
while 1:
	connectionSocket, addr = serverSocket.accept()
	again = True

	# Once connected, exchange messages until quit message received
	while again:
		userInput = ''
		message = ''  # TODO: try deleting this line, prob superflous
		message = connectionSocket.recv(513)
		message = message.decode()

		if message == "\quit":
			again = False
			print("Peer has requested disconnect.\nServer is listening for more connections.")
			break
		else:
			print(message)
			userInput = input(serverHandle + "> ")
			userInput.rstrip('\n')
			if userInput != "\quit":
				response = serverHandle + "> " + userInput
			else:
				response = userInput
			connectionSocket.sendall(response.encode())
			if userInput == '\quit':
				print("Sent \quit command, disconnected from client.\nServer is listening for more connections.")
				break

	# Once done exchanging messages, close the socket
	connectionSocket.close()