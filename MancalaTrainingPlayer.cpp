#include "MancalaTrainingPlayer.hpp"

TrainingPlayer::TrainingPlayer(MancalaHeuristic h, Player p) :
    predictor(), board(), p(p), h(h)
{
    history.push_back(board);
}

void TrainingPlayer::set_player(Player p_)
{
    p = p_;
}

void TrainingPlayer::set_epsilon(double e)
{
  epsilon = e;
}

void TrainingPlayer::read_nn(std::istream& is)
{
    is >> predictor;
}

void TrainingPlayer::output_nn(std::ostream& os)
{
    os << predictor;
}

Move TrainingPlayer::get_play()
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

void TrainingPlayer::output_statistics(std::ostream& os, double reward) const
{
    double r = reward;
    double decay_rate = 0.9;

    Player winner = current_state().getWinner();

    if (reward == 0) {
        if (winner == p) {
            r = 1;
        } else if (winner == (p == J1 ? J2 : J1)) {
            r = -1;
        } else {
            r = 0;
        }
    }

    double coeff = std::pow(decay_rate, history.size() - 1);

    for (auto state : history) {
        if (!state.isValidState()) {
            continue;
        }

        os << '\"' << as_string(state) << '\"' << ", " << coeff*r << std::endl;
        coeff /= decay_rate;
    }
}

Move TrainingPlayer::choice(
    std::list<std::pair<Move, GameState>>
    possible_moves) const
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<double> uni(0, 1);

    auto r = uni(rng);

    // RANDOM ACTION
    if (r <= epsilon) {
        std::uniform_int_distribution<int> uni_ret(1, 6);

        Move ret = (Move) uni_ret(rng);

        //        std::cerr << "Player: " << (int) p << std::endl;

        while (current_state().simulateMove(ret).getSeedsAt((p == J1 ? J2 : J1),
                GRANERO) == 48) {
            ret = (Move) uni_ret(rng);
        }

        // std::cerr << "choice -> random action" << std::endl;
        // std::cerr << as_string(current_state().simulateMove(ret)) << std::endl;


        return ret;
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

    size_t depth = 6;
    Node root((GameStateWrapper(current_state(), p)));
    //    std::cout << "current_state: " << as_string(current_state()) << std::endl;
    double initial_alpha = -1e10;
    double initial_beta = 1e10;

    // Some function magic here
    // auto aux_lambda = [](TrainingPlayer & p, GameStateWrapper gsw) {
    //     return p.heuristic_function(gsw);
    // };

    // auto h = std::bind(aux_lambda, *this, std::placeholders::_1);

    //std::cerr << "About to call negamax" << std::endl;
    auto p = negamax_alpha_beta(root, depth, initial_alpha, initial_beta, h);

    auto ret = p.first;

    static int i = 0;

    // if ((i % 50) == 0) {
    // std::cerr << "P" << (this->p == J1 ? "1" : "2") << ": " << "Board returned ("
    //           << (int)ret << "): " <<
    //           as_string(current_state().simulateMove(ret)) << " with h: " << p.second <<
    //           std::endl;
    // }

    i++;

    return ret;
}

// double TrainingPlayer::heuristic_function(GameStateWrapper gsw) const
// {

//     Player other = p == J1 ? J2 : J1;

//     // Avoid suicide
//     if (gsw.state().getSeedsAt(other, GRANERO) == 48) {
//         //std::cerr << "P" << (p == J1 ? "1" : "2") << ": " <<  as_string(
//         //                gsw.state()) << " -> " << 0 << std::endl;
//         return 0;
//     }

//     if (gsw.state().getSeedsAt(p, GRANERO) == 48) {
//         return 1;
//     }

//     double h = predictor.forward(to_vector_type(gsw.state()))(0);
//     //double ret = h;
//     double ret = p == J1 ? h : 1 - h;

//     // Player other = p == J1 ? J2 : J1;
//     // double ret = gsw.state().getSeedsAt(p, GRANERO) - gsw.state().getSeedsAt(other,
//     //              GRANERO);

//     // if (gsw.state().getSeedsAt(J1, GRANERO) == 48
//     //         || gsw.state().getSeedsAt(J2, GRANERO) == 48) {
//     //     std::cerr << "P" << (p == J1 ? "1" : "2") << ": " <<  as_string(
//     //                   gsw.state()) << " -> " << ret << std::endl;
//     // }


//     return ret;
// }

// I may not actually use this during training
//void TrainingPlayer::run_simulation()
//{
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
//}
