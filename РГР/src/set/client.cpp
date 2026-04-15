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
#include <termios.h>
#include <dirent.h>
#include "chat.h"
#include <sys/stat.h>

#define LOGGING 100
#define SUCCESS 200
#define NO_ANSWER 500
#define MAX_CHATS 10
#define MAX_DELAY 10

FILE* f_manual = fopen("info/manual.txt", "rb");
const int m_size = sizeof(*f_manual) / sizeof(char);
char s_manual[m_size];
char mc;

static struct termios original_t;
void disableEcho() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    original_t = t;
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void enableEcho() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_t);
}

using namespace std;


void cli_decode_msg(char* msg, int mlen, char* output, char* request, int& status);
void cli_input(string& text);
bool client_loop(int&, int&, int&, string&, int&, char*, char*, char*, char*, 
        int&, struct timeval&, struct sockaddr_in&, sockaddr_in&, char*, string);
bool commandexists(string command);



int socket_init(int& sock, struct sockaddr_in* addr){
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        cout << "НЕ УДАЛОСЬ СОЗДАТЬ КЛИЕНТА!" << endl;
        return -1;
    }
    if(bind(sock, (sockaddr*)addr, sizeof(struct sockaddr_in)) < 0)
    {
        cout << "НЕ УДАЛОСЬ ИНИЦИАЛИЗИРОВАТЬ КЛИЕНТА!" << endl;
        return -1;
    }
    return sock;
}


//--------------------------------------------------------------------------------------------

