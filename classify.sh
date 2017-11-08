#!/bin/bash

python convert.py $1 $2 $3 $4
POS_COUNT=$(cat train_pos.txt | grep -c "#")
NEG_COUNT=$(cat train_neg.txt | grep -c "#")
POS_MULTIPLY=$((3 * $POS_COUNT / 9))
NEG_MULTIPLY=$((1 * $NEG_COUNT / 10))
./gaston $POS_MULTIPLY train_pos.txt gaston_pos.txt
./gaston $NEG_MULTIPLY train_neg.txt gaston_neg.txt
./sgi