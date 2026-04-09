// СТАРТАП-ИГРА (ИСХОДНЫЙ КОД)
#include "game.h"

int get_line_b(char* n_line, char* o_line, int start, int len, char b){
    if (start >= len) {
        n_line[0] = '\0';
        return len;
    }
    
    int i = 0;
    int j = start;
    
    while (j < len && i < len - 1) {
        char c = o_line[j];
        if (c == b) {
            strcat(n_line, "\0");  
            return j + 1;
        }
        n_line[i] = c;
        i++;
        j++;
    }
    
    //n_line[i] = '\0';
    strcat(n_line, "\0");
    return len - 1;
}

ostream& operator<<(ostream& os, const Card& c){
    os << c.text;
    return os;
}

Card::Card(string _text_1): text(_text_1){}
Card::~Card(){};

string Card::get_text() const{
    return text;
}


ostream& operator<<(ostream& os, const Player& p){
    os << p.name <<  " (";
    for(vector<Card*>::iterator it = p.p_profs->begin(); it != p.p_profs->end(); it++){
        os << (**it) << ", ";
    }
    os << ")" <<":      " << p.score;
    if (p.getStatus() == LEFT){
        os << "     (Вышел)";
    }
    return os;
}


Player::Player(string _name, int _id): name(_name), score(0), p_profs(new vector<Card*>) , 
            p_skills(new vector<Card*>), p_emoji(nullptr), id(_id), status(WAIT_ACCEPT){};

Player::~Player(){
    p_skills->clear();
    delete p_skills;

    p_profs->clear();
    delete p_profs;

    delete p_emoji;
};

void Player::addScore(int _score){
    score += _score;
}

void Player::addSkill(Card* sk){
    p_skills->push_back(sk);
}

void Player::addProf(Card* pr){
    p_profs->push_back(pr);
}

void Player::addEmoji(Card* ej){
    p_emoji = ej;
}

void Player::remEmoji(){
    p_emoji = nullptr;
}

int Player::get_id() const{
    return id;
}

string Player::get_nick() const{
    return name;
}

void Player::remSkill(Card* sk){
    for(vector<Card*>::iterator c = p_skills->begin(); c != p_skills->begin(); c++){
        if (*c == sk){
            p_skills->erase(c);
            break;
        } 
    }
}

void Player::setStatus(int ns){
    status = ns;
}

int Player::getStatus() const{
    return status;
}

int Player::getScore() const{
    return score;
}

string Player::print_skills() const{
    string res;
    if(p_skills){
        for(vector<Card*>::const_iterator sk = p_skills->begin(); sk != p_skills->end(); sk++){
            //cout << "    - " <<**sk << endl;
            res += "    - ";
            res += (**sk).get_text();
            res += "\n";
        }
    }
    return res;
}

string Player::print_profs() {
    string result;
    if(p_profs && !p_profs->empty()){
        for(auto it = p_profs->begin(); it != p_profs->end(); ++it){
            if(it != p_profs->begin()){
                result += ", ";
            }
            result += (*it)->get_text();
        }
    }
    return result;
}


vector<Card*>* Player::getSkills() const{
    return p_skills;
}

Card* Player::getEmoji() const{
    return p_emoji;
}


Game::Game(int _pmax): p_max(_pmax)
{
    game_init();
}

