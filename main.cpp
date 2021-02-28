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

enum class Color { Blue, Red };

struct Board {
    bool turn;
    vector<vector<Color>> stones;

    pair<Position, Position> count_tower() const;
    void my_move(const Hand &hand);
    void opp_move(const Hand &hand);
    Board(bool _turn = true) : turn(_turn) {
        stones = vector<vector<Color>>(board_size);
        stones[0] = vector<Color>(init_stone_num, Color::Blue);
        stones[board_size - 1] = vector<Color>(init_stone_num, Color::Red);
    }
};

pair<Position, Position> Board::count_tower() const {
    Position my = 0, opp = 0;
    for (const auto &v : stones) {
        if (v.size() > 0) {
            if (v.back() == Color::Blue)
                my++;
            else
                opp++;
        }
    }
    return {my, opp};
}

void Board::my_move(const Hand &hand) {
    pair<Position, Position> tower_num = count_tower();
    assert(turn == true);
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
        assert(sz >= m.num);
        assert(stones[m.pos].back() == Color::Blue);
        assert(m.pos + tower_num.first < board_size);
        for (int i = 0; i < m.num; i++) {
            stones[m.pos + tower_num.first].push_back(
                stones[m.pos][sz - m.num + i]);
        }
        stones[m.pos].resize(sz - m.num);
    }
    turn = false;
}

void Board::opp_move(const Hand &hand) {
    pair<Position, Position> tower_num = count_tower();
    assert(turn == false);
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
        assert(sz >= m.num);
        assert(stones[m.pos].back() == Color::Red);
        assert(m.pos - tower_num.second >= 0);
        for (int i = 0; i < m.num; i++) {
            stones[m.pos - tower_num.second].push_back(
                stones[m.pos][sz - m.num + i]);
        }
        stones[m.pos].resize(sz - m.num);
    }
    turn = true;
}

struct AI {
    Hand calc_hand(const Board &board);
};

Hand AI::calc_hand(const Board &board) {
    vector<Hand> hands;
    const auto &stones = board.stones;
    board.count_tower();
    int blue_num = board.count_tower().first;
    for (int i = 0; i < board_size; i++) {
        if (stones[i].size() > 0) {
            if (stones[i].back() == Color::Blue) {
                if (i + blue_num < board_size) {
                    for (int j = 1; j <= stones[i].size(); j++) {
                        hands.push_back(Move(i, j));
                    }
                }
            }
        }
    }
    if (hands.size() == 0) hands.push_back(Pass());
    return hands.front();
}

struct Player {
    Board board;
    AI ai;
    void play();
    bool input_init() const;
    bool input_playing() const;
    Hand input_hand() const;
    void input_wait() const;
    int input_score() const;
    void output_hand(const Hand &hand) const;
};

void Player::play() {
    bool is_first = input_init();
    board = Board(is_first);
    if (is_first) {
        Hand my_hand = ai.calc_hand(board);
        input_wait();
        board.my_move(my_hand);
        output_hand(my_hand);
    }
    while (input_playing()) {
        Hand opp_hand = input_hand();
        board.opp_move(opp_hand);
        Hand my_hand = ai.calc_hand(board);
        input_wait();
        board.my_move(my_hand);
        output_hand(my_hand);
    }
    int score = input_score();
}

bool Player::input_init() const {
    string msg;
    int is_second, timeout;
    cin >> msg;
    assert(msg == "init");
    cin >> is_second >> timeout;
    // TODO timeout
    return is_second == 0;
}

bool Player::input_playing() const {
    string msg;
    cin >> msg;
    assert(msg == "played" || msg == "result");
    return msg == "played";
}

Hand Player::input_hand() const {
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

void Player::input_wait() const {
    string msg;
    cin >> msg;
    assert(msg == "wait");
}

int Player::input_score() const {
    int score;
    cin >> score;
    assert(-18 <= score && score <= 18);
    return score;
}

void Player::output_hand(const Hand &hand) const {
    cout << to_string(hand) << endl;
}

int main() {
    Player p;
    p.play();
    return 0;
}