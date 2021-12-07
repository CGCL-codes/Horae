Since a single github file cannot exceed the limit of 200MB, we give the download address of the data sets here.

+ lkml: http://konect.cc/files/download.tsv.lkml-reply.tar.bz2
+ wiki-talk: http://konect.cc/files/download.tsv.wiki_talk_en.tar.bz2
+ stackoverflow: http://konect.cc/files/download.tsv.sx-stackoverflow.tar.bz2
+ caida: https://www.caida.org/catalog/datasets/passive_dataset/

In addition, due to the restrictions of caida, you need to apply for the permission to use the caida data set by yourself

The file balanced.cpp is used for making the dataset uniform in each timeslice, here we give some instructions on how to use it.
``` code
build: g++ -o balanced balanced.cpp
run: ./balanced -dataset <dataset name> -num <int, the number of elements of each time slice> -output <the output dataset which is uniform>
e.g.: ./balanced -dataset wiki-talk -num 4796 -output wiki-talk-balanced
```