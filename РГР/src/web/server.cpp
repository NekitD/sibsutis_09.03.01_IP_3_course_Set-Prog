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
    ser_decode_msg(a_msg, BUFF_LEN, output, request);
    if (strncmp(request, "join", 4) == 0){
        GAME->addPlayer(output, socket);
        id = GAME->get_player_id(output);
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
        rec_l = recv(socket, a_msg, BUFF_LEN, 0);
        p_status = GAME->get_player_status(id);
        g_status = GAME->getStatus();
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
                    cout << endl << "Игра начинается!\n" << endl;
                    send_to_all(SUBS, "Игра начинается!|", BUFF_LEN);
                    GAME->setStatus(START);
                }
            }
            continue;
        }

        if(p_status == WAITING || p_status == READY_TO_PLAY){
            send(socket, " ", 1, 0);
            continue;
        }

        if(p_status == EMPLOYER){
            if(g_status == JOB_MAKE){
                if(strncmp(request, "sendhist", 9) != 0){
                    strcat(s_msg, " Вы - работодатель!\n");
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
                GAME->EmployInfo()->print_profs();
                cout << endl;
                send(socket, "||WAITING", BUFF_LEN, 0);
                GAME->set_player_status(id, WAITING);
                GAME->setStatus(P_PRE);
                sleep(1);
                continue;
            }
            continue;
        }

        if(g_status == P_PRE){
            if (p_status == ANSWERING){
                if(strncmp(request, "claim", 6) == 0){
                    int vn = -1;
                    if(strncmp(output, "1", 1) == 0){
                        vn = 0;
                    }else if(strncmp(output, "2", 1) == 0){
                        vn = 1;
                    }else if(strncmp(output, "3", 1) == 0){
                        vn = 2;
                    }
                    cout << "Соискатель "<< GAME->get_player_nick(id) << " претендует на вакансию " 
                        << GAME->EmployInfo()->getProfs()->at(vn)->get_text() << endl;
                    GAME->EmployInfo()->add_claim(vn, id);
                    send(socket, "|claim|ANSWERING", BUFF_LEN, 0);
                    continue;
                }
                if(strncmp(request, "readytoanswer", 14) != 0){ 
                    GAME->PassCards(GAME->get_skills(), GAME->getPlayer(id)->getSkills(), SKILL_NUM);
                    GAME->GiveEmojiToPlayer(GAME->getPlayer(id));
                    vector<Card*>* p_skills = GAME->getPlayer(id)->getSkills();
                    Card* emo = GAME->getPlayer(id)->getEmoji();
                    strcat(s_msg, " Эмоция: ");
                    strcat(s_msg, emo->get_text().c_str());
                    strcat(s_msg, "\n");
                    strcat(s_msg, " Навыки:\n");
                    for(vector<Card*>::const_iterator sk = p_skills->begin(); sk != p_skills->end(); sk++){
                        strcat(s_msg, " - ");
                        strcat(s_msg, (*sk)->get_text().c_str());
                        strcat(s_msg, "\n");
                    } 
                    strcat(s_msg, "|areanswerm");
                    strcat(s_msg, "|ANSWERING");
                    cout << "Соискатель " << GAME->get_player_nick(id) << " готовится отвечать..." << endl;
                    send(socket, s_msg, BUFF_LEN, 0);
                    continue;
                }
                GAME->setStatus(P_MAKE);
                strcat(s_msg, "|giveanswerm");
                strcat(s_msg, "|ANSWERING");
                cout << "Соискатель " << GAME->get_player_nick(id) << " пишет резюме..." << endl;
                send(socket, s_msg, BUFF_LEN, 0);
                continue;
            }
            continue;
        }

        if(g_status == P_MAKE){
            if (p_status == ANSWERING){
                if(strncmp(request, "sendanswer", 14) != 0){
                    continue;
                }
                cout << GAME->get_player_nick(id) << ":" << endl;
                cout << output << endl;
                cout << endl;
                cout << "Время для вопросов." << endl;
                GAME->setStatus(QUESTIONS);
                send(socket, "||WAITING", BUFF_LEN, 0);
                sleep(1);
                continue;
            }
            sleep(1);
            continue;
        }

        if(g_status == QUESTIONS){
            if(id != GAME->get_answering_id()){
                if(strncmp(request, "noquest", 8) == 0){
                    cout << GAME->get_player_nick(id) << " не имеет больше вопросов." << endl;
                    GAME->set_player_status(id, WAITING);
                    if(GAME->no_questions()){
                        GAME->setStatus(P_OPEN);
                    }
                    continue;
                }

                strcat(s_msg, GAME->get_player_nick(id).c_str());
                if(strncmp(request, "quest", 6) == 0){
                    GAME->add_question((string)output);
                    strcat(s_msg, ": |gquest|QUESTIONING");
                }else{
                    strcat(s_msg, ": ||QUESTIONING");
                }
                send(socket, s_msg, BUFF_LEN, 0);
                continue;
            }

            if(strncmp(request, "aquest", 7) == 0){
                cout <<  GAME->get_player_nick(id) << ": " << output << endl;
            }
            cout << endl;
            
            string qu = *(GAME->get_questions()->begin());
            cout << qu << endl;
            strcat(s_msg, qu.c_str());
            strcat(s_msg, "|quest|ANSWERING");
            GAME->rem_question();
            send(socket, s_msg, BUFF_LEN, 0);
            continue;
        }

        if(g_status == SCORES){
            if(id != GAME->get_answering_id()){
                int score = 0;
                if(strncmp(request, "score", 6) == 0){
                    if(strncmp(output, "1", 1) == 0){
                        score = 1;
                    }else if(strncmp(output, "2", 1) == 0){
                        score = 2;
                    }else if(strncmp(output, "3", 1) == 0){
                        score = 3;
                    }else if(strncmp(output, "4", 1) == 0){
                        score = 4;
                    }else if(strncmp(output, "5", 1) == 0){
                        score = 5;
                    }
                    cout << GAME->get_player_nick(id) << " поставил оценку." << endl;
                    GAME->add_scoreb(score);
                    GAME->set_player_status(id, WAITING);
                    if(GAME->score_over()){
                        GAME->getPlayer(GAME->get_answering_id())->addScore(GAME->get_scoreb());
                        cout << GAME->get_player_nick(GAME->get_answering_id()) << " получил " 
                            << GAME->get_scoreb() << " очков!" << endl;
                        GAME->setStatus(JOB_CHOICE);
                        GAME->set_player_status(GAME->getEmployerId(), EMPLOYER);
                    }
                    continue;
                }
                send(socket, "||SCORING", BUFF_LEN, 0);
                continue;
            }
        }

        if(g_status == JOB_CHOICE){
            if(p_status == EMPLOYER){
                if(strncmp(request, "jchoice", 8) == 0){
                    // код для распаковки и обработки выбора
                    //...
                    GAME->set_player_status(id, WAITING);
                    GAME->setEmployer(GAME->getEmployer() + 1);
                    GAME->setStatus(START);
                    continue;
                }
                // упаковать данные для выбора в s_msg
                //...
                strcat(s_msg, "|givejchoice|EMPLOYER");
                send(socket, s_msg, BUFF_LEN, 0);
                continue;
            }
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
            if(emp >= GAME->get_players()->size()){
                GAME->setStatus(OVER);
                continue;
            }
            cout << "==============================================================" << endl;
            cout << "Раунд " << GAME->getEmployer() + 1 << ":" << endl;
            cout << "==============================================================" << endl;
            cout << "Работодатель: " << GAME->get_player_nick(emp) << endl;
            cout << "Работодатель придумывает историю своей компании..." << endl;
            GAME->set_player_status(emp, EMPLOYER);
            GAME->setStatus(JOB_MAKE);
        }
        if (status == P_PRE){
            GAME->set_answering_num(1);
            GAME->set_player_status(GAME->get_answering_id(), ANSWERING);
            continue;
        }
        if(status == P_OPEN){
            GAME->open_p(GAME->get_answering_id());
            GAME->set_scoreb(0);
            GAME->setStatus(SCORES);
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


void send_to_all(vector<int>* s_sockets, char* msg, int mlen){
    for(vector<int>::iterator it = s_sockets->begin(); it != s_sockets->end(); it++){
        send(*it, msg, mlen, 0);
    }
}

// Структкра сообщения (клиент -> сервер) "output|request"
void ser_decode_msg(char* msg, int mlen, char* output, char* request){
    bzero(output, mlen);
    bzero(request, mlen);
    int bc = get_line_b(output, msg, 0, mlen, '|');
    bc = get_line_b(request, msg, bc, mlen, ' ');
}