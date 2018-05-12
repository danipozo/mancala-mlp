#!/bin/bash

cleanup ()
{
kill -s SIGTERM $!
exit 0
}

trap cleanup SIGINT SIGTERM

while [ 1 ]
do
    echo "Generating experience"

    time for((i=0; i<5; i++))
    do
      # Generate game experience
      ./self_play_parallel data/test_file nn_file 1

      # Insert into database
      python utils/insert_cumulative_reward.py state_reward_cumulative.sqlite data     
    done

    echo "Training"
    time utils/train_on_database state_reward_cumulative.sqlite nn_file
    
    wait $!
done
