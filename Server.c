#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>

// Max number of characters in each log file
#define MAXLOGSIZE 200
// Max number of different log Files
#define MAXLOGFILES 2

long int getFileSize(int fileDescriptor);
char *getPathname(char root[],int logcounter,const char *path);
char *getFileName(char root[],int logcounter);
void check(int *fd,int maxsize,int *logcounter, char root[],int linetowrite,const char *directoryPath);
void run(int sock,struct sockaddr_in client_addr,char root[],int *p, int *plogc,const char *directoryPath);
int fileExistsInDirectory(const char *directoryPath, const char *fileName);
void rotateLogFiles(int logcounter, char root[],const char directoryPath[]);
char *createLine(char message[],char client_ip[],char client_name[]);
int countTxtFiles(const char *folderPath);
void writeConnectionLog(int fd, const char *clientName); 


int main(int argc, char *argv[]){

   int port;
    char *directoryPath;

    // Set default values if not provided through command line arguments
    if (argc >= 3) {
        port = atoi(argv[1]);
        directoryPath = argv[2];
    } else {
        // Set default values here
        port = 7777;       // Default port number
        directoryPath = "./";  // Default directory path
    }
  
  // Localhost address
  char *ip = "127.0.0.1";

  int server_sock,newsockfd,pid;

  struct sockaddr_in server_addr,client_addr;
  socklen_t addr_size;
  
  // The name before the index of each file: example log0.txt
  char root[20] = "log";

  // The counter of the file log ex 0 for log0.txt
  int logcounter = 0;

  // Filedescriptor
  int fd = 0;

  // Pointer to file descriptor and logcounter
  int *p = &fd;
  int *plogc = &logcounter;

  // Variable used to verify the bind
  int n;

  // Creation of the socket
  server_sock = socket(AF_INET,SOCK_STREAM,0);

  // Check if the socket creation is failed 
  if( server_sock < 0 ){
    perror("Socket creation error");
    exit(1);
  }

  printf("TCP server socket created\n");

  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);


  // Binding
  n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

  // Check if the binding failed
  if( n < 0 ){
  perror("Binding error");
  exit(1);
  }
  printf("Binding to the port number %d\n", port);
  
  
  
 // Now we start listening for the client
  listen(server_sock, 5);
  printf("Listening ...\n");


 
    
    while (1) {

      // Accepting clients
      addr_size = sizeof(client_addr);
      newsockfd = accept(server_sock,(struct sockaddr*)&client_addr, &addr_size);

      // Setting the filedescriptor to the value returned by the open of log0.txt
      *p = open(getPathname(root,*plogc,directoryPath), O_WRONLY | O_APPEND | O_CREAT, 0644);

      // First check on the log file
      check(p,MAXLOGSIZE,plogc,root,0,directoryPath);

      

      if (newsockfd < 0) perror("ERROR on accept");

        pid = fork();
        if (pid < 0) perror("ERROR on fork");

        if (pid == 0)  {

          
          close(server_sock);
          run(newsockfd,client_addr,root,p,plogc,directoryPath);
          exit(0);

        } else close(newsockfd);

     } 

  return 0;
}


// Return the number of characters in the file 
long int getFileSize(int fileDescriptor) {
    long int size = 0;

    // Move the file position indicator to the end of the file
    if (lseek(fileDescriptor, 0, SEEK_END) == -1) {
        perror("Error seeking to end of file");
        return -1;  // Error
    }

    // Get the current file position, which is the size of the file
    size = lseek(fileDescriptor, 0, SEEK_CUR);

    // Reset the file position indicator to the beginning of the file
    if (lseek(fileDescriptor, 0, SEEK_SET) == -1) {
        perror("Error seeking to start of file");
        return -1;  // Error
    }

    return size;
}

// Return the complete filename given the letter part before the index, and the index 
char *getFileName(char root[],int index){
  char convertedLogCounter[5];
    sprintf(convertedLogCounter, "%d", index);

    // Calculate the length of the resulting string
    size_t resultLen = strlen(root) + strlen(convertedLogCounter) + strlen(".txt") + 1;

    // Allocate memory for the result
    char *result = (char *)malloc(resultLen);

    // Check if memory allocation is successful
    if (result != NULL) {
        // Construct the result string
        snprintf(result, resultLen, "%s%s%s", root, convertedLogCounter, ".txt");
    }

    return result;
}

