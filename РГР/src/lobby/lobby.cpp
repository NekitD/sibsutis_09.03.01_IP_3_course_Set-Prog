// СТАРТАП-ЛОББИ (ИСХОДНЫЙ КОД)

#include "lobby.h"

void* player_thread(void* arg)
{
    player_args* pargs = (player_args*)arg;
    int socket = pargs->socket;
    Game* GAME = pargs->game;
    vector<int>* SUBS = pargs->subs;
    int lobby_id = pargs->lobby_id;
    StartupDbContext* CONTEXT = pargs->context;
    pthread_mutex_t* mutex = pargs->mutex;

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
    if (recv(socket, a_msg, BUFF_LEN, 0) <= 0){
        strcat(s_msg, "   Неудачная попытка подключения|common|");
        pthread_mutex_lock(mutex);
        send_to_all(SUBS, s_msg, BUFF_LEN);
        pthread_mutex_unlock(mutex);
        close(socket);
        pthread_exit(0);
    }
    ser_decode_msg(a_msg, BUFF_LEN, output, request);
    if (strncmp(request, "join", 4) == 0){
        strcat(s_msg, GAME->addPlayer(output, socket).c_str());
        strcat(s_msg, "|common|");
        pthread_mutex_lock(mutex);
        send_to_all(SUBS, s_msg, BUFF_LEN);
        pthread_mutex_unlock(mutex);
        bzero(s_msg, BUFF_LEN);
        id = GAME->get_player_id(output);
        if(id < 0){
            strcat(s_msg, "   ОШИБКА ID.|common|");
            pthread_mutex_lock(mutex);
            send_to_all(SUBS, s_msg, BUFF_LEN);
            pthread_mutex_unlock(mutex);
            close(socket);
            pthread_exit(0);
        }
        pthread_mutex_lock(mutex);
        CONTEXT->set_lobby_num(lobby_id, GAME->getPnum());
        strcat(s_msg, "|getready|PRE_TO_PLAY");
        send(socket, s_msg, BUFF_LEN, 0);
        GAME->set_player_status(id, PRE_TO_PLAY);
        pthread_mutex_unlock(mutex);
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
            pthread_mutex_lock(mutex);
            strcat(s_msg, GAME->remPlayer(id).c_str());
            strcat(s_msg, "|common|");
            CONTEXT->set_lobby_num(lobby_id, GAME->getPnum());
            for(vector<int>::iterator itd = SUBS->begin(); itd != SUBS->end(); itd++){
                if(*itd == id){
                    SUBS->erase(itd);
                    break;
                }
            }
            send_to_all(SUBS, s_msg, BUFF_LEN);
            pthread_mutex_unlock(mutex);
            break;
        }
        if (rec_l < 0){
            sprintf(s_msg, "   Разрыв соединения с игроком %s из-за ошибки сокета.|common|", GAME->get_player_nick(id).c_str());
            pthread_mutex_lock(mutex);
            GAME->remPlayer(id);
            for(vector<int>::iterator itd = SUBS->begin(); itd != SUBS->end(); itd++){
                if(*itd == id){
                    SUBS->erase(itd);
                    break;
                }
            }
            pthread_mutex_unlock(mutex);
            break;
        }
        ser_decode_msg(a_msg, BUFF_LEN, output, request);

        if(p_status == PRE_TO_PLAY){
            if(strncmp(request, "readytoplay", 12) == 0){
                pthread_mutex_lock(mutex);
                SUBS->push_back(socket);
                GAME->set_player_status(id, READY_TO_PLAY);
                sprintf(s_msg, "   %s готов играть!\n   Готовы: %d / %d\n|common|", GAME->get_player_nick(id).c_str(), 
                                            GAME->getRnum(), GAME->getMnum());
                send_to_all(SUBS, s_msg, BUFF_LEN);
                pthread_mutex_unlock(mutex);
                bzero(s_msg, BUFF_LEN);
                strcat(s_msg, "||READY_TO_PLAY");
                send(socket, s_msg, BUFF_LEN, 0);
                if (GAME->isGameReady()){
                    bzero(s_msg, BUFF_LEN);
                    strcat(s_msg, "\n======================================\n");
                    strcat(s_msg, "       ИГРА НАЧИНАЕТСЯ!");
                    strcat(s_msg, "\n======================================\n");
                    strcat(s_msg, "|common|");
                    pthread_mutex_lock(mutex);
                    send_to_all(SUBS, s_msg, BUFF_LEN);
                    GAME->setStatus(START);
                    CONTEXT->set_lobby_status(lobby_id, true);
                    pthread_mutex_unlock(mutex);
                }
            }
            continue;
        }

        if(p_status == WAITING || p_status == READY_TO_PLAY){
            //send(socket, "EmptyMes", BUFF_LEN, 0);
            continue;
        }

        if(g_status == JOB_MAKE){
            if(p_status == EMPLOYER){
                if(strncmp(request, "sendhist", 9) != 0){
                    strcat(s_msg, "        Вы - работодатель!\n");
                    strcat(s_msg, " В Вашей компании открыты следующие вакансии:\n");
                    pthread_mutex_lock(mutex);
                    GAME->PassCards(GAME->get_profs(), GAME->EmployInfo()->getProfs(), EMPLOYER_PROFS_NUM);
                    pthread_mutex_unlock(mutex);
                    vector<Card*>* emp_profs = GAME->EmployInfo()->getProfs();
                    for(vector<Card*>::const_iterator pr = emp_profs->begin(); pr != emp_profs->end(); pr++){
                        strcat(s_msg, "     ");
                        strcat(s_msg, (*pr)->get_text().c_str());
                        strcat(s_msg, "\n");
                    }
                    strcat(s_msg, "     Придумате историю Вашей компании!"); 
                    strcat(s_msg, "|givehist");
                    strcat(s_msg, "|EMPLOYER");
                    send(socket, s_msg, BUFF_LEN, 0);
                    continue;
                }
                pthread_mutex_lock(mutex);
                GAME->EmployInfo()->setManual((string)output);
                pthread_mutex_unlock(mutex);
                strcat(s_msg, "\n   История:\n");
                strcat(s_msg, GAME->EmployInfo()->getManual().c_str());
                strcat(s_msg, "\n");
                strcat(s_msg, GAME->EmployInfo()->print_profs().c_str());
                strcat(s_msg, "|common|");
                pthread_mutex_lock(mutex);
                send_to_all_exept(SUBS, socket, s_msg, BUFF_LEN);
                pthread_mutex_unlock(mutex);
                bzero(s_msg, BUFF_LEN);
                send(socket, "||WAITING", BUFF_LEN, 0);
                pthread_mutex_lock(mutex);
                GAME->set_player_status(id, WAITING);
                GAME->setStatus(P_PRE);
                pthread_mutex_unlock(mutex);
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
                    sprintf(s_msg, "   Соискатель %s претендует на вакансию %s|common|", 
                        GAME->get_player_nick(id).c_str(), GAME->EmployInfo()->getProfs()->at(vn)->get_text().c_str());
                    pthread_mutex_lock(mutex);
                    send_to_all_exept(SUBS, socket, s_msg, BUFF_LEN);
                    GAME->EmployInfo()->add_claim(vn, id);
                    pthread_mutex_unlock(mutex);
                    send(socket, "|claim|ANSWERING", BUFF_LEN, 0);
                    continue;
                }
                if(strncmp(request, "readytoanswer", 14) != 0){ 
                    pthread_mutex_lock(mutex);
                    GAME->PassCards(GAME->get_skills(), GAME->getPlayer(id)->getSkills(), SKILL_NUM);
                    GAME->GiveEmojiToPlayer(GAME->getPlayer(id));
                    pthread_mutex_unlock(mutex);
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
                    send(socket, s_msg, BUFF_LEN, 0);
                    bzero(s_msg, BUFF_LEN);
                    sprintf(s_msg, "   Соискатель %s готовится отвечать...\n|common|", GAME->get_player_nick(id).c_str());
                    pthread_mutex_lock(mutex);
                    send_to_all_exept(SUBS, socket, s_msg, BUFF_LEN);
                    pthread_mutex_unlock(mutex);
                    continue;
                }
                pthread_mutex_lock(mutex);
                GAME->setStatus(P_MAKE);
                pthread_mutex_unlock(mutex);
                strcat(s_msg, "|giveanswerm");
                strcat(s_msg, "|ANSWERING");
                send(socket, s_msg, BUFF_LEN, 0);
                bzero(s_msg, BUFF_LEN);
                sprintf(s_msg, "   Соискатель %s пишет резюме...\n|common|", GAME->get_player_nick(id).c_str());
                pthread_mutex_lock(mutex);
                send_to_all_exept(SUBS, socket, s_msg, BUFF_LEN);
                pthread_mutex_unlock(mutex);
                continue;
            }
            continue;
        }

        if(g_status == P_MAKE){
            if (p_status == ANSWERING){
                if(strncmp(request, "sendanswer", 14) != 0){
                    continue;
                }
                sprintf(s_msg, "   %s:", GAME->get_player_nick(id).c_str());
                strcat(s_msg, "   ");
                strcat(s_msg, output);
                strcat(s_msg, "\n\n   Время для вопросов.|common|");
                pthread_mutex_lock(mutex);
                send_to_all_exept(SUBS, socket, s_msg, BUFF_LEN);
                pthread_mutex_unlock(mutex);
                pthread_mutex_lock(mutex);
                GAME->setStatus(QUESTIONS);
                vector<Player*>* tmq = GAME->get_players();
                for(vector<Player*>::iterator pl = tmq->begin(); pl != tmq->end(); pl++){
                    if((*pl)->get_id() != GAME->get_answering_id()){
                        (*pl)->setStatus(QUESTIONING);
                    }
                }
                pthread_mutex_unlock(mutex);
                bzero(s_msg, BUFF_LEN);
                send(socket, "\n\n   Время для вопросов.||WAITING", BUFF_LEN, 0);
                sleep(1);
                continue;
            }
            sleep(1);
            continue;
        }

        if(g_status == QUESTIONS){
            if(GAME->get_player_status(id) == QUESTIONING){
                if(strncmp(request, "noquest", 8) == 0){
                    sprintf(s_msg, "   %s не имеет больше вопросов.|common|", GAME->get_player_nick(id).c_str());
                    pthread_mutex_lock(mutex);
                    send_to_all_exept(SUBS, socket, s_msg, BUFF_LEN);
                    GAME->set_player_status(id, WAITING);
                    pthread_mutex_unlock(mutex);
                    bzero(s_msg, BUFF_LEN);
                    send(socket, "||WAITING", BUFF_LEN, 0);
                    if(GAME->no_questions()){
                        pthread_mutex_lock(mutex);
                        GAME->set_player_status(GAME->get_answering_id(), WAITING);
                        GAME->setStatus(P_OPEN);
                        pthread_mutex_unlock(mutex);
                    }
                    continue;
                }

                strcat(s_msg, GAME->get_player_nick(id).c_str());
                if(strncmp(request, "quest", 6) == 0){
                    pthread_mutex_lock(mutex);
                    GAME->add_question((string)output);
                    pthread_mutex_unlock(mutex);
                    strcat(s_msg, ": |gquest|QUESTIONING");
                }else{
                    strcat(s_msg, ": ||QUESTIONING");
                }
                send(socket, s_msg, BUFF_LEN, 0);
                continue;
            }

            if(strncmp(request, "aquest", 7) == 0){
                strcat(s_msg, "    ");
                strcat(s_msg, GAME->get_player_nick(id).c_str());
                strcat(s_msg, ":");
                strcat(s_msg, output);
                strcat(s_msg, "\n\n|common|");
                pthread_mutex_lock(mutex);
                send_to_all_exept(SUBS, socket, s_msg, BUFF_LEN);
                GAME->rem_question();
                pthread_mutex_lock(mutex);
                bzero(s_msg, BUFF_LEN);
                if(GAME->no_questions()){
                    pthread_mutex_lock(mutex);
                    GAME->set_player_status(GAME->get_answering_id(), WAITING);
                    GAME->setStatus(P_OPEN);
                    pthread_mutex_unlock(mutex);
                    continue;
                }
            }
            
            if(!(GAME->get_questions()->empty())){
                string qu = *(GAME->get_questions()->begin());
                sprintf(s_msg, "   %s\n|common|", qu.c_str());
                pthread_mutex_lock(mutex);
                send_to_all(SUBS, s_msg, BUFF_LEN);
                pthread_mutex_unlock(mutex);
                bzero(s_msg, BUFF_LEN);
                strcat(s_msg, "|quest|ANSWERING");
                send(socket, s_msg, BUFF_LEN, 0);
                continue;
            }
            send(socket, "||WAITING", BUFF_LEN, 0);
        }

        if(g_status == SCORES){
            if(GAME->get_player_status(id) == SCORING){
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
                    sprintf(s_msg, "   %s поставил оценку.\n|common|", GAME->get_player_nick(id).c_str());
                    pthread_mutex_lock(mutex);
                    send_to_all(SUBS, s_msg, BUFF_LEN);
                    pthread_mutex_unlock(mutex);
                    bzero(s_msg, BUFF_LEN);
                    pthread_mutex_lock(mutex);
                    GAME->add_scoreb(score);
                    GAME->set_player_status(id, WAITING);
                    pthread_mutex_unlock(mutex);
                    if(GAME->score_over()){
                        GAME->getPlayer(GAME->get_answering_id())->addScore(GAME->get_scoreb());

                        sprintf(s_msg, "\n   %s получил %d очков!|common|", GAME->get_player_nick(GAME->get_answering_id()).c_str()
                            ,GAME->get_scoreb() );
                        pthread_mutex_lock(mutex);
                        send_to_all(SUBS, s_msg, BUFF_LEN);
                        GAME->set_answering_num(GAME->get_answering_num() + 1);
                        pthread_mutex_unlock(mutex);
                        bzero(s_msg, BUFF_LEN);
                        pthread_mutex_lock(mutex);
                        if(GAME->get_answering_id() == GAME->getEmployerId()){
                            GAME->setStatus(JOB_CHOICE);
                            GAME->set_player_status(GAME->getEmployerId(), EMPLOYER);
                        }else{
                            GAME->setStatus(P_PRE);
                        }
                        pthread_mutex_unlock(mutex);
                    }
                    send(socket, "||WAITING", BUFF_LEN, 0);
                    continue;
                }
                send(socket, "||SCORING", BUFF_LEN, 0);
                continue;
            }
        }

        if(g_status == JOB_CHOICE){
            if(p_status == EMPLOYER){
                if(strncmp(request, "jchoice", 8) == 0){
                    // формат распаковки выбора: "1:2,2:1,3:3"
                    char* token = strtok(output, ",");
                    while (token != NULL) {
                        int vac_num, player_id;
                        sscanf(token, "%d:%d", &vac_num, &player_id);
                        pthread_mutex_lock(mutex);
                        GAME->EmployInfo()->add_assignment(vac_num - 1, player_id); 
                        pthread_mutex_unlock(mutex);
                        token = strtok(NULL, ",");
                    }
                    strcat(s_msg, "\n");
                    strcat(s_msg, GAME->assign_professions().c_str());
                    strcat(s_msg, "|common|");
                    send_to_all(SUBS, s_msg, BUFF_LEN);
                    pthread_mutex_lock(mutex);
                    GAME->set_player_status(id, WAITING);
                    GAME->setEmployer(GAME->getEmployer() + 1);
                    GAME->setStatus(START);
                    pthread_mutex_unlock(mutex);
                    send(socket, "||WAITING", BUFF_LEN, 0);
                    sleep(1);
                    continue;
                }
        
                bzero(s_msg, BUFF_LEN);
                vector<Card*>* vacancies = GAME->EmployInfo()->getProfs();
        
                for (size_t i = 0; i < vacancies->size(); i++) {
                    char buffer[256];
                    sprintf(buffer, "   Вакансия %ld: %s\n", i+1, vacancies->at(i)->get_text().c_str());
                    strcat(s_msg, buffer);
            
                    vector<int>* claimants = GAME->EmployInfo()->get_claims_for_vacancy(i);
                    strcat(s_msg, " Претенденты: ");
                    if (claimants && !claimants->empty()) {
                        for (size_t j = 0; j < claimants->size(); j++) {
                            char id_str[10];
                            sprintf(id_str, "%d", claimants->at(j));
                            strcat(s_msg, id_str);
                            strcat(s_msg, ") ");
                            strcat(s_msg, GAME->get_player_nick(claimants->at(j)).c_str());
                            if (j < claimants->size() - 1) strcat(s_msg, ", ");
                        }
                    } else {
                        strcat(s_msg, "нет");
                    }
                    strcat(s_msg, "\n\n");
                }
                sprintf(s_msg, "   Работодатель %s выбирает сотрудников...\n|common|", 
                    GAME->get_player_nick(GAME->getEmployerId()).c_str());
                pthread_mutex_lock(mutex);
                send_to_all(SUBS, s_msg, BUFF_LEN);
                pthread_mutex_unlock(mutex);
                bzero(s_msg, BUFF_LEN);
                strcat(s_msg, " Введите выбор в формате: номер_вакансии:номер_игрока (через запятую)\n");
                strcat(s_msg, " Пример: 1:2,2:1,3:3");
                strcat(s_msg, "|givejchoice|EMPLOYER");
                send(socket, s_msg, BUFF_LEN, 0);
                sleep(1);
                continue;
            }
        }
    }
    close(socket);
    pthread_exit(0);
}

