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
    pthread_mutex_init(&db_mutex, 0);
    if(!conn->is_open()){
        cout << "ОШИБКА: НЕ УДАЛОСЬ УСТАНОВИТЬ СОЕДИНЕНИЕ" << endl;
        exit(1);
    }
    string init_script = "CREATE TABLE IF NOT EXISTS USERS (ID Serial Primary Key," + 
                        (string)"login varchar(256)," +
	                    "password varchar(256)," +
	                    "online int," +
                        "score int," +
                        "address varchar(255)," +
                        "port int);" +
                        "CREATE TABLE IF NOT EXISTS GAMES (" +
	                    "ID Serial Primary Key," +
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
        pthread_mutex_destroy(&db_mutex);
    }
}

connection* StartupDbContext::getConnection(){
    return conn;
}

bool StartupDbContext::isConnected(){
    return (conn != nullptr);
}

int StartupDbContext::auth(string login, string password, string addr, int port){
    pthread_mutex_lock(&db_mutex);
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        pthread_mutex_unlock(&db_mutex);
        return -1;
    }

    string q = "SELECT * FROM users WHERE login = $1";
    work w(*conn);
    result res = w.exec_params(q, login);
    w.commit();

    if(res.empty()){
        pthread_mutex_unlock(&db_mutex);
        return L_WRONG;
    }

    string stored_hash = res[0]["password"].as<string>();
    if (crypto_pwhash_str_verify(stored_hash.c_str(),password.c_str(),password.length()) != 0){
        pthread_mutex_unlock(&db_mutex);
        return L_WRONG;
    }


    if(res[0]["online"].as<int>() == 1){
        pthread_mutex_unlock(&db_mutex);
        return L_ONLINE;
    }

    string update_q = "UPDATE users SET online = $1, address = $2, port = $3 WHERE login = $4";
    w.exec_params(update_q, 1, addr, port, login);
    w.commit();
    pthread_mutex_unlock(&db_mutex);
    return L_SUCCESS;
}

int StartupDbContext::reg(string login, string password){
    pthread_mutex_lock(&db_mutex);
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        pthread_mutex_unlock(&db_mutex);
        return -1;
    }
    string q = "SELECT * FROM users WHERE login = $1";
    work w(*conn);
    result res = w.exec_params(q, login);
    w.commit();

    if(!res.empty()){
        pthread_mutex_unlock(&db_mutex);
        return R_BUSY;
    }

    q = "INSERT INTO users (login, password, online, score) VALUES ($1, $2, $3, $4)";
    res = w.exec_params(q, login, password, 0, 0);
    w.commit();


    q = "SELECT * FROM users WHERE login = $1 AND password = $2";

    res = w.exec_params(q, login, password);
    w.commit();

    if(res.empty()){
        pthread_mutex_unlock(&db_mutex);
        return R_FAIL;
    }
    pthread_mutex_unlock(&db_mutex);
    return R_SUCCESS;
}


int StartupDbContext::logout(string login){
    pthread_mutex_lock(&db_mutex);
    if(!isConnected()){
        cout << "NO CON" << endl;
        pthread_mutex_unlock(&db_mutex);
        return -1;
    }
    
    work w(*conn);
    string q = "UPDATE users SET online = $1, address = null, port = null WHERE login = $2";
    w.exec_params(q, 0, login);
    w.commit();
    pthread_mutex_unlock(&db_mutex);
    return 0;
}

