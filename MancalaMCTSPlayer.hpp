#include "MCTS.hpp"
#include "GameState_mod.h"
#include "nn.hpp"
#include "alpha_beta.hpp"

#include <random>
#include <stack>
#include <sstream>
#include <set>
#include <functional>

class GameStateWrapper
{
    GameState gs;

  public:
    GameStateWrapper() = default;
    GameStateWrapper(GameState gs) : gs(gs) {}

    GameState state() const
    {
        return gs;
    }

    bool is_final() const
    {
        return gs.isFinalState();
    }

    std::list<Move> possible_actions() const
    {
        std::list<Move> ret = { M1, M2, M3, M4, M5, M6 };

        return ret;
    }

    GameStateWrapper apply_action(Move a) const
    {
        return GameStateWrapper(gs.simulateMove(a));
    }
};

using Node = StateNode<GameStateWrapper, Move>;

class MCTSPlayer : public MCTS<GameState, Move>
{
  public:

  MCTSPlayer(::Player p = J1);

    Move get_play() override;

    void read_nn(std::istream& is);
    void output_nn(std::ostream& os);
    void output_statistics(std::ostream& os) const;

  private:
    MLP predictor;
    GameState board;
  ::Player p;

  protected:
    Move choice(std::list<std::pair<Move, GameState>>
                possible_moves)
    const override;
    void run_simulation() override;

    double heuristic_function(GameStateWrapper gsw) const;
};
