#include <sqlite3.h>
#include "nn.hpp"
#include <vector>

#include <iostream>
#include <sstream>
#include <fstream>

int main(int argc, char** argv)
{

    if (argc < 3) {
        std::cerr << "Pass database file and nn file as arguments" << std::endl;
        exit(EXIT_FAILURE);
    }

    /*
     * EXTRACT DATA FROM DATABASE
     */
    sqlite3* db_handle;

    if (sqlite3_open(argv[1], &db_handle) != SQLITE_OK) {
        std::cerr << "Error opening database" << std::endl;
        exit(EXIT_FAILURE);
    }

    sqlite3_stmt* statement;

    const size_t stmt_size = 999;
    char statement_string[stmt_size] =
        "SELECT * FROM state_reward WHERE occurrences > 5";

    int prepare_code = sqlite3_prepare_v2(db_handle, statement_string,
                                          stmt_size,
                                          &statement, NULL);

    if (prepare_code != SQLITE_OK) {
        std::cerr << "Error preparing statement" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<std::pair<MLP::VectorType, MLP::VectorType>> data;

    bool done = false;
    const char* text_state, *text_reward;
    int bytes_state, bytes_reward;

    constexpr size_t data_points_limit = 15000;
    size_t data_points_read = 0;

    while (!done) {
        if (data_points_read >= data_points_limit) {
            break;
        }

        switch (sqlite3_step(statement)) {
            case SQLITE_ROW: {
                bytes_state = sqlite3_column_bytes(statement, 0);
                bytes_reward = sqlite3_column_bytes(statement, 2);
                text_state  = (const char*) sqlite3_column_text(statement, 0);
                text_reward = (const char*) sqlite3_column_text(statement, 2);

                std::string state_str(text_state, bytes_state);
                std::string reward_str(text_reward, bytes_reward);

                std::istringstream ss(state_str);

                MLP::VectorType data_point(14, 1);
                MLP::VectorType data_label(1, 1);

                for (int i = 0; i < 14; i++) {
                    ss >> data_point(i);
                }


                // Normalize labels
                data_label(0) = (std::stod(reward_str) + 1) / 2;

                data.push_back({data_point, data_label});



                data_points_read++;
                break;
            }

            case SQLITE_DONE:
                done = true;
                break;

            default:
                fprintf(stderr, "Failed.\n");
                exit(EXIT_FAILURE);
        }
    }

    sqlite3_finalize(statement);

    while (sqlite3_close(db_handle) != SQLITE_OK);

    std::cout << "Data points collected: " << data_points_read << std::endl;

    /*
     * USE DATA TO TRAIN PREDICTOR
     */
    size_t epochs = 700;
    size_t mini_batch_size = 50;
    double eta = 0.05;
    double momentum = 0.9;

    double regularization_parameter = 0.0000001;

    MLP predictor({1, 2, 1}, regularization_parameter);

    std::ifstream nn_file(argv[2]);
    nn_file >> predictor;
    nn_file.close();

    predictor.train(data, epochs, mini_batch_size, eta, momentum);

    size_t correct = 0;
    double correct_epsilon = 0.05;

    size_t i = 0;

    for (auto [x, y] : data) {
        if (std::abs(y(0) - predictor.forward(x)(0)) < correct_epsilon) {
            correct++;
        }

        if ((i % 20) == 0) {
            std::cout << "Data: " << y(0) << " prediction: " << predictor.forward(x)(
                          0) <<  " (" << std::abs(y(0) - predictor.forward(x)(0)) << ")" <<  std::endl;
        }

        i++;
    }

    std::cout << "Correct percentage: " << ((double)correct) / data.size() <<
              std::endl;

    std::ofstream nn_file_out(argv[2]);
    nn_file_out << predictor;

    return 0;
}