void Game::game_init()
{
        //--------------------------------------------------
        g_profs = new vector<Card*>;
        g_skills = new vector<Card*>;
        g_emoji = new vector<Card*>;
        g_players = new vector<Player*>;
        g_employ = new Employ_Info;
        g_questions = new vector<string>;
        //--------------------------------------------------
        g_profs->push_back(new Card("🐟 Рыбак"));
        g_profs->push_back(new Card("🎭 Конферансье"));
        g_profs->push_back(new Card("🎨 Маляр"));
        g_profs->push_back(new Card("⚔️ Военный"));
        g_profs->push_back(new Card("📚 Библиотекарь"));
        g_profs->push_back(new Card("✈️ Пилот"));
        g_profs->push_back(new Card("🎩 Фокусник"));
        g_profs->push_back(new Card("🌾 Фермер"));
        g_profs->push_back(new Card("🔧 Сантехник"));
        g_profs->push_back(new Card("📮 Почтальон"));
        g_profs->push_back(new Card("💇 Стилист"));
        g_profs->push_back(new Card("🗺️ Экскурсовод"));
        g_profs->push_back(new Card("🏎️ Гонщик"));
        g_profs->push_back(new Card("🆘 Спасатель"));
        g_profs->push_back(new Card("💼 Офисный менеджер"));
        g_profs->push_back(new Card("🧪 Химик"));
        g_profs->push_back(new Card("📸 Фотограф"));
        g_profs->push_back(new Card("🎵 Музыкант"));
        g_profs->push_back(new Card("👮 Полицейский"));
        g_profs->push_back(new Card("🌱 Садовник"));
        g_profs->push_back(new Card("⚖️ Адвокат"));
        g_profs->push_back(new Card("📰 Журналист"));
        g_profs->push_back(new Card("📦 Доставщик"));
        g_profs->push_back(new Card("🎭 Актер"));
        g_profs->push_back(new Card("🛒 Продавец"));
        g_profs->push_back(new Card("🏅 Спортсмен"));
        g_profs->push_back(new Card("🔒 Охранник"));
        g_profs->push_back(new Card("👗 Модельер"));
        g_profs->push_back(new Card("🧹 Дворник"));
        g_profs->push_back(new Card("🔥 Пожарный"));
        g_profs->push_back(new Card("🧮 Бухгалтер"));
        g_profs->push_back(new Card("🎙️ Радиоведущий"));
        g_profs->push_back(new Card("☕ Бариста"));
        g_profs->push_back(new Card("🐑 Пастух"));
        g_profs->push_back(new Card("🎨 Художник"));
        g_profs->push_back(new Card("🍳 Повар"));
        g_profs->push_back(new Card("🏗️ Строитель"));
        g_profs->push_back(new Card("📖 Учитель"));
        g_profs->push_back(new Card("🤡 Клоун"));
        g_profs->push_back(new Card("🏠 Домработница"));
        g_profs->push_back(new Card("⚓ Моряк"));
        g_profs->push_back(new Card("📝 Поэт"));
        g_profs->push_back(new Card("🩺 Доктор"));
        g_profs->push_back(new Card("🚀 Космонавт"));
        g_profs->push_back(new Card("🔨 Кузнец"));
        g_profs->push_back(new Card("💻 Программист"));
        g_profs->push_back(new Card("🔧 Автомеханик"));
        g_profs->push_back(new Card("⚙️ Инженер"));
        g_profs->push_back(new Card("🍽️ Официант"));
        g_profs->push_back(new Card("💊 Медсестра"));
        ShuffleCards(g_profs);
        //--------------------------------------------------
        g_skills->push_back(new Card("У меня острое зрение."));
        g_skills->push_back(new Card("Я быстро печатаю на клавиатуре."));
        g_skills->push_back(new Card("Я умею держать себя в руках."));
        g_skills->push_back(new Card("Я хорошо ориентируюсь на местности."));
        g_skills->push_back(new Card("Я хорошо разбираюсь в налогах."));
        g_skills->push_back(new Card("Я могу запоминать большие объёмы информации."));
        g_skills->push_back(new Card("Я умею аргументированно спорить."));
        g_skills->push_back(new Card("Я умею пропалывать грядки."));
        g_skills->push_back(new Card("Я знаю столицы всех стран."));
        g_skills->push_back(new Card("У меня заразительный смех."));
        g_skills->push_back(new Card("Я с душой рассказываю стихи."));
        g_skills->push_back(new Card("Я могу найти всё, что угодно в интернете."));
        g_skills->push_back(new Card("Я быстро бегаю."));
        g_skills->push_back(new Card("Я умею ухаживать за цветами."));
        g_skills->push_back(new Card("Я умею вести переговоры."));
        g_skills->push_back(new Card("Я умею художественно свистеть."));
        g_skills->push_back(new Card("Я умею чинить всё, что угодно."));
        g_skills->push_back(new Card("Я могу прыгать через скакалку целый день."));
        g_skills->push_back(new Card("Я умею находить общий язык с детьми."));
        g_skills->push_back(new Card("Я никогда не опаздываю."));
        g_skills->push_back(new Card("Я хорошо разбираюсь в технике."));
        g_skills->push_back(new Card("Я умею танцевать вальс."));
        g_skills->push_back(new Card("Я безошибочно считаю в уме."));
        g_skills->push_back(new Card("Я умею рисовать."));
        g_skills->push_back(new Card("У меня хорошее чувство ритма."));
        g_skills->push_back(new Card("У меня каллиграфический почерк."));
        g_skills->push_back(new Card("Я умею организовывать своё время."));
        g_skills->push_back(new Card("Я умею садиться на шпагат."));
        g_skills->push_back(new Card("Я очень доходчиво объясняю."));
        g_skills->push_back(new Card("Я умею внимательно слушать."));
        g_skills->push_back(new Card("Я умею вязать морские узлы."));
        g_skills->push_back(new Card("Я хорошо фотографирую."));
        g_skills->push_back(new Card("Я знаю расписание автобусов наизусть."));
        g_skills->push_back(new Card("Я прирождённый оратор."));
        g_skills->push_back(new Card("Я свободно говорю на пяти языках."));
        g_skills->push_back(new Card("Я умею плавать."));
        g_skills->push_back(new Card("Я умею вкусно готовить."));
        g_skills->push_back(new Card("Я могу стоять на одной ноге с закрытыми глазами."));
        g_skills->push_back(new Card("У меня аналитический склад ума."));
        g_skills->push_back(new Card("Я легко нахожу общий язык с людьми."));
        g_skills->push_back(new Card("Я умею определять птиц по голосам."));
        g_skills->push_back(new Card("Я умею рубить дрова."));
        g_skills->push_back(new Card("Я умею управлять повозкой с лошадью."));
        g_skills->push_back(new Card("Я умею лепить из пластилина."));
        g_skills->push_back(new Card("Я красиво пою."));
        g_skills->push_back(new Card("Я легко завожу новые знакомства."));
        g_skills->push_back(new Card("Я творчески подхожу к любой задаче."));
        g_skills->push_back(new Card("Я умею играть в шахматы."));
        g_skills->push_back(new Card("Я умею вышивать крестиком."));
        g_skills->push_back(new Card("Я умею ездить на мотоцикле."));
        g_skills->push_back(new Card("Я хорошо знаю физику."));
        g_skills->push_back(new Card("Я умею кататься на коньках."));
        g_skills->push_back(new Card("Я умею лепить куличики."));
        g_skills->push_back(new Card("Я умею отличать правду от лжи."));
        g_skills->push_back(new Card("Я умею хорошо знаю химию."));
        g_skills->push_back(new Card("Я легко перевоплощаюсь."));
        g_skills->push_back(new Card("Я умею предсказывать погоду."));
        g_skills->push_back(new Card("Я виртуозно играю на скрипке."));
        g_skills->push_back(new Card("Я умею делать массаж."));
        g_skills->push_back(new Card("Я умею оказывать первую помощь."));
        g_skills->push_back(new Card("Я умею надувать большие пузыри из жвачки."));
        g_skills->push_back(new Card("Я хорошо знаю законы."));
        g_skills->push_back(new Card("Я умею делать причёски."));
        g_skills->push_back(new Card("Я умею выявлять преимущества положения."));
        g_skills->push_back(new Card("Я умею показывать фокусы."));
        g_skills->push_back(new Card("Я умею метко стрелять."));
        g_skills->push_back(new Card("Я умею разжигать костёр."));
        g_skills->push_back(new Card("Я умею отличать съедобные грибы от ядовитых."));
        g_skills->push_back(new Card("Я притягиваю удачу."));
        g_skills->push_back(new Card("Я умею убеждать."));
        ShuffleCards(g_skills);
        //--------------------------------------------------
        g_emoji->push_back(new Card("Вы во всём сомневаетесь, даже в собственных способсностях."));
        g_emoji->push_back(new Card("Вам всё лень."));
        g_emoji->push_back(new Card("Вы чувствуете себя очень виноватым."));
        g_emoji->push_back(new Card("У вас много слов-паразитов: ну, как бы, это, типа, в общем..."));
        g_emoji->push_back(new Card("Вы изо всех сил стараетесь понравиться."));
        g_emoji->push_back(new Card("Вокруг вас летает назойливая муха."));
        g_emoji->push_back(new Card("Вы хотите спать и нникак не можете взбодриться."));
        g_emoji->push_back(new Card("Вы много смеётесь по поводу и без него."));
        g_emoji->push_back(new Card("Вы зануда."));
        g_emoji->push_back(new Card("Вы говорите с акцентом."));
        g_emoji->push_back(new Card("Вы сильно торопитесь."));
        g_emoji->push_back(new Card("Вам холодно."));
        g_emoji->push_back(new Card("Вы рассеяны, постоянно что-то забываете, путаете и теряете."));
        g_emoji->push_back(new Card("Вы постоянно переспрашиваете."));
        g_emoji->push_back(new Card("Вы ведёте себя развязно, дерзко."));
        g_emoji->push_back(new Card("Вы часто используете уменьшительно-ласкательные формы: счётик, грабельки, микрофончик..."));
        g_emoji->push_back(new Card("Вы гордитесь своими детьми и хвалитесь их успехами"));
        g_emoji->push_back(new Card("У вас меланхолия, ничто не радует, нет в жизни счастья."));
        g_emoji->push_back(new Card("Вы ёрзаете на стуле."));
        g_emoji->push_back(new Card("У вас криминальное прошлое."));
        g_emoji->push_back(new Card("Вы слишком самоуверенны, всем вокруг с вами невероятно повезло."));
        g_emoji->push_back(new Card("Вам жарко"));
        g_emoji->push_back(new Card("Вы боитесь общественного мнения."));
        g_emoji->push_back(new Card("Вы сентиментальны, легко можете растрогаться и даже всплакнуть."));
        g_emoji->push_back(new Card("Вы говорите слишком тихо."));
        g_emoji->push_back(new Card("Вы жуёте жвачку."));
        g_emoji->push_back(new Card("Вы чрезвычайно нервничаете."));
        g_emoji->push_back(new Card("Вы постоянно поправляете причёску."));
        g_emoji->push_back(new Card("Вы голодны."));
        g_emoji->push_back(new Card("Вы говорите по-простецки, не церемонясь."));
        g_emoji->push_back(new Card("Вы чешетесь."));
        g_emoji->push_back(new Card("Вы обо всех заботитесь, всем сочувствуете и сопереживаете."));
        g_emoji->push_back(new Card("У вас апатия, всё безразлично."));
        g_emoji->push_back(new Card("У вас приступ панического страха."));
        g_emoji->push_back(new Card("Вам трудно угодить, вы избирательны и капризны."));
        g_emoji->push_back(new Card("Вы слишком громко говорите."));
        g_emoji->push_back(new Card("Вы сильно простужены."));
        g_emoji->push_back(new Card("Вы склонны всё преувеличивать."));
        g_emoji->push_back(new Card("Вы вот-вот чихнёте."));
        g_emoji->push_back(new Card("Вас всё вокруг раздражает."));
        g_emoji->push_back(new Card("Вы излагаете мысли быстро, активно, нетерпеливо."));
        g_emoji->push_back(new Card("Вы говорите самозабвенно, не обращая ни на что и ни на кого внимания."));
        g_emoji->push_back(new Card("Вы икаете."));
        g_emoji->push_back(new Card("У вас дислексия."));
        g_emoji->push_back(new Card("Вы любите сокращения."));
        g_emoji->push_back(new Card("Вы постоянно напеваете."));
        g_emoji->push_back(new Card("Вы плохо говориет по-русски."));
        g_emoji->push_back(new Card("Вы льете воду."));
        g_emoji->push_back(new Card("Вы презираете других соискателей."));
        g_emoji->push_back(new Card("Вы хотите много денег."));
        ShuffleCards(g_emoji);
        
        status = PRE;
        employer = 0;
}

