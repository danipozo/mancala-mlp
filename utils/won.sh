#!/bin/bash

reward=`tail -1 $1 | cut -f 2 -d ','`


if [ `echo $reward'>'0 | bc -l` -ne 0  ]; then
  exit 0
else
  exit 1
fi
