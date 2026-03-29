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
    cout << "Введите ваш ник: ";
    cin >> nick;
    cout << "Введите адрес игры (без пробелов): ";
    cin >> g_host;
    cout << endl << "Введите порт игры: ";
    cin >> g_port;

    hp = gethostbyname(g_host);

    bzero((char*)&s_addr, (sizeof(struct sockaddr_in)));
    s_addr.sin_family = AF_INET;
    bcopy(hp->h_addr, &s_addr.sin_addr, hp->h_length) ;
    s_addr.sin_port = htons(g_port);

    if (connect(c_sock, (sockaddr*)&s_addr, sizeof(struct sockaddr_in)) < 0) {
        printf("СОЕДИНЕНИЕ С СЕРВЕРОМ НЕ УДАЛОСЬ!\n");
        return -1;
    }
   while(true){
        
    }
    char s_msg[BUFF_LEN] = "";
    char a_msg[BUFF_LEN] = "";
    strcat(s_msg, nick.c_str());
    strcat(s_msg, "|join");

    if(send(c_sock, s_msg, BUFF_LEN, 0) < 0){
        cout << "НЕ УДАЛОСЬ ОТПРАВИТЬ ДАННЫЕ ИГРОКА!" << endl;
        return -1;
    }

    int status = WAIT_ACCEPT;
    int rec = 0;
    //---------------------------------------------------------------------
    for(;;){
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
        if(status == WAIT_ACCEPT)
        {
            if (strncmp(a_msg,"accepted",9) != 0)
            { 
                continue;
            }
            status = PRE_TO_PLAY;
            cout << "Нажмите Enter, когда будете готовы начать игру..." << endl;
        }
        if(status == PRE_TO_PLAY){

        }
    }
    close(c_sock);
    return 0;
}