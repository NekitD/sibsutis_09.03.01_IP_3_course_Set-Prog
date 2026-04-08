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
	                    "size int," + "busy int, port int, began bool, creator int);";
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
    string q2 = "SELECT * FROM users WHERE id = $1";
    work w(*conn);
    result res = w.exec(q);
    w.commit();
    result res1 = w.exec_params(q, res1[0]["creator"].as<int>());
    w.commit();
    string answer;
    string creator = res1[0]["login"].as<string>();
    answer += "============================================================================================\n";
    answer += "     ID           Название           Создатель          К-во игроков         Игра началась\n";
    answer += "=============================================================================================\n";
    for(size_t i = 0; i < res.size(); i++){
        char buff[256];
        bool stat = res[i]["began"].as<bool>();
        sprintf(buff, "   %d          %s           %s          %d/%d         \n", 
            res[i]["id"].as<int>(), res[i]["name"].as<string>(), creator, 
            res[i]["busy"].as<int>(),res[i]["size"].as<int>());
        if(stat){
            char buff2[3] = "*";
            strcat(buff, buff2);
        }
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

int StartupDbContext::add_lobby(string creator, string name, int num){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return -1;
    }
    string q = "INSERT INTO games (name, size, busy, creator) VALUES ($1, $2, $3, $4)";
    string q1 = "SELECT * FROM users WHERE login = $1";
    work w(*conn);
    result r = w.exec_params(q, creator);
    w.commit();

    int cid = r[0]["id"].as<int>();

    w.exec_params(q, name, num, 0, cid);
    w.commit();
    q = "SELECT * FROM games WHERE name = $1, size = $2, began = $3, creator = $4";
    result res = w.exec_params(q, name, num, false, cid);
    w.commit();
    if(res.empty()){
        return -1;
    }
    q = "SELECT * FROM games ORDER BY id DESC";
    res = w.exec(q);
    w.commit();
    return res[0]["id"].as<int>();

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

void StartupDbContext::set_lobby_num(int id, int nv){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return;
    }
    string q = "UPDATE games SET busy = $1 WHERE id = $2";
    work w(*conn);
    w.exec_params(q, nv, id);
    w.commit();
}

void StartupDbContext::rm_lobby(int id){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return;
    }
    string q = "DELETE FROM games WHERE id = $1";
    work w(*conn);
    w.exec_params(q, id);
    w.commit();
} 

void StartupDbContext::set_lobby_port(int id, int port){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return;
    }
    string q = "UPDATE games SET port = $1 WHERE id = $2";
    work w(*conn);
    w.exec_params(q, port, id);
    w.commit();
}

void StartupDbContext::set_lobby_status(int id, bool ready){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return;
    }
    string q = "UPDATE games SET began = $1 WHERE id = $2";
    work w(*conn);
    w.exec_params(q, ready, id);
    w.commit();
}

void StartupDbContext::add_player_score(string login, int score){
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        return;
    }
    string q = "UPDATE users SET score = $1 WHERE login = $2";
    string q1 = "SELECT * FROM users WHERE login = $1";
    work w(*conn);
    result r = w.exec_params(q1, login);
    result res = w.exec_params(q, r[0]["score"].as<int>() + score, login);
    w.commit();
}


