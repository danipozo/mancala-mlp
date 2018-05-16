#!/bin/bash

cleanup ()
{
kill -s SIGTERM $!
exit 0
}

trap cleanup SIGINT SIGTERM

epochs=0
while [ 1 ]
do
    echo Epoch $epochs
    echo "Generating experience"

    time for((i=0; i<1; i++))
    do
      # Generate game experience
	./self_play_parallel nn_player0 nn_player1 data/player0/match data/player1/match

	# won=0
	# for i in $(ls data)
	# do
	#     utils/won.sh data/$i
	#     if [ $? -eq 0 ]; then
	# 	won=`expr $won + 1`
	#     fi
	# done

	# if [ $won -ge 20 ] && [ $epochs -ge 10 ]; then
	#     cp nn_file nn_snapshots/automatic-`date "+%d-%m-%H:%M:%S"`
	# fi

      # Insert into database
	python utils/insert_cumulative_reward.py state_reward_cumulative0.sqlite data/player0
	      python utils/insert_cumulative_reward.py state_reward_cumulative1.sqlite data/player1    
    done

    echo "Training"
    time utils/train_on_database state_reward_cumulative0.sqlite nn_player0
    time utils/train_on_database state_reward_cumulative1.sqlite nn_player1

    epochs=`expr $epochs + 1`
    
    wait $!
done
