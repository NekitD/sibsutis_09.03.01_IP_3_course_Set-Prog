// СТАРТАП-ИГРА (БИБЛИОТЕКА)

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <algorithm>
#include <bits/stdc++.h>
#include "dbcontext.h"

using namespace std;

#define BUFF_LEN 4096
#define MIN_P 3
#define MAX_P 3
#define EMPLOYER_PROFS_NUM 3
#define SKILL_NUM 4
#define REWARD_K 10

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
    SCORING,
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
        string print_profs() const;
        string getManual() const;
        void setManual(string n_man);
        void add_claim(int vac, int id);

        vector<int>* get_claims_for_vacancy(int vac_index);
        void clear_claims();

        map<int, int> assignments; 
        void add_assignment(int vac, int player);
        void clear_assignments();
        int get_assignment(int vac);
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

        string print_skills() const;
        string print_profs();

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
        Game(int _pmax);
        ~Game();
        void game_init();
        vector<Card*>* get_profs() const;
        vector<Card*>* get_skills() const;
        vector<Card*>* get_emoji() const;
        vector<Player*>* get_players() const;
        void print_profs() const;
        void print_skills() const;
        void print_emoji() const;
        char* print_players() const;
        int getStatus() const;
        int getPnum() const;
        int getRnum() const;
        int getMnum() const;
        int get_player_id(char* nick) const;
        string get_player_nick(int id) const;
        int get_player_status(int id) const;
        bool isGameReady() const;
        string get_players_list() const;

        void set_player_status(int id, int ns);
        void setStatus(int ns);
        char* addPlayer(char* nick, int id);
        char* remPlayer(int id);

        Player* getPlayer(int id) const;
        int getEmployerId() const;
        void setEmployer(int ne);
        int getEmployer() const;

        Employ_Info* EmployInfo();
        void ShuffleCards(vector<Card*>* cards);
        void PassCards(vector<Card*>* giver, vector<Card*>* accepter, int n_cards);
        void GiveEmojiToPlayer(Player* player);

        void set_answering_num(int na);
        int get_answering_num() const;
        int get_answering_id() const;

        vector<string>* get_questions() const;
        void add_question(string q);
        void rem_question();
        bool no_questions() const;

        string open_p(int id) const;

        int get_scoreb() const;
        void set_scoreb(int ns);
        void add_scoreb(int as);
        bool score_over() const;

        string assign_professions();

        void drop_cards();

        string Endgame(StartupDbContext* context);

        int count_reward(int score, int pnum, bool winner, int winn, int k);

    private:
        int p_num = 0;
        int p_max;
        vector<Card*>* g_profs;
        vector<Card*>* g_skills;
        vector<Card*>* g_emoji;
        vector<Player*>* g_players;
        Employ_Info* g_employ;
        int status = PRE;
        int employer;
        int answering_num;
        vector<string>* g_questions;
        int score_buf;
};

