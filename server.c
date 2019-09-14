#include<stdio.h>
#include<stdlib.h> //atoi()
#include<arpa/inet.h> //sockaddr_in
#include<sys/socket.h>
#include<string.h> //bzero()
#include<sys/stat.h>
#include<unistd.h> //write()

#define BUFFSIZE 256 //buffer size

/*print error message and stop program*/
void error(char *msg) {
    perror(msg);
    exit(1);
}

/*convert a long integer to string*/
char *to_string(long int a) {
    static char tmp[BUFFSIZE]="", str[BUFFSIZE]="";
    int i=0,j;
    while(a!=0){
        tmp[i]=a%10+'0';
        a/=10;
        i++;
    }
    for(i-=1,j=0;i>-1;--i,++j){
        str[j]=tmp[i];
    }
    return str;
}


/* argv[1] is port, argv[2] is file name */
int main(int argc, char *argv[]) {
    int port = atoi(argv[1]);
    char *filename = argv[2];
    if(strlen(filename) > BUFFSIZE) {
        printf("File name must be shortter than %d words.\n", BUFFSIZE);
        return 0;
    }
    int sockfd, cli_sockfd;                 //server sockfd, client sockfd
    struct sockaddr_in serv_addr, cli_addr; //server address, client address
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0)
        error("Cannot open socket");
    bzero((char*)&serv_addr, sizeof(serv_addr)); //initialize
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) { //bind to server
        error("Cannot Bind");
    }
    printf("Waiting for receiver...\n");
    listen(sockfd, 5); //max amount of client
    socklen_t clilen = sizeof(cli_addr);        //a special type to store size of cli_addr
    cli_sockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
    if(cli_sockfd < 0){
        error("Error on acceptance");
    }
    
    FILE *fp;       //file to transfer
    struct stat st; //file status, which can get file size
    stat(filename, &st);
    unsigned char buffer[BUFFSIZE];
    int wr,len;     //return value of write(), return value of fread()
    double fsiz;    //file size [KB]
    fp = fopen(filename, "rb"); //open as a binary file
    if(!fp) {
        error("Cannot open file");
    }
    wr = write(cli_sockfd, filename, strlen(filename)); //send file name to client
    if(wr < 0){
        error("Error sending file name");
    }
    usleep(10);
    wr = write(cli_sockfd, to_string(st.st_size), BUFFSIZE);   //send file size to client
    if(wr < 0){
        error("Error sending file size");
    }
    printf("Connect successfully. Ready to transfer \'%s\'[%.2lf KB]...\n", filename, (double)st.st_size/1024);
    
    while(1) {
        usleep(100);
        len = fread(buffer, 1, BUFFSIZE, fp);		    //read data   
        if(len > 0){
            write(cli_sockfd, buffer, len);
        }
        if(len < BUFFSIZE){     //check reading end of file normally and break while
            if(feof(fp)){
                printf("Transfer completed.\n");
            }
            if(ferror(fp)){
                printf("Error reading file.\n");
            }
            break;
        }
        bzero(buffer, BUFFSIZE);
    }
    close(sockfd);
    fclose(fp);
    return 0;
}
