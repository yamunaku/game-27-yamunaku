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

string to_string(const Hand &hand) {
    if (holds_alternative<Pass>(hand)) {
        return "pass";
    } else {
        const Move &m = get<Move>(hand);
        return "move " + to_string(m.pos) + " " + to_string(m.num);
    }
}

constexpr Position board_size = 9;
constexpr Num init_stone_num = 9;

enum class Color { Red, Blue };

struct Board {
    bool turn;
    vector<vector<Color>> stones;
    const pair<Position, Position> count_tower();
    void move(Hand hand);

    Board(bool _turn = true) : turn(_turn) {
        stones = vector<vector<Color>>(board_size);
        stones[0] = vector<Color>(init_stone_num, Color::Blue);
        stones[board_size - 1] = vector<Color>(init_stone_num, Color::Red);
    }
};

const pair<Position, Position> Board::count_tower() {
    Position my = 0, opp = 0;
    for (auto &v : stones) {
        if (v.size() > 0) {
            if (v.back() == Color::Blue)
                my++;
            else
                opp++;
        }
    }
    return {my, opp};
}

void Board::move(Hand hand) {
    pair<Position, Position> tower_num = count_tower();
    if (turn) {
        if (holds_alternative<Pass>(hand)) {
            for (int i = 0; i < board_size; i++) {
                if (stones[i].size() > 0) {
                    if (stones[i].back() == Color::Blue) {
                        assert(i + tower_num.first >= board_size);
                    }
                }
            }
        } else {
            const Move &m = get<Move>(hand);
            int sz = stones[m.pos].size();
            assert(sz > m.num);
            assert(stones[m.pos].back() == Color::Blue);
            assert(m.pos + tower_num.first < board_size);
            for (int i = 0; i < m.num; i++) {
                stones[m.pos + tower_num.first].push_back(
                    stones[m.pos][sz - m.num + i]);
            }
            stones[m.pos].resize(sz - m.num);
        }
    } else {
        if (holds_alternative<Pass>(hand)) {
            for (int i = 0; i < board_size; i++) {
                if (stones[i].size() > 0) {
                    if (stones[i].back() == Color::Red) {
                        assert(i - tower_num.second < 0);
                    }
                }
            }
        } else {
            const Move &m = get<Move>(hand);
            int sz = stones[m.pos].size();
            assert(sz > m.num);
            assert(stones[m.pos].back() == Color::Red);
            assert(m.pos - tower_num.first >= 0);
            for (int i = 0; i < m.num; i++) {
                stones[m.pos - tower_num.first].push_back(
                    stones[m.pos][sz - m.num + i]);
            }
            stones[m.pos].resize(sz - m.num);
        }
    }
    turn = !turn;
}

struct Player {
    Board board;
    void play();
    bool input_init();
    bool input_playing();
    Hand input_hand();
    void input_wait();
    int input_score();
    void output_hand(const Hand &hand);
};

void Player::play() {
    bool is_first = input_init();
    board = Board(is_first);
    if (is_first) {
        input_wait();
        Hand my_hand;  // TODO
        board.move(my_hand);
        output_hand(my_hand);
    }
    while (input_playing()) {
        Hand opponent_hand = input_hand();
        board.move(opponent_hand);
        input_wait();
        Hand my_hand;  // TODO
        board.move(my_hand);
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

void Player::output_hand(const Hand &hand) { cout << to_string(hand) << endl; }

int main() {
    Player p;
    p.play();
    return 0;
}