#include "chat.h"

void* msg_thread(void* args){
    chat_args* margs = (chat_args*)args;
    int socket = margs->socket;
    string login = margs->login;
    char amsg[BUFF_LEN];
    char nick[BUFF_LEN];
    char smsg[BUFF_LEN];
    if(recv(socket, amsg, BUFF_LEN, 0) > 0){
        send(socket, "|received", BUFF_LEN, 0);
        sscanf(amsg, "%[^:]:%[^|]", nick, smsg);
        string chatname = CHATS_DIR;
        chatname += login;
        chatname += "/";
        chatname += nick;
        chatname += ".txt";
        FILE* f_chat = fopen(chatname.c_str(), "a");
        fprintf(f_chat, smsg);
        fclose(f_chat);
    }
    delete margs;
    close(socket);
    pthread_exit(0);
}


void* msg_accept_thread(void* args){
    chat_args* cargs = (chat_args*)args;
    int socket = cargs->socket;
    string login = cargs->login;

    for(;;)
    {
        int ssock = accept(socket, 0, 0);
        pthread_t mt;
        
        chat_args* margs = new chat_args();
        margs->socket = ssock;
        margs->login = login;
        
        pthread_create(&mt, NULL, msg_thread, (void*)margs);
        pthread_detach(mt);
    }
    close(socket);
    pthread_exit(0);
}