#include "dbcontext.h"


StartupDbContext::StartupDbContext(string _address, int _port): address(_address), port(_port){

}

StartupDbContext::~StartupDbContext(){

}

bool StartupDbContext::auth(string login, string password){

}
bool StartupDbContext::reg(string login, string password){

}

char* StartupDbContext::get_games() const{

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


