// СЕРВЕР (ИСХОДНЫЙ КОД)
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <unistd.h> 
#include <arpa/inet.h>
//#include <thread> 
#include <pthread.h>
#include <cstring>
#include "lobby.h"
#include "dbcontext.h"

#define MAX_USERS 100

using namespace std;

StartupDbContext* CONTEXT;

void ser_decode_msg(char* msg, int mlen, char* output, char* request);
void send_to_all(vector<int>*, char*, int);

void* user_thread(void* arg)
{
    int socket = (int)(long)arg;
    //char s_msg[BUFF_LEN] = "";
    char a_msg[BUFF_LEN] = "";
    //--------------------------------
    char output[BUFF_LEN] = "";
    char request[BUFF_LEN] = "";
    //--------------------------------
    int rec_l = 0;
    // Обработка запросов пользователей
    for(;;)
    {
        recv(rec_l, a_msg, BUFF_LEN, 0);
        if(rec_l <= 0){
            close(socket);
            pthread_exit(0);
        }
        ser_decode_msg(a_msg, BUFF_LEN, output, request);

        if(strncmp(request, "getplayers", 11) == 0){
            send(socket, CONTEXT->get_players_on(), BUFF_LEN, 0);
            continue;
        }

        if(strncmp(request, "getallplayers", 14) == 0){
            send(socket, CONTEXT->get_players_all(), BUFF_LEN, 0);
            continue;
        }

        if(strncmp(request, "rate", 5) == 0){
            send(socket, CONTEXT->get_rating(), BUFF_LEN, 0);
            continue;
        }

        if(strncmp(request, "makelob", 8) == 0){
            int num = 0;
            char c = ' ';
            char* name = "";
            int count = 0;
            while(c != ':'){
                c = request[count];
                if(c == '\0'){
                    num = -1;
                    break;
                }
                sprintf(name, "%c", c);
                count++;
            }
            if(num < 0){
                send(socket, "|NO", BUFF_LEN, 0);
                continue;
            }
            c = request[count + 1];
            if(c == '3'){
                num = 3;
            }else if(c == '4'){
                num = 4;
            }else if(c == '5'){
                num = 5;
            }else if(c == '6'){
                num = 6;
            }

            if(num < 3 || num > 6){
                send(socket, "|NO", BUFF_LEN, 0);
                continue;
            }

            if(CONTEXT->add_lobby(name, num)){
                send(socket, "|success", BUFF_LEN, 0);
            }else{
                send(socket, "|NO", BUFF_LEN, 0);
            }
            continue;
        }

        if(strncmp(request, "join", 5) == 0){
            int id = atoi(output);
            if(CONTEXT->join_lobby(id)){
                send(socket, "|allow", BUFF_LEN, 0);
            }else{
                send(socket, "|NO", BUFF_LEN, 0);
            }
            continue;
        }

        if(strncmp(request, "getchats", 9) == 0){
            send(socket, CONTEXT->get_chats(), BUFF_LEN, 0);
            continue;
        }

        // чат с игроком открывается в обход сервера в отдельном соединении между игроками.

        send(socket, " ", 2, 0); // пустое сообщение, чтобы если что, клиент не застрял
    }
    close(socket);
    pthread_exit(0);
}

int main()
{
    srand(time(NULL));
    pid_t server_id = getpid();
    cout << "   ID сервера: " << server_id << endl;
    struct sockaddr_in s_addr;

    int ser_socket = socket(AF_INET, SOCK_STREAM, 0);
    int usr_socket = 0; 

    if (ser_socket < 0) {
        cout << "   ОШИБКА: НЕ УДАЛОСЬ ЗАПУСТИТЬ СЕРВЕР!" << endl;
        return -1;
    }

    bzero((char*)&s_addr, sizeof(struct sockaddr_in));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_port = 0;

    if(bind(ser_socket, (sockaddr*)&s_addr, sizeof(struct sockaddr_in)) < 0){
        cout << "   ОШИБКА: НЕ УДАЛОСЬ ИНИЦИАЛИЗИРОВАТЬ СЕРВЕР!" << endl;
        return -1;
    }
    unsigned int s_len = sizeof(struct sockaddr_in);
    if (getsockname(ser_socket, (struct sockaddr*)&s_addr, &s_len) < 0){
        cout << "   ОШИБКА: НЕ УДАЛОСЬ НАЙТИ ПОРТ СЕРВЕРА!" << endl;
        return -1;
    }
    cout << "   АДРЕС СЕРВЕРА: " << inet_ntoa(s_addr.sin_addr) << endl;
    cout << "   ПОРТ СЕРВЕРА: " << ntohs(s_addr.sin_port) << endl;

    if(listen(ser_socket, MAX_USERS) < 0){
        cout << "   ОШИБКА: НЕ УДАЛОСЬ ОТКРЫТЬ СЕРВЕР!" << endl;
        return -1;
    }
    string admin;
    string pswrd;
    cout << "Ser login: ";
    cin >> admin;
    cout << "Ser password: ";
    cin >> pswrd;
    CONTEXT = new StartupDbContext(S_ADDRESS, admin, pswrd, S_PORT);
    // Приём пользователей
    for(;;)
    {
        usr_socket = accept(ser_socket, 0, 0);
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, user_thread, (void*)&usr_socket);
        pthread_detach(thread_id);

    }
    close(ser_socket);
    return 0;
}

// Структкра сообщения (клиент -> сервер) "output|request"
void ser_decode_msg(char* msg, int mlen, char* output, char* request){
    bzero(output, mlen);
    bzero(request, mlen);
    int bc = get_line_b(output, msg, 0, mlen, '|');
    bc = get_line_b(request, msg, bc, mlen, ' ');
}