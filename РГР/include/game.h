#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <algorithm>
#include <bits/stdc++.h>

using namespace std;

#define BUFF_LEN 4096
#define MIN_P 3
#define MAX_P 3
#define EMPLOYER_PROFS_NUM 3
#define SKILL_NUM 4

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
        P_ANSWER, // Игрок отвечает на первый пришедший вопрос P_ANSWER -> QUESTIONS 
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
    WAITING,
    EMPLOYER,
    ANSWERING,
    QUESTIONING,
    LEFT
};

//----------------------------------------------------------------------------------------
int get_line_b(char*, char*, int, int, char);
//----------------------------------------------------------------------------------------

class Card 
{  
    friend ostream& operator<<(ostream& os, const Card& c);
    public: 
        Card(string _text_1);
        ~Card();
        string get_text() const;
    private:
        string text;
};


class Employ_Info
{
    public:
        Employ_Info();
        ~Employ_Info();
        vector<Card*>* getProfs() const;
        void print_profs() const;
        string getManual() const;
        void setManual(string n_man);
        void add_claim(int vac, int id);
    private:
        vector<Card*>* e_profs;
        string manual;
        vector<int>* claim_matrix;
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

        vector<Card*>* getSkills() const;
        Card* getEmoji() const;

        void print_skills() const;

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
        vector<Card*>* get_profs() const;
        vector<Card*>* get_skills() const;
        vector<Card*>* get_emoji() const;
        vector<Player*>* get_players() const;
        void print_profs() const;
        void print_skills() const;
        void print_emoji() const;
        void print_players() const;
        int getStatus() const;
        int getPnum() const;
        int getRnum() const;
        int get_player_id(char* nick) const;
        string get_player_nick(int id) const;
        int get_player_status(int id) const;
        bool isGameReady() const;

        void set_player_status(int id, int ns);
        void setStatus(int ns);
        void addPlayer(char* nick, int id);
        void remPlayer(int id);

        Player* getPlayer(int id) const;
        int getEmployerId() const;
        void setEmployer(int ne);
        int getEmployer() const;

        Employ_Info* EmployInfo();
        void ShuffleCards(vector<Card*>* cards);
        void PassCards(vector<Card*>* giver, vector<Card*>* accepter, int n_cards);
        void GiveEmojiToPlayer(Player* player);

        void set_answering_num(int na);
        int get_answering_id() const;

        vector<string>* get_questions() const;
        void add_question(string q);
        void rem_question();
        bool no_questions() const;

        void open_p(int id) const;

        void Endgame() const;

    private:
        int p_num = 0;
        vector<Card*>* g_profs;
        vector<Card*>* g_skills;
        vector<Card*>* g_emoji;
        vector<Player*>* g_players;
        Employ_Info* g_employ;
        int status = PRE;
        int employer;
        int answering_num;
        vector<string>* g_questions;
};

