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
vector<int>* SUBS = new vector<int>;

void ser_decode_msg(char* msg, int mlen, char* output, char* request);
void send_to_all(vector<int>*, char*, int);

void player_thread(int socket)
{
    int id = 0;
    char s_msg[BUFF_LEN] = "";
    char a_msg[BUFF_LEN] = "";
    //--------------------------------
    char output[BUFF_LEN] = "";
    char request[BUFF_LEN] = "";
    //--------------------------------
    int p_status = WAIT_ACCEPT;
    int rec_l = 0;
    int g_status = GAME->getStatus();
    if (recv(socket, a_msg, BUFF_LEN, 0) <= 0){;
        cout << "Неудачная попытка подключения" << endl;
        close(socket);
    }
    char nick[BUFF_LEN];
    int eon = get_line_b(nick, a_msg, 0, BUFF_LEN, '|');
    get_line_b(request, a_msg, eon, BUFF_LEN, ' ');
    if (strncmp(request, "join", 4) == 0){
        GAME->addPlayer(nick, socket);
        id = GAME->get_player_id(nick);
        if(id < 0){
            cout << "ОШИБКА ID." << endl;
            close(socket);
        }
        strcat(s_msg, "||PRE_TO_PLAY");
        send(socket, s_msg, BUFF_LEN, 0);
        GAME->set_player_status(id, PRE_TO_PLAY);
    }
    for(;;){
        bzero(s_msg, BUFF_LEN);
        bzero(a_msg, BUFF_LEN);
        bzero(output, BUFF_LEN);
        bzero(request, BUFF_LEN);
        p_status = GAME->get_player_status(id);
        g_status = GAME->getStatus();
        rec_l = recv(socket, a_msg, BUFF_LEN, 0);
        if (rec_l == 0){
            GAME->remPlayer(id);
            break;
        }
        if (rec_l < 0){
            cout << "Разрыв соединения с игроком " << GAME->get_player_nick(id) << " из-за ошибки сокета." << endl;
            break;
        }
        ser_decode_msg(a_msg, BUFF_LEN, output, request);

        if(p_status == PRE_TO_PLAY){
            if(strncmp(request, "readytoplay", 12) == 0){
                cout << GAME->get_player_nick(id) << " готов играть!" << endl;
                GAME->set_player_status(id, READY_TO_PLAY);
                send(socket, "||READY_TO_PLAY", BUFF_LEN, 0);
                cout << "Готовы: " << GAME->getRnum() << " / " << GAME->getPnum() << "\n" << endl;
                if (GAME->isGameReady()){
                    GAME->setStatus(START);
                    cout << endl << "Игра начинается!\n" << endl;
                    send_to_all(SUBS, "Игра начинается!|", BUFF_LEN);
                }
            }
            continue;
        }

        if(p_status == WAITING || p_status == READY_TO_PLAY){
            send(socket, " ", 1, 0);
        }

        if(p_status == EMPLOYER){
            if(g_status == JOB_MAKE){
                if(strncmp(request, "sendhist", 9) != 0){
                    strcat(s_msg, "Вы - работодатель!\n");
                    strcat(s_msg, "В Вашей компании открыты следующие вакансии:\n");
                    GAME->PassCards(GAME->get_profs(), GAME->EmployInfo()->getProfs(), EMPLOYER_PROFS_NUM);
                    vector<Card*>* emp_profs = GAME->EmployInfo()->getProfs();
                    for(vector<Card*>::const_iterator pr = emp_profs->begin(); pr != emp_profs->end(); pr++){
                        strcat(s_msg, " ");
                        strcat(s_msg, (*pr)->get_text().c_str());
                        strcat(s_msg, "\n");
                    }
                    strcat(s_msg, "Придумате историю Вашей компании!"); 
                    strcat(s_msg, "|givehist");
                    strcat(s_msg, "|EMPLOYER");
                    send(socket, s_msg, BUFF_LEN, 0);
                    continue;
                }
                GAME->EmployInfo()->setManual((string)output);
                cout << "История:" << endl;
                cout << GAME->EmployInfo()->getManual() << endl;
                GAME->setStatus(P_PRE);
                continue;
            }
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
        if (status == PRE){
            ss_socket = accept(sm_socket, 0, 0);
            SUBS->push_back(ss_socket);
            thread ct(player_thread, ss_socket);
            ct.detach();
            sleep(1);
            continue;
        }
        if(status == START){
            GAME->print_players();
            int emp = GAME->getEmployerId();
            cout << "==============================================================" << endl;
            cout << "Раунд " << GAME->getEmployer() + 1 << ":" << endl;
            cout << "==============================================================" << endl;
            cout << "Работодатель: " << GAME->get_player_nick(emp) << endl;
            cout << "Работодатель придумывает историю своей компании..." << endl;
            GAME->set_player_status(emp, EMPLOYER);
            GAME->setStatus(JOB_MAKE);
            //send_to_all(SUBS, "|update|", BUFF_LEN);
        }
        if(status == OVER){
            GAME->Endgame();
            break;
        }
    }
    close(sm_socket);
    return 0;
}


void send_to_all(vector<int>* s_sockets, char* msg, int mlen){
    for(vector<int>::iterator it = s_sockets->begin(); it != s_sockets->end(); it++){
        send(*it, msg, mlen, 0);
    }
}

// Структкра сообщения (клиент -> сервер) "output|request"
void ser_decode_msg(char* msg, int mlen, char* output, char* request){
    int bc = get_line_b(output, msg, 0, mlen, '|');
    bc = get_line_b(request, msg, bc, mlen, ' ');
}