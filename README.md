# Horae: A Graph Stream Summarization Structure for Efficient Temporal Range Query
Horae is a graph stream summarization structure for efficient temporal range queries. Horae can deal with temporal queries with arbitrary and elastic range while guaranteeing one-sided and controllable errors. More to the point, Horae provides a worst query time of *O*(*log |L|*), where *|L|* is the length of query range. Hoare leverages multi-layer storage and *Binary Range Decomposition* (*BRD*) algorithm to decompose the temporal range query to logarithmic time interval queries and executes these queries in corresponding layers.

# Introduction

The emerging graph stream represents an evolving graph formed as a timing sequence of elements (updated edges) through a continuous stream. Each element in a graph stream is formally denoted as (*s<sub>i</sub>*, *d<sub>i</sub>*,*w<sub>i</sub>*, *t<sub>i</sub>*) (*i* ≥ 0), meaning the directed edge of a graph *G* = (*V*, *E*), *i.e.*, *s<sub>i</sub>* → *d<sub>i</sub>* (*s<sub>i</sub>* ∈ *V*, *d<sub>i</sub>* ∈ *V*, *s<sub>i</sub>* → *d<sub>i</sub>* ∈ *E*), is produced at time *t<sub>i</sub>* with a weight value *w<sub>i</sub>*. An edge can appear multiple times at different time instants with different weights. Such a general data form is widely used in big data applications, such as user behavior analysis in social networks, close contact tracking in epidemic prevention, and vehicle surveillance in smart cities.

Real-world big data applications can create tremendously large-scale graph stream data. The enormous data scale makes the management of graph streams extremely challenging, especially in the aspects of (1) storing the continuously produced and large-scale datasets, and (2) supporting queries relevant to both graph topology and temporal information. To address these issues, recent research has mainly focused on graph stream summarization techniques which aim at achieving practicable storage and supporting various queries relevant to graph topology at the cost of slight accuracy sacrifice. 

However, existing summarization structures are unable to store the temporal information in a graph stream and thus fail to support temporal queries. In this work, we propose Horae, a novel graph stream summarization structure to efficiently support temporal range queries. By exploring a time prefix embedded multi-layer summarization structure, Horae can effectively handle a temporal range query of an arbitrary range length *L* with a worst query processing time of *O(log L)*. The basic idea of Horae's time prefix embedded multi-layer summarization structure is as follows.

An arbitrary temporal range of length *L* can be decomposed to at most *2log L* sub-ranges, where all the time points in each sub-range have the same binary code prefix. For example, [*t*<sub>8</sub>, *t*<sub>13</sub>] = [*t*<sub>8</sub>, *t*<sub>11</sub>] + [*t*<sub>12</sub>, *t*<sub>13</sub>], where all the time points between *t*<sub>8</sub> (*i.e.*, 1000) and *t*<sub>11</sub> (*i.e.*, 1011) have the same common prefix '10', while all the time points between *t*<sub>12</sub> (*i.e.*, 1100) and *t*<sub>13</sub> (*i.e.*, 1101) have the same prefix '110'. Here, we define the *prefix size* as the number of binary digits in the common prefix (*e.g.*, the prefix size of '10' is two while that of '110' is three).

A Horae structure contains a number of *l* = ⌈ *log*<sub>2</sub>(*t<sub>u</sub>* + 1) ⌉ + 1 layers, where *t<sub>u</sub>* is the current time point of a graph stream. To cope with the infinity in the time dimension, the number of layers in Horae dynamically increases as *t<sub>u</sub>* grows. Horae arranges the layers according to different prefix sizes. Each layer leverages a matrix to store the complete graph stream data aggregated by the sub-ranges with the same prefix size. Consider the example with *t*<sub>*u*</sub> = *t*<sub>7</sub>, Horae has four layers. The first layer contains eight sub-ranges \{[*t*<sub>0</sub>] ([0000]), [*t*<sub>1</sub>] ([0001]), ..., [*t*<sub>7</sub>] ([0111])\} with prefix size of four; the second layer contains four sub-ranges \{[*t*<sub>0</sub>, *t*<sub>1</sub>] ([0000, 0001]), [*t*<sub>2</sub>, *t*<sub>3</sub>] ([0010, 0011]), ..., [*t*<sub>6</sub>, *t*<sub>7</sub>] ([0110, 0111])\} with prefix size of three, and so on. Formally, the *p<sup>th</sup>* layer aggregates the graph stream data by the sub-ranges \{[{*q* ⋅ 2<sup>*p* - 1</sup>}, {(*q* + 1) ⋅ 2<sup>*p* - 1</sup>} - 1] (*q* ≥ 0)\} with prefix size *l - p* + 1. The sub-ranges of each layer have the same prefix size, *i.e.*, the same range length. In a nutshell, each layer of the structure represents a summarization of a graph stream with carefully selected granularity (corresponds to a prefix size). During the construction of each layer, Horae combines each edge with the time prefix of the corresponding size for inserting the edge information to the corresponding matrix. Similarly, Horae combines an edge/node and the sub-range prefix to perform a sub-range query.