int main()
{

    FILE* f_manual = fopen("info/manual.txt", "rb");
    char* s_manual;
    if (f_manual != NULL) {
        fseek(f_manual, 0, SEEK_END);
        long m_size = ftell(f_manual);
        fseek(f_manual, 0, SEEK_SET);
        s_manual = (char*)malloc(m_size + 1);
        size_t bytes_read = fread(s_manual, 1, m_size, f_manual);
        s_manual[bytes_read] = '\0';
        fclose(f_manual);
    }

    struct hostent *hp;

    int c_sock;
    int chat_sock;
    int lobby_sock;

    struct sockaddr_in c_addr;
    struct sockaddr_in s_addr;

    bzero((char*)&c_addr, (sizeof(struct sockaddr_in)));
    c_addr.sin_family = AF_INET;
    c_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    c_addr.sin_port = 0;

    if(socket_init(c_sock, &c_addr) < 0){
        return -1;
    }

    char g_host[BUFF_LEN] = "";
    int g_port = 0;

    char s_msg[BUFF_LEN] = "";
    char a_msg[BUFF_LEN] = "";

    int rec = 0;
    char output[BUFF_LEN] = "";
    char request[BUFF_LEN] = "";
    int status = WAIT_ACCEPT;
    string login;
    string password;

    struct timeval old_tv;
    socklen_t len = sizeof(old_tv);
    getsockopt(c_sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&old_tv, &len);
    struct timeval tv;
    tv.tv_sec = MAX_DELAY;
    tv.tv_usec = 0;
    setsockopt(c_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
    //---------------------------------------------------------------------
    cout << "=============================================================" << endl;
    cout << "               Добро пожаловать в СТАРТАП!                   " << endl;
    cout << "=============================================================" << endl;
    
    for(;;){ // ОСНОВНОЙ ЦИКЛ ЖИЗНИ КЛИЕНТА
        //============================================================
        // 1. ПОИСК СЕРВЕРА
        //============================================================
        for(;;)
        {
            bzero(s_msg, BUFF_LEN);
            bzero(a_msg, BUFF_LEN);
            bzero(output, BUFF_LEN);
            bzero(request, BUFF_LEN);
            cout << "\n   ВВЕДИТЕ АДРЕС СЕРВЕРА (или exit, чтобы выйти): ";
            cin >> g_host;
            if(strncmp(g_host, "exit", 5) == 0){
                char conf = ' ';
                cout << "=======================Выход из игры!=======================" << endl;
                do{
                    cout << "       Вы уверены, что хотите выйти? (y/N): ";
                    cin >> conf;
                } while (conf != 'y' && conf != 'Y' && conf != 'n' && conf != 'N');
                if(conf == 'y' || conf == 'Y'){
                    cout << "=======================Игра закрывается!=======================" << endl;
                    return 0;
                } else {
                    continue;
                }
            }
            cout << "   ВВЕДИТЕ ПОРТ СЕРВЕРА: ";
            cin >> g_port;
            cout << endl;

            hp = gethostbyname(g_host);

            bzero((char*)&s_addr, (sizeof(struct sockaddr_in)));
            s_addr.sin_family = AF_INET;
            bcopy(hp->h_addr, &s_addr.sin_addr, hp->h_length);
            s_addr.sin_port = htons(g_port);

            if (connect(c_sock, (sockaddr*)&s_addr, sizeof(struct sockaddr_in)) < 0) {
                cout << "   СОЕДИНЕНИЕ С СЕРВЕРОМ НЕ УДАЛОСЬ!" << endl;
                cout << endl;
                continue;
            }


            struct sockaddr_in local_addr;
            char client_ip[INET_ADDRSTRLEN];
            socklen_t len = sizeof(local_addr);
            if (getsockname(c_sock, (struct sockaddr*)&local_addr, &len) == 0) {
                inet_ntop(AF_INET, &local_addr.sin_addr, client_ip, sizeof(client_ip));
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
                        string r_pas = "fagshdgfkldafdjvixopmklfeakjsgbiodsfkagksjbfjizdpos;clfmaklgnbfxijpozks;lfmklbf";
                        disableEcho();
                        do{
                            cout << "       Придумайте пароль: ";
                            cin >> password;
                            cout << endl;
                            cout << "       Повторите пароль: ";
                            cin >> r_pas;
                            cout << endl;
                            if(password != r_pas){
                                cout << "Пароли не совпадают!" << endl;
                            } else {
                                break;
                            }
                        }while(true);
                        enableEcho();
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
                    if(status == NO_ANSWER){
                        cout << "\n       СЕРВЕР НЕ ОТВЕЧАЕТ(" << endl;
                        break;
                    }
                    lr = ' ';
                    cout << endl;
                    continue;
                }

                cout << endl;
                cout << "       Логин: ";
                cin >> login;
                cout << "       Пароль: ";
                disableEcho();
                cin >> password;
                cout << endl;
                enableEcho();

                sprintf(s_msg, "%s:%s:%s|login",
                login.c_str(), password.c_str(), client_ip);
                send(c_sock, s_msg, BUFF_LEN, 0);
                ans = -1;
                ans = recv(c_sock, a_msg, BUFF_LEN, 0);
                status = LOGGING;

                if(ans > 0){
                    cli_decode_msg(a_msg, BUFF_LEN, output, request, status);
                }else{
                    cout << "\n       СЕРВЕР НЕ ОТВЕЧАЕТ(" << endl;
                    cout << endl;
                    status = NO_ANSWER;
                    break;
                }

                if(strncmp(request, "success", 8) == 0){
                    cout << endl;
                    status = SUCCESS;
                    break;
                }

                if(strncmp(request, "wrong", 6) == 0){
                    cout << "   Неверный логин или пароль!" << endl;
                    lr = ' ';
                    cout << endl;
                    continue;
                }

                if(strncmp(request, "online", 7) == 0){
                    cout << "   Пользователь уже онлайн!" << endl;
                    lr = ' ';
                    cout << endl;
                    continue;
                }
        
            }while(lr != 'L' && lr != 'l' && lr != 'R' && lr != 'r' && lr != 'E' && lr != 'e');

            if(status == SUCCESS){
                break;
            } else {
                close(c_sock);
                if(socket_init(c_sock, &c_addr) < 0){
                    cout << "   Не удалось реинициализировать клиента." << endl;
                    return -1;
                }
                continue;
            }
        }

        string chats_path = "info/chats/" + login + "/";
        mkdir(chats_path.c_str(), 0777);

        cout << "           Вы успешно вошли на сервер!" << endl;
        cout << "  ПОДСКАЗКА: для просмотра доступных команд введите help" << endl;
        
        //============================================================================
        // ОТКРЫВАЕМ ПРОСЛУШКУ ПОЛЬЗОВАТЕЛЮ
        //============================================================================
        int chat_server_sock;
        struct sockaddr_in chat_addr;
        bzero(&chat_addr, sizeof(chat_addr));
        chat_addr.sin_family = AF_INET;
        chat_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        chat_addr.sin_port = 0;
        if (socket_init(chat_server_sock, &chat_addr) < 0) {
            cout << "Не удалось создать сокет для приёма сообщений" << endl;
        } else {
            socklen_t addr_len = sizeof(chat_addr);
            getsockname(chat_server_sock, (sockaddr*)&chat_addr, &addr_len);
            int chat_port = ntohs(chat_addr.sin_port);
            
            bzero(s_msg, BUFF_LEN);
            sprintf(s_msg, "%s:%d|setchatport", login.c_str(), chat_port);
            send(c_sock, s_msg, BUFF_LEN, 0);
            if(recv(c_sock, a_msg, BUFF_LEN, 0) < 0 || strncmp(request, "success", 8) != 0){
                cout << "Не удалось обеспечить приём сообщений (server error)" << endl;
            }else{
                listen(chat_server_sock, MAX_CHATS);
            
                chat_args* cargs = new chat_args();
                cargs->socket = chat_server_sock;
                cargs->login = login;
            
                pthread_t accept_tid;
                pthread_create(&accept_tid, NULL, msg_accept_thread, (void*)cargs);
                pthread_detach(accept_tid);
            
               // cout << "Приём сообщений запущен на порту " << chat_port << endl;
            }
        }
        //============================================================================

        while(client_loop(c_sock, chat_sock, lobby_sock, login, rec, 
            s_msg, a_msg, output, request, status, old_tv, s_addr, c_addr, s_manual,
            chats_path)); // ЦИКЛ СЕССИИ НА СЕРВЕРЕ
        close(chat_server_sock);
        close(c_sock);
        if(socket_init(c_sock, &c_addr) < 0){
            cout << "   Не удалось реинициализировать клиента." << endl;
            return -1;
        }
    }
    free(s_manual);
    close(c_sock);
    return 0;
}