Game::~Game()
{
    if(g_profs){
        g_profs->clear();
        delete g_profs;
    }
    if(g_skills){
        
        g_skills->clear();
        delete g_skills;
    }
    if(g_emoji){
        g_emoji->clear();
        delete g_emoji;
    }
    if(g_players){
        g_players->clear();
        delete g_players;
    }
}

vector<Card*>* Game::get_profs() const{
    return g_profs;
}

vector<Card*>* Game::get_skills() const{
    return g_skills;
}

vector<Card*>* Game::get_emoji() const{
    return g_emoji;
}

vector<Player*>* Game::get_players() const{
    return g_players;
}

void Game::print_profs() const{
    if(g_profs){
        for(vector<Card*>::const_iterator pr = g_profs->begin(); pr != g_profs->end(); pr++){
            cout << **pr << endl;
        }
    }
}
void Game::print_skills() const{
    if(g_skills){
        for(vector<Card*>::const_iterator sk = g_skills->begin(); sk != g_skills->end(); sk++){
            cout << *sk << endl;
        }
    }
}
void Game::print_emoji() const{
    if(g_emoji){
        for(vector<Card*>::const_iterator ej = g_emoji->begin(); ej != g_emoji->end(); ej++){
            cout << *ej << endl;
        }
    }
}
string Game::print_players() const {
    string result;
    
    if (g_players == nullptr || g_players->empty()){
        result = "   НЕТ ИГРОКОВ";
        return result;
    }
    
    result += "=========ИГРОКИ=========\n";
    
    for(auto p = g_players->begin(); p != g_players->end(); ++p){
        char buffer[BUFF_LEN];
        snprintf(buffer, BUFF_LEN, 
            "   %s (%s):   %d  %s\n", 
            (*p)->get_nick().c_str(), 
            (*p)->print_profs().c_str(), 
            (*p)->getScore(), 
            (get_player_status((*p)->get_id()) == LEFT) ? "(Вышел)" : "");
        result += buffer;
    }
    
    result += "========================\n";
    return result;
}

