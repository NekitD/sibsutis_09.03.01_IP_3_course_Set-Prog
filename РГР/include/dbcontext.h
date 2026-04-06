#include <iostream>
#include <mysql/mysql.h>
#include <string>
#include <memory>

#define S_ADDRESS "localhost"
#define S_PORT 5432


using namespace std;


class StartupDbContext {
    public:
    StartupDbContext();
    ~StartupDbContext();

    bool auth();
    bool reg();
    char* get_games() const;
    char* get_players_on()const;
    char* get_players_all()const;
    char* get_rating() const;
    char* get_chats() const;

    bool add_lobby();
    bool join_lobby(); // bool ?

    private:
        string address;
        int port;
};