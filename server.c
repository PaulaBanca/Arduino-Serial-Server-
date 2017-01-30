/*
    C socket server example
*/
 
#include<stdio.h>
#include<string.h>   // String function definitions
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write  // for usleep()

#include <stdlib.h>
#include <getopt.h>

#include "arduino-serial-lib.h"

void usage(void)
{
    printf("Usage: server -b <bps> -p <serialport> [OPTIONS]\n"
    "\n"
    "Options:\n"
    "  -h, --help                 Print this help message\n"
    "  -b, --baud=baudrate        Baudrate (bps) of Arduino (default 9600)\n"
    "  -a, --arduino=serialport   Serial port Arduino is connected to\n"
    "  -p, --serverport=port      The port to listen to connections on\n"
    "\n"
    "Note: Order is important. Set '-b' baudrate before opening port'-p'. \n"
    "\n");
    exit(EXIT_SUCCESS);
}

void error(char* msg) {
    fprintf(stderr, "%s\n",msg);
    exit(EXIT_FAILURE);
}

int startArduino(char *serialport, int baudrate) {
    const int buf_max = 256;

    int fd = -1;
    char eolchar = '\n';
    int timeout = 5000;
    char buf[buf_max];
    int rc,n;
    
    fd = serialport_init(serialport, baudrate);
    if( fd==-1 ) error("couldn't open port");
    printf("opened port %s\n",serialport);
    serialport_flush(fd);

    return fd;
}
 
int startServer(uint16_t port, int arduinofd) {

    int socket_desc , client_sock , c , read_size, rc;
    struct sockaddr_in server , client;
    char client_message[2000];
    char log_file_path[100];
    char *message;
     
    FILE *fp;
    char *env_p = getenv("HOME");
    if (!env_p) {
        perror("HOME environment variable not found");
        return EXIT_FAILURE;     
    }
    strcat(log_file_path,env_p);
    strcat(log_file_path,"/arduino_server_log.txt");
    fp = fopen(log_file_path, "w+");
    if(!fp) {
        perror("File opening failed");
        return EXIT_FAILURE;
    }
 
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1) {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server.sin_port = htons(port);
     
    //Bind
    if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);

    if (client_sock < 0) {
        perror("accept failed");
        return 1;
    }
    
    char *client_ip = inet_ntoa(client.sin_addr);
    int client_port = ntohs(client.sin_port);
    
    puts("Connection accepted");
     
    //Receive a message from client
    while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 ) {
        printf("Forwarding to Arduino: %c\n", client_message[0]);
        rc = serialport_write(arduinofd, client_message);
        fprintf(fp, "Recieved %s\n",client_message);
        fflush(fp);
        if(rc==-1) error("error writing");
        //Send the message back to client
        // if (write(client_sock , client_message , strlen(client_message)) < 0){
        //     perror("write failed");
        // }
    }
     
    if(read_size == 0) {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1) {
        perror("recv failed");
    }
    fclose(fp);
}

int main(int argc , char *argv[]) {
    const int buf_max = 256;

    char serialport[buf_max];
    int baudrate = 9600;  // default
    uint16_t serverport;

    if (argc==1) {
        usage();
    }

    /* parse options */
    int option_index = 0, opt;
    static struct option loptions[] = {
        {"help",       no_argument,       0, 'h'},
        {"arduino",    required_argument, 0, 'a'},
        {"serverport", required_argument, 0, 'p'},
        {"baud",       required_argument, 0, 'b'},
        {NULL,         0,                 0, 0}
    };

    while(1) {
        opt = getopt_long (argc, argv, "hp:b:a:",
                           loptions, &option_index);
        if (opt==-1) break;
        switch (opt) {
        case '0': break;
        case 'h':
            usage();
            break;
        case 'p':
            serverport=strtol(optarg,NULL,10);
            break;
        case 'b':
            baudrate = strtol(optarg,NULL,10);
            break;
        case 'a':
            strcpy(serialport,optarg);
            break;
        }
    }
    return startServer(serverport,startArduino(serialport,baudrate));
}
