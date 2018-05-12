#include "nn.hpp"

std::ostream& operator<<(std::ostream& os, const MLP& mlp)
{
    for (auto s : mlp.sizes) {
        os << s << " ";
    }

    os << std::endl;


    for (auto w : mlp.weights) {
        os << w << std::endl;
    }

    os << std::endl;

    for (auto b : mlp.biases) {
        os << b << std::endl;
    }

    os << std::endl;

    return os;
}

Eigen::MatrixXd read_matrix_from_stream(std::istream& is,
                                        int rows,
                                        int columns)
{
    Eigen::MatrixXd ret(rows, columns);

    for (int i = 0; i < rows * columns; i++) {
        double x;
        is >> x;

        ret(i % rows, i / rows) = x;
    }

    return ret;
}

Eigen::VectorXd read_vector_from_stream(std::istream& is,
                                        int size)
{
    Eigen::VectorXd ret(size);

    for (int i = 0; i < size; i++) {
        double x;
        is >> x;

        ret(i) = x;
    }

    return ret;
}

std::istream& operator>>(std::istream& is, MLP& mlp)
{
    std::string first_line;
    std::getline(is, first_line);

    std::istringstream sstream(first_line);
    std::vector<size_t> layer_sizes;

    for (size_t n; sstream >> n;) {
        layer_sizes.push_back(n);
    }

    std::vector<Eigen::MatrixXd> weights;

    for (size_t i = 0; i < layer_sizes.size() - 1; i++) {
        auto w = read_matrix_from_stream(is, layer_sizes[i + 1],
                                         layer_sizes[i]);
        weights.push_back(w);
    }

    std::vector<Eigen::VectorXd> biases;

    for (size_t i = 0; i < layer_sizes.size() - 1; i++) {
        auto b = read_vector_from_stream(is, layer_sizes[i + 1]);
        biases.push_back(b);
    }

    mlp.sizes = layer_sizes;
    mlp.weights = weights;
    mlp.biases = biases;

    return is;
}
