#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <unistd.h> 

#define BUFF_LEN 81

int main()
{
    double i = 0;
    int client_socket = 0;
    char hostname[80];
    int port = 0;
    char msg[BUFF_LEN] = "";
    char s_msg[BUFF_LEN] = "";

    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    struct hostent *hp, *gethostbyname();

    //---------------------------------------------------------------------
    
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(client_socket < 0){
        printf("CLIENT SOCKET CREATION FAILED!\n");
        return -1;
    }

    bzero((char*)&client_addr, (sizeof(struct sockaddr_in)));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_addr.sin_port = 0;
    
    if(bind(client_socket, &client_addr, sizeof(struct sockaddr_in)) < 0){
        printf("CLIENT SOCKET INIT FAILED!\n");
        return -1;
    }
    //---------------------------------------------------------------------
    printf("HELLO USER! LET'S HAVE SOME FUN \n");
    printf("Input some value: ");
    scanf("%lf", &i);
    sprintf(msg, "%.2f", i);
    printf("\nInput hostname (no spaces): ");
    scanf("%s", &hostname);
    printf("\nInput port: ");
    scanf("%d", &port);
    //---------------------------------------------------------------------

    bzero((char*)&server_addr, (sizeof(struct sockaddr_in)));
    server_addr.sin_family = AF_INET;
    hp = gethostbyname(hostname);
    if (hp == NULL) {
        printf("Cannot resolve hostname: %s\n", hostname);
        close(client_socket);
        return -1;
    }
    bcopy(hp->h_addr, &server_addr.sin_addr, hp->h_length) ;
    server_addr.sin_port = htons(port);

    if (connect(client_socket, &server_addr, sizeof(struct sockaddr_in)) < 0) {
        printf("CONNECTION TO SERVER FAILED!\n");
        return -1;
    }

    //---------------------------------------------------------------------
    int length = 0;
    for(;;){
        if(send(client_socket, msg, BUFF_LEN, 0) < 0)
            {
                printf("SEND TO SERVER FAILED!\n");
                break;
            }
        printf("%.2f was sent.\n", i);

        length = sizeof(server_addr) ;
        bzero(s_msg, sizeof(BUFF_LEN) );
        if ((recv(client_socket, s_msg, BUFF_LEN, 0) ) < 0)
        { 
            printf("Invalid server socket.\n");
            break;
        } else {
            printf( "Server answered -  %s\n", s_msg);
        }
        sleep(i);
    }

    close(client_socket);
    return 0;
}