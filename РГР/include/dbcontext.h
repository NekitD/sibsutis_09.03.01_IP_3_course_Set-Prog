#include <iostream>
#include <mysql/mysql.h>
#include <string>
#include <memory>

#define S_ADDRESS "localhost"
#define S_PORT 5432


using namespace std;


class StartupDbContext {
    
    public:
        StartupDbContext(string _address, int _port);
        ~StartupDbContext();

        bool auth(string login, string password);
        bool reg(string login, string password);
        char* get_games() const;
        char* get_players_on()const;
        char* get_players_all()const;
        char* get_rating() const;
        char* get_chats() const;

        bool add_lobby(string name, int num);
        bool join_lobby(int id); // bool ?

    private:
        string address;
        int port;
};