// Generate the complete path of a file
char *getPathname(char root[],int logcounter,const char *path){
 

    // Allocate memory for the result
    size_t pathLen = strlen(path);
    size_t filenameLen = strlen(getFileName(root,logcounter));
    char *result = (char *)malloc(pathLen + 1 + filenameLen + 1);

    // Copy the path
    strcpy(result, path);

    // Add a directory separator if needed
    if (pathLen > 0 && path[pathLen - 1] != '/' && path[pathLen - 1] != '\\') {
        strcat(result, "/");
    }

    // Concatenate the filename
    strcat(result, getFileName(root,logcounter));
    return result;

}

// Remove the oldest log file and shift the other one position back: 
// For example copy the content of log2.txt in log1.txt and the content of log1.txt in log0.txt
void rotateLogFiles(int logcounter, char root[], const char directoryPath[]) {
    
    // Delete the oldest log file (log0.txt)
    remove(getPathname(root, 0,directoryPath));

    // Shift the remaining log files to make room for the new one
    for (int i = 1 ; i <= logcounter ; i++) {
        char oldPath[50], newPath[50];
        memset(oldPath,0,50);
        memset(newPath,0,50);
       
        strcpy(oldPath,getPathname(root,i,directoryPath));
        
        strcpy(newPath,getPathname(root,(i -1),directoryPath));
       
        rename(oldPath,newPath);
        //remove(oldPath);
    }
}

// Return 1 if a file named fileName is in the directory directoryPath, else 0
int fileExistsInDirectory(const char *directoryPath, const char *fileName) {
    DIR *dir;
    struct dirent *entry;
    dir = opendir(directoryPath);
    if (dir == NULL) {
        perror("Error opening directory");
        return -1;  // Error opening directory
    }

    while ((entry = readdir(dir)) != NULL) {
       

        if (entry->d_type == DT_REG && strcmp(entry->d_name, fileName) == 0) {
            closedir(dir);
            return 1;  // File found
        }
    }

    closedir(dir);
    return 0;  // File not found
}

// Count all the .txt files in the given directory
int countTxtFiles(const char *folderPath) {
    DIR *dir;
    struct dirent *entry;
    int count = 0;

    // Open the directory
    dir = opendir(folderPath);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    // Traverse the directory
    while ((entry = readdir(dir)) != NULL) {
        // Check if the entry is a regular file and ends with ".txt"
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".txt") != NULL) {
            count++;
        }
    }

    // Close the directory
    closedir(dir);

    return count;
}


//Check the status of logfile at the start of the server and everytime we want to write on log file
// To decide the best file to choose
void check(int *fd, int maxsize,int *logcounter,char root[],int linetowrite,const char *directoryPath){
  int counter = -1;
  
  
  // Over all the .txt files count only the ones named filenameIndex.txt:
  // For example log0.txt log1.txt ...
  // The counter starts from -1 so that it's already set to the index of the latest file written
  int txtNumber = countTxtFiles(directoryPath);

  for(int i=0; i<txtNumber; i++){
    counter += fileExistsInDirectory(directoryPath,getFileName(root,i));
  }
  // If i have more log files than the MAXLOGFILES, i delete those in excess, in temporal order
  if(counter > (MAXLOGFILES - 1) ){
    for(int  i=0; i < (counter - MAXLOGFILES) +1; i++){
      rotateLogFiles(counter,root,directoryPath);
    }
    counter = MAXLOGFILES - 1 ; 
  }

  // If there's no log file i create one
  if(counter == -1){
    (*logcounter) = 0;
    *fd = open(getPathname(root,*logcounter,directoryPath), O_WRONLY | O_APPEND | O_CREAT, 0644);
  }
  // Else i open the most recent one
  else{
    (*logcounter) = counter;
    *fd = open(getPathname(root,*logcounter,directoryPath), O_WRONLY | O_APPEND | O_CREAT, 0644);
  }
  
  // Before writing i check if there's space in the actual log file
  long int currentFileSize = getFileSize(*fd) + linetowrite;

  if(currentFileSize > maxsize){
    // If there's no space and i reached the MAXLOGFILES, i rotate and open a new one
    if(counter == (MAXLOGFILES -1 )){
      
      rotateLogFiles(counter, root,directoryPath);
      *fd = open(getPathname(root,counter,directoryPath), O_WRONLY | O_APPEND | O_CREAT, 0644);
      (*logcounter) = counter;
    }
    // Otherwise i open a new logfile
    else{
      (*logcounter)++;
       *fd = open(getPathname(root,*logcounter,directoryPath), O_WRONLY | O_APPEND | O_CREAT, 0644);
    }
  // If there's space i write in the open one    
  }else *fd = open(getPathname(root,*logcounter,directoryPath), O_WRONLY | O_APPEND | O_CREAT, 0644);
}