int Game::getStatus() const{
    return status;
}

void Game::setStatus(int ns){
    status = ns;
}

string Game::addPlayer(char* nick, int id){
    Player* p = new Player(nick, id);
    g_players->push_back(p);
    p_num = getPnum();
    
    char buffer[BUFF_LEN];
    snprintf(buffer, BUFF_LEN, 
        "   %s присоединился к игре!\n   Игроков: %d / %d\n   Готовы: %d / %d\n\n", 
        nick, getPnum(), p_max, getRnum(), p_max);
    
    if(getPnum() >= MAX_P){
        setStatus(FULL);
    }
    
    return string(buffer);
}

int Game::getPnum() const{
    return g_players->size();
}

int Game::getMnum() const{
    return p_max;
}

int Game::get_player_id(char* nick) const{
    for(vector<Player*>::iterator p = g_players->begin(); p != g_players->end(); p++){
        if(strcmp((*p)->get_nick().c_str(), nick) == 0){
            return (*p)->get_id();
        }
    }
    return -1;
}

string Game::get_player_nick(int id) const{
    for(vector<Player*>::iterator p = g_players->begin(); p != g_players->end(); p++){
        if((*p)->get_id() == id){
            return (*p)->get_nick();
        }
    }
    return NULL;
}

int Game::get_player_status(int id) const{
    for(vector<Player*>::iterator p = g_players->begin(); p != g_players->end(); p++){
        if((*p)->get_id() == id){
            return (*p)->getStatus();
        }
    }
    return WAIT_ACCEPT;
}

