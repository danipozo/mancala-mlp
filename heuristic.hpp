#ifndef HEURISTIC_HPP
#define HEURISTIC_HPP

#include "GameState_mod.h"
#include "StateNode.hpp"
#include "nn.hpp"

#include <list>


class GameStateWrapper
{
    GameState gs;
    Player p;

  public:
    GameStateWrapper() = default;
    GameStateWrapper(GameState gs, Player p) : gs(gs), p(p) {}

    GameState state() const
    {
        return gs;
    }

    Player player() const
    {
        return p;
    }

    bool is_final() const
    {
        return gs.isFinalState();
    }

    bool is_maximizing() const
    {
        // std::cout << "P" << (p == J1 ? "1" : "2") << " " <<  (gs.getCurrentPlayer() == p ? "maximizing" : "minimizing") <<
        //              std::endl;
        return gs.getCurrentPlayer() == p;
    }

    std::list<Move> possible_actions() const
    {
        std::list<Move> ret = { M1, M2, M3, M4, M5, M6 };

        return ret;
    }

    GameStateWrapper apply_action(Move a) const
    {
        return GameStateWrapper(gs.simulateMove(a), p);
    }
};

MLP::VectorType to_vector_type(GameState gs);

using Node = StateNode<GameStateWrapper, Move>;

class MancalaHeuristic
{
    MLP mlp[2];

  public:
    MancalaHeuristic() = default;

    void read_nn_for(Player p, std::istream& is);

    double operator()(Node n) const;
};

#endif // HEURISTIC_HPP
