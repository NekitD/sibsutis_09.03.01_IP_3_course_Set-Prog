#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

using namespace std;

#define BUFF_LEN 128


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

enum emoji_id{
    doubting = 0,

};


int encode_msg(char* msg, int& status, int len = BUFF_LEN);
int decode_msg(char* msg, int& status, int len = BUFF_LEN);



//-------------------------

class Card 
{  
    friend ostream& operator<<(ostream& os, const Card& c);
    public: 
        Card(string _text_1): text(_text_1) {};
        ~Card(){};
    private:
        string text;
};


class Employer
{
    private:
        vector<Card*>* p_profs;
        string manual;
};

class Player
{
    friend ostream& operator<<(ostream& os, const Player& p);
    public:
        Player(string _name, int _id): name(_name), score(0), p_profs(new vector<Card*>) , 
            p_skills(new vector<Card*>), id(_id){};
        ~Player(){};

        void addScore(int _score);
        void addSkill(Card* sk);
        void addProf(Card* pr);
        void addEmoji(Card* ej);

        void remSkill(Card* sk);
        void remEmoji();

    private:
        int id;
        string name;
        int score = 0;
        vector<Card*>* p_skills;
        vector<Card*>* p_profs;
        Card* p_emoji;
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

    private:
        int p_num = 0;
        vector<Card*>* g_profs;
        vector<Card*>* g_skills;
        vector<Card*>* g_emoji;
        vector<Player*>* g_players;
};

