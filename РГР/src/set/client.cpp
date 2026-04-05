//СТАРТАП-КЛИЕНТ
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <unistd.h> 
#include "game.h"

using namespace std;

void cli_decode_msg(char* msg, int mlen, char* output, char* request, int& status);
void cli_input(string& text);


int main()
{
    struct hostent *hp;

    int c_sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in c_addr;
    struct sockaddr_in s_addr;

    char g_host[BUFF_LEN] = "";
    int g_port = 0;

    if(c_sock < 0){
        cout << "НЕ УДАЛОСЬ СОЗДАТЬ КЛИЕНТА!" << endl;
        return -1;
    }

    bzero((char*)&c_addr, (sizeof(struct sockaddr_in)));
    c_addr.sin_family = AF_INET;
    c_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    c_addr.sin_port = 0;
    
    if(bind(c_sock, (sockaddr*)&c_addr, sizeof(struct sockaddr_in)) < 0){
        cout << "НЕ УДАЛОСЬ ИНИЦИАЛИЗИРОВАТЬ КЛИЕНТА!" << endl;
        return -1;
    }
    string nick;
    cout << "   Введите ваш ник (без пробелов): ";
    cin >> nick;
    searchgame:
    cout << "   Введите адрес игры (без пробелов): ";
    cin >> g_host;
    cout << "   Введите порт игры: ";
    cin >> g_port;
    cout << endl;
    hp = gethostbyname(g_host);

    bzero((char*)&s_addr, (sizeof(struct sockaddr_in)));
    s_addr.sin_family = AF_INET;
    bcopy(hp->h_addr, &s_addr.sin_addr, hp->h_length) ;
    s_addr.sin_port = htons(g_port);

    if (connect(c_sock, (sockaddr*)&s_addr, sizeof(struct sockaddr_in)) < 0) {
        cout << "   СОЕДИНЕНИЕ С СЕРВЕРОМ НЕ УДАЛОСЬ!" << endl;
        return -1;
    }

    char s_msg[BUFF_LEN] = "";
    char a_msg[BUFF_LEN] = "";
    strcat(s_msg, nick.c_str());
    strcat(s_msg, "|join");

    if(send(c_sock, s_msg, BUFF_LEN, 0) < 0){
        cout << "   НЕ УДАЛОСЬ ОТПРАВИТЬ ДАННЫЕ ИГРОКА!" << endl;
        return -1;
    }

    int rec = 0;
    char output[BUFF_LEN] = "";
    char request[BUFF_LEN] = "";
    int status = WAIT_ACCEPT;
    //---------------------------------------------------------------------
    for(;;){
        bzero(s_msg, BUFF_LEN);
        bzero(a_msg, BUFF_LEN);
        bzero(output, BUFF_LEN);
        bzero(request, BUFF_LEN);
        rec = recv(c_sock, a_msg, BUFF_LEN, 0);
        if (rec == 0)
        { 
            cout << "СОЕДИНЕНИЕ ПРЕРВАНО!" << endl;
            break;
        }
        if (rec < 0)
        { 
            cout << " ОШИБКА СЕРВЕРА! (Invalid server socket)" << endl;
            break;
        }
        cli_decode_msg(a_msg, BUFF_LEN, output, request, status);

        if(strncmp(request, "common", 7) == 0){
            cout << "   " << output << endl;
        }

        if(status == WAIT_ACCEPT)
        {
            // Надо будет потом добавить таймаут и возвращение к поиску игры.
            //continue;
        }
        if(status == PRE_TO_PLAY){
            char a;
            cout << "   Вы успешно присоединились к игре!" << endl;
            cout << "   Готовы начать игру?" << endl;
            do{
                cout << "   Введите Y(Готов)/q(Выйти): ";
                cin >> a;
            } while (a != 'y' && a != 'Y' && a != 'q' && a != 'Q');
            if (a == 'q' || a == 'Q'){
                break;
            }
            if(send(c_sock, "|readytoplay", BUFF_LEN, 0) < 0){
                cout << "   Не удалось отправить сообщение. Попробуйте ещё раз." << endl;
            } else {
                cout << "   Ожидание других игроков..." << endl;
                bzero(a_msg, BUFF_LEN);
            }
            continue;
        }
        if(status == WAITING || status == READY_TO_PLAY){
            send(c_sock, " ", 1, 0);
            sleep(1);
            continue;
        }
        if(status == EMPLOYER){
            if(strncmp(request, "givehist", 9) == 0){
                string manual;
                cout << endl;
                output[0] = ' ';
                cout << output << endl;
                cin.ignore();
                cli_input(manual);
        
                strcat(s_msg, manual.c_str());
                strcat(s_msg, "|sendhist");
                send(c_sock, s_msg, BUFF_LEN, 0);
                continue;
            }
            if(strncmp(request, "givejchoice", 11) == 0){
                cout << endl;
                cout << output << endl;
        
                string choice;
                cout << "   Введите ваш выбор: ";
                cin.ignore();
                getline(cin, choice);
        
                bzero(s_msg, BUFF_LEN);
                strcat(s_msg, choice.c_str());
                strcat(s_msg, "|jchoice");
                send(c_sock, s_msg, BUFF_LEN, 0);
                continue;
            }
            continue;
        }

        if(status == ANSWERING){
            if(strncmp(request, "areanswerm", 11) == 0){
                cout << endl;
                cout << output << endl;
                int v = 0;
                do{
                    cout << "   Введите номер вакансии, на которую вы претендуете (1 - 3): ";
                    cin >> v;
                } while(v != 1 && v != 2 && v != 3);
                sprintf(s_msg, "%d", v);
                strcat(s_msg, "|claim");
                send(c_sock, s_msg, BUFF_LEN, 0);
            }

            if(strncmp(request, "claim", 6) == 0){
                char r = ' ';
                do{
                    cout << "\n   Работодатель готов Вас выслушать. Введите 'Y', когда будете готовы отвечать на собеседовании: ";
                    cin >> r;
                } while(r != 'Y' && r != 'y');
                send(c_sock, "|readytoanswer", BUFF_LEN, 0);
                continue;
            }

            if(strncmp(request, "giveanswerm", 11) == 0){
                string resume;
                cout << "   Минута пошла! Удачи!" << endl;
                cin.ignore();
                cli_input(resume);
                strcat(s_msg, resume.c_str());
                strcat(s_msg, "|sendanswer");
                send(c_sock, s_msg, BUFF_LEN, 0);
                continue;
            }

            if(strncmp(request, "quest", 6) == 0){
                string answ;
                cout << "   Вопрос." << endl;
                cout << "   " << output << endl;
                cout << endl;
                cout << "   Введите ваш ответ." << endl;
                cli_input(answ);
                strcat(s_msg, answ.c_str());
                strcat(s_msg, "|aquest");
                send(c_sock, s_msg, BUFF_LEN, 0);
                continue;
            }
            continue;
        }

        if(status == QUESTIONING){
            if(strncmp(request, "gquest", 7) == 0){
                cout << "\n Ваш вопрос отправлен и дойдёт до адресата в порядке очереди." << endl;
            }
            char h = ' ';
            do{
                cout << "\n Есть ли у Вас вопросы к соискателю? (Y/n): ";
                cin >> h;
            } while(h != 'Y' && h != 'y' && h != 'N' && h != 'n');
            if(h == 'N' || h == 'n'){
                send(c_sock, "|noquest", BUFF_LEN, 0);
                continue;
            }
            string question;
            cout << "   Напишите Ваш вопрос." << endl;
            cin.ignore();
            cli_input(question);
            strcat(s_msg, output);
            strcat(s_msg, question.c_str());
            strcat(s_msg, "|quest");
            send(c_sock, s_msg, BUFF_LEN, 0);
        }

        if(status == SCORING){
            int score = 0;
            do{
                cout << "\n Поставьте выступившему оценку от 1 до 5: ";
                cin >> score;
            } while(score < 1 || score > 5);
            sprintf(s_msg, "%d", score);
            strcat(s_msg, "|score");
            send(c_sock, s_msg, BUFF_LEN, 0);
            continue;
        }
    }
    close(c_sock);
    return 0;
}


