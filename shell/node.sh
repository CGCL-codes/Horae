#!/bin/bash

# usage: ./node.sh horae-1-addr-bmap <dataset> in/out\

if [ ! -d "cmd-output" ]; then
    mkdir "cmd-output"
fi
if [ ! -d "cmd-output/$2" ]; then
    mkdir "cmd-output/$2"
fi
if [ ! -d "cmd-output/$2/$1" ]; then
    mkdir "cmd-output/$2/$1"
fi
./$1 -dataset 3 -pgss -para -w 5656 -d 5658 -gl 86400 -nodefrequence -$3 -qtimes 1 -fplength 14 -write 2>&1 | tee cmd-output/$2/$1/pgss_node_$3_14bit_result.txt

# ./node.sh horae-2x8-kick-bmap stackoverflow out && ./node.sh horae-2x8-no-kick-bmap stackoverflow out && ./node.sh horae-4x4-kick-bmap stackoverflow out && ./node.sh horae-4x4-no-kick-bmap stackoverflow out && ./node.sh horae-2x8-kick-nmbp stackoverflow out && ./node.sh horae-2x8-no-kick-nmbp stackoverflow out && ./node.sh horae-4x4-kick-nmbp stackoverflow out && ./node.sh horae-4x4-no-kick-nmbp stackoverflow out && ./node.sh horae-2x8-kick-nmnp stackoverflow out && ./node.sh horae-2x8-no-kick-nmnp stackoverflow out && ./node.sh horae-4x4-kick-nmnp stackoverflow out && ./node.sh horae-4x4-no-kick-nmnp stackoverflow out
# ./node.sh horae-2x8-kick-bmap stackoverflow in && ./node.sh horae-2x8-no-kick-bmap stackoverflow in && ./node.sh horae-4x4-kick-bmap stackoverflow in && ./node.sh horae-4x4-no-kick-bmap stackoverflow in && ./node.sh horae-2x8-kick-nmbp stackoverflow in && ./node.sh horae-2x8-no-kick-nmbp stackoverflow in && ./node.sh horae-4x4-kick-nmbp stackoverflow in && ./node.sh horae-4x4-no-kick-nmbp stackoverflow in && ./node.sh horae-2x8-kick-nmnp stackoverflow in && ./node.sh horae-2x8-no-kick-nmnp stackoverflow in && ./node.sh horae-4x4-kick-nmnp stackoverflow in && ./node.sh horae-4x4-no-kick-nmnp stackoverflow in