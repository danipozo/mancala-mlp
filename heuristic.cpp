#include "heuristic.hpp"

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


void MancalaHeuristic::read_nn_for(Player p, std::istream& is)
{
    int idx = p == J1 ? 0 : 1;

    is >> mlp[idx];
}

double MancalaHeuristic::operator()(Node n) const
{

    Player other = n.state().player() == J1 ? J2 : J1;

    // Avoid suicide
    if (n.state().state().getSeedsAt(other, GRANERO) == 48) {
        //std::cerr << "P" << (p == J1 ? "1" : "2") << ": " <<  as_string(
        //                gsw.state()) << " -> " << 0 << std::endl;
        return 0;
    }

    if (n.state().state().getSeedsAt(n.state().player(), GRANERO) == 48) {
        return 1;
    }


    // Assume J1: maximizing -> 0 index
    int idx = n.is_maximizing() ? 0 : 1;

    // J2: change index
    if (n.state().player() == J2) {
        idx = 1 - idx;
    }

    return mlp[idx].forward(to_vector_type(n.state().state()))(0);
}
