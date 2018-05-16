#ifndef NN_HPP
#define NN_HPP

#include "Eigen/Dense"

#include <vector>
#include <initializer_list>
#include <algorithm>
#include <numeric>
#include <utility>
#include <random>
#include <cmath>

#include <iostream>
#include <sstream>

struct sigmoid {
    double operator()(double x) const
    {
        return 1. / (1. + std::exp(-x));
    }
};

struct sigmoid_prime {
    double operator()(double x) const
    {
        return std::exp(-x) / std::pow(1. + std::exp(-x), 2);
    }
};

class MLP
{
  public:
    using MatrixType = Eigen::MatrixXd;
    using VectorType = Eigen::VectorXd;

  private:
    using WeightType = MatrixType;
    using BiasType = VectorType;

    std::vector<size_t> sizes;
    std::vector<WeightType> weights;
    std::vector<BiasType> biases;

    double regularization_parameter = 0;

  public:

    MLP() = default;

    MLP(std::vector<size_t> layers, double reg_term = 0) : sizes(layers),
        regularization_parameter(reg_term)
    {
        for (auto it = layers.begin(); it != (layers.end() - 1);
                it++) {
            weights.push_back(Eigen::MatrixXd::Random(*(it + 1), *it));
            biases.push_back(Eigen::VectorXd::Random(*(it + 1)));
        }
    }

    // MLP(std::initializer_list<size_t> layers, double reg_term) : sizes(layers),
    //     regularization_parameter(reg_term)
    // {
    //     for (auto it = layers.begin(); it != (layers.end() - 1);
    //             it++) {
    //         weights.push_back(Eigen::MatrixXd::Random(*(it + 1), *it));
    //         biases.push_back(Eigen::VectorXd::Random(*(it + 1)));
    //     }
    // }

    size_t num_layers() const
    {
        return sizes.size();
    }

    Eigen::VectorXd forward(Eigen::VectorXd input) const
    {
        int i = 0;

        return std::accumulate(weights.begin(), weights.end(),
                               input,
        [&i, this](WeightType a, WeightType w) {
            return (w * a + biases[i++]).unaryExpr(sigmoid());
        });
    }

    double cost_function(Eigen::VectorXd activations,
                         Eigen::VectorXd desired_output)
    {

        // double regularization_term = 0;

        // for (auto& w : weights)
        //     regularization_term += 1.0 / 2 * w.unaryExpr([](double c) {
        //     return std::pow(c, 2);
        // }).sum();

        // regularization_term *= regularization_parameter;

        double cost_term = 1.0 / 2 * (activations - desired_output).unaryExpr([](
        double c) {
            return std::pow(c, 2);
        }).sum();

        return cost_term // + regularization_term
               ;
    }

    // The cost function is the norm *squared*, and this is its gradient with respect to
    // some activations, given desired outputs for those
    Eigen::VectorXd cost_gradient_activations(
        Eigen::VectorXd activations,
        Eigen::VectorXd desired_outputs)
    {
        return activations - desired_outputs;
        // return ( desired_outputs*(activations.unaryExpr([](double c){
        //       return 1-c;
        //       })) +  activations*(desired_outputs.unaryExpr([](double c){
        //       return 1-c;
        // 	  })) ) * activations.unaryExpr([](double c)
        // 					{
        // 					  return c*(1-c);
        // 					});
    }

