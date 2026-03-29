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

static struct termios original_termios;


int set_noncanonical_mode() {
    struct termios new_termios;
    
    if (tcgetattr(STDIN_FILENO, &original_termios) != 0) {
        return -1;
    }
    
    new_termios = original_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 1; 
    new_termios.c_cc[VTIME] = 0; 
    
    return tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

int restore_terminal_mode() {
    return tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

int readkey(enum keys *address) {
    char ch;
    int bytes_read = read(STDIN_FILENO, &ch, 1);
    
    if (bytes_read <= 0) {
        return -1;
    }
    
    if (ch == '\r' || ch == '\n') { 
        *address = ENTER;
    } else {
        return -1;
    }
    
    return 0;
}



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
    cout << "Введите ваш ник (без пробелов): ";
    cin >> nick;
    searchgame:
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
    enum keys key_a;
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
            if (strncmp(a_msg,"FULL",4) == 0)
            { 
                cout << "В этой игре больше нет свободных мест." << endl;
                goto searchgame;
            }
            if (strncmp(a_msg,"accepted",9) != 0)
            { 
                continue;
            }
            status = PRE_TO_PLAY;
            cout << "Нажмите Enter, когда будете готовы начать игру... (или Esc, если хотите выйти)" << endl;
            if (set_noncanonical_mode() != 0){
                cout << "Ошибка терминала!" << endl;
                return -1;
            }
        }
        if(status == PRE_TO_PLAY){
            int rk = readkey(&key_a);
            if(rk == ESC){
                cout << "Выход из игры..." << endl;
                restore_terminal_mode();
                return 0;
            }
            if (rk == 0 || rk == ENTER){
                status = READY_TO_PLAY;
                restore_terminal_mode();
                if(send(c_sock, "readytoplay", BUFF_LEN, 0) < 0){
                    cout << "Не удалось отправить сообщение. Попробуйте ещё раз." << endl;
                } else {
                    cout << "Ожидание других игроков..." << endl;
                }
            }
        }
        if(status = READY_TO_PLAY){

        }
    }
    close(c_sock);
    return 0;
}