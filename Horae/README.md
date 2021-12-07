# This folder contains the source code files of the Horae

## How to use?
### Environment
We implement Horae in a Red Hat Enterprise Linux Server release 6.2 with an Intel 2.60GHz CPU and 64GB RAM, the size of one cache line in the server is 64 bytes. 

The g++ version we use is 7.3.0.

Build & Run

```txt
make
```
After executing the make command, we get the binary executable programs "horae" and "horae-compacted".

### Configurations
Some important parameter settings and their descriptions are as follows.
| Command-line parameters | Descriptions                                       |
|:----------------------- | :------------------------------------------------- |
| **-w**                  | the width of the hash matrix                       |
| **-d**                  | the depth of the hash matrix                       |
| **-gl**                 | granularity length                                 |
| **-slot**               | slot numbers of one bucket                         |
| **-fplength**           | fingerprint length                                 |
| **-edgeweight**         | run edge weight query                              |
| **-edgeexistence**      | run edge existence query                           |
| **-nodeinweight**       | run node-in aggregated weight query                |
| **-nodeoutweight**      | run node-out aggregated weight query               |
| **-bool**               | run bool query                                     |
| **-dataset**            | choose dataset for testing                         |
| **-filename**           | the file path of dataset                           |
| **-input_dir**          | the folder path of input files                     |
| **-output_dir**         | the folder path of output files                    |
| **-para_ins**           | parallel insertion                                 |
| **-seq_ins**            | serial insertion                                   |
| **-para_query**         | execute query tasks in parallel                    |
| **-seq_query**          | execute query tasks serially                       |
| **-row_addrs**          | number of alternative addresses for matrix rows    |
| **-col_addrs**          | number of alternative addresses for matrix columns |
| **-kick**               | add kick out stategy                               |
| **-cache_align**        | add cache align stategy                            |
| **-write**              | output test results to file                        |


We give a simple example of how to run the horae with these parameters:
``` code
e.g. ./horae -dataset <int> -filename <path> -w <int> -d <int> -gl <int> -fplength <int> -para_ins -slot <int> -edgeweight -write -input_dir <path> -output_dir <path>
e.g. ./horae -dataset 3 -filename Dataset/stackoverflow -para_ins -w 5656 -d 5656 -gl 86400 -qtimes 1 -edgeweight -write -output_dir TestFiles/stk-test/output/ -fplength 14 -kick -cache_align -write
e.g. ./horae-compacted -dataset 3 -filename Dataset/stackoverflow -para_ins -w 5656 -d 5656 -gl 86400 -qtimes 1 -edgeweight -write -output_dir TestFiles/stk-test/output/ -fplength 14 -kick -cache_align -write
```


## Authors and Copyright

Horae is developed in National Engineering Research Center for Big Data Technology and System, Cluster and Grid Computing Lab, Services Computing Technology and System Lab, School of Computer Science and Technology, Huazhong University of Science and Technology, Wuhan, China by Ming Chen (mingc@hust.edu.cn), Renxiang Zhou (mr\_zhou@hust.edu.cn), Hanhua Chen (chen@hust.edu.cn), Jiang Xiao (jiangxiao@hust.edu.cn), Hai Jin (hjin@hust.edu.cn).

Copyright (C) 2020, [STCS & CGCL](http://grid.hust.edu.cn/) and [Huazhong University of Science and Technology](http://www.hust.edu.cn).

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.