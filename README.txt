PROJECT OF CSAP ACADEMIC YEAR 2023/2024

STUDENT: ANDREA ZANACCO 2133081



INSTRUCTIONS:

+ Inside this folder you should find these files:
	
	singleTests folder
	AutomatedTests.sh
	Client.c
	Server.c
	Report.txt
	Makefile
	README.txt
	
+ To compile(inside the folder):

	make	
+ To run tests:

1)	chmod +x AutomatedTests.sh
2) 	./AutomatedTests

(The AutomatedTests script will compile everything and show you an example of client-server connection.
After closing the terminal panels that it will open, one for the server, one for the client, you will find the log of the connection inside the log0.txt file that will automatically be created by the server in this folder.)



+ To run the project:

1) ./server.out <listening_port> <directory_to_store_log_files>
	example: ./server.out 7777 /home/youruser/Desktop
	or: ./server.out 7777 ./ 
	
	to run with default settings
	or: ./server.out
	
	 
	
2) ./client.out <server_ip> <server_port> <client_name>
	example: 127.0.0.1 7777 client1
	to run with default settings
	or: ./client.out 
	
+ (if Server and Client are on the same machine use localhost as <server_ip>: 127.0.0.1)
+ (if you get: Binding error: Address already in use, you just have change the port number parameter)



+ After that, the server is wating for message from the client that can be type in on the stdin.
If the client writes EOF is disconnected from the server

+ You can connect any client number at any time just opening a new terminal tab and invoke: 
	./client.out <server_ip> <server_port> <client_name>
	
+ You can change MAXLOGSIZE and MAXLOGFILES to decide the maximum number of charcters in each log file and the number of different log files that you want to keep, by changing the corrispondent values in Server.c (lines 10 and 11, now set at:  MAXLOGSIZE 200, MAXLOGFILE 1)

+ You can appreciate the dynamic handling of the file(For example, if in the last execution, MAXLOGFILES = 3, so in the folder path you actually have: log0.txt,log1.txt,log2.txt
and now MAXLOGFILES = 2, the content of the last logfiles will be shifted so that the threshold is respected from the beginning, so you will obtain: log0.txt,log1.txt containing the content of the previous log1.txt and log2.txt)