void* lobby_thread(void* arg)
{

    int sm_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (sm_socket < 0) {
        cout << "   ОШИБКА: НЕ УДАЛОСЬ СОЗДАТЬ ЛОББИ!" << endl;
        close(sm_socket);
        pthread_exit(0);
    }
    struct sockaddr_in s_addr;

    bzero((char*)&s_addr, sizeof(struct sockaddr_in));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_port = 0;
    if(bind(sm_socket, (sockaddr*)&s_addr, sizeof(struct sockaddr_in)) < 0){
        cout << "   ОШИБКА: НЕ УДАЛОСЬ ИНИЦИАЛИЗИРОВАТЬ ЛОББИ!" << endl;
        close(sm_socket);
        pthread_exit(0);
    }
    unsigned int s_len = sizeof(struct sockaddr_in);
    if (getsockname(sm_socket, (struct sockaddr*)&s_addr, &s_len) < 0){
        cout << "   ОШИБКА: НЕ УДАЛОСЬ НАЙТИ ПОРТ ЛОББИ!" << endl;
        close(sm_socket);
        pthread_exit(0);
    }

    lobby_args* largs = (lobby_args*)arg;
    int lobby_id = largs->id;
    int lobby_size = largs->size;
    StartupDbContext* CONTEXT = largs->context;
    CONTEXT->set_lobby_port(lobby_id, ntohs(s_addr.sin_port));
    pthread_mutex_t gmutex;
    pthread_mutex_init(&gmutex, 0);

    vector<int>* SUBS = new vector<int>;
    Game* GAME = new Game(lobby_size);

    srand(time(NULL));

    int ss_socket = 0; 

    if(listen(sm_socket, lobby_size) < 0){
        cout << "   ОШИБКА: НЕ УДАЛОСЬ ОТКРЫТЬ ЛОББИ!" << endl;
        pthread_mutex_destroy(&gmutex);
        close(sm_socket);
        pthread_exit(0);
    }

    GAME->setStatus(PRE);
    int status;
    char s_msg[BUFF_LEN];
    for(;;)
    {
        status = GAME->getStatus();
        if (status == PRE){
            ss_socket = accept(sm_socket, 0, 0);
            pthread_t thread_id;
            player_args pargs;
            pargs.game = GAME;
            pargs.subs = SUBS;
            pargs.socket = ss_socket;
            pargs.context = CONTEXT;
            pargs.lobby_id = lobby_id;
            pargs.mutex = &gmutex;
            pthread_create(&thread_id, NULL, player_thread, (void*)&pargs);
            pthread_detach(thread_id);
            sleep(1);
            continue;
        }
        if(status == START){
            if((size_t)GAME->getEmployer() >= GAME->get_players()->size()){
                pthread_mutex_lock(&gmutex);
                GAME->setStatus(OVER);
                pthread_mutex_unlock(&gmutex);
                continue;
            }
            bzero(s_msg, BUFF_LEN);
            //GAME->print_players();
            pthread_mutex_lock(&gmutex);
            GAME->drop_cards();
            pthread_mutex_unlock(&gmutex);
            int emp = GAME->getEmployerId();
            sprintf(s_msg, "%s==============================================================\n       Раунд %d:\n==============================================================\n   Работодатель: %s\n   Работодатель придумывает историю своей компании...|common|\n",
                GAME->print_players().c_str(), GAME->getEmployer() + 1, GAME->get_player_nick(emp).c_str());
            pthread_mutex_lock(&gmutex);
            GAME->set_player_status(emp, EMPLOYER);
            GAME->setStatus(JOB_MAKE);
            GAME->set_answering_num(1);
            send_to_all(SUBS, s_msg, BUFF_LEN);
            pthread_mutex_unlock(&gmutex);
            bzero(s_msg, BUFF_LEN);
            continue;
        }
        if (status == P_PRE){
            pthread_mutex_lock(&gmutex);
            GAME->set_player_status(GAME->get_answering_id(), ANSWERING);
            pthread_mutex_unlock(&gmutex);
            continue;
        }
        if(status == P_OPEN){
            sprintf(s_msg, "\n%s\n\n   Время для выставления оценок!\n|common|", GAME->open_p(GAME->get_answering_id()).c_str());
            pthread_mutex_lock(&gmutex);
            GAME->set_scoreb(0);
            GAME->setStatus(SCORES);
            vector<Player*>* tms = GAME->get_players();
            for(vector<Player*>::iterator pl = tms->begin(); pl != tms->end(); pl++){
                if((*pl)->get_id() != GAME->get_answering_id()){
                    (*pl)->setStatus(SCORING);
                }
            }
            send_to_all(SUBS, s_msg, BUFF_LEN);;
            pthread_mutex_unlock(&gmutex);
            continue;
        }
        if(status == OVER){
            bzero(s_msg, BUFF_LEN);
            strcat(s_msg, GAME->Endgame(CONTEXT).c_str());
            strcat(s_msg, "|over|");
            pthread_mutex_lock(&gmutex);
            send_to_all(SUBS, s_msg, BUFF_LEN);
            CONTEXT->rm_lobby(lobby_id);
            pthread_mutex_unlock(&gmutex);
            sleep(1);
            break;
        }
    }
    pthread_mutex_destroy(&gmutex);
    close(sm_socket);
    pthread_exit(0);
}


void send_to_all(vector<int>* s_sockets, char* msg, int mlen){
    for(vector<int>::iterator it = s_sockets->begin(); it != s_sockets->end(); it++){
        send(*it, msg, mlen, 0);
    }
}

void send_to_all_exept(vector<int>* s_sockets, int exep, char* msg, int mlen){
    for(vector<int>::iterator it = s_sockets->begin(); it != s_sockets->end(); it++){
        if(*it != exep){
            send(*it, msg, mlen, 0);
        }
    }
}

// Структкра сообщения (клиент -> сервер) "output|request"
void ser_decode_msg(char* msg, int mlen, char* output, char* request){
    bzero(output, mlen);
    bzero(request, mlen);
    int bc = get_line_b(output, msg, 0, mlen, '|');
    bc = get_line_b(request, msg, bc, mlen, ' ');
}