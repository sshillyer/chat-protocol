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

serverPort = int(str(sys.argv[1])) # how to import from command line?
serverHandle = "Host A"
serverSocket = socket(AF_INET, SOCK_STREAM)
serverSocket.bind(('', serverPort))
serverSocket.listen(1)

print('The server is ready to receive')
while 1:
	connectionSocket, addr = serverSocket.accept()
	while 1:
		userInput = ''
		message = ''
		message = connectionSocket.recv(513)
		print(message.decode())
		#userInput = "Hi from chaserv.py"
		userInput = input(serverHandle + "> ")
		userInput.rstrip('\n')
		response = serverHandle + "> " + userInput 
		print("Response will be: " + response)
		connectionSocket.sendall(response.encode())
	# connectionSocket.close()