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

using namespace std;


void ser_decode_msg(char* msg, int mlen, char* output, char* request);
void send_to_all(vector<int>*, char*, int);

void* user_thread(void* arg)
{
    int socket = (int)(long)arg;
    for(;;)
    {

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

    int sm_socket = socket(AF_INET, SOCK_STREAM, 0);
    int ss_socket = 0; 

    if (sm_socket < 0) {
        cout << "   ОШИБКА: НЕ УДАЛОСЬ СОЗДАТЬ ИГРУ!" << endl;
        return -1;
    }

    bzero((char*)&s_addr, sizeof(struct sockaddr_in));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_port = 0;

    if(bind(sm_socket, (sockaddr*)&s_addr, sizeof(struct sockaddr_in)) < 0){
        cout << "   ОШИБКА: НЕ УДАЛОСЬ ИНИЦИАЛИЗИРОВАТЬ СЕРВЕР!" << endl;
        return -1;
    }
    unsigned int s_len = sizeof(struct sockaddr_in);
    if (getsockname(sm_socket, (struct sockaddr*)&s_addr, &s_len) < 0){
        cout << "   ОШИБКА: НЕ УДАЛОСЬ НАЙТИ ПОРТ СЕРВЕРА!" << endl;
        return -1;
    }
    cout << "   АДРЕС СЕРВЕРА: " << inet_ntoa(s_addr.sin_addr) << endl;
    cout << "   ПОРТ СЕРВЕРА: " << ntohs(s_addr.sin_port) << endl;

    if(listen(sm_socket, MAX_P) < 0){
        cout << "   ОШИБКА: НЕ УДАЛОСЬ ОТКРЫТЬ ИГРУ!" << endl;
        return -1;
    }

    int status;
    for(;;)
    {
        ss_socket = accept(sm_socket, 0, 0);
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, user_thread, (void*)&ss_socket);
        pthread_detach(thread_id);

        if(ss_socket > 0){
            cout << "DEBUG: Присоединение" << endl;
        }

        // СЕРВЕР ПОКА ПУСТ
    }
    close(sm_socket);
    return 0;
}

// Структкра сообщения (клиент -> сервер) "output|request"
void ser_decode_msg(char* msg, int mlen, char* output, char* request){
    bzero(output, mlen);
    bzero(request, mlen);
    int bc = get_line_b(output, msg, 0, mlen, '|');
    bc = get_line_b(request, msg, bc, mlen, ' ');
}