// КОНТЕКСТ БАЗЫ ДАННЫХ (БИБЛИОТЕКА)
#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <memory>
#include <sodium.h>

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
        int logout(string login);
        string get_lobbies();
        string get_players_on();
        string get_players_all();
        string get_rating();
        string get_chats();

        void set_lobby_num(int id, int nv);
        int add_lobby(string creator, string name, int num);
        int join_lobby(int id);
        void rm_lobby(int id);
        void set_lobby_port(int id, int port);
        void set_lobby_status(int id, bool ready);
        void add_player_score(string login, int score);

    private:
        string address;
        int port;
        connection* conn;
        string database;
        string user;
        string password;
};