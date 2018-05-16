template<class S, class A, class H>
std::pair<typename StateNode<S, A>::Action, double> negamax_alpha_beta(
    StateNode<S, A> node, size_t depth, double alpha, double beta,
    H heuristic)
{

    int color = node.is_maximizing() ? 1 : -1;

    //    std::cerr << "Depth: " << 3 - depth << std::endl;

    if (depth == 0 || node.is_final()) {
        //  std::cerr << "Max: " << node.is_maximizing() << " -> " << as_string(
        //                node.state().state()) << " with h: " << color*heuristic(node.state()) <<
        //            std::endl;


        return std::make_pair(typename StateNode<S, A>::Action(),
                              color * heuristic(node));
    }

    auto children = node.children();
    double best_value = -1e10; // Very negative number, -∞ in practice
    typename StateNode<S, A>::Action best_action;

    for (auto child_p : children) {
        auto child = child_p.second;

        double next_alpha = alpha;
        double next_beta = beta;

        if (child.is_maximizing()) {
            next_alpha = -beta;
            next_beta = -alpha;
        }

        auto r = negamax_alpha_beta(child, depth - 1, next_alpha, next_beta,
                                    heuristic);

        if (child.is_maximizing() != node.is_maximizing()) {
            r.second *= -1;
        }

        if (r.second > best_value) {
            best_value = r.second;
            best_action = child_p.first;
        }

        alpha = std::max(alpha, r.second);

        if (alpha >= beta) {
            continue;
        }
    }

    return std::make_pair(best_action, best_value);
}
