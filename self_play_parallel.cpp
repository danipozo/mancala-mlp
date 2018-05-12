#include "GameState_mod.h"
#include "MancalaMCTSPlayer.hpp"

#include <fstream>
#include <thread>

int main(int argc, char** argv)
{
    const size_t num_files = 100;
    const size_t num_threads = 8;

    
    auto thread_fn = [ & ](size_t id, size_t lower, size_t upper) {
        for (int i = lower; i < upper; i++) {
	  GameState gs;

	  MCTSPlayer players[2] = {J1, J2};
            if (argc >= 4) {
	      std::ifstream nn_file(argv[2]);

                players[0].read_nn(nn_file);
                nn_file.clear();
                nn_file.seekg(0, std::ios::beg);
                players[1].read_nn(nn_file);
            }

            while (!gs.isFinalState()) {
                gs = gs.simulateMove(
                         players[gs.getCurrentPlayer()].get_play());
                players[gs.getCurrentPlayer()].update_state(gs);
            }

            if (gs.getCurrentPlayer() != J1) {
                players[0].update_state(gs);
            }

            if (argc >= 2) {
	      std::ofstream os(std::string(argv[1])+std::to_string(id*num_files/num_threads+i));
                players[0].output_statistics(os);
            }

            if (argc < 4) {
                std::ofstream nn_file(argv[2]);
                players[0].output_nn(nn_file);
            }
        }
    };

    std::thread t[num_threads];

    for(size_t i=0; i<num_threads; i++)
      t[i] = std::thread(thread_fn, i, i*num_files/num_threads, (i+1)*num_files/num_threads);


    for(size_t i=0; i<num_threads; i++)
      t[i].join();
      
    return 0;
}
