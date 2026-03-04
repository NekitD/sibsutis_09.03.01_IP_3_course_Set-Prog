#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/time.h>

#define BUFF_LEN 81

int handler(int client_socket) {
    char msg[BUFF_LEN] = "";
    char answer[BUFF_LEN] = "";
    const char ans_b[] = "SERVER: I received - ";
    int msgLength;
    
    bzero(msg, BUFF_LEN);
    bzero(answer, BUFF_LEN);
    
    msgLength = recv(client_socket, msg, BUFF_LEN, 0);
    
    if (msgLength <= 0) {
        if (msgLength == 0) {
            printf("Client on socket %d closed connection\n", client_socket);
        } else {
            printf("Error on socket %d\n", client_socket);
        }
        return -1; 
    }
    
    strcpy(answer, ans_b);
    strncat(answer, msg, msgLength);
    answer[msgLength + strlen(ans_b)] = '\0';
    
    printf("SERVER: socket for client - %d\n", client_socket);
    printf("SERVER: message length - %d\n", msgLength);
    printf("SERVER: message - %s\n\n", msg);
    
    send(client_socket, answer, strlen(answer), 0);
    
    return 0;
}


int main()
{
    struct sockaddr_in server_addr;
    int server_socket = 0;

    fd_set read_ds;
    fd_set active_ds;

    int fd = 0, fd_n = 0, socket_for_client = 0;
    //--------------------------------------------------------
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(server_socket < 0){
        printf("SERVER SOCKET CREATION FAILED!\n");
        return -1;
    }

    bzero((char*)&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = 0;
    
    if(bind(server_socket, &server_addr, sizeof(struct sockaddr_in)) < 0){
        printf("SERVER SOCKET INIT FAILED!\n");
        return -1;
    }

    int length = 0;
    length = sizeof(server_addr);
    if (getsockname(server_socket, &server_addr, &length ))
    {
        printf("FAILED TO GET THE SERVER PORT!\n");
        return -1;
    }
    printf("SERVER: port - %d\n\n", ntohs(server_addr.sin_port ));
    if (listen(server_socket, 5) < 0){
        printf("LISTENING BEGIN FAILED\n");
        return -1;
    }

    //------------------------------------------------------------
    fd_n = getdtablesize();
    FD_ZERO(&active_ds);
    FD_SET(server_socket, &active_ds);

    char msg[BUFF_LEN] = "";
    int msgLength = 0;
    char answer[BUFF_LEN] = "";
    const char ans_b[BUFF_LEN] = "SERVER: I received - ";

    for(;;){
        memcpy(&read_ds, &active_ds, sizeof(fd_set));
        if(select(fd_n, &read_ds, (fd_set*)0, (fd_set*)0, (struct timeval*)0) < 0){
            printf("SELECT ERROR\n");
            return -1;
        }

        if(FD_ISSET(server_socket, &read_ds)){
            socket_for_client = accept(server_socket, 0, 0);
            if (socket_for_client < 0){
                printf("ACCEPT FAILED\n");
                continue;
            }
            FD_SET(socket_for_client, &active_ds);
        }
        int bytes = -1;
        for(fd = 0; fd < fd_n; fd++){
            if(fd != server_socket && FD_ISSET(fd, &read_ds)){
                if(handler(fd) < 0){
                    close(fd);
                    FD_CLR(fd, &active_ds);
                    printf("Socket %d closed and removed\n\n", fd);
                }
            }
        }
    }
    close(server_socket);
    return 0;
}