// Структкра сообщения (сервер -> клиент) "output|request|p_status"
void cli_decode_msg(char* msg, int mlen, char* output, char* request, int& status){
    bzero(output, mlen);
    bzero(request, mlen);
    int bc = get_line_b(output, msg, 0, mlen, '|');
    bc = get_line_b(request, msg, bc, mlen, '|');
    char pstat[mlen] = "";
    bc = get_line_b(pstat, msg, bc, mlen, ' ');
    if (strncmp(pstat, "WAIT_ACCEPT", 12) == 0){
        status = WAIT_ACCEPT;
    }
    if (strncmp(pstat, "PRE_TO_PLAY", 12) == 0){
        status = PRE_TO_PLAY;
    }
    if(strncmp(pstat, "READY_TO_PLAY", 14) == 0){
        status = READY_TO_PLAY;
    }
    if(strncmp(pstat, "WAITING", 8) == 0){
        status = WAITING;
    }
    if(strncmp(pstat, "EMPLOYER", 9) == 0){
        status = EMPLOYER;
    }
    if(strncmp(pstat, "ANSWERING", 10) == 0){
        status = ANSWERING;
    }
    if(strncmp(pstat, "QUESTIONING", 12) == 0){
        status = QUESTIONING;
    }
    if(strncmp(pstat, "SCORING", 8) == 0){
        status = SCORING;
    }
    if(strncmp(pstat, "LEFT", 5) == 0){
        status = LEFT;
    }
}

void cli_input(string& text){
    text += " ";
    cout << "   (для завершения введите пустую строку или 'END'):\n" << endl;
    //cin.ignore();
    int max_len = BUFF_LEN - 300;
    string line;
    while (true) {
        cout << "(" << text.size() - 1 << "/" << max_len << "): ";
        getline(cin, line);
        if(text.size() + line.size() > max_len){
            cout << "   Не хватает места!" << endl;
            continue;
        }
        if (line.empty() || line == "END") {
            break;
        }
        text += line + "\n";
    }
        
    if (!text.empty() && text.back() == '\n') {
        text.pop_back();
    }
}