# Horae: A Graph Stream Summarization Structure for Efficient Temporal Range Query
Horae is a graph stream summarization structure for efficient temporal range query. Horae can deal with temporal queries with arbitrary and elastic range while guaranteeing one-sided and controllable errors. More to the point, Horae provides a worst query time of O(log{|L|}), where |L| is the length of query range. Hoare leverages multi-layer storage and Binary Range Decomposition (BRD) algorithm to decompose the time range query to logarithmic time interval queries and executes these queries in corresponding layers.

## How to use?
### Environment
We implement Horae in a Red Hat Enterprise Linux Server release 6.2 with an Intel 2.60GHz CPU and 64GB RAM, the size of one cache line in the server is 64 bytes. 

The g++ version we use is 7.3.0.

Build

```txt
make
./Horae
```

### Configurations
Some important parameters setting and theirs descriptions are as follows.
| Command-line parameters | Descriptions                         |
|:----------------------- | :----------------------------------- |
| **-w**                  | the width of the hash matrix         |
| **-d**                  | the depth of the hash matrix         |
| **-gl**                 | granularity length                   |
| **-slot**               | slot numbers of one bucket           |
| **-fplength**           | fingerprint length                   | 
| **-edgeweight**         | run edge weight query                |
| **-edgeexistence**      | run edge existence query             |
| **-nodeinweight**       | run node-in aggregated weight query  |
| **-nodeoutfrequence**   | run node-out aggregated weight query |
| **-bool**               | run bool query                       |
| **-filename**           | the file path of dataset             |
| **-input_dir**          | the folder path of input files       |
| **-output_dir**         | the folder path of output files      |

We give a simple example of how to use these parameters:
``` code
e.g. ./Horae -filename <path> -w <int> -d <int> -gl <int> -fplength <int> -slot <int> -edgefrequence -input_dir <path> -output_dir <path>
```


## Author and Copyright

Horae is developed in National Engineering Research Center for Big Data Technology and System, Cluster and Grid Computing Lab, Services Computing Technology and System Lab, School of Computer Science and Technology, Huazhong University of Science and Technology, Wuhan, China by Ming Chen (mingc@hust.edu.cn), Renxiang Zhou (mr\_zhou@hust.edu.cn), Hanhua Chen (chen@hust.edu.cn), Jiang Xiao (jiangxiao@hust.edu.cn), Hai Jin (hjin@hust.edu.cn).

Copyright (C) 2020, [STCS & CGCL](http://grid.hust.edu.cn/) and [Huazhong University of Science and Technology](http://www.hust.edu.cn).

