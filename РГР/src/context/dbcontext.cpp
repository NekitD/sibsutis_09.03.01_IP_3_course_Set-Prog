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
	                    "size int," + "busy int, port int);";
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
    if(!isConnected()){
        cout << "NO CON" << endl;
        return -1;
    }
    
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
    string q = "SELECT * FROM games ORDER BY id";
    work w(*conn);
    result res = w.exec(q);
    w.commit();
    string answer;
    answer += "===================================================\n";
    answer += "     ID           Название          К-во игроков\n";
    answer += "===================================================\n";
    for(size_t i = 0; i < res.size(); i++){
        char buff[256];
        sprintf(buff, "   %d          %s          %d\n", res[i]["id"].as<int>(), res[i]["name"].as<string>(), 
                                                                            res[i]["size"].as<int>());
        answer += buff;
        answer += "---------------------------------------------------\n";
    }
    return answer;

}
string StartupDbContext::get_players_on(){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return "";
    }
    string q = "SELECT * FROM users WHERE online = $1 ORDER BY id";
    work w(*conn);
    result res = w.exec_params(q, 1);
    w.commit();
    string answer;
    answer += "===================================================\n";
    answer += "           ID                   Логин          \n";
    answer += "===================================================\n";
    for(size_t i = 0; i < res.size(); i++){
        char buff[256];
        sprintf(buff, "           %d                    %s\n", res[i]["id"].as<int>(), res[i]["login"].as<string>().c_str());
        answer += buff;
        answer += "---------------------------------------------------\n";
    }
    return answer;
}
string StartupDbContext::get_players_all(){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return "";
    }
    string q = "SELECT * FROM users ORDER BY id";
    work w(*conn);
    result res = w.exec(q);
    w.commit();
    string answer;
    answer += "===================================================\n";
    answer += "        ID          Логин        Статус\n";
    answer += "===================================================\n";
    for(size_t i = 0; i < res.size(); i++){
        char buff[256];
        char buff2[20];
        sprintf(buff, "       %d            %s          ", res[i]["id"].as<int>(), res[i]["login"].as<string>().c_str());
        if(res[i]["online"].as<int>() == 1){
            sprintf(buff2, "%s\n", "Онлайн");
        }else{
            sprintf(buff2, "%s\n", "Не в сети");
        }
        strcat(buff, buff2);
        answer += buff;
        answer += "---------------------------------------------------\n";
    }
    return answer;
}

string StartupDbContext::get_rating(){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return "";
    }
    string q = "SELECT * FROM users ORDER BY score DESC, id ASC";
    work w(*conn);
    result res = w.exec(q);
    w.commit();
    string answer;
    answer += "===================================================\n";
    answer += "    Место       ID          Логин        Рейтинг\n";
    answer += "===================================================\n";

    int place = 1;
    int score = res[0]["score"].as<int>();

    for(size_t i = 0; i < res.size(); i++){
        int cscore = res[i]["score"].as<int>();
        if(cscore != score){
            place++;
            score = cscore;
        }
        char buff[256];
        sprintf(buff, "     %ld)         %d          %s             %d\n", place, res[i]["id"].as<int>(), 
                                                res[i]["login"].as<string>().c_str(), cscore);
        answer += buff;
        answer += "---------------------------------------------------\n";
    }
    return answer;

}
string StartupDbContext::get_chats(){

}

bool StartupDbContext::add_lobby(string name, int num){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return false;
    }
    string q = "INSERT INTO games (name, size, busy) VALUES ($1, $2, $3)";
    work w(*conn);
    w.exec_params(q, name, num, 0);
    w.commit();
    q = "SELECT FROM games WHERE name = $1, size = $2";
    result res = w.exec_params(q, name, num);
    w.commit();
    if(res.empty()){
        return false;
    }
    return true;

}
int StartupDbContext::join_lobby(int id){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return -1;
    }
    string q = "SELECT * FROM games WHERE id = $1";
    work w(*conn);
    result res = w.exec_params(q, id);
    w.commit();
    if(res.empty()){
        return -1;
    }
    return res[0]["port"].as<int>();
}