bool client_loop(int& c_sock, int& chat_sock, int& lobby_sock, string& login, int& rec, 
    char* s_msg, char* a_msg, char* output, char* request, int& status, struct timeval& old_tv,
    struct sockaddr_in& s_addr, struct sockaddr_in& c_addr, char* manual, string chats_path)
{
    //============================================================
    // 2. Командная строка клиента для взаимодействия с сервером
    //============================================================
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
        if(!commandexists(command)){
            cout << "Неизвестная команда '" << command << "'" << endl;
            cout << "ПОДСКАЗКА: для просмотра доступных команд введите help" << endl;
            continue;
        }
        
        //--------------------------------------------------------------------
        // Инструкция
        //--------------------------------------------------------------------
        if(strncmp(command.c_str(), "help", 5) == 0){
            cout << endl;
            cout << "'ps' - показать список игроков онлайн." << endl;
            cout << "'psa' - показать список всех игроков." << endl;
            cout << "'rt' - показать рейтинг." << endl;
            cout << "'ls' - показать список лобби." << endl;
            cout << "'mkl' - создать лобби." << endl;
            cout << "'join' - присоединиться к лобби." << endl;
            cout << "'chats' - показать чаты." << endl;
            cout << "'chat' - показать чат с игроком." << endl;
            cout << "'mes' - написать сообщение игроку." << endl;
            cout << "'clchat' - очистить чат с игроком." << endl;
            cout << "'about' - об игре." << endl;
            cout << "'exit' - выйти." << endl;

            //cout << "'rm chat [id игрока]' - удалить чат с игроком" << endl; --- В ДОЛГИЙ ЯЩИК
            //cout << "'report [id игрока]' - отправить жалобу на игрока." << endl; --- В ДОЛГИЙ ЯЩИК
            continue;
        }

        //--------------------------------------------------------------------
        // Запрос списка игроков
        //--------------------------------------------------------------------
        if(strncmp(command.c_str(), "psa", 6) == 0){
            strcat(s_msg, "|getallplayers");
            send(c_sock, s_msg, BUFF_LEN, 0);
            ans = -1;
            ans = recv(c_sock, a_msg, BUFF_LEN, 0);
            cli_decode_msg(a_msg, BUFF_LEN, output, request, status);
            if(ans > 0){
                cout << output << endl;
                continue;
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
                continue;
            }
        }

