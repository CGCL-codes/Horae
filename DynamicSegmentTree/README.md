# This folder contains the source code files of the dynamic segment tree and the test data files

## How to use?
### Environment
We implement Horae in a Red Hat Enterprise Linux Server release 6.2 with an Intel 2.60GHz CPU and 64GB RAM, the size of one cache line in the server is 64 bytes. 

The g++ version we use is 7.3.0.

Build & run

```txt
make
```
After executing the make command, we get the binary executable program "segtree".

### Configurations
Some important parameters setting and theirs descriptions are as follows.
| Command-line parameters | Descriptions                                       |
|:----------------------- | :------------------------------------------------- |
| **-w**                  | the width of the hash matrix                       |
| **-d**                  | the depth of the hash matrix                       |
| **-gl**                 | granularity length                                 |
| **-write**              | save test results to specified file                |
| **-fplength**           | fingerprint length                                 | 
| **-edgeweight**         | run edge weight query                              |
| **-edgeexistence**      | run edge existence query                           |
| **-nodeinweight**       | run node-in aggregated weight query                |
| **-nodeoutfrequence**   | run node-out aggregated weight query               |
| **-bool**               | run bool query                                     |
| **-filename**           | the file path of dataset                           |
| **-input_dir**          | the folder path of input files                     |
| **-output_dir**         | the folder path of output files                    |
| **-para_query**         | execute query tasks in parallel                    |
| **-seq_query**          | execute query tasks serially                       |
| **-row_addrs**          | number of alternative addresses for matrix rows    |
| **-col_addrs**          | number of alternative addresses for matrix columns |
| **-kick**               | add kick out stategy                               |
| **-cache_align**        | add cache align stategy                            |


We give a simple example of how to use these parameters:
``` code
e.g. ./segtree -filename <path> -w <int> -d <int> -gl <int> -fplength <int> -input_dir <path> -output_dir <path>
e.g. ./segtree -dataset 7 -filename ../Dataset/wiki-talk -w 128 -d 128 -gl 86400 -fplength 14 -bp 100 -query -write -input_dir TestFiles/wiki/input/ -output_dir TestFiles/wiki/output/ -edgeweight -write 
```