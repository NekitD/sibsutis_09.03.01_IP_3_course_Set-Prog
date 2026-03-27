#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <unistd.h> 
#include "game.h"

using namespace std;

int main()
{
    struct hostent *hp, *gethostbyname(char*);

    int c_sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in* c_addr = new struct sockaddr_in;
    struct sockaddr_in* s_addr = new struct sockaddr_in;

    char g_host[BUFF_LEN] = "";
    int g_port = 0;

    if(c_sock < 0){
        cout << "НЕ УДАЛОСЬ СОЗДАТЬ КЛИЕНТА!" << endl;
        return -1;
    }

    bzero((char*)c_addr, (sizeof(struct sockaddr_in)));
    c_addr->sin_family = AF_INET;
    c_addr->sin_addr.s_addr = htonl(INADDR_ANY);
    c_addr->sin_port = 0;
    
    if(bind(c_sock, c_addr, sizeof(struct sockaddr_in)) < 0){
        cout << "НЕ УДАЛОСЬ ИНИЦИАЛИЗИРОВАТЬ КЛИЕНТА!" << endl;
        return -1;
    }
    cout << "Input hostname of the game (no spaces): ";
    cin >> g_host;
    cout << endl << "Input port of the game: ";
    cin >> g_port;

    hp = gethostbyname(g_host);

    bzero((char*)s_addr, (sizeof(struct sockaddr_in)));
    bcopy(hp->h_addr, &s_addr->sin_addr, hp->h_length) ;
    s_addr->sin_port = htons(g_port);

        if (connect(c_sock, s_addr, sizeof(struct sockaddr_in)) < 0) {
        printf("CONNECTION TO SERVER FAILED!\n");
        return -1;
    }

    //---------------------------------------------------------------------
    int length = 0;
    char* msg[BUFF_LEN];
    for(;;){
        if ((recv(c_sock, msg, BUFF_LEN, 0) ) < 0)
        { 
            cout << " ОШИБКА СЕРВЕРА! (Invalid server socket)" << endl;
            break;
        }
        

    }

    close(c_sock);
    return 0;
}