#ifndef BOT_MCTS_HPP
#define BOT_MCTS_HPP

#include <iostream>

#include <map>
#include <list>


template<class BoardState, class Action>
class MCTS
{
  public:
    enum class Player {P1, P2};

    // template<class K, class V>
    // using DictType = std::map<K, V>;

    using HistoryType =
        std::list<BoardState>;

    void update_state(const BoardState&
                      state);
    virtual Action get_play() = 0;

  protected:
    //    DictType<BoardState, std::pair<uint64_t, uint64_t>>
    //         plays_and_wins;

    HistoryType history;

    virtual Action choice(
        std::list<std::pair<Action, BoardState>>
        possible_moves) const = 0;

    BoardState current_state() const;

    virtual void run_simulation() = 0;

};

#include "MCTS.cpp"

#endif // BOT_MCTS_HPP
