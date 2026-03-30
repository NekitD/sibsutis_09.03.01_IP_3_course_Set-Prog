//СТАРТАП-БИБЛИОТЕКА
#include "game.h"


int encode_msg(char* msg, int& status, int len){
    return 0;
}

int decode_msg(char* msg, int& status, int len){
    return 0;
}

int get_line_b(char* n_line, char* o_line, int start, int len, char b){
    char c;
    for(int i = 0, j = start; i < len, j < len; i++, j++){
        c = o_line[j];
        if(c == b){
            return j + 1;
        }
        n_line[i] = c;
    }
    return len - 1;
}

ostream& operator<<(ostream& os, const Card& c){
    os << c.text;
    return os;
}

Card::Card(string _text_1): text(_text_1){}
Card::~Card(){};


ostream& operator<<(ostream& os, const Player& p){
    os << p.name <<  "(";
    for(vector<Card*>::iterator it = p.p_profs->begin(); it != p.p_profs->end(); it++){
        os << (**it) << ", ";
    }
    os << ")" <<":    " << p.score;
    if (p.getStatus() == LEFT){
        os << "(Вышел)";
    }
    return os;
}


Player::Player(string _name, int _id): name(_name), score(0), p_profs(new vector<Card*>) , 
            p_skills(new vector<Card*>), p_emoji(nullptr), id(_id), status(WAIT_ACCEPT){};

Player::~Player(){};

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

Game::Game()
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

        status = PRE;
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

vector<Card*>* Game::get_profs(){
    return g_profs;
}

vector<Card*>* Game::get_skills(){
    return g_skills;
}

vector<Card*>* Game::get_emoji(){
    return g_emoji;
}

vector<Player*>* Game::get_players(){
    return g_players;
}

void Game::print_profs(){
    if(g_profs){
        for(vector<Card*>::iterator pr = g_profs->begin(); pr != g_profs->end(); pr++){
            cout << **pr;
        }
    }
}
void Game::print_skills(){
    if(g_skills){
        for(vector<Card*>::iterator sk = g_skills->begin(); sk != g_skills->end(); sk++){
            cout << *sk;
        }
    }
}
void Game::print_emoji(){
    if(g_emoji){
        for(vector<Card*>::iterator ej = g_emoji->begin(); ej != g_emoji->end(); ej++){
            cout << *ej;
        }
    }
}
void Game::print_players(){
    if(g_players){
        for(vector<Player*>::iterator p = g_players->begin(); p != g_players->end(); p++){
            cout << *p;
        }
    } 
}

int Game::getStatus() const{
    return status;
}

void Game::setStatus(int ns){
    status = ns;
}

void Game::addPlayer(char* nick){
    Player* p = new Player(nick, p_num + 1);
    g_players->push_back(p);
    p_num = getPnum();
    cout << nick << " присоединился к игре!" << endl;
    cout << "Игроков: " << getPnum() << " / {" << MIN_P << " - " << MAX_P << "}" << endl;
    cout << "Готовы: " << getRnum() << " / " << getPnum() << "\n" << endl;
    if(getPnum() >= MAX_P){
        setStatus(FULL);
    }
}

int Game::getPnum() const{
    return g_players->size();
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

void Game::remPlayer(int id){
    for(vector<Player*>::iterator p = g_players->begin(); p != g_players->end(); p++){
        if((*p)->get_id() == id){
            if (getStatus() == PRE || getStatus() == FULL){
                cout << get_player_nick(id) << " покинул игру." << endl;
                g_players->erase(p);
                cout << "Игроков: " << getPnum() << " / {" << MIN_P << " - " << MAX_P << "}" << endl;
                cout << "Готовы: " << getRnum() << " / " << getPnum() << "\n" << endl;
            } else {
                (*p)->setStatus(LEFT);
                if(getPnum() < MIN_P){
                    cout << "Количество игроков меньше " << MIN_P << endl;
                    setStatus(OVER);
                }
            }
            break;
        }
    }
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
    for(vector<Player*>::iterator it = g_players->begin(); it != g_players->end(); it++){
        if ((*it)->getStatus() == READY_TO_PLAY){
            ready++;
        }
    }
    return ready;
}


bool Game::isGameReady() const{
    return ((getPnum() >= MIN_P ) && (getRnum() == getPnum()));
}


void Game::Endgame() const{
    int max = 0;
    int c_score = 0;
    vector<Player*> winners;
    for(vector<Player*>::iterator pl = g_players->begin(); pl != g_players->end(); pl++){
        cout << (**pl) << endl;
        c_score = (*pl)->getScore();
        if(c_score > max){
            max = c_score;
        }
    }
    for(vector<Player*>::iterator pl = g_players->begin(); pl != g_players->end(); pl++){
        cout << (**pl) << endl;
        if((*pl)->getScore() == max){
            winners.push_back(*pl);
        }
    }
    if(winners.size() > 1){
        cout << "ПОБЕДИТЕЛИ:" << endl;
    } else {
        cout << "ПОБЕДИТЕЛЬ:" << endl;
    }
    
    for(vector<Player*>::iterator pl = g_players->begin(); pl != g_players->end(); pl++){
        cout << (**pl).get_nick() << endl;
    }
}