// Create the line with the message and the client infos
char *createLine(char message[],char client_ip[],char client_name[]){
  char temp[2048];

  // Get current time in seconds since the Epoch
  time_t currentTime = time(NULL);

  // Convert the timestamp to a string
  char *timestampString = ctime(&currentTime);

  strcpy(temp, client_ip);
  strcat(temp, " ");
  strcat(temp, client_name);  // Use strcat instead of strcpy here
  strcat(temp, " ");
  strcat(temp, timestampString);
  strcat(temp, message);
  char *line = strdup(temp);
  return line;
}

void writeConnectionLog(int fd, const char *clientName) {
    char line[2048];

    // Get current time in seconds since the Epoch
    time_t currentTime = time(NULL);

    // Convert the timestamp to a string
    char *timestampString = ctime(&currentTime);

    // Create the log entry
    sprintf(line, "%s connected %s", clientName, timestampString);

    // Write the log entry to the file
    write(fd, line, strlen(line));

    // Print the log entry to the console (optional)
    printf("%s\n", line);
}


// Main operations done by the server
void run(int sock,struct sockaddr_in client_addr,char root[],int *p, int *plogc,const char *directoryPath){

  char buffer[1024];
  char line[2048];
  char clientName[1024];

  // Receive the client name from the client
  ssize_t receivedBytes = recv(sock, clientName, sizeof(clientName), 0);
  if (receivedBytes <= 0) {
    // Close the client
    close(sock);

    // Close log file
    close(*p);

    printf("Connection closed\n");

    // Clean the buffer and line
    bzero(buffer, 1024);
    bzero(line, 2048);

    return;
    }

    // Null-terminate the clientName string
    clientName[receivedBytes] = '\0';


    // Write connection information to the log file
    writeConnectionLog(*p, clientName);

  while(1){
    // Get client's IP address
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

    
    
    // Clean the buffer
    bzero(buffer,1024);

    printf("\nPress CTRL + C to close the Server\nWaiting for messages...\n");
    //recieving from the client
    ssize_t recievedBytes = recv(sock,buffer,sizeof(buffer),0);

    if(recievedBytes <= 0 || strcmp(buffer,"EOF\n") == 0){
        //close the client
        
      close(sock);
      
      // Close log file
      close(*p);
      
      printf("Connection with %s closed\n",clientName);
        
      // Clean the buffer
      bzero(buffer,1024);

        break;
          
    }

    else{
      // Print the message
      printf("Client: %s\n",buffer);

      // Add the information to the text 
      strcpy(line,createLine(buffer,client_ip,clientName));

      // Check if there's space for writing
      check(p,MAXLOGSIZE,plogc,root,strlen(line),directoryPath);

     
      printf("Writing on:%s\n",getPathname(root,*plogc,directoryPath));
      //saving it in log file
      write(*p,line,strlen(line));

      printf("Actual file size: %ld \n", getFileSize(*p));

      //clean the buffer and line
      bzero(buffer,1024);
      bzero(line,strlen(line));

    } 
  }
}
