#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h> //struct hostent
#include<netdb.h> //gethostbyname()
#include<string.h>
#include<unistd.h> //usleep()

#define BUFFSIZE 256 //buffer size

/*print error message and stop program*/
void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[]) {
    int port = atoi(argv[1]), sockfd;
    char *serverip = argv[2];
    struct sockaddr_in serv_addr;
    struct hostent *server; //server IP structure
    server = gethostbyname(serverip);
    if(server == NULL) {
        fprintf(stderr,"Error, no such host");
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        error("Error opening socket");
    }
    
    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(serverip); //copy ip to serv_addr
    printf("trying connect server...\n");
    //keep trying to connect to server until server open socket
    while(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0){
        usleep(100);
    }
    printf("Connected successfully.\n");
    
    FILE *fp;                           //file to download from server
    int r;                              //return value of read()
    char fsiz_str[BUFFSIZE], buffer[BUFFSIZE];              //file name
    long int fsiz;                      //size of file (unit: byte)
    bzero(buffer, BUFFSIZE);
    r = read(sockfd, buffer, BUFFSIZE);           //read file name from server
    if(r<0){
        error("Error reading file name");
    }
    usleep(10);
    r = read(sockfd, fsiz_str, BUFFSIZE);         //read file size
    if(r<0){
        error("Error reading file size");
    }
    fsiz = atoi(fsiz_str);              //convert file size from char to long int
    if(fsiz/1024 < 1024){
        printf("Ready to download \'%s\'[%.2lf KB]...\n", buffer, (double)fsiz/1024);
    } else {
        printf("Ready to download \'%s\'[%.2lf MB]...\n", buffer, (double)fsiz/1048576);
    }
    fp = fopen(buffer,"ab");                  //open a binary file, which name was received from server
    if(fp == NULL){
        error("Error opening file");
    }
    
    while(r = read(sockfd, buffer, BUFFSIZE)) {
        if(r < 0){
            error("Error reading trunk");
        }
        fwrite(buffer, 1, r, fp);  //write data into the binary file
        bzero(buffer, BUFFSIZE);
    }
    printf("Compeleted.\n");
    close(sockfd);
    fclose(fp);
    return 0;
}