    /*
     * Backpropagation algorithm. Computes the gradients with respect to
     * weights and biases in each layer.
     */
    std::pair<std::vector<Eigen::MatrixXd>, std::vector<Eigen::VectorXd>>
            compute_gradients(
                std::pair<Eigen::VectorXd, Eigen::VectorXd> data)
    {
        std::vector<Eigen::VectorXd> zs, primes;
        std::vector<Eigen::MatrixXd> as;

        // Input as first activation
        as.push_back(std::get<0>(data));

        // Calculate outputs, activations and sigmoid
        // derivatives in activations for each layer
        for (size_t i = 0; i < num_layers() - 1; i++) {
            zs.push_back(weights[i]*as[i] + biases[i]);
            as.push_back(zs[i].unaryExpr(sigmoid()));
            primes.push_back(zs[i].unaryExpr(sigmoid_prime()));
        }

        // Vector of errors
        std::vector<Eigen::VectorXd> errors(num_layers() - 1);

        // Calculate error on final layer
        errors[num_layers() - 2] = cost_gradient_activations(
                                       as[num_layers() - 1],
                                       std::get<1>(data)).cwiseProduct(primes[num_layers() - 2]);

        // Backpropagate the error
        for (int i = num_layers() - 3; i >= 0; i--) {
            errors[i] = (weights[i + 1].transpose() * errors[i +
                         1]).cwiseProduct(primes[i]);
        }

        for (size_t i = 0; i < num_layers() - 1; i++) {
            // Should be aliasing-safe, as it is coeff-wise
            as[i] = errors[i] * as[i].transpose();
        }

        as.pop_back();

        return std::make_pair(as, errors);
    }

    struct mini_batch_update_data {
        std::vector<Eigen::VectorXd> biases_update;
        std::vector<Eigen::MatrixXd> weights_update;
        double momentum;

        mini_batch_update_data(std::vector<size_t> sizes,
                               double momentum) : momentum(momentum)
        {
            for (auto it = sizes.begin(); it != (sizes.end() - 1);
                    it++) {
                weights_update.push_back(Eigen::MatrixXd::Zero(*(it + 1),
                                         *it));
                biases_update.push_back(Eigen::VectorXd::Zero(*(it + 1)));
            }
        }
    };

    void update_with_mini_batch(
        std::vector<std::pair<Eigen::VectorXd, Eigen::VectorXd>>
        mini_batch, double eta, mini_batch_update_data& update_data)
    {
        for (auto m : mini_batch) {
            auto nabla = compute_gradients(m);
            auto nabla_w = nabla.first;
            auto nabla_b = nabla.second;

            for (size_t i = 0; i < num_layers() - 1; i++) {
                update_data.weights_update[i] = update_data.momentum *
                                                update_data.weights_update[i] + eta / mini_batch.size() *
                                                nabla_w[i] + regularization_parameter * (eta / mini_batch.size()) * weights[i]
                                                ;
                update_data.biases_update[i] = update_data.momentum *
                                               update_data.biases_update[i] + eta / mini_batch.size() *
                                               nabla_b[i];

                weights[i] -= update_data.weights_update[i];
                biases[i] -= update_data.biases_update[i];
            }
        }
    }

    void train(
        std::vector<std::pair<Eigen::VectorXd, Eigen::VectorXd>>
        data, size_t epochs,
        size_t mini_batch_size, double eta, double momentum = 0)
    {
        std::random_device rd;
        std::mt19937 g(rd());

        for (size_t i = 0; i < epochs; i++) {
            mini_batch_update_data update_data(sizes, momentum);

            for (size_t j = 0; j < data.size() / mini_batch_size; j++) {
                decltype(data) mini_batch;
                std::sample(data.begin(), data.end(),
                            std::back_inserter(mini_batch),
                            mini_batch_size, g);

                update_with_mini_batch(mini_batch, eta, update_data);
            }
        }
    }

    friend std::ostream& operator<<(std::ostream& os,
                                    const MLP& mlp);
    friend std::istream& operator>>(std::istream& os, MLP& mlp);
};

std::ostream& operator<<(std::ostream& os, const MLP& mlp);

Eigen::MatrixXd read_matrix_from_stream(std::istream& is,
                                        int rows,
                                        int columns);

Eigen::VectorXd read_vector_from_stream(std::istream& is,
                                        int size);

std::istream& operator>>(std::istream& is, MLP& mlp);

#endif // NN_HPP
