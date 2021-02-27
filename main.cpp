#include <bits/stdc++.h>
using namespace std;

using Position = int;
using Num = int;

struct Pass {};
struct Move {
    Position pos;
    Num num;
    Move(Position _pos, Num _num) : pos(_pos), num(_num) {}
};

using Hand = variant<Pass, Move>;

string to_string(Hand hand) {
    struct ToStringVisitor {
        string operator()(Pass p) { return "pass"; }
        string operator()(Move m) {
            return "move " + to_string(m.pos) + " " + to_string(m.num);
        }
    };
    return visit(ToStringVisitor{}, hand);
}

struct Player {
    void play();
    bool input_init();
    bool input_playing();
    Hand input_hand();
    void input_wait();
    int input_score();
    void output_hand(Hand hand);
};

void Player::play() {
    bool is_first = input_init();
    if (is_first) {
        input_wait();
        Hand my_hand;  // TODO
        output_hand(my_hand);
    }
    while (input_playing()) {
        Hand opponent_hand = input_hand();  // TODO
        input_wait();
        Hand my_hand;  // TODO
        output_hand(my_hand);
    }
    int score = input_score();
}

bool Player::input_init() {
    string msg;
    int is_second, timeout;
    cin >> msg;
    assert(msg == "init");
    cin >> is_second >> timeout;
    // TODO timeout
    return is_second == 0;
}

bool Player::input_playing() {
    string msg;
    cin >> msg;
    assert(msg == "played" || msg == "result");
    return msg == "played";
}

Hand Player::input_hand() {
    string msg;
    cin >> msg;
    assert(msg == "move" || msg == "pass");
    if (msg == "pass") return Pass();
    Position pos;
    Num num;
    cin >> pos >> num;
    assert(0 <= pos && pos < 9);
    assert(0 <= num);
    return Move(pos, num);
}

void Player::input_wait() {
    string msg;
    cin >> msg;
    assert(msg == "wait");
}

int Player::input_score() {
    int score;
    cin >> score;
    assert(-18 <= score && score <= 18);
    return score;
}

void Player::output_hand(Hand hand) { cout << to_string(hand) << endl; }

int main() {
    Player p;
    p.play();
    return 0;
}