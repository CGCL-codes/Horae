#!/bin/bash

# usage: ./edge.sh horae-1-addr-bmap <dataset>\

if [ ! -d "cmd-output" ]; then
    mkdir "cmd-output"
fi
if [ ! -d "cmd-output/$2" ]; then
    mkdir "cmd-output/$2"
fi
if [ ! -d "cmd-output/$2/$1" ]; then
    mkdir "cmd-output/$2/$1"
fi
./$1 -dataset 3 -pgss -para -w 5656 -d 5658 -gl 86400 -edgefrequence -qtimes 1 -fplength 6 -write 2>&1 | tee cmd-output/$2/$1/pgss_edgequery_6bit_result.txt

# ./edge.sh horae-2x8-kick-bmap stackoverflow && ./edge.sh horae-2x8-no-kick-bmap stackoverflow && ./edge.sh horae-4x4-kick-bmap stackoverflow && ./edge.sh horae-4x4-no-kick-bmap stackoverflow && ./edge.sh horae-2x8-kick-nmbp stackoverflow && ./edge.sh horae-2x8-no-kick-nmbp stackoverflow && ./edge.sh horae-4x4-kick-nmbp stackoverflow && ./edge.sh horae-4x4-no-kick-nmbp stackoverflow && ./edge.sh horae-2x8-kick-nmnp stackoverflow && ./edge.sh horae-2x8-no-kick-nmnp stackoverflow && ./edge.sh horae-4x4-kick-nmnp stackoverflow && ./edge.sh horae-4x4-no-kick-nmnp stackoverflow