        //--------------------------------------------------------------------
        // Запрос рейтинга
        //--------------------------------------------------------------------
        if(strncmp(command.c_str(), "rt", 3) == 0){
            strcat(s_msg, "|rate");
            send(c_sock, s_msg, BUFF_LEN, 0);
            ans = -1;
            ans = recv(c_sock, a_msg, BUFF_LEN, 0);
            cli_decode_msg(a_msg, BUFF_LEN, output, request, status);
            if(ans > 0){
                cout << output << endl;
                continue;
            }
        }

        //--------------------------------------------------------------------
        // Запрос списка лобби
        //--------------------------------------------------------------------
        if(strncmp(command.c_str(), "ls", 3) == 0){
            strcat(s_msg, "|getlobby");
            send(c_sock, s_msg, BUFF_LEN, 0);
            ans = -1;
            ans = recv(c_sock, a_msg, BUFF_LEN, 0);
            cli_decode_msg(a_msg, BUFF_LEN, output, request, status);
            if(ans > 0){
                cout << output << endl;
                continue;
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
            sprintf(s_msg, "%d:", num);
            strcat(s_msg, name.c_str());
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
                continue;
            }
        }

        //--------------------------------------------------------------------
        // Запрос на присоединение к лобби
        //--------------------------------------------------------------------
        if(strncmp(command.c_str(), "join", 5) == 0){
            int lobby_id;
            cout << "   Введите ID лобби: ";
            cin >> lobby_id;
            
            sprintf(s_msg, "%d", lobby_id);
            strcat(s_msg, "|join");
            send(c_sock, s_msg, BUFF_LEN, 0);
            bzero(s_msg, BUFF_LEN);
            ans = -1;
            ans = recv(c_sock, a_msg, BUFF_LEN, 0);
            if(ans > 0){
                cli_decode_msg(a_msg, BUFF_LEN, output, request, status);
                if(strncmp(request, "allow", 6) == 0){
                    if (socket_init(lobby_sock, &c_addr) < 0){
                        cout << "Не удалось подключится к лобби. (sock error)" << endl;
                        continue;
                    }
                    int lobby_port = atoi(output);
                    sockaddr_in lobby_addr;
                    bzero(&lobby_addr, sizeof(struct sockaddr_in));
                    bcopy(&s_addr, &lobby_addr, sizeof(struct sockaddr_in));
                    lobby_addr.sin_family = AF_INET;
                    lobby_addr.sin_port = htons(lobby_port);
                    if(connect(lobby_sock, (sockaddr*)&lobby_addr, sizeof(struct sockaddr_in)) < 0){
                        cout << "Не удалось подключится к лобби." << endl;
                        continue;
                    }
                    status = WAIT_ACCEPT;
                    strcat(s_msg, login.c_str());
                    strcat(s_msg, "|join");
                    send(lobby_sock, s_msg, BUFF_LEN, 0);
                    break;
                } else {
                    cout << "Не удалось найти указанное лобби." << endl;
                    continue;
                }
                continue;
            }
        }


        //--------------------------------------------------------------------
        // Запрос списка чатов
        //--------------------------------------------------------------------
        if(strncmp(command.c_str(), "chats", 6) == 0){
            string chats_path = "chats/"; // или твоя переменная
    
            DIR* dir = opendir(chats_path.c_str());
            if(dir == NULL){
                cout << "Папка с чатами не найдена: " << chats_path << endl;
                continue;
            }
    
            vector<string> chats;
            struct dirent* entry;
    
            while((entry = readdir(dir)) != NULL){
                string filename = entry->d_name;
                if(filename.length() > 4 && filename.substr(filename.length() - 4) == ".txt"){
                    string chat_name = filename.substr(0, filename.length() - 4);
                    chats.push_back(chat_name);
                }
            }
            closedir(dir);
    
            if(chats.empty()){
                cout << "Нет активных чатов" << endl;
            } else {
                cout << "Активные чаты:" << endl;
                for(size_t i = 0; i < chats.size(); i++){
                    cout << "   " << i+1 << ") " << chats[i] << endl;
                }
            }
            continue;
        }

