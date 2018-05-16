#!/bin/bash

for f in $(ls $1)
do
    num_lines=`wc -l $1/$f | cut -f 1 -d ' '`
    if [ "$num_lines" -gt $2 ]; then
      echo $1/$f  
    fi
done
