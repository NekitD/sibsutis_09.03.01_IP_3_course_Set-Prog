#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFF_LEN 4096
#define CHATS_DIR "info/chats/"

using namespace std;

typedef struct chat_args{
    int socket;
    string login;
}chat_args;


void* msg_thread(void* args);
void* msg_accept_thread(void* args);