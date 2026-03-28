#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include <thread> 
#include "game.h"

using namespace std;

Game* GAME = new Game;


void* player_thread(int socket)
{
    for(;;){

    }
    close(socket);
    exit(0);
}



int main()
{
    pid_t server_id = getpid();
    cout << "ID игры: " << server_id << endl;
    struct sockaddr_in s_addr;

    int sm_socket = socket(AF_INET, SOCK_STREAM, 0);
    int ss_socket = 0; 

    if (sm_socket < 0) {
        cout << "ОШИБКА: НЕ УДАЛОСЬ СОЗДАТЬ ИГРУ!" << endl;
        return -1;
    }

    bzero((char*)&s_addr, sizeof(struct sockaddr_in));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_port = 0;

    if(bind(sm_socket, (sockaddr*)&s_addr, sizeof(struct sockaddr_in)) < 0){
        cout << "ОШИБКА: НЕ УДАЛОСЬ ИНИЦИАЛИЗИРОВАТЬ ИГРУ!" << endl;
        return -1;
    }
    unsigned int s_len = sizeof(struct sockaddr_in);
    if (getsockname(sm_socket, (struct sockaddr*)&s_addr, &s_len) < 0){
        cout << "ОШИБКА: НЕ УДАЛОСЬ НАЙТИ ПОРТ ИГРЫ!" << endl;
        return -1;
    }
    cout << "АДРЕС ИГРЫ: " << inet_ntoa(s_addr.sin_addr) << endl;
    cout << "ПОРТ ИГРЫ: " << ntohs(s_addr.sin_port) << endl;
    if(listen(sm_socket, 6) < 0){
        cout << "ОШИБКА: НЕ УДАЛОСЬ ОТКРЫТЬ ИГРУ!" << endl;
        return -1;
    }
    int status;
    char msg[BUFF_LEN];
    char request[10];
    for(;;)
    {
        status = GAME->getStatus();
        if (status == PRE){
            if (recv(sm_socket, msg, BUFF_LEN, 0) < 0) {
                continue;
            }
            char c;
            do{
                c = sscanf(msg, "%c", &c);
                strncat(request, msg, 1);
            } while(c != '\0' && c != '|');
            if (strncmp(request, "join", 4) == 0){
                char nick[10];
                do{
                    c = sscanf(msg, "%c", &c);
                    strncat(nick, msg, 1);
                } while(c != '\0' && c != '|');
                ss_socket = accept(sm_socket, 0, 0);
                if (ss_socket < 0) {
                    cout << "НЕ УДАЛОСЬ ПРИНЯТЬ ИГРОКА!" << endl;
                    continue;
                }
                GAME->addPlayer(nick);
                thread ct(&player_thread, ss_socket);
                ct.detach();
            }
        }
    }
    close(sm_socket);
    return 0;
}