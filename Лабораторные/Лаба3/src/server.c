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

#define BUFF_LEN 81

pthread_mutex_t mut;

char msg[BUFF_LEN] = "";
int msgLength = 0;
char ans_b[BUFF_LEN] = "SERVER: I received - ";
char answer[BUFF_LEN] = "";

void* client_thread(int socket_for_client){
    for( ; ; ) {
        pthread_mutex_lock(&mut);
        bzero(msg, sizeof(BUFF_LEN));
        bzero(answer, sizeof(BUFF_LEN));
        if ( (msgLength = recv(socket_for_client, msg, BUFF_LEN, 0) ) < 0 )
        { 
            printf("Invalid client socket.\n");
            break;
        }
        strcat(answer, ans_b);
        strcat(answer, msg);
        printf("SERVER: socket for client - %d\n", socket_for_client) ;
        printf("SERVER: message length - %d\n", msgLength);
        printf("SERVER: message - %s\n\n", msg);
        send(socket_for_client, answer, BUFF_LEN, 0);
        pthread_mutex_unlock(&mut);
    }
    close(socket_for_client);
    return;
}


int main()
{
    struct sockaddr_in server_addr;
    pthread_t ct;
    int server_socket = 0;
    int socket_for_client = 0;

    pthread_mutex_init(&mut, 0);

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
    //------------------------------------------------------------
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

    for( ; ; ) {
        socket_for_client = accept(server_socket, 0, 0);
        if (socket_for_client < 0) {
            printf("ACCEPT FAILED\n");
            return -1;
        }
        if (pthread_create(&ct, NULL, &client_thread, (void*)socket_for_client) < 0){
            printf("THREAD CREATION ERROR");
            return -1;
        }
        pthread_join(ct, NULL);
    }
    close(server_socket);
    pthread_mutex_destroy(&mut);
    return 0;
}