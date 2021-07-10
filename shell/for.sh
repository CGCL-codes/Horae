#!/bin/bash
# s=0
# usage: ./for.sh horae-1-addr-bmap <dataset>
if [ ! -d "cmd-output" ]; then
    mkdir "cmd-output"
fi
if [ ! -d "cmd-output/$2" ]; then
    mkdir "cmd-output/$2"
fi
if [ ! -d "cmd-output/$2/$1" ]; then
    mkdir "cmd-output/$2/$1"
fi
if [ ! -d "cmd-output/$2/$1/node-in-10w" ]; then
    mkdir "cmd-output/$2/$1/node-in-10w"
fi
for ((i=1;i<=10;i=i+1))
do
    # s=$(($s+$i))
    # ./a.out -dataset$i
    # ./horae -dataset 3 -pgss -para -w 5656 -d 5658 -gl 86400 -nodefrequence -in -qtimes 1 -fplength 14 -input_dir ../node-in/file-gen$i  2>&1 | tee cmd_output/stackoverflow/pgss_node_in_14bit_result_gen$i.txt
	./$1 -dataset 3 -pgss -para -w 5656 -d 5658 -gl 86400 -nodefrequence -in -qtimes 1 -fplength 14 -input_dir ../node-in/file-gen$i  2>&1 | tee cmd-output/$2/$1/node-in-10w/pgss_node_in_14bit_result_gen$i.txt
done

# ./for.sh horae-2x8-kick-bmap stackoverflow && ./for.sh horae-2x8-no-kick-bmap stackoverflow && ./for.sh horae-4x4-kick-bmap stackoverflow && ./for.sh horae-4x4-no-kick-bmap stackoverflow
# ./for.sh horae-2x8-kick-nmbp stackoverflow && ./for.sh horae-2x8-no-kick-nmbp stackoverflow && ./for.sh horae-4x4-kick-nmbp stackoverflow && ./for.sh horae-4x4-no-kick-nmbp stackoverflow
# ./for.sh horae-2x8-kick-nmnp stackoverflow && ./for.sh horae-2x8-no-kick-nmnp stackoverflow && ./for.sh horae-4x4-kick-nmnp stackoverflow && ./for.sh horae-4x4-no-kick-nmnp stackoverflow
