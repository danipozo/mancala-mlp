#include "GameState_mod.h"
#include "MancalaTrainingPlayer.hpp"

#include <fstream>
#include <thread>

int main(int argc, char** argv)
{
    const size_t num_files = 32;
    const size_t num_threads = 8;

    const double epsilon = 0.1;
    
    MancalaHeuristic h1, h2;

    if (argc >= 3) {
                std::ifstream nn_file1(argv[1]);
		std::ifstream nn_file2(argv[2]);

                h1.read_nn_for(J1, nn_file1);
                // nn_file1.clear();
                // nn_file1.seekg(0, std::ios::beg);
		// h2.read_nn_for(J1, nn_file1);

	        h1.read_nn_for(J2, nn_file2);
                // nn_file2.clear();
                // nn_file2.seekg(0, std::ios::beg);
		// h2.read_nn_for(J2, nn_file2);
    }

    auto thread_fn = [ & ](size_t id, size_t lower, size_t upper) {
        for (size_t i = lower; i < upper; i++) {

            if (i == (upper + lower) / 2) {
                std::cout << "Thread " << id << ": half work finished" << std::endl;
            }

            GameState gs;
            TrainingPlayer players[2] = {{h1, J1}, {h1, J2}};

            players[0].set_epsilon(epsilon);
	    players[1].set_epsilon(epsilon);


            while (!gs.isFinalState()) {
                gs = gs.simulateMove(
                         players[gs.getCurrentPlayer()].get_play());
                players[gs.getCurrentPlayer()].update_state(gs);
            }

            if (gs.getCurrentPlayer() != J1) {
                players[0].update_state(gs);
            }

            if (argc >= 5) {
	      auto file_name1 = std::string(argv[3]) + std::to_string(id * num_files /
								      num_threads + (i - lower));
                std::ofstream os(file_name1);
		
                players[0].output_statistics(os);

	        auto file_name2 = std::string(argv[4]) + std::to_string(id * num_files /
								      num_threads + (i - lower));
                std::ofstream os2(file_name2);
		
                players[1].output_statistics(os2);

            }

            // if (argc < 4) {
            //     std::ofstream nn_file(argv[2]);
            //     players[0].output_nn(nn_file);
            // }
        }

        std::cout << "[" << argv[0] << "]: thread " << id << " finished" << std::endl;
    };

    std::thread t[num_threads];

    for (size_t i = 0; i < num_threads; i++) {
        std::cout << "Invoking thread " << i << " with [" << i * num_files / num_threads
                  << "," << (i + 1) * num_files / num_threads << ")" << std::endl;
        t[i] = std::thread(thread_fn, i, i * num_files / num_threads,
                           (i + 1) * num_files / num_threads);
    }


    for (size_t i = 0; i < num_threads; i++) {
        t[i].join();
    }

    return 0;
}
