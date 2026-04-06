// КЛИЕНТ (ИСХОДНЫЙ КОД)
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

#define LOGGING 100
#define SUCCESS 200
#define NO_ANSWER 500
#define EXIT 1000

#define REDIRECT 2000 
/*Сокет сервера прекращает соединение с игроком 
и игрок пытается подсоединиться к лобби, адрес и порт которого ему прислал сервер
и там с игроком уже общается лобби через сокет, который оно выделяет для него*/

#define CHATTING 3000 


#define MAX_DELAY 10
    

using namespace std;

void cli_decode_msg(char* msg, int mlen, char* output, char* request, int& status);
void cli_input(string& text);
bool client_loop(int&, string&, int&, char*, char*, char*, char*, int&);

int main()
{
    struct hostent *hp;

    int c_sock = socket(AF_INET, SOCK_STREAM, 0);
    int chat_sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in c_addr;
    struct sockaddr_in s_addr;

    char g_host[BUFF_LEN] = "";
    int g_port = 0;

    if(c_sock < 0 || chat_sock < 0){
        cout << "НЕ УДАЛОСЬ СОЗДАТЬ КЛИЕНТА!" << endl;
        return -1;
    }

    bzero((char*)&c_addr, (sizeof(struct sockaddr_in)));
    c_addr.sin_family = AF_INET;
    c_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    c_addr.sin_port = 0;
    
    if(bind(c_sock, (sockaddr*)&c_addr, sizeof(struct sockaddr_in)) < 0 
    || bind(chat_sock, (sockaddr*)&c_addr, sizeof(struct sockaddr_in)) < 0 )
    {
        cout << "НЕ УДАЛОСЬ ИНИЦИАЛИЗИРОВАТЬ КЛИЕНТА!" << endl;
        return -1;
    }

    char s_msg[BUFF_LEN] = "";
    char a_msg[BUFF_LEN] = "";

    int rec = 0;
    char output[BUFF_LEN] = "";
    char request[BUFF_LEN] = "";
    int status = WAIT_ACCEPT;
    string login;
    string password;
    //---------------------------------------------------------------------
    cout << "=============================================================" << endl;
    cout << "               Добро пожаловать в СТАРТАП!                   " << endl;
    cout << "=============================================================" << endl;
    
    //============================================================
    // 1. ПОИСК СЕРВЕРА
    //============================================================
    for(;;)
    {
        bzero(s_msg, BUFF_LEN);
        bzero(a_msg, BUFF_LEN);
        bzero(output, BUFF_LEN);
        bzero(request, BUFF_LEN);
        cout << "   ВВЕДИТЕ АДРЕС СЕРВЕРА (или exit, чтобы выйти): ";
        cin >> g_host;
        if(strncmp(g_host, "exit", 5) == 0){
            char conf = ' ';
            cout << "===========Выход из игры!===========" << endl;
            do{
                cout << "       Вы уверены, что хотите выйти? (y/N): ";
                cin >> conf;
            } while (conf != 'y' && conf != 'Y' && conf != 'n' && conf != 'N');
            if(conf == 'y' || conf == 'Y'){
                cout << "       Игра закрывается!" << endl;
                return 0;
            } else {
                continue;
            }
        }
        cout << "   ВВЕДИТЕ ПОРТ СЕРВЕРА:";
        cin >> g_port;
        cout << endl;

        hp = gethostbyname(g_host);

        bzero((char*)&s_addr, (sizeof(struct sockaddr_in)));
        s_addr.sin_family = AF_INET;
        //bcopy(hp->h_addr, &s_addr.sin_addr, hp->h_length) ;
        inet_aton(g_host, &s_addr.sin_addr);
        s_addr.sin_port = htons(g_port);

        if (connect(c_sock, (sockaddr*)&s_addr, sizeof(struct sockaddr_in)) < 0) {
            cout << "   СОЕДИНЕНИЕ С СЕРВЕРОМ НЕ УДАЛОСЬ!" << endl;
            cout << endl;
            continue;
        }
        char lr = ' ';
        do{
            bzero(s_msg, BUFF_LEN);
            bzero(a_msg, BUFF_LEN);
            bzero(output, BUFF_LEN);
            bzero(request, BUFF_LEN);
            cout << "       Логин(L)/Регистрация(R)/Выйти(E): ";
            cin >> lr;
        

            if(lr == 'E' || lr == 'e'){
                return 0;
            }
            int ans = -1;

            if(lr == 'R' || lr == 'r'){
                do{
                    do{
                        cout << "       Придумайте логин: ";
                        cin >> login;
                    } while(login.size() <= 0);
  
                    cout << "       Придумайте пароль: ";
                    cin >> password;
                    strcat(s_msg, login.c_str());
                    strcat(s_msg, ":");
                    strcat(s_msg, password.c_str());
                    strcat(s_msg, "|register");
                    send(c_sock, s_msg, BUFF_LEN, 0);
                    ans = -1;
                    ans = recv(c_sock, a_msg, BUFF_LEN, 0);
                    status = LOGGING;
                    if(ans > 0){
                        cli_decode_msg(a_msg, BUFF_LEN, output, request, status);
                    }else{
                        status = NO_ANSWER;
                        cout << "       СЕРВЕР НЕ ОТВЕЧАЕТ(" << endl;
                        break;
                    }

                    if(strncmp(request, "logbusy", 8) == 0){
                        cout << "       Логин уже занят, придумайте другой..." << endl;
                        continue;
                    }
                    if(strncmp(request, "success", 8) == 0){
                        cout << "       Регистрация пройдена!" << endl;
                        break;
                    }
                    if(strncmp(request, "rfail", 6) == 0){
                        cout << "       Не удалось пройти регистрацию..." << endl;
                        break;
                    }
                } while(true);
            }

            if(status == NO_ANSWER){
                break;
            }
            cout << endl;
            cout << "       Логин: ";
            cin >> login;
            cout << "       Пароль: ";
            cin >> password;
            strcat(s_msg, login.c_str());
            strcat(s_msg, ":");
            strcat(s_msg, password.c_str());
            strcat(s_msg, "|login");
            send(c_sock, s_msg, BUFF_LEN, 0);
            ans = -1;
            ans = recv(c_sock, a_msg, BUFF_LEN, 0);
            status = NO_ANSWER;

            if(ans > 0){
                cli_decode_msg(a_msg, BUFF_LEN, output, request, status);
            }

            if(strncmp(request, "success", 8) == 0){
                status = SUCCESS;
            }

            if(strncmp(request, "wrong", 6) == 0){
                cout << "Неверный логин или пароль!" << endl;
                continue;
            }

            if(strncmp(request, "online", 7) == 0){
                cout << "Пользователь уже онлайн!" << endl;
                continue;
            }

            if(status == SUCCESS || status == NO_ANSWER){
                break;
            }
        
        }while(lr != 'L' && lr != 'l' && lr != 'R' && lr != 'r' && lr != 'E' && lr != 'e');

        if(status == SUCCESS){
            break;
        } else {
            continue;
        }
    }

    cout << "           Вы успешно вошли на сервер!" << endl;
    cout << "  ПОДСКАЗКА: для просмотра доступных команд введите help" << endl;
    
    while(client_loop(c_sock, login, rec, s_msg, a_msg, output, request, status)); // ОСНОВНОЙ ЦИКЛ СЕССИИ

    close(c_sock);
    return 0;
}

