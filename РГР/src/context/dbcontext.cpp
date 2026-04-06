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
    string init_script = "CREATE TABLE IF NOT EXISTS USERS (ID int Not Null Primary Key," + 
                        (string)"socket int," +
	                    "login varchar(256)," +
	                    "password varchar(256)," +
	                    "online int);" +
                        "score int);" +
                        "CREATE TABLE IF NOT EXISTS GAMES (" +
	                    "ID int Not Null Primary Key," +
	                    "socket int," +
	                    "name varchar(256)," +
	                    "size int);";
    work w(*conn);
    result res = w.exec(init_script);
    w.commit();
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

int StartupDbContext::auth(string login, string password){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return -1;
    }
    string q = "SELECT * FROM users WHERE login = $1 AND password = $2";
    work w(*conn);
    result res = w.exec_params(q, login, password);
    w.commit();

    if(res.empty()){
        return L_WRONG;
    }
    if(res[0]["online"].as<int>() == 1){
        return L_ONLINE;
    }

    return L_SUCCESS;
}

int StartupDbContext::reg(string login, string password){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return -1;
    }
    string q = "SELECT * FROM users WHERE login = $1";
    work w(*conn);
    result res = w.exec_params(q, login);
    w.commit();

    if(!res.empty()){
        return R_BUSY;
    }

    q = "INSERT INTO users (login, password, online, score) VALUES ($1, $2, $3, $4)";
    res = w.exec_params(q, login, password, 0, 0);


    q = "SELECT * FROM users WHERE login = $1 AND password = $2";

    res = w.exec_params(q, login, password);
    w.commit();

    if(res.empty()){
        return R_FAIL;
    }

    return R_SUCCESS;
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