To efficiently evaluate temporal range queries on top of the Horae structure, we further design a novel *Binary Range Decomposition* (BRD) algorithm. The BRD algorithm decomposes a temporal range query with an arbitrary length *L* into at most *O(log L)* sub-range queries against different layers of the structure. Therefore, Horae reduces the query processing time to a logarithmic scale. Experimental results show that Horae reduces the latency of temporal range queries by two to three orders of magnitude compared to existing designs.


# Horae Structure
<img src="images/Horae-structure.png" width=1000 alt="Horae-structure"/><br/>

The Horae structure contains *l* = ⌈ *log*<sub>2</sub>(*T<sub>u</sub>* + 1) ⌉ + 1 layers. It starts with a single layer initially and creates one new layer whenever the current time slice of the graph stream increases to a larger power of two (*T<sub>u</sub>* = 2<sup>*i*</sup>, *i* ≥ 0). Horae arranges the layers based on different prefix sizes. Each layer aggregates the complete graph stream data by a corresponding prefix size. In the snapshot, the 1<sup>*st*</sup> layer contains four sub-ranges \{[*T*<sub>0</sub>] (000), [*T*<sub>1</sub>] (001), ..., [*T*<sub>3</sub>] (011)\}, all with the prefix size of three; the 2<sup>*nd*</sup> layer contains two sub-ranges \{[*T*<sub>0</sub>, *T*<sub>1</sub>] (<u>00</u>0, <u>00</u>1), [*T*<sub>2</sub>, *T*<sub>3</sub>] (<u>01</u>0, <u>01</u>1)\}, both with the prefix size of two; the 3<sup>*rd*</sup> layer contains one sub-range \{[*T*<sub>0</sub>, *T*<sub>3</sub>] (<u>0</u>00, <u>0</u>11)\} with the prefix size of one. Formally, the *p<sup>th</sup>* layer aggregates the graph stream data by the sub-ranges \{[0, 2<sup>*p* - 1</sup> - 1], [2<sup>*p* - 1</sup>, 2 ⋅ 2<sup>*p* - 1</sup> - 1], ...\} with the same prefix size of *l - p* + 1. All the sub-ranges of the *p<sup>th</sup>* layer have the same range length 2<sup>*p* - 1</sup>. We define the same range length of each sub-range in the *p<sup>th</sup>* layer as the *granularity* of the *p<sup>th</sup>* layer. In a word, the *p<sup>th</sup>* layer of Horae represents a graph stream summarization of granularity 2<sup>*p* - 1</sup>.

# Binary Range Decomposition
<img src="images/Horae-BRD-3cases.png" width=1000 alt="Horae-BRD"/><br/>
We design the BRD algorithm to quickly decompose an arbitrary time range query to multiple window queries of different layers. For any time range of length *L*, we can find an *m* which satisfies 2<sup>*m*</sup> ≤ *L* < 2<sup>*m* + 1</sup> (*i.e.*, *m* = ⌊ *log*<sub>2</sub> *L* ⌋). The granularity of the (*m* + 1)<sup>*th*</sup> layer is 2<sup>*m*</sup>. BRD is based on the observation: if the time range aligns with one side of a window in the (*m* + 1)<sup>*th*</sup> layer, decomposing the range equals decomposing its length, *i.e.*, converting *L* to itd binary form.

