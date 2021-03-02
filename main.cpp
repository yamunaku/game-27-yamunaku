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
constexpr int depth_of_calc = 7;

enum class Color { Blue, Red };

struct Board {
   private:
    bool turn;
    vector<vector<Color>> stones;

   public:
    bool get_turn() const;
    const vector<vector<Color>> &get_stones() const;
    vector<Hand> my_next() const;
    vector<Hand> opp_next() const;
    pair<Position, Position> count_tower() const;
    void my_move(const Hand &hand);
    void opp_move(const Hand &hand);
    Board(bool _turn = true) : turn(_turn) {
        stones = vector<vector<Color>>(board_size);
        stones[0] = vector<Color>(init_stone_num, Color::Blue);
        stones[board_size - 1] = vector<Color>(init_stone_num, Color::Red);
    }
    Board(const Board &board)
        : turn(board.get_turn()), stones(board.get_stones()) {}
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

bool Board::get_turn() const { return turn; }
const vector<vector<Color>> &Board::get_stones() const { return stones; }

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

vector<Hand> Board::my_next() const {
    vector<Hand> hands;
    int blue_num = count_tower().first;
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
    return hands;
}

vector<Hand> Board::opp_next() const {
    vector<Hand> hands;
    int red_num = count_tower().second;
    for (int i = 0; i < board_size; i++) {
        if (stones[i].size() > 0) {
            if (stones[i].back() == Color::Red) {
                if (i - red_num >= 0) {
                    for (int j = 1; j <= stones[i].size(); j++) {
                        hands.push_back(Move(i, j));
                    }
                }
            }
        }
    }
    if (hands.size() == 0) hands.push_back(Pass());
    return hands;
}

int eval(const Board &board) {
    const vector<vector<Color>> &stones = board.get_stones();
    int score = 0;
    score += 5 * stones.back().size();
    score -= 5 * stones.front().size();
    for (int i = 1; i < board_size - 1; i++) {
        if (stones[i].size() > 0) {
            if (stones[i].back() == Color::Blue) {
                score += stones[i].size();
            }
        }
    }
    return score;
}
struct TreeNode {
   private:
    bool is_first;
    unique_ptr<Board> board_p;
    pair<Hand, int> best_hand;
    vector<pair<Hand, unique_ptr<TreeNode>>> children;

    void make_children();

   public:
    void calc_value(int height);
    pair<Hand, int> get_best_hand() const;
    unique_ptr<TreeNode> choose(const Hand &hand);
    TreeNode(const Board &board)
        : is_first(board.get_turn()), best_hand(pair<Hand, int>(Pass(), 0)) {
        board_p = make_unique<Board>(board);
    }
    TreeNode(const Board &board, const Hand &hand)
        : is_first(!board.get_turn()), best_hand(pair<Hand, int>(Pass(), 0)) {
        board_p = make_unique<Board>(board);
        if (board_p->get_turn())
            board_p->my_move(hand);
        else
            board_p->opp_move(hand);
    }
};

void TreeNode::calc_value(int height) {
    if (height <= 0) {
        best_hand.second = eval(*board_p);
        return;
    }
    make_children();
    for (const auto &ch : children) {
        ch.second->calc_value(height - 1);
    }
    if (is_first) {
        sort(children.begin(), children.end(),
             [](const auto &l, const auto &r) {
                 return l.second->get_best_hand().second >
                        r.second->get_best_hand().second;
             });
    } else {
        sort(children.begin(), children.end(),
             [](const auto &l, const auto &r) {
                 return l.second->get_best_hand().second <
                        r.second->get_best_hand().second;
             });
    }
    best_hand = {children.front().first,
                 children.front().second->get_best_hand().second};
}

void TreeNode::make_children() {
    if (!board_p) return;
    if (is_first) {
        const vector<Hand> hands = board_p->my_next();
        for (const auto &m : hands) {
            children.emplace_back(m, make_unique<TreeNode>(*board_p, m));
        }
    } else {
        const vector<Hand> hands = board_p->opp_next();
        for (const auto &m : hands) {
            children.emplace_back(m, make_unique<TreeNode>(*board_p, m));
        }
    }
    board_p.reset();
}

pair<Hand, int> TreeNode::get_best_hand() const { return best_hand; }

unique_ptr<TreeNode> TreeNode::choose(const Hand &hand) {
    for (auto &ch : children) {
        if (holds_alternative<Pass>(ch.first) &&
            holds_alternative<Pass>(hand)) {
            return move(ch.second);
        } else if (holds_alternative<Move>(ch.first) &&
                   holds_alternative<Move>(hand)) {
            const Move &m1 = get<Move>(ch.first);
            const Move &m2 = get<Move>(hand);
            if (m1.pos == m2.pos && m1.num == m2.num) return move(ch.second);
        }
    }
    assert(false);
    return make_unique<TreeNode>(Board());
}

struct GameTree {
   private:
    unique_ptr<TreeNode> root;

   public:
    Hand get_best_hand();
    void choose(const Hand &hand);
    GameTree(const Board &board) : root(make_unique<TreeNode>(board)) {}
};

Hand GameTree::get_best_hand() {
    root->calc_value(depth_of_calc);
    return root->get_best_hand().first;
}

void GameTree::choose(const Hand &hand) {
    unique_ptr<TreeNode> tmp = root->choose(hand);
    root.reset();
    root = move(tmp);
}

struct AI {
   private:
    GameTree tree;

   public:
    Hand calc_hand();
    void my_move(const Hand &hand);
    void opp_move(const Hand &hand);
    AI(const Board &board = Board()) : tree(board) {}
};

Hand AI::calc_hand() { return tree.get_best_hand(); }

void AI::my_move(const Hand &hand) { tree.choose(hand); }
void AI::opp_move(const Hand &hand) { tree.choose(hand); }

struct Player {
   private:
    AI ai;
    bool is_first;

    bool input_init() const;
    bool input_playing() const;
    Hand input_hand() const;
    void input_wait() const;
    int input_score() const;
    void output_hand(Hand hand) const;

   public:
    void play();
};

void Player::play() {
    is_first = input_init();
    ai = AI(Board(is_first));
    ai.calc_hand();
    if (is_first) {
        Hand my_hand = ai.calc_hand();
        ai.my_move(my_hand);
        input_wait();
        output_hand(my_hand);
    }
    while (input_playing()) {
        Hand opp_hand = input_hand();
        ai.opp_move(opp_hand);
        Hand my_hand = ai.calc_hand();
        ai.my_move(my_hand);
        input_wait();
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
    if (!is_first) pos = board_size - 1 - pos;
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

void Player::output_hand(Hand hand) const {
    if (!is_first) {
        if (holds_alternative<Move>(hand)) {
            Move &m = get<Move>(hand);
            m.pos = board_size - 1 - m.pos;
        }
    }
    cout << to_string(hand) << endl;
}

int main() {
    Player p;
    p.play();
    return 0;
}