#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cstring>
#include <termios.h>
#include <unistd.h>

using namespace std;

#define BUFF_LEN 128
#define MIN_P 3
#define MAX_P 6


enum keys {
    ENTER = 0,
    ESC
};

enum prof_id {
    fisher = 0,
    intertainer,
    painter,
    military,
    librarian,
    pilot,
    magician,
    farmer,
    plumber,
    postman,
    stylist,
    guide,
    racer,
    savior,
    officeman,
    chemic,
    photograph,
    musician,
    policeman,
    gardener,
    lawyer,
    journalist,
    deliver,
    actor,
    sellman,
    sportsman,
    security,
    fashioner,
    streetsweper,
    firefighter,
    acountant,
    radio,
    barista,
    shepherd,
    artist,
    cooker,
    builder,
    teacher,
    clawn,
    household,
    sailor,
    poet,
    doctor,
    astronaut, 
    smith,
    programmer,
    carmechanic,
    enjineer,
    waiter,
    nurse
};

enum game_status {
    PRE = 0, // набор лобби.
    FULL,
    START, // все игроки выразили готовность начать игру.
    // ИГРОВОЙ ЦИКЛ:
    //--------------------------------------------------------------------
    JOB_MAKE, // работодатель придумывает историю предприятия. 
    //Цикл выступления соискателей:
        //---------------------------------------------
        P_PRE, // Игрок готовится выступать.
        P_MAKE, // Игрок выступает.
        QUESTIONS, // Режим принятия вопросов. Если ещё есть вопросы, то QUESTIOS -> P_ANSWER
        P_ANSWER, // Игрок отвечает на первый пришедший вопрос P_ANSWER -> QUESTIOS 
        P_OPEN, // Карты игрока вскрываются.
        SCORES, // Выставление оценки.
        //---------------------------------------------
    JOB_CHOICE, // Работодатель выбирает 
    //--------------------------------------------------------------------
    OVER,
    AFTERGAME
};

enum player_status {
    WAIT_ACCEPT = 0,
    PRE_TO_PLAY,
    READY_TO_PLAY,
    EMPLOYER,
    ANSWERING,
    LEFT
};


int encode_msg(char* msg, int& status, int len = BUFF_LEN);
int decode_msg(char* msg, int& status, int len = BUFF_LEN);
int get_line_b(char*, char*, int, int, char);

//-------------------------

class Card 
{  
    friend ostream& operator<<(ostream& os, const Card& c);
    public: 
        Card(string _text_1);
        ~Card();
    private:
        string text;
};


class Employ_Info
{
    private:
        vector<Card*>* e_profs;
        string manual;
};

class Player
{
    friend ostream& operator<<(ostream& os, const Player& p);
    public:
        Player(string _name, int _id);
        ~Player();

        void addScore(int _score);
        void addSkill(Card* sk);
        void addProf(Card* pr);
        void addEmoji(Card* ej);

        void remSkill(Card* sk);
        void remEmoji();

        int get_id() const;
        string get_nick() const;

        void setStatus(int ns);
        int getStatus() const;
        int getScore() const;

    private:
        int id;
        string name;
        int score = 0;
        vector<Card*>* p_skills;
        vector<Card*>* p_profs;
        Card* p_emoji;
        int status = PRE_TO_PLAY;
};

class Game 
{
    public:
        Game();
        ~Game();
        void game_init();
        vector<Card*>* get_profs();
        vector<Card*>* get_skills();
        vector<Card*>* get_emoji();
        vector<Player*>* get_players();
        void print_profs();
        void print_skills();
        void print_emoji();
        void print_players();
        int getStatus() const;
        void setStatus(int ns);
        void addPlayer(char* nick);
        void remPlayer(int id);
        int getPnum() const;
        int getRnum() const;
        int get_player_id(char* nick) const;
        string get_player_nick(int id) const;
        void set_player_status(int id, int ns);
        bool isGameReady();

        void Endgame() const;

    private:
        int p_num = 0;
        vector<Card*>* g_profs;
        vector<Card*>* g_skills;
        vector<Card*>* g_emoji;
        vector<Player*>* g_players;
        Employ_Info* g_employ;
        int status = PRE;
};

