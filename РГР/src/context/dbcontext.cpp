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
    string init_script = "CREATE TABLE IF NOT EXISTS USERS (ID Serial Primary Key," + 
                        (string)"socket int," +
	                    "login varchar(256)," +
	                    "password varchar(256)," +
	                    "online int," +
                        "score int);" +
                        "CREATE TABLE IF NOT EXISTS GAMES (" +
	                    "ID Serial Primary Key," +
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

    string q = "SELECT * FROM users WHERE login = $1";
    work w(*conn);
    result res = w.exec_params(q, login);
    w.commit();

    if(res.empty()){
        return L_WRONG;
    }

    string stored_hash = res[0]["password"].as<string>();
    if (crypto_pwhash_str_verify(stored_hash.c_str(),password.c_str(),password.length()) != 0){
        return L_WRONG;
    }


    if(res[0]["online"].as<int>() == 1){
        return L_ONLINE;
    }

    string update_q = "UPDATE users SET online = $1 WHERE login = $2";
    w.exec_params(update_q, 1, login);
    w.commit();

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


int StartupDbContext::logout(string login){
    if(!isConnected()) return -1;
    
    work w(*conn);
    string q = "UPDATE users SET online = $1 WHERE login = $2";
    w.exec_params(q, 0, login);
    w.commit();
    
    return 0;
}

string StartupDbContext::get_lobbies(){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return "";
    }
    string q = "SELECT * FROM games";
    work w(*conn);
    result res = w.exec(q);
    w.commit();
    char* answer = "";
    strcat(answer, "---------------------------------------------------\n");
    strcat(answer, "   ID      Название        К-во игроков\n");
    strcat(answer, "---------------------------------------------------\n");
    for(size_t i = 1; i <= res.size(); i++){
        sprintf(answer, "   %d     %s      %d\n", res[i]["id"].as<int>(), res[i]["name"].as<string>(), 
                                                                            res[i]["size"].as<int>());
    }
    return answer;

}
string StartupDbContext::get_players_on(){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return "";
    }
    string q = "SELECT * FROM users WHERE online = $1";
    work w(*conn);
    result res = w.exec_params(q, 1);
    w.commit();
    string answer;
    answer += "---------------------------------------------------\n";
    answer += "    ID      Логин\n";
    answer += "---------------------------------------------------\n";
    for(size_t i = 0; i < res.size(); i++){
        char buff[256];
        sprintf(buff, "     %d     %s\n", res[i]["id"].as<int>(), res[i]["login"].as<string>().c_str());
        answer += buff;
    }
    return answer;
}
string StartupDbContext::get_players_all(){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return "";
    }
    string q = "SELECT * FROM users WHERE online = $1";
    work w(*conn);
    result res = w.exec_params(q, 1);
    w.commit();
    char* answer = "";
    strcat(answer, "---------------------------------------------------\n");
    strcat(answer, "   ID      Логин        Статус\n");
    strcat(answer, "---------------------------------------------------\n");
    for(size_t i = 1; i <= res.size(); i++){
        sprintf(answer, "   %d     %s       ", res[i]["id"], res[i]["login"]);
        if(res[i]["online"].as<int>()){
            sprintf(answer, "Онлайн");
        }else{
            sprintf(answer, "Не в сети");
        }
        strcat(answer, "\n");
    }
    return answer;
}

string StartupDbContext::get_rating(){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return "";
    }
    string q = "SELECT * FROM users ORDER BY score";
    work w(*conn);
    result res = w.exec_params(q, 1);
    w.commit();
    char* answer = "";
    strcat(answer, "---------------------------------------------------\n");
    strcat(answer, "    Место   ID      Логин        Рейтинг\n");
    strcat(answer, "---------------------------------------------------\n");
    for(size_t i = 1; i <= res.size(); i++){
        sprintf(answer, "   %d)   %d     %s       %d\n", i+1, res[i]["id"], res[i]["login"], res[i]["score"]);
    }
    return answer;

}
string StartupDbContext::get_chats(){

}

bool StartupDbContext::add_lobby(string name, int num){

}
bool StartupDbContext::join_lobby(int id){// bool ?
}


