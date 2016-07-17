--------------------------------------------------------------------------------
Program Author:  Shawn S Hillyer
Date:            7/15/2016

Description:     This README.txt file explains how to compile, execute, and 
                 control the chatclient and chatserv.py programs.
--------------------------------------------------------------------------------

TLDR (Too-Long-Didn't Read):
1) Unzip package to directory
2) Execute 'make' at command line prompt on Linux
3) Execute 'python3 chatserv.py portnum'  (where portnum is a valid port number)
4) Execute 'chatclient hostname portnum'  in a separate terminal/window
5) Enter a handle in chatclient terminal and press enter. Max 10 characters
   (excess left in stdnin will be sent immediately as first message, known bug)
6) Type a message in chatclient and press enter. Max 500 characters.
7) Type a response in the chatserv.py process and press enter.
8) Client and server alternate prompting for messages until a user
   types '\quit' as the message. This will send the \quit command to the peer
   then close the connection.
9) chatserv will continue to listen for new connections after the \quit command
   is sent or received using the same port. To terminate chatserv, send a SIG_INT
   signal (Ctrl-C by default)




Additional notes on some of the steps:


1) Unzip all files into a directory of your choice. This should unpack the
   following files:
   * README.txt: This file
   * chatclient.c: Source code for a client that will connect to chatserv.py
   * chatclient.h: Header file for chatclient.c
   * chatserv.py:  Python3 script to run a server that will allow chatclient
     to connect. This is more of a peer-to-peer server than a tree server that
     creates threads.

2) At a Linux command line (for example, on the OSU "FLIP" server), type
   the 'make' command. This will compile chatclient.c and create the executable
   'chatclient'

3) Pick a port on which you would like to have the chatserv.py process listen.
   * Ports 1024-49151 are the User Ports> Best practice is to use these.
   * Ports 49152-66535 are the Dynamic Ports (assigned by the operating system)
   * Ports 0-1023 are the Well Known Ports assigned by Iana. 
   (Citation: Various posts on stackoverflow that cite www.iana.org)

   Once selected, execute the server and the client using these commands in order:
   (Ensure to execute in separate windows/terminals!)

     a)  python3 chatserv.py portnum
4)   b)  chatclient hostname portnum

   If you're running the chatserv.py script and the chatclient application on
   the same machine, hostname should be the string 'localhost', while portnum
   should be the port you selected from above. If the machine you're running
   has a hostname that can be retrieved using DNS, then using that hostname
   should work as well.

NOTE: If chatserv.py indicates port is unavailable, choose another before 
      launching chatclient; it will fail to connect unless the server is running.

5) chatclient will prompt the user for a handle. Type in up to 10 characters
   followed by the [enter] key. Exceeding the 10 character limit will overflow
   the stdin stream and cause the client to send the first message using the
   excess characters; this is a known bug, but does not break the program.

6) chatclient will present a prompt with the chosen username like this:
   handle> _         (_ is the cursor location)
   Type a message up to 500 characters in chatclient and press [enter] to send
   the message.
7) chatserv will print the received message then prompt the user on its end in
   the same fashion. Type a message up to 500 characters and press [enter] to send.

8) Client and server take turns sending messages until one of the processes
   types '\quit' as the message. This will send the \quit command to the peer
   then close the connection.
9) chatserv will continue to listen for new connections after the \quit command
   is sent or received using the same port. To terminate chatserv, send a SIGINT
   signal (Ctrl-C by default)