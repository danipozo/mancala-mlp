#ifndef ALPHA_BETA_HPP
#define ALPHA_BETA_HPP

#include "StateNode.hpp"

template<class S, class A, class H>
std::pair<typename StateNode<S, A>::Action, double> negamax_alpha_beta(
    StateNode<S, A> node, size_t depth, double alpha, double beta, int color,
    H heuristic);

#include "alpha_beta.cpp"

#endif // ALPHA_BETA_HPP
