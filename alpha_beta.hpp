#ifndef ALPHA_BETA_HPP
#define ALPHA_BETA_HPP

#include <list>
#include <utility>
#include <algorithm>

template<class S, class A>
class StateNode
{
  public:
    using State = S;
    using Action = A;

  private:
    State s;

  public:
    StateNode(State s) : s(s) {}

    State state() const
    {
        return s;
    }

    bool is_final() const
    {
        return state().is_final();
    }

    std::list<std::pair<Action, StateNode>> children() const
    {
        std::list<std::pair<Action, StateNode>> ret;
        std::list<Action> possible_actions = state().possible_actions();

        std::transform(possible_actions.begin(), possible_actions.end(),
        std::back_inserter(ret), [ = ](Action a) {
            return std::make_pair(a, StateNode(s.apply_action(a)));
        });

        return ret;
    }
};

template<class S, class A, class H>
std::pair<typename StateNode<S, A>::Action, double> negamax_alpha_beta(
    StateNode<S, A> node, size_t depth, double alpha, double beta, int color,
    H heuristic);

#include "alpha_beta.cpp"

#endif // ALPHA_BETA_HPP
