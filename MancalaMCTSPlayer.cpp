#include "MancalaMCTSPlayer.hpp"

MCTSPlayer::MCTSPlayer(::Player p) :
  predictor({14, 100, 100, 1}, 0), board(), p(p)
{
    history.push_back(board);
}

void MCTSPlayer::read_nn(std::istream& is)
{
    is >> predictor;
}

void MCTSPlayer::output_nn(std::ostream& os)
{
    os << predictor;
}

Move MCTSPlayer::get_play()
{
    std::list<std::pair<Move, GameState>> l;

    for (int i = 1; i < 7; i++) {
        auto a = static_cast<Move>(i);
        auto gs_ = current_state().simulateMove(a);

        l.push_back(std::make_pair(a, gs_));
    }

    return choice(l);
}

std::string as_string(const GameState& gs)
{
    std::ostringstream os;

    for (auto p : {
                0, 1
            })

        for (int pos = 0; pos < 7; pos++) {
            os << (int) gs.getSeedsAt(static_cast<Player>(p),
                                      static_cast<Position>(pos)) << " ";
        }

    return os.str();
}

void MCTSPlayer::output_statistics(std::ostream& os) const
{
    int reward;

    ::Player winner = current_state().getWinner();

    if (winner == p) {
        reward = 1;
    } else if (winner == (p==J1 ? J2 : J1)) {
        reward = -1;
    } else {
        reward = 0;
    }

    for (auto state : history) {
        os << '\"' << as_string(state) << '\"' << ", " << reward << std::endl;
    }
}

MLP::VectorType to_vector_type(GameState gs)
{
    MLP::VectorType ret(14);

    for (auto p : {
                0, 1
            })

        for (int pos = 0; pos < 7; pos++) {
            ret[p * 7 + pos] = gs.getSeedsAt(static_cast<Player>(p),
                                             static_cast<Position>(pos));
        }

    return ret;
}

Move MCTSPlayer::choice(
    std::list<std::pair<Move, GameState>>
    possible_moves) const
{
    double epsilon = 0.1;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<double> uni(0, 1);

    auto r = uni(rng);

    // RANDOM ACTION
    if (r <= epsilon) {
        std::uniform_int_distribution<int> uni_ret(1, 6);

        //	std::cerr << "choice -> random action" << std::endl;
        return static_cast<Move>(uni_ret(rng));
    }

    // BEST ACTION

    // Move ret = M1;
    // double heuristic_value = -1;

    // //        std::cerr << "Heuristic:" << std::endl;
    // for (auto p : possible_moves) {
    //     double h = predictor.forward(to_vector_type(p.second))[0];
    // 	//  std::cerr << as_string(p.second) << " -> " <<  h << std::endl;

    //     if (h > heuristic_value) {
    //         heuristic_value = h;
    //         ret = p.first;
    //     }
    // }

    size_t depth = 5;
    Node root((GameStateWrapper(current_state())));
    //    std::cout << "current_state: " << as_string(current_state()) << std::endl;
    double initial_alpha = -1e10;
    double initial_beta = 1e10;

    // Some function magic here
    auto aux_lambda = [](MCTSPlayer & p, GameStateWrapper gsw) {
        return p.heuristic_function(gsw);
    };

    auto h = std::bind(aux_lambda, *this, std::placeholders::_1);

    //    std::cout << "About to call negamax" << std::endl;
    auto p = negamax_alpha_beta(root, depth, initial_alpha, initial_beta, 1, h);

    auto ret = p.first;

    //std::cout << "P" << (this->p == J1 ? "1" : "2") << ": " << "Board returned: " << as_string(current_state().simulateMove(ret)) << " with h: " << p.second << std::endl;

    return ret;
}

double MCTSPlayer::heuristic_function(GameStateWrapper gsw) const
{
    double h = predictor.forward(to_vector_type(gsw.state()))(0);
    double ret = p == J1 ? h : 1-h;

    //std::cout << "P" << (p == J1 ? "1" : "2") << ": " <<  as_string(gsw.state()) << " -> " << ret << std::endl;


    return ret;
}

// I may not actually use this during training
void MCTSPlayer::run_simulation()
{
    // using DepthType = size_t;

    // GameState node_board = current_state();
    // auto node = std::make_pair(node_board, size_t(0));
    // std::stack<std::pair<GameState, DepthType>> s;

    // s.push(node);

    // while (!s.empty()) {
    //     node = s.top();
    //     s.pop();

    //     if (node.second > depth || node.first.isFinalState()) {
    //         continue;
    //     }

    //     GameState gs = node.first;
    //     std::list<std::pair<Move, GameState>> l;

    //     for (int i = 1; i < 7; i++) {
    //         auto a = static_cast<Move>(i);
    //         auto gs_ = gs.simulateMove(a);

    //         l.push_back(std::make_pair(a, gs_));
    //     }

    //     // Push node selected by choice into the stack
    //     s.push(std::make_pair(gs.simulateMove(choice(l)),
    //                           node.second + 1));
    // }

    // return;
}