string Game::remPlayer(int id){
    string result;
    
    for(auto it = g_players->begin(); it != g_players->end(); ++it){
        if((*it)->get_id() == id){
            char buffer[BUFF_LEN];
            sprintf(buffer, "   %s покинул игру.\n", get_player_nick(id).c_str());
            result += buffer;
            
            if (getStatus() == PRE || getStatus() == FULL){
                delete *it;
                g_players->erase(it);
                
                char buf[BUFF_LEN];
                sprintf(buf, "   Игроков: %d / %d\n   Готовы: %d / %d\n",
                    getPnum(), p_max, getRnum(), p_max);
                result += buf;
            } else {
                (*it)->setStatus(LEFT);
                setStatus(OVER);
            }
            return result;
        }
    }
    
    return "   Игрок не найден.\n";
}

void Game::set_player_status(int id, int ns){
    for(vector<Player*>::iterator p = g_players->begin(); p != g_players->end(); p++){
        if((*p)->get_id() == id){
            (*p)->setStatus(ns);
        }
    }
}

int Game::getRnum() const{
    int ready = 0;
    for(vector<Player*>::const_iterator it = g_players->begin(); it != g_players->end(); it++){
        if ((*it)->getStatus() == READY_TO_PLAY){
            ready++;
        }
    }
    return ready;
}