bool client_loop(int& c_sock, string& login, int& rec, char* s_msg, char* a_msg, char* output, char* request, int& status)
{
    //============================================================
    // 2. Командная строка клиента для взаимодействия с сервером
    //============================================================
    struct timeval old_tv;
    socklen_t len = sizeof(old_tv);
    getsockopt(c_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&old_tv, &len);
    struct timeval tv;
    tv.tv_sec = MAX_DELAY;
    tv.tv_usec = 0;
    setsockopt(c_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
    for(;;)
    {
        bzero(s_msg, BUFF_LEN);
        bzero(a_msg, BUFF_LEN);
        bzero(output, BUFF_LEN);
        bzero(request, BUFF_LEN);
        cout << endl;
        string command;
        int ans = -1;
        cout << "[" << login << "]$ ";
        cin >> command;
        strcat(s_msg, login.c_str());
        //--------------------------------------------------------------------
        // Инструкция
        //--------------------------------------------------------------------
        if(strncmp(command.c_str(), "help", 5) == 0){
            cout << endl;
            cout << "'ps' - показать список игроков онлайн." << endl;
            cout << "'ps -a' - показать список всех игроков." << endl;
            cout << "'rate' - показать рейтинг." << endl;
            cout << "'ls' - показать список лобби." << endl;
            cout << "'mkl' - создать лобби." << endl;
            cout << "'join [порт лобби]' - присоединиться к лобби." << endl;
            cout << "'chats' - показать чаты." << endl;
            cout << "'chat' - открыть чат с игроком (если нет, создаётся)." << endl;
            //cout << "'rm chat [id игрока]' - удалить чат с игроком" << endl; --- В ДОЛГИЙ ЯЩИК
            //cout << "'report [id игрока]' - отправить жалобу на игрока." << endl; --- В ДОЛГИЙ ЯЩИК
            cout << "'exit' - выйти." << endl;
        }

        //--------------------------------------------------------------------
        // Запрос списка игроков
        //--------------------------------------------------------------------
        if(strncmp(command.c_str(), "ps -a", 6) == 0){
            strcat(s_msg, "|getallplayers");
            send(c_sock, s_msg, BUFF_LEN, 0);
            ans = -1;
            ans = recv(c_sock, a_msg, BUFF_LEN, 0);
            cli_decode_msg(a_msg, BUFF_LEN, output, request, status);
            if(ans > 0){
                cout << output << endl;
            }
        }

        if(strncmp(command.c_str(), "ps", 2) == 0){
            strcat(s_msg, "|getplayers");
            send(c_sock, s_msg, BUFF_LEN, 0);
            ans = -1;
            ans = recv(c_sock, a_msg, BUFF_LEN, 0);
            cli_decode_msg(a_msg, BUFF_LEN, output, request, status);
            if(ans > 0){
                cout << output << endl;
            }
        }

        //--------------------------------------------------------------------
        // Запрос рейтинга
        //--------------------------------------------------------------------
        if(strncmp(command.c_str(), "rate", 5) == 0){
            strcat(s_msg, "|rate");
            send(c_sock, s_msg, BUFF_LEN, 0);
            ans = -1;
            ans = recv(c_sock, a_msg, BUFF_LEN, 0);
            cli_decode_msg(a_msg, BUFF_LEN, output, request, status);
            if(ans > 0){
                cout << output << endl;
            }
        }

        //--------------------------------------------------------------------
        // Запрос списка лобби
        //--------------------------------------------------------------------
        if(strncmp(command.c_str(), "ls", 5) == 0){
            strcat(s_msg, "|getlobby");
            send(c_sock, s_msg, BUFF_LEN, 0);
            ans = -1;
            ans = recv(c_sock, a_msg, BUFF_LEN, 0);
            cli_decode_msg(a_msg, BUFF_LEN, output, request, status);
            if(ans > 0){
                cout << output << endl;
            }
        }

        //--------------------------------------------------------------------
        // Запрос на создание лобби
        //--------------------------------------------------------------------
        if(strncmp(command.c_str(), "mkl", 5) == 0){
            string name;
            cout << "Введите название лобби (или exit для отмены): ";
            cin >> name;
            if(strncmp(name.c_str(), "exit", 5) == 0){
                continue;
            }
            int num = -1;
            do{
                cout << "Введите количество игроков (3-6, или 0 для отмены): ";
                cin >> num;
            } while((num < 3 || num > 6) && num != 0);
            if(num == 0){
                continue;
            }
            sprintf(s_msg, "%s:", name);
            sprintf(s_msg, "%d", num);
            strcat(s_msg, "|makelob");
            send(c_sock, s_msg, BUFF_LEN, 0);
            ans = -1;
            ans = recv(c_sock, a_msg, BUFF_LEN, 0);
            cli_decode_msg(a_msg, BUFF_LEN, output, request, status);
            if(ans > 0){
                if(strncmp(request, "success", 8) == 0){
                    cout << "Ваше лобби '" << name << "' успешно создано!" << endl;
                }else{
                    cout << "Не удалось создать лобби." << endl;
                }
            }
        }

        //--------------------------------------------------------------------
        // Запрос на присоединение к лобби
        //--------------------------------------------------------------------
        if(strncmp(command.c_str(), "join", 5) == 0){
            int lobby_port;
            cout << "   Введите порт лобби: ";
            cin >> lobby_port;
            
            sprintf(s_msg, "%d", lobby_port);
            strcat(s_msg, "|join");
            send(c_sock, s_msg, BUFF_LEN, 0);
    
            ans = -1;
            ans = recv(c_sock, a_msg, BUFF_LEN, 0);
            if(ans > 0){
                cli_decode_msg(a_msg, BUFF_LEN, output, request, status);
                if(strncmp(request, "allow", 6) == 0){
                    sockaddr_in lobby_addr;
                    lobby_addr.sin_family = AF_INET;
                    inet_aton(output, &lobby_addr.sin_addr);
                    lobby_addr.sin_port = lobby_port;
                    if(connect(c_sock, (sockaddr*)&lobby_addr, sizeof(struct sockaddr_in)) < 0){
                        cout << "Не удалось подключится к лобби." << endl;
                        continue;
                    }
                } else {
                    cout << "Не удалось найти указанное лобби." << endl;
                    continue;
                }
            }
        }


        //--------------------------------------------------------------------
        // Запрос списка чатов
        //--------------------------------------------------------------------
        if(strncmp(command.c_str(), "chats", 6) == 0){
            strcat(s_msg, "|getchats");
            send(c_sock, s_msg, BUFF_LEN, 0);
            ans = -1;
            ans = recv(c_sock, a_msg, BUFF_LEN, 0);
            if(ans > 0){
                cli_decode_msg(a_msg, BUFF_LEN, output, request, status);
                cout << output << endl;
            }

        }

        //--------------------------------------------------------------------
        // Запрос чата с игроком
        //--------------------------------------------------------------------
        if(strncmp(command.c_str(), "chat", 5) == 0){
            string target_id;
            cout << "   Введите id игрока для чата: ";
            cin >> target_id;
    
            bzero(s_msg, BUFF_LEN);
            strcat(s_msg, target_id.c_str());
            strcat(s_msg, "|chatrequest");
            send(c_sock, s_msg, BUFF_LEN, 0);
            ans = -1;
            ans = recv(c_sock, a_msg, BUFF_LEN, 0);
            if(ans > 0){
                cli_decode_msg(a_msg, BUFF_LEN, output, request, status);
                cout << output << endl;
                status = CHATTING;
                break;
            }
        }


        //--------------------------------------------------------------------
        // Выход из игры
        //--------------------------------------------------------------------

        if(strncmp(command.c_str(), "exit", 5) == 0){
            char conf = ' ';
            cout << "===========Выход из игры!===========" << endl;
            do{
                cout << "       Вы уверены, что хотите выйти? (y/N): ";
                cin >> conf;
            } while (conf != 'y' && conf != 'Y' && conf != 'n' && conf != 'N');
            if(conf == 'y' || conf == 'Y'){
                cout << "       Игра закрывается!" << endl;
                return false;
            } else {
                continue;
            }
        }

        if(ans <= 0)
        {
            cout << "\nСЕРВЕР НЕ ОТВЕЧАЕТ(" << endl;
            continue;
        }

        cout << "Неизвестная команда '" << command << "'" << endl;
    }
    
    setsockopt(c_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&old_tv, sizeof(old_tv));
    //============================================================
    // 3. Чат с другим игроком
    //============================================================

    if(status == CHATTING)
    {
        for(;;)
        {
            // retutn true; (будет где-то, чтобы продолжить цикл сессии)
        }
    }
    
    
    //============================================================
    // 4. Игра
    //============================================================
    for(;;)
    {
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
            if(strncmp(request, "Игра окончена!", 27) == 0){
                return true; // возвращение в командную строку
            }
        }

        if(status == WAIT_ACCEPT)
        {
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
    if(strncmp(pstat, "SUCCESS", 8) == 0){
        status = SUCCESS;
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