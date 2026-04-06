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


class StartupDbContext {
    
    public:
        StartupDbContext(string _address, int _port, string _database, string _user, string _password);
        ~StartupDbContext();

        connection* getConnection();
        bool isConnected();
        result query(const string&);
        void exec(const string& script);


        bool auth(string login, string password);
        bool reg(string login, string password);
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