bool Game::isGameReady() const{
    return ((getPnum() >= MIN_P ) && (getRnum() == getPnum()));
}


Player* Game::getPlayer(int id) const{
    for(vector<Player*>::iterator p = g_players->begin(); p != g_players->end(); p++){
        if((*p)->get_id() == id){
            return *p;
        }
    }
}
int Game::getEmployerId() const{
    return g_players->at(getEmployer())->get_id();
}

int Game::getEmployer() const{
    return employer;
}

void Game::setEmployer(int ne){
    employer = ne;
}

void Game::PassCards(vector<Card*>* giver, vector<Card*>* accepter, int n_cards){
    Card* card;
    for(int i = 0; i < n_cards; i++){
        card = *(giver->begin());
        giver->erase(giver->begin());
        accepter->push_back(card);
    }
}

void Game::GiveEmojiToPlayer(Player* player){
    if (g_emoji->empty()) {
        cout << "Ошибка: нет эмоций в колоде!" << endl;
        return;
    }
    Card* emo = g_emoji->front();
    g_emoji->erase(g_emoji->begin());
    player->addEmoji(emo);
}

void Game::ShuffleCards(vector<Card*>* cards){
    random_device rd;
    mt19937 g(rd());
    shuffle(cards->begin(), cards->end(), g);
}

Employ_Info* Game::EmployInfo(){
    return g_employ;
}

void Game::set_answering_num(int na){
    answering_num = na;
}


int Game::get_answering_num() const{
    return answering_num;
}

int Game::get_answering_id() const{
    if(getEmployer() + answering_num >= g_players->size()){
        return g_players->at(getEmployer() + answering_num - g_players->size())->get_id();
    }
    return g_players->at(getEmployer() + answering_num)->get_id();
}

vector<string>* Game::get_questions() const{
    return g_questions;
}

void Game::add_question(string q){
    g_questions->push_back(q);
}

void Game::rem_question(){
    get_questions()->erase(g_questions->begin());
}

bool Game::no_questions() const{
    if(status != QUESTIONS){
        return true;
    }
    for(vector<Player*>::iterator p = g_players->begin(); p != g_players->end(); p++){
        if((*p)->getStatus() == QUESTIONING){
            return false;
        }
    }
    if(!(get_questions()->empty())){
        return false;
    }
    return true;
}

string Game::open_p(int id) const {
    string result;
    Player* p = getPlayer(id);
    result += "   Карты " + get_player_nick(id) + ":\n";
    if(p->getEmoji()){
        result += "   Эмоция: " + p->getEmoji()->get_text() + "\n";
    } else {
        result += "   Эмоция: (не получена)\n";
    }
    result += "   Навыки:\n";
    result += p->print_skills();
    return result;
}

int Game::get_scoreb() const{
    return score_buf;
}

void Game::set_scoreb(int ns){
    score_buf = ns;
}

