// СТАРТАП-ЛОББИ (БИБЛИОТЕКА)
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include <pthread.h>
#include "game.h"
#include <cstring>

using namespace std;

typedef struct player_args {
    int socket;
    Game* game;
    vector<int>* subs;
    int lobby_id;
    StartupDbContext* context;
    pthread_mutex_t* mutex;
} player_args;


typedef struct lobby_args {
    int id;
    StartupDbContext* context;
    int size;
} lobby_args;


void ser_decode_msg(char* msg, int mlen, char* output, char* request);
void send_to_all(vector<int>*, char*, int);
void* player_thread(void* arg);
void* lobby_thread(void* arg);