        //--------------------------------------------------------------------
        // Запрос чата с игроком
        //--------------------------------------------------------------------
        if(strncmp(command.c_str(), "chat", 5) == 0){
            string nick;
            cout << "Введите ник игрока: ";
            cin >> nick;
            string chatname = chats_path + nick + ".txt";
            FILE* f_chat = fopen(chatname.c_str(), "r");
            char* s_chat;
            if(f_chat == NULL){
                cout << "Чат не найден..." << endl;
                continue;
            }
            fseek(f_chat, 0, SEEK_END);
            long m_size = ftell(f_chat);
            fseek(f_chat, 0, SEEK_SET);
            s_chat = (char*)malloc(m_size + 1);
            size_t bytes_read = fread(s_chat, 1, m_size, f_chat);
            s_chat[bytes_read] = '\0';
            fclose(f_chat);
            cout << "Чат с " << nick << ":" << endl;
            if(m_size <= 0){
                cout << "Чат пуст..." << endl;
                continue;
            }
            cout << s_chat << endl;
            continue;
        }

        //--------------------------------------------------------------------
        // Запрос очистки чата с игроком
        //--------------------------------------------------------------------
        if(strncmp(command.c_str(), "clchat", 7) == 0){
            string nick;
            cout << "Введите ник игрока: ";
            cin >> nick;
            string chatname = chats_path + nick + ".txt";
            FILE* f_chat = fopen(chatname.c_str(), "w");
            if(f_chat == NULL){
                cout << "Чат не найден..." << endl;
                continue;
            }
            cout << "Чат с игроком " << nick << " очищен..." << endl;
            continue;
        }

        if(strncmp(command.c_str(), "mes", 4) == 0){
            string t_nick;
            cout << "Введите ник игрока: ";
            cin >> t_nick;
            string chatname = chats_path + t_nick + ".txt";
            strcat(s_msg, t_nick.c_str());
            strcat(s_msg, "|finduser");
            send(c_sock, s_msg, BUFF_LEN, 0);
            ans = -1;
            ans = recv(c_sock, a_msg, BUFF_LEN, 0);
            cli_decode_msg(a_msg, BUFF_LEN, output, request, status);
            if(ans > 0){
                if(strncmp(request, "nop", 4) == 0){
                    cout << "Игрок " << t_nick << " не найден." << endl;
                    continue;
                }  
                if(strncmp(request, "off", 4) == 0){
                    cout << "Игрок " << t_nick << " не в сети." << endl;
                    continue;
                }  

                close(chat_sock);
                if(socket_init(chat_sock, &c_addr) < 0){
                    cout << "Не удалось инициализировать отправку сообщения." << endl;
                    continue;
                }   

                char ip_char[BUFF_LEN] = "";
                int port = 0;
                
                sscanf(output, "%[^:]:%d", ip_char, &port);

                struct sockaddr_in t_addr;
                bzero(&t_addr, sizeof(struct sockaddr_in));
                t_addr.sin_family = AF_INET;
                inet_aton(ip_char, &t_addr.sin_addr);
                t_addr.sin_port = htons(port);  

                if(connect(chat_sock, (sockaddr*)&t_addr, sizeof(sockaddr_in)) < 0){
                    cout << "Не удалось установить соединение с " << t_nick << "." << endl;
                    continue;
                }
                bzero(s_msg, BUFF_LEN);
                char ts[BUFF_LEN];
                string mes;
                cout << "Введите ваше сообщение:" << endl;
                cin.ignore();
                cli_input(mes);
                strcat(ts, login.c_str());
                strcat(ts, ": ");
                strcat(ts, mes.c_str());
                strcat(ts, "\n");
                strcat(s_msg, ts);
                strcat(s_msg, "|sent");
                send(chat_sock, s_msg, BUFF_LEN, 0);
                recv(chat_sock, a_msg, BUFF_LEN, 0);
                cli_decode_msg(a_msg, BUFF_LEN, output, request, status);
                if(strncmp(request, "received", 9) == 0){
                    cout << "Сообщение отправлено!" << endl;
                    FILE* f_chat = fopen(chatname.c_str(), "a");
                    if(f_chat == NULL){
                        cout << "Чат не найден..." << endl;
                        bzero(ts, BUFF_LEN);
                        continue;
                    }
                    fprintf(f_chat, "%s", ts);
                    fclose(f_chat);
                }else{
                    cout << "Не удалось доставить сообщение! Попробуйте ещё раз." << endl;
                }
                bzero(ts, BUFF_LEN);
                continue;
            }
        }

