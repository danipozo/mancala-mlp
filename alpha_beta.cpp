template<class S, class A, class H>
std::pair<typename StateNode<S, A>::Action, double> negamax_alpha_beta(
    StateNode<S, A> node, size_t depth, double alpha, double beta, int color,
    H heuristic)
{
    if (depth == 0 || node.is_final()) {
        // std::cerr << "Heuristic value returned: " << heuristic(node.state()) <<
        //     std::endl;

        return std::make_pair(typename StateNode<S, A>::Action(),
                              color * heuristic(node.state()));
    }

    auto children = node.children();
    double best_value = -1e10; // Very negative number, -∞ in practice
    typename StateNode<S, A>::Action best_action;

    for (auto child_p : children) {
        auto child = child_p.second;
        auto r = negamax_alpha_beta(child, depth - 1, -beta, -alpha, -color,
                                    heuristic);
        r.second *= -1;

        if (r.second > best_value) {
            best_value = r.second;
            best_action = child_p.first;
        }

        alpha = std::max(alpha, r.second);

        if (alpha >= beta) {
            break;
        }
    }

    return std::make_pair(best_action, best_value);
}
