#include "Player.hpp"
#include "GameState_mod.h"
#include "nn.hpp"
#include "alpha_beta.hpp"
#include "heuristic.hpp"

#include <random>
#include <stack>
#include <sstream>
#include <set>
#include <functional>


class TrainingPlayer : public AbstractPlayer<GameState, Move>
{
  public:

    TrainingPlayer() = default;
    TrainingPlayer(MancalaHeuristic h, Player p = J1);

    Move get_play() override;

    void set_player(Player p);
  void set_epsilon(double e);

    void read_nn(std::istream& is);
    void output_nn(std::ostream& os);
    void output_statistics(std::ostream& os, double reward = 0) const;

  private:
    MLP predictor;
    GameState board;
    Player p;
    MancalaHeuristic h;

  double epsilon = -1;

  protected:
    Move choice(std::list<std::pair<Move, GameState>>
                possible_moves)
    const override;
    //  void run_simulation() override;

    //double heuristic_function(GameStateWrapper gsw) const;
};