        //--------------------------------------------------------------------
        //Об игре
        //--------------------------------------------------------------------
        if(strncmp(command.c_str(), "about", 6) == 0){
            cout << manual << endl;
            continue;
        }


        //--------------------------------------------------------------------
        // Выход из игры
        //--------------------------------------------------------------------

        if(strncmp(command.c_str(), "exit", 5) == 0){
            char conf = ' ';
            cout << "=======================Выход из игры!=======================" << endl;
            do{
                cout << "       Вы уверены, что хотите выйти? (y/N): ";
                cin >> conf;
            } while (conf != 'y' && conf != 'Y' && conf != 'n' && conf != 'N');
            if(conf == 'y' || conf == 'Y'){
                cout << "=======================Выход с сервера!=======================" << endl;
                strcat(s_msg, "|exit");
                send(c_sock, s_msg, BUFF_LEN, 0);
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

    }
    
    //============================================================
    // 3. Игра
    //============================================================
    for(;;)
    {
        bzero(s_msg, BUFF_LEN);
        bzero(a_msg, BUFF_LEN);
        bzero(output, BUFF_LEN);
        bzero(request, BUFF_LEN);
        rec = recv(lobby_sock, a_msg, BUFF_LEN, 0);
        if (rec == 0)
        { 
            cout << "СОЕДИНЕНИЕ ПРЕРВАНО!" << endl;
            close(lobby_sock);
            return false;
        }
        if (rec < 0)
        { 
            cout << " ОШИБКА СЕРВЕРА! (Invalid server socket)" << endl;
            close(lobby_sock);
            return false;
        }
        cli_decode_msg(a_msg, BUFF_LEN, output, request, status);


        if(strncmp(request, "over", 5) == 0){
            cout << output << endl;
            sleep(3);
            close(lobby_sock);
            return true; // возвращение в командную строку
        }

        if(strncmp(request, "common", 7) == 0){
            cout << output << endl;
           send(lobby_sock, "EmptyMes", 2, 0);
           continue;
        }

        if(status == WAIT_ACCEPT)
        {
            //continue;
        }
        if(status == PRE_TO_PLAY){
            if(strncmp(request, "getready", 9) == 0){
                char a;
                cout << "   Вы успешно присоединились к игре!" << endl;
                cout << "   Готовы начать игру?" << endl;
                do{
                    cout << "   Введите Y(Готов)/q(Выйти): ";
                    cin >> a;
                } while (a != 'y' && a != 'Y' && a != 'q' && a != 'Q');
                if (a == 'q' || a == 'Q'){
                    close(lobby_sock);
                    socket_init(lobby_sock, &c_addr);
                    return true;
                }
                if(send(lobby_sock, "|readytoplay", BUFF_LEN, 0) < 0){
                    cout << "   Не удалось отправить сообщение. Попробуйте ещё раз." << endl;
                } else {
                    cout << "   Ожидание других игроков..." << endl;
                    cout << endl;
                }
                continue;
            }
            continue;
        }
        if(status == WAITING || status == READY_TO_PLAY){
            if(output[0] != ' ' && output[0] != '\0'){
                cout << output << endl;
            }
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
                send(lobby_sock, s_msg, BUFF_LEN, 0);
                cout << endl;
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
                send(lobby_sock, s_msg, BUFF_LEN, 0);
                continue;
            }
            continue;
        }

        if(status == ANSWERING){
            if(strncmp(request, "areanswerm", 11) == 0){
                cout << "   Ваша очередь на собеседование!" << endl;
                cout << output << endl;
                int v = 0;
                do{
                    cout << "   Введите номер вакансии, на которую вы претендуете (1 - 3): ";
                    cin >> v;
                } while(v != 1 && v != 2 && v != 3);
                sprintf(s_msg, "%d", v);
                strcat(s_msg, "|claim");
                send(lobby_sock, s_msg, BUFF_LEN, 0);
                continue;
            }

            if(strncmp(request, "claim", 6) == 0){
                char r = ' ';
                do{
                    cout << "\n   Работодатель готов Вас выслушать. Введите 'Y', когда будете готовы отвечать на собеседовании: ";
                    cin >> r;
                } while(r != 'Y' && r != 'y');
                send(lobby_sock, "|readytoanswer", BUFF_LEN, 0);
                continue;
            }

            if(strncmp(request, "giveanswerm", 11) == 0){
                string resume;
                cout << "   Минута пошла! Удачи!" << endl;
                cin.ignore();
                cli_input(resume);
                strcat(s_msg, resume.c_str());
                strcat(s_msg, "|sendanswer");
                send(lobby_sock, s_msg, BUFF_LEN, 0);
                continue;
            }

            if(strncmp(request, "quest", 6) == 0){
                string answ;
                cout << output << endl;
                cout << endl;
                cout << "   Введите ваш ответ." << endl;
                cli_input(answ);
                strcat(s_msg, answ.c_str());
                strcat(s_msg, "|aquest");
                send(lobby_sock, s_msg, BUFF_LEN, 0);
                continue;
            }
            if(strncmp(request, "print", 6) == 0){
                cout << output << endl;
            }
            strcat(s_msg, "|hquest");
            send(lobby_sock, s_msg, BUFF_LEN, 0);
            continue;
        }

        if(status == QUESTIONING){
            if(strncmp(request, "gquest", 7) == 0){
                cout << "\n Ваш вопрос отправлен и дойдёт до адресата в порядке очереди." << endl;
                send(lobby_sock, " ", 2, 0);
                continue;
            }
            char h = ' ';
            do{
                cout << "\n Есть ли у Вас вопросы к соискателю? (Y/n): ";
                cin >> h;
            } while(h != 'Y' && h != 'y' && h != 'N' && h != 'n');
            if(h == 'N' || h == 'n'){
                send(lobby_sock, "|noquest", BUFF_LEN, 0);
                cout << "   Ожидайте заверешния секции с вопросами..." << endl;
                continue;
            }
            string question;
            cout << "   Напишите Ваш вопрос." << endl;
            cin.ignore();
            cli_input(question);
            strcat(s_msg, output);
            strcat(s_msg, question.c_str());
            strcat(s_msg, "|quest");
            send(lobby_sock, s_msg, BUFF_LEN, 0);
            continue;
        }

        if(status == SCORING){
            int score = 0;
            if(strncmp(request, "print", 6) == 0){
                cout << output << endl;
                send(lobby_sock, " ", 2, 0);
                continue;
            }
            if(strncmp(request, "score", 6) == 0){
                do{
                    cout << "\n Поставьте выступившему оценку от 1 до 5: ";
                    cin >> score;
                } while(score < 1 || score > 5);
                sprintf(s_msg, "%d", score);
                strcat(s_msg, "|score");
                send(lobby_sock, s_msg, BUFF_LEN, 0);
                cout << "   Оценка отправлена!" << endl;
                continue;
            } 
            send(lobby_sock, " ", 2, 0);
            continue;
        }
    }

    return true;
}

// Структкра сообщения (сервер -> клиент) "output|request|p_status"
void cli_decode_msg(char* msg, int mlen, char* output, char* request, int& status){
    bzero(output, mlen);
    bzero(request, mlen);
    int bc = get_line_b(output, msg, 0, mlen, '|');
    bc = get_line_b(request, msg, bc, mlen, '|');
    char pstat[BUFF_LEN] = "";
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
    cout << "   (для завершения введите пустую строку или 'END'):\n" << endl;
    //cin.ignore();
    text = " ";
    long unsigned int max_len = OUT_LEN;
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
        text += "   " + line + "\n";
    }
        
    if (!text.empty() && text.back() == '\n') {
        text.pop_back();
    }
}

bool commandexists(string command){
    return command == "ps" || command == "psa" || command == "rt" || command == "exit" || command == "ls"
            || command == "mkl" || command == "join" || command == "chat" || command == "chats" || command == "help"
            || command == "about" || command == "clchat" || command == "mes";
}