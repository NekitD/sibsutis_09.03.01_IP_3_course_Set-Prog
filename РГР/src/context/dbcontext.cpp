// КОНТЕКСТ БАЗЫ ДАННЫХ (ИСХОДНЫЙ КОД)
#include "dbcontext.h"


StartupDbContext::StartupDbContext(string _address, int _port, string _database, string _user, string _password):
address(_address), port(_port), database(_database), user(_user), password(_password)
{
    string conn_str = "host=" + address + " " +
                    "port=" + to_string(port) + " " +
                    "dbname=" + database + " " +
                    "user=" + user + " " +
                    "password=" + password + " ";

    conn = new connection(conn_str);
    if(!conn->is_open()){
        cout << "ОШИБКА: НЕ УДАЛОСЬ УСТАНОВИТЬ СОЕДИНЕНИЕ" << endl;
        exit(1);
    }
    cout << "СОЕДИНЕНИЕ С БАЗОЙ ДАННЫХ УСПЕШНО УСТАНОВЛЕНО!" << endl;
}

StartupDbContext::~StartupDbContext(){
    if(conn){
        conn->close();
        delete conn;
        conn = nullptr;
    }
}

connection* StartupDbContext::getConnection(){
    return conn;
}

bool StartupDbContext::isConnected(){
    return (conn != nullptr && conn->is_open());
}

result StartupDbContext::query(const string& script){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return result();
    }

    work w(*conn);
    result res = w.exec(script);
    w.commit();
    return res;
}

void StartupDbContext::exec(const string& script){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return;
    }
    work w(*conn);
    w.exec(script);
    w.commit();
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