For convenience, we call a sub-range of each layer a *window*. We use the notation *W<sub>p</sub><sup>q</sup>* to denote the window with prefix *q* in the *p<sup>th</sup>* layer, and more generally *W<sub>p</sub><sup>q</sup>* = [*q* ⋅ 2<sup>*p* - 1</sup>, (*q* + 1) ⋅ 2<sup>*p* - 1</sup> - 1]. For example, we have *W*<sub>2</sub><sup>2</sup> = [*T*<sub>4</sub>, *T*<sub>5</sub>]. For a window [*T<sub>i</sub>*, *T<sub>j</sub>*] of length *N*, [*T<sub>i</sub>*, *T<sub>j</sub>*] = *W*<sub>(*log*<sub>2</sub>*N*+1)</sub><sup>⌊*T*<sub>*i*</sub>/*N*⌋</sup> = *W*<sub>(*log*<sub>2</sub>*N*+1)</sub><sup>⌊*T*<sub>*j*</sub>/*N*⌋</sup>. The windows of the *p<sup>th</sup>* and (*p* + 1)<sup>*th*</sup> layers have the following relationship: the two adjacent windows can be merged into a larger window of the upper layer. Formally, *W*<sub>*p*</sub><sup>2*i*</sup> + *W*<sub>*p*</sub><sup>2*i* + 1</sup> = *W*<sub>*p* + 1</sub><sup>*i*</sup> (*i* ≥ 0), where *W*<sub>*p*</sub><sup>2*i*</sup> and *W*<sub>*p*</sub><sup>2*i* + 1</sup> are called *sibling windows*.

Given an arbitrary query range [*T<sub>b</sub>*, *T<sub>e</sub>*] with the length *L* = *T<sub>e</sub>* - *T<sub>b</sub>* + 1, we formalize the decomposition result as follows: *DE*([*T<sub>b</sub>*, *T<sub>e</sub>*]) = *W*<sub>*p*<sub>1</sub></sub><sup>*q*<sub>1</sub></sup> + *W*<sub>*p*<sub>2</sub></sub><sup>*q*<sub>2</sub></sup> + ... + *W<sub>p<sub>f</sub></sub><sup>q<sub>f</sub></sup>*, where *W<sub>p<sub>i</sub></sub><sup>q<sub>i</sub></sup>* and *W<sub>p<sub>j</sub></sub><sup>q<sub>j</sub></sup>* cannot be sibling windows for any two *i*, *j* ∈ [1, *f*] and *i* ≠ *j*. Here, we identify an important property of the *window*: for a time range [*T<sub>i</sub>*, *T<sub>j</sub>*] of length *N* (*N* = 2<sup>*k*</sup> (*k* ≥ 0)), it is a *window* if and only if (*T<sub>j</sub>* + 1) % *N* = 0. 

For an arbitrary time range, it may align perfectly with a window, align with one side of a window, or not align with any window in the (*m* + 1)<sup>*th*</sup> layer. Accordingly, we summarize the following three cases and apply different strategies. The figure above shows examples of the three cases (4 ≤ *L* < 8 and *m* = 2).


## How to use?
### Environment
We implement Horae in a Red Hat Enterprise Linux Server release 6.2 with an Intel 2.60GHz CPU and 64GB RAM, the size of one cache line in the server is 64 bytes. 

The g++ version we use is 7.3.0.

Build & Run

```txt
make
./horae
```

### Configurations
Some important parameters setting and theirs descriptions are as follows.
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
| **-baseline**           | run baseline code                                  |
| **-horae**              | run horae code                                     |
| **-row_addrs**          | number of alternative addresses for matrix rows    |
| **-col_addrs**          | number of alternative addresses for matrix columns |
| **-kick**               | add kick out stategy                               |
| **-cache_align**        | add cache align stategy                            |
| **-write**              | output test results to file                        |


We give a simple example of how to use these parameters:
``` code
e.g. ./horae -dataset <int> -filename <path> -w <int> -d <int> -gl <int> -fplength <int> -horae -para_ins -slot <int> -edgeweight -write -input_dir <path> -output_dir <path>
e.g. ./horae -dataset 9 -filename Dataset/stackoverflow -horae -para_ins -w 5656 -d 5656 -gl 86400 -qtimes 1 -edgeweight -write -output_dir TestFiles/stk-test/output/ -fplength 14 -kick -cache_align 
```


## Author and Copyright

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