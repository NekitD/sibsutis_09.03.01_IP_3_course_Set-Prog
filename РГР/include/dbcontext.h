// КОНТЕКСТ БАЗЫ ДАННЫХ (БИБЛИОТЕКА)
#include <iostream>
#include <mysql/mysql.h>
#include <pqxx/pqxx>
#include <string>
#include <memory>

#define S_ADDRESS "localhost"
#define S_PORT 5432
#define DATABASE "StartupDatabase"


using namespace std;
using namespace pqxx;


enum login_status{
    L_WRONG = 0,
    L_ONLINE,
    L_SUCCESS
};

enum reg_status{
    R_BUSY = 0,
    R_SUCCESS,
    R_FAIL
};


typedef struct User {
	int ID;
	int socket;
	char* login;
	char* password;
	int online;
    int score;
} User;

typedef struct Lobby {
    int ID;
	int socket;
	char* name;
	int size;
} Lobby;


class StartupDbContext {
    
    public:
        StartupDbContext(string _address, int _port, string _database, string _user, string _password);
        ~StartupDbContext();

        connection* getConnection();
        bool isConnected();

        int auth(string login, string password);
        int reg(string login, string password);
        char* get_lobbies() const;
        char* get_players_on()const;
        char* get_players_all()const;
        char* get_rating() const;
        char* get_chats() const;

        bool add_lobby(string name, int num);
        bool join_lobby(int id); // bool ?

    private:
        string address;
        int port;
        connection* conn;
        string database;
        string user;
        string password;
};