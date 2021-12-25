#ifndef _PARAMS_H
#define _PARAMS_H
#include <iostream>
#include <stdlib.h>

// the parameters of linear congruential method
#define increment 739
#define modulus 1048576
#define multiplier 5

#define SLOTNUM 2 	// This is the parameter to control the maximum number of rooms in a bucket.

typedef uint32_t weight_type;
typedef int64_t time_type;

#define CACHESLOT 4 	// one 64 Bytes cacheline contains 4 slotrooms

#ifdef H1
#define HASH 1
#elif H2
#define HASH 2
#elif H3
#define HASH 3
#elif H4
#define HASH 4
#elif H5
#define HASH 5
#elif H6
#define HASH 6
#else
#define HASH 0
#endif


#endif //_PARAMS_H