void Game::add_scoreb(int as){
    score_buf += as;
}

bool Game::score_over() const{
    if(status != SCORES){
        return true;
    }
    for(vector<Player*>::iterator p = g_players->begin(); p != g_players->end(); p++){
        if((*p)->getStatus() == SCORING){
            return false;
        }
    }
    return true;
}


string Game::assign_professions() {
    string result;
    vector<Card*>* vacancies = g_employ->getProfs();
    
    for (size_t i = 0; i < vacancies->size(); i++) {
        int chosen_player_id = g_employ->get_assignment(i);
        vector<int>* claimants = g_employ->get_claims_for_vacancy(i);
        Card* profession = vacancies->at(i);
        
        if (!claimants || claimants->empty()) {
            char buffer[256];
            sprintf(buffer, "Вакансия \"%s\" никому не досталась (нет претендентов)\n", 
                    profession->get_text().c_str());
            result += buffer;
            continue;  
        }
        
        if (chosen_player_id != -1) {
            auto it = find(claimants->begin(), claimants->end(), chosen_player_id);
            if (it != claimants->end()) {  
                Player* chosen_player = getPlayer(chosen_player_id);
                if (chosen_player) {
                    chosen_player->addProf(profession);
                    chosen_player->addScore(3);
                    char buffer[256];
                    sprintf(buffer, "Вакансия \"%s\" достаётся %s (выбор работодателя)!\n", 
                            profession->get_text().c_str(), 
                            chosen_player->get_nick().c_str());
                    result += buffer;
                    continue;
                }
            } else {
                char buffer[256];
                sprintf(buffer, "Работодатель выбрал %s, но он не претендовал на вакансию \"%s\"!\n", 
                        getPlayer(chosen_player_id)->get_nick().c_str(),
                        profession->get_text().c_str());
                result += buffer;
            }
        }
        
        if (claimants && !claimants->empty()) {
            int random_index = rand() % claimants->size();
            int random_player_id = (*claimants)[random_index];
            Player* chosen_player = getPlayer(random_player_id);
            
            if (chosen_player) {
                chosen_player->addProf(profession);
                chosen_player->addScore(3);
                char buffer[256];
                sprintf(buffer, "Вакансия \"%s\" достаётся %s (случайный выбор среди претендентов)!\n", 
                        profession->get_text().c_str(), 
                        chosen_player->get_nick().c_str());
                result += buffer;
            }
        }
    }
    
    g_employ->clear_claims();
    g_employ->clear_assignments();
    vacancies->clear();
    
    return result;
}

string Game::get_players_list() const {
    string result;
    for (auto p : *g_players) {
        if (p->getStatus() != LEFT && p->get_id() != getEmployerId()) {
            char buffer[256];
            sprintf(buffer, "%d) %s\n", p->get_id(), p->get_nick().c_str());
            result += buffer;
        }
    }
    return result;
}

void Game::drop_cards(){

    vector<Card*>* to_hand_s = new vector<Card*>;
    vector<Card*>* to_hand_e = new vector<Card*>;
    for(vector<Player*>::iterator pl = g_players->begin(); pl != g_players->end(); pl++){
        if(!(*pl)->getSkills()->empty()){
            PassCards((*pl)->getSkills(), to_hand_s, SKILL_NUM);
        }
        if((*pl)->getEmoji() != nullptr){
            to_hand_e->push_back((*pl)->getEmoji());
            (*pl)->remEmoji();
        }
    }
    if(!to_hand_s->empty()){
        ShuffleCards(to_hand_s);
        PassCards(to_hand_s, g_skills, to_hand_s->size());
        delete to_hand_s;
    }
    if(!to_hand_e->empty()){
        ShuffleCards(to_hand_e);
        PassCards(to_hand_e, g_emoji, to_hand_e->size());
        delete to_hand_e;
    } 
}


