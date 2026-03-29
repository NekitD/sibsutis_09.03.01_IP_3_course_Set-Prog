// СТАРТАП-СЕРВЕР
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
#include <cstring>

using namespace std;

Game* GAME = new Game;

void player_thread(int socket)
{
    int id = 0;
    char s_msg[BUFF_LEN] = "";
    char a_msg[BUFF_LEN] = "";
    char request[BUFF_LEN];
    int p_status = WAIT_ACCEPT;
    int rec_l = 0;
    int g_status = PRE;
    for(;;){
        bzero(s_msg, BUFF_LEN);
        bzero(a_msg, BUFF_LEN);
        g_status = GAME->getStatus();
        rec_l = recv(socket, a_msg, BUFF_LEN, 0);
        if (rec_l == 0){
            GAME->remPlayer(id);
            break;
        }
        if (rec_l < 0){
            cout << "Разрыв соединения с игроком" << GAME->get_player_nick(id) << " из-за ошибки сокета." << endl;
            break;
        }
        if(p_status == WAIT_ACCEPT){
            if(g_status == FULL){
                send(socket, "FULL", BUFF_LEN, 0);
                break;
            }
            char nick[BUFF_LEN];
            int eon = get_line_b(nick, a_msg, 0, BUFF_LEN, '|');
            get_line_b(request, a_msg, eon, BUFF_LEN, ' ');
            if (strncmp(request, "join", 4) == 0){
                GAME->addPlayer(nick);
                id = GAME->get_player_id(nick);
                if(id < 0){
                    cout << "ОШИБКА ID." << endl;
                    break;
                }
                strcat(s_msg, "accepted");
                send(socket, s_msg, BUFF_LEN, 0);
                p_status = PRE_TO_PLAY;
            }
            continue;
        }
        if(p_status = PRE_TO_PLAY){
            if (g_status == START){
                send(socket, "Игра начинается!|", BUFF_LEN, 0);
                p_status = WAITING;
                continue;
            }
            strncpy(request, a_msg, 12);
            if(strncmp(request, "readytoplay", 12) == 0){
                cout << GAME->get_player_nick(id) << " готов играть!" << endl;
                GAME->set_player_status(id, READY_TO_PLAY);
                if (GAME->isGameReady()){
                    GAME->setStatus(START);
                }
            }
            continue;
        }
        if (p_status == WAITING){
            continue;
        }
    }
    close(socket);
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

    if(listen(sm_socket, MAX_P) < 0){
        cout << "ОШИБКА: НЕ УДАЛОСЬ ОТКРЫТЬ ИГРУ!" << endl;
        return -1;
    }
    cout << endl << "Игроков: " << GAME->getPnum() << " / {" << MIN_P << " - " << MAX_P << "}" << endl;
    cout << "Готовы: " << GAME->getRnum() << " / " << GAME->getPnum() << "\n" << endl;
    GAME->setStatus(PRE);
    int status;
    for(;;)
    {
        status = GAME->getStatus();
        if (status == PRE || status == FULL){
            ss_socket = accept(sm_socket, 0, 0);
            thread ct(player_thread, ss_socket);
            ct.detach();
            if (status == PRE && GAME->getPnum() == MAX_P){
                GAME->setStatus(FULL);
            }
            continue;
        }
        if(status == OVER){
            GAME->Endgame();
            break;
        }
    }
    close(sm_socket);
    return 0;
}