string StartupDbContext::get_lobbies(){
    pthread_mutex_lock(&db_mutex);
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        pthread_mutex_unlock(&db_mutex);
        return "";
    }
    
    string q = "SELECT g.id, g.name, g.size, g.busy, g.began, u.login as creator "
               "FROM games g JOIN users u ON g.creator = u.id "
               "ORDER BY g.id";
    
    work w(*conn);
    result res = w.exec(q);
    w.commit();
    pthread_mutex_unlock(&db_mutex);
    string answer;
    answer += "=============================================================================================\n";
    answer += "     ID           Название           Создатель          К-во игроков         Игра началась\n";
    answer += "=============================================================================================\n";
    
    for(int i = 0; i < res.size(); i++){
        char buff[512];
        bool stat = res[i]["began"].as<bool>();
        snprintf(buff, sizeof(buff), "      %d             %s              %s                %d/%d               %s\n", 
            res[i]["id"].as<int>(), 
            res[i]["name"].as<string>().c_str(), 
            res[i]["creator"].as<string>().c_str(),
            res[i]["busy"].as<int>(),
            res[i]["size"].as<int>(),
            stat ? "*" : " "
        );
        answer += buff;
        answer += "---------------------------------------------------------------------------------------------\n";
    }
    return answer;
}
string StartupDbContext::get_players_on(){
    pthread_mutex_lock(&db_mutex);
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        pthread_mutex_unlock(&db_mutex);
        return "";
    }
    string q = "SELECT * FROM users WHERE online = $1 ORDER BY id";
    work w(*conn);
    result res = w.exec_params(q, 1);
    w.commit();
    pthread_mutex_unlock(&db_mutex);
    string answer;
    answer += "===================================================\n";
    answer += "           ID                   Логин          \n";
    answer += "===================================================\n";
    for(int i = 0; i < res.size(); i++){
        char buff[256];
        sprintf(buff, "           %d                    %s\n", res[i]["id"].as<int>(), res[i]["login"].as<string>().c_str());
        answer += buff;
        answer += "---------------------------------------------------\n";
    }
    return answer;
}
string StartupDbContext::get_players_all(){
    pthread_mutex_lock(&db_mutex);
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        pthread_mutex_unlock(&db_mutex);
        return "";
    }
    string q = "SELECT * FROM users ORDER BY id";
    work w(*conn);
    result res = w.exec(q);
    w.commit();
    pthread_mutex_unlock(&db_mutex);
    string answer;
    answer += "===================================================\n";
    answer += "        ID          Логин        Статус\n";
    answer += "===================================================\n";
    for(int i = 0; i < res.size(); i++){
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
    pthread_mutex_lock(&db_mutex);
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        pthread_mutex_unlock(&db_mutex);
        return "";
    }
    string q = "SELECT * FROM users ORDER BY score DESC, id ASC";
    work w(*conn);
    result res = w.exec(q);
    w.commit();
    pthread_mutex_unlock(&db_mutex);
    string answer;
    answer += "===================================================\n";
    answer += "    Место       ID          Логин        Рейтинг\n";
    answer += "===================================================\n";

    int place = 1;
    int score = res[0]["score"].as<int>();

    for(int i = 0; i < res.size(); i++){
        int cscore = res[i]["score"].as<int>();
        if(cscore != score){
            place++;
            score = cscore;
        }
        char buff[256];
        sprintf(buff, "     %d)         %d          %s             %d\n", place, res[i]["id"].as<int>(), 
                                                res[i]["login"].as<string>().c_str(), cscore);
        answer += buff;
        answer += "---------------------------------------------------\n";
    }
    return answer;

}

int StartupDbContext::add_lobby(string creator, string name, int num){
    pthread_mutex_lock(&db_mutex);
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        pthread_mutex_unlock(&db_mutex);
        return -1;
    }
    
    work w(*conn);

    string q1 = "SELECT id FROM users WHERE login = $1";
    string q2 = "INSERT INTO games (name, size, busy, creator, began) VALUES ($1, $2, $3, $4, $5) RETURNING id";
    result r = w.exec_params(q1, creator);
    w.commit();
    if(r.empty()){
        pthread_mutex_unlock(&db_mutex);
        return -1;
    }
    int cid = r[0]["id"].as<int>();
        
    result res = w.exec_params(q2,name, num, 0, cid, false);  
    int game_id = res[0]["id"].as<int>();
    w.commit();
    pthread_mutex_unlock(&db_mutex);    
    return game_id;
}

int StartupDbContext::join_lobby(int id){
    pthread_mutex_lock(&db_mutex);
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        pthread_mutex_unlock(&db_mutex);
        return -1;
    }
    string q = "SELECT * FROM games WHERE id = $1";
    work w(*conn);
    result res = w.exec_params(q, id);
    w.commit();
    pthread_mutex_unlock(&db_mutex);
    if(res.empty()){
        return -1;
    }
    return res[0]["port"].as<int>();
}

void StartupDbContext::set_lobby_num(int id, int nv){
    pthread_mutex_lock(&db_mutex);
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        pthread_mutex_unlock(&db_mutex);
        return;
    }
    string q = "UPDATE games SET busy = $1 WHERE id = $2";
    work w(*conn);
    w.exec_params(q, nv, id);
    w.commit();
    pthread_mutex_unlock(&db_mutex);
}

void StartupDbContext::rm_lobby(int id){
    pthread_mutex_lock(&db_mutex);
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        pthread_mutex_unlock(&db_mutex);
        return;
    }
    string q = "DELETE FROM games WHERE id = $1";
    work w(*conn);
    w.exec_params(q, id);
    w.commit();
    pthread_mutex_unlock(&db_mutex);
} 

void StartupDbContext::set_lobby_port(int id, int port){
    pthread_mutex_lock(&db_mutex);
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        pthread_mutex_unlock(&db_mutex);
        return;
    }
    string q = "UPDATE games SET port = $1 WHERE id = $2";
    work w(*conn);
    w.exec_params(q, port, id);
    w.commit();
    pthread_mutex_unlock(&db_mutex);
}

void StartupDbContext::set_lobby_status(int id, bool ready){
    pthread_mutex_lock(&db_mutex);
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        pthread_mutex_unlock(&db_mutex);
        return;
    }
    string q = "UPDATE games SET began = $1 WHERE id = $2";
    work w(*conn);
    w.exec_params(q, ready, id);
    w.commit();
    pthread_mutex_unlock(&db_mutex);
}

void StartupDbContext::add_player_score(string login, int score){
    pthread_mutex_lock(&db_mutex);
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        pthread_mutex_unlock(&db_mutex);
        return;
    }
    string q = "UPDATE users SET score = $1 WHERE login = $2";
    string q1 = "SELECT * FROM users WHERE login = $1";
    work w(*conn);
    result r = w.exec_params(q1, login);
    w.commit();
    result res = w.exec_params(q, r[0]["score"].as<int>() + score, login);
    w.commit();
    pthread_mutex_unlock(&db_mutex);
}


void StartupDbContext::clear_online(){
    pthread_mutex_lock(&db_mutex);
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        pthread_mutex_unlock(&db_mutex);
        return;
    }
    string q = "UPDATE users SET online = $1";
    work w(*conn);
    w.exec_params(q, 0);
    w.commit();
    pthread_mutex_unlock(&db_mutex);
}

void StartupDbContext::clear_lobbies(){
    pthread_mutex_lock(&db_mutex);
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        pthread_mutex_unlock(&db_mutex);
        return;
    }
    string q = "TRUNCATE TABLE games";
    work w(*conn);
    w.exec(q);
    w.commit();
    pthread_mutex_unlock(&db_mutex);
}


bool StartupDbContext::finduser(string nick, string& ip, int& port, bool& online){
    pthread_mutex_lock(&db_mutex);
    if(!isConnected()){
        cout << "ОШИБКА: НЕТ СОЕДИНЕНИЯ С БАЗОЙ!" << endl;
        pthread_mutex_unlock(&db_mutex);
        return false;
    }
    string q = "SELECT * FROM users WHERE login = $1";
    work w(*conn);
    result res = w.exec_params(q, nick);
    w.commit();
    pthread_mutex_unlock(&db_mutex);
    if(res.empty()){
        return false;
    }
    ip = res[0]["address"].as<string>();
    port = res[0]["port"].as<int>();
    if(res[0]["online"].as<int>() == 0){
        online = false;
    }else{
        online = true;
    }
    return true;
}