string Game::Endgame(StartupDbContext* context){
    string result;
    int max = 0;
    int c_score = 0;
    
    result += "======================================\n";
    result += "       ИГРА ОКОНЧЕНА!\n";
    result += "======================================\n";
    
    // Сохраняем информацию об игроках в строку
    string players_info;
    for(auto pl : *g_players){
        char buffer[256];
        sprintf(buffer, "%s (очки: %d)\n", 
                pl->get_nick().c_str(), 
                pl->getScore());
        players_info += buffer;
    }
    result += players_info;
    
    for(auto pl : *g_players){
        if(pl->getStatus() != LEFT && pl->getScore() > max){
            max = pl->getScore();
        }
    }
    
    vector<Player*> winners;
    for(auto pl : *g_players){
        if(pl->getStatus() != LEFT && pl->getScore() == max){
            winners.push_back(pl);
        }
    }
    
    result += "\n";
    
    for(auto pl : *g_players){
        char buffer[256];
        int reward;
        
        if(pl->getStatus() == LEFT){
            reward = -REWARD_K * getPnum();
            sprintf(buffer, "%s (вышел из игры) - изменение рейтинга: %d\n", 
                    pl->get_nick().c_str(), reward);
        }
        else if(pl->getScore() == max){
            reward = (REWARD_K * getPnum()) / winners.size();
            sprintf(buffer, "%s - ПОБЕДИТЕЛЬ! Очки: %d, изменение рейтинга: +%d\n", 
                    pl->get_nick().c_str(), pl->getScore(), reward);
        }
        else {
            reward = -REWARD_K / (getPnum() - winners.size());
            sprintf(buffer, "%s - очки: %d, изменение рейтинга: %d\n", 
                    pl->get_nick().c_str(), pl->getScore(), reward);
        }
        
        result += buffer;
        context->add_player_score(pl->get_nick(), reward);
    }
    
    result += "\n";
    if(winners.size() > 1){
        result += "       ПОБЕДИТЕЛИ:\n";
    } else {
        result += "       ПОБЕДИТЕЛЬ:\n";
    }
    
    for(auto pl : winners){
        result += "      " + pl->get_nick() + "\n";
    }
    
    result += "======================================\n";
    
    return result;
}


Employ_Info::Employ_Info(){
    e_profs = new vector<Card*>;
    claim_matrix = new vector<int>[EMPLOYER_PROFS_NUM];
}

Employ_Info::~Employ_Info(){
    e_profs->clear();
    delete e_profs;
}


vector<Card*>* Employ_Info::getProfs() const{
    return e_profs;
}

string Employ_Info::getManual() const{
    return manual;
}

void Employ_Info::setManual(string n_man){
    manual = n_man;
}

string Employ_Info::print_profs() const{
    string res = "";
    if(e_profs){
        // cout << "-------------------" << endl;
        // cout << " Вакансии:" << endl;
        // cout << "-------------------" << endl;
        res += "-------------------\n";
        res += "Вакансии:\n";
        res += "-------------------\n";
        int i = 1;
        for(vector<Card*>::const_iterator pr = e_profs->begin(); pr != e_profs->end(); pr++){
            //cout << " " << i << ") " << **pr << endl;
            char buf[256];
            sprintf(buf, " %d) %s\n", i, (*pr)->get_text().c_str());
            res += buf;
            i++;
        }
        //cout << "-------------------" << endl;
        res += "-------------------\n";
    }
    return res;
}

void Employ_Info::add_claim(int vac, int id){
    claim_matrix[vac].push_back(id);
}

vector<int>* Employ_Info::get_claims_for_vacancy(int vac_index) {
    if (vac_index >= 0 && vac_index < EMPLOYER_PROFS_NUM) {
        return &claim_matrix[vac_index];
    }
    return nullptr;
}

void Employ_Info::clear_claims() {
    for (int i = 0; i < EMPLOYER_PROFS_NUM; i++) {
        claim_matrix[i].clear();
    }
}

void Employ_Info::add_assignment(int vac, int player){ 
    assignments[vac] = player; 
}

void Employ_Info::clear_assignments(){ 
    assignments.clear(); 
}

int Employ_Info::get_assignment(int vac){ 
    return assignments.count(vac) ? assignments[vac] : -1; 
}


