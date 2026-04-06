// КОНТЕКСТ БАЗЫ ДАННЫХ (ИСХОДНЫЙ КОД)
#include "dbcontext.h"


StartupDbContext::StartupDbContext(string _address, string admin, string pass, int _port): address(_address), port(_port){
    if(mysql_library_init(0, NULL, NULL)){
        cout << "ОШИБКА: НЕ УДАЛОСЬ ИНИЦИАЛИЗИРОВАТЬ SQL-библиотеку" << endl;
        exit(1);
    }
    connect = mysql_init(NULL);
    if(connect == NULL){
        cout << "ОШИБКА: НЕ УДАЛОСЬ ИНИЦИАЛИЗИРОВАТЬ СОЕДИНЕНИЕ" << endl;
        exit(1);
    }
    if(mysql_real_connect(connect, S_ADDRESS, admin.c_str(), pass.c_str(), DATABASE, 0, NULL, 0) == NULL){
        cout << "ОШИБКА: НЕ УДАЛОСЬ УСТАНОВИТЬ СОЕДИНЕНИЕ" << endl;
        exit(1);
    }
    cout << "СОЕДИНЕНИЕ С БАЗОЙ ДАННЫХ УСПЕШНО УСТАНОВЛЕНО!" << endl;
}

StartupDbContext::~StartupDbContext(){
    mysql_close(connect);
    delete connect;
    mysql_library_end();
}

bool StartupDbContext::auth(string login, string password){

}
bool StartupDbContext::reg(string login, string password){

}

char* StartupDbContext::get_lobbies() const{

}
char* StartupDbContext::get_players_on()const{

}
char* StartupDbContext::get_players_all()const{

}

char* StartupDbContext::get_rating() const{

}
char* StartupDbContext::get_chats() const{

}

bool StartupDbContext::add_lobby(string name, int num){

}
bool StartupDbContext::join_lobby(int id){// bool ?
}


