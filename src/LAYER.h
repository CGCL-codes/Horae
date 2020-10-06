#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <cmath>
#include <stdlib.h>
#include <bitset>
#include <memory.h>
#include <algorithm>
#include "HashFunction.h"
using namespace std;
// the parameters of linear congruential method
#define increment 739
#define modulus 1048576
#define multiplier 5

#define SLOTNUM 2 	// This is the parameter to control the maximum number of rooms in a bucket.

typedef unsigned int weight_type;

struct basket {
	unsigned short src[SLOTNUM];
	unsigned short dst[SLOTNUM]; 
	weight_type weight[SLOTNUM];
};

struct node {
	unsigned int key;
	weight_type weight;
};



class findx {
public:
	findx(const unsigned int va) { value = va; }
	//unsigned int GetValue() { return value; }
	bool operator()(const vector<node>::value_type &nod) {
		if (nod.key == value)
			return true;
		else
			return false;
	}
private:
	unsigned int value;
};

class Layer {
private:
	const unsigned int granularity;
	const unsigned int width;
	const unsigned int depth;
	const int fingerprintLength;
	basket* value;
	vector<node> precursorAdjacencyList;
	vector<vector<node>> successorAdjacencyList;

	unsigned int n1, n2;	//n1 for successor, n2 for precursor
	map<unsigned int, unsigned int> precursorIndex;
	map<unsigned int, unsigned int> successorIndex;
	int getMinIndex(unsigned int* a, int length);

public:
	Layer(unsigned int granularity, unsigned int width, unsigned int depth, int fingerprintLength);
	Layer(const Layer &layer);
	~Layer();
	void insert(string src, string dst, weight_type weight);
	weight_type edgeQuery(string src, string dst);
	weight_type nodeQuery(string vertex, int type);		//src_type = 0 dst_type = 1
	unsigned int getGranularity() const;
};
unsigned int Layer::getGranularity() const{
	if(this == NULL) {
		cout << "NULL pointer!!" << endl;
		getchar();
		exit(-1);
	}
	return this->granularity;
}
Layer::Layer(unsigned int granularity, unsigned int width, unsigned int depth, int fingerprintLength)
: granularity(granularity), width(width), depth(depth), fingerprintLength(fingerprintLength) {
	this->n1 = 0;									// precursor index
	this->n2 = 0;									// successor index
	unsigned int msize = width * depth;

	posix_memalign((void**)&value, 64, sizeof(basket) * msize); 	// 64-byte alignment of the requested space
	memset(this->value, 0, sizeof(basket) * msize);
}
Layer::Layer(const Layer &layer)
: granularity(layer.getGranularity() * 2), width(layer.width), depth(layer.depth), fingerprintLength(layer.fingerprintLength) {
	if(this  == NULL) cout << "FUCCCCCCK" << endl;
	this->n1 = layer.n1;
	this->n2 = layer.n2;
	this->precursorIndex = layer.precursorIndex;
	this->successorIndex = layer.successorIndex;
	this->precursorAdjacencyList.resize(layer.precursorAdjacencyList.size());
	this->successorAdjacencyList.resize(layer.successorAdjacencyList.size());
	
	this->precursorAdjacencyList.assign(layer.precursorAdjacencyList.begin(), layer.precursorAdjacencyList.end());
	for (int i = 0; i < layer.successorAdjacencyList.size(); i++) {
		this->successorAdjacencyList[i].assign(layer.successorAdjacencyList[i].begin(), layer.successorAdjacencyList[i].end());
	}
	unsigned int msize = width * depth;
	posix_memalign((void**)&value, 64, sizeof(basket) * msize);		//// 64-byte alignment of the requested space
	for(unsigned int i = 0; i < msize; i++) {
		for(int j = 0; j < SLOTNUM; j++) {
			this->value[i].src[j] = layer.value[i].src[j];
			this->value[i].dst[j] = layer.value[i].dst[j];
			this->value[i].weight[j] = layer.value[i].weight[j];
		}
	}
}
Layer::~Layer() {
	delete[] value;
	vector<node>().swap(precursorAdjacencyList);
	vector<vector<node>>().swap(successorAdjacencyList);
}
int Layer::getMinIndex(unsigned int* a, int length){
	int min = a[0];
	int index = 0;
	for(int i = 1; i < length; i++){
		if(a[i] < min){
			min = a[i];
			index = i;
		}
	}
	return index;
}
// src is the ID of the source node, dst is the ID of the destination node, weight is the weight of the edge.
void Layer::insert(string src, string dst, weight_type weight) {
	unsigned int hash_src = (*hfunc[0])((unsigned char*)(src.c_str()), src.length());
	unsigned int hash_dst = (*hfunc[0])((unsigned char*)(dst.c_str()), dst.length());
	//unsigned int mask = pow(2, fingerprintLength) - 1;
	unsigned int mask = (1 << fingerprintLength) - 1;
	unsigned int head = 16384; //pow(2, 14);
	unsigned short fp_src = hash_src & mask;
	if (fp_src == 0) fp_src += 1;
	unsigned int addr_src = (hash_src >> fingerprintLength) % depth;
	unsigned short fp_dst = hash_dst & mask;
	if (fp_dst == 0) fp_dst += 1;
	unsigned int addr_dst = (hash_dst >> fingerprintLength) % width;

	unsigned int k1 = (addr_src << fingerprintLength) + fp_src;
	unsigned int k2 = (addr_dst << fingerprintLength) + fp_dst;

	// Alternative address -- 4 * 2 * 2cache
	int seed1[4] = { 0 };		// row
	int seed2[2] = { 0 };		//column
	seed1[0] = fp_src;
	seed1[1] = (seed1[0] * multiplier + increment) % modulus;
	seed1[2] = (seed1[1] * multiplier + increment) % modulus;
	seed1[3] = (seed1[2] * multiplier + increment) % modulus;
	seed2[0] = fp_dst;
	seed2[1] = (seed2[0] * multiplier + increment) % modulus;

	bool inserted = false;	
	for (int i = 0; i < 4; i++) {
		int row_addr = (addr_src + seed1[i]) % depth;
		for (int j = 0; j < 2; j++) {
			int column_addr = (addr_dst + seed2[j]) % width;
			int column_addr_alt;
			for (int k = 0; k < 2; k++) {
				int pos;
				if (k == 0) {
					pos = row_addr * width + column_addr;
				}
				else {
					column_addr_alt = (column_addr ^ (fp_dst % 4)) % width;
					pos = row_addr * width + column_addr_alt;
				}
				for (int m = 0; m < SLOTNUM; m++) {
					if (((value[pos].src[m] >> 14) == i) && ((value[pos].dst[m] >> 14) == (j * 2 + k)) && ((value[pos].src[m] & mask) == fp_src) && ((value[pos].dst[m] & mask) == fp_dst)) {
						value[pos].weight[m] += weight;
						inserted = true;
						return;
					}
					if (value[pos].src[m] == 0 && value[pos].weight[m] == 0) {
						value[pos].src[m] = fp_src + head * i;
						value[pos].dst[m] = fp_dst + head * (j * 2 + k);
						value[pos].weight[m] = weight;
						inserted = true;
						return;
					}
				}
			}
		}
	}
	if (!inserted) {
		map<unsigned int, unsigned int>::iterator it = successorIndex.find(k1);
		if (it != successorIndex.end()) {			// find k1->k2 in successorAdjacencyList
			unsigned int tag = it->second;
			vector<node>::iterator iter = find_if(successorAdjacencyList[tag].begin(), successorAdjacencyList[tag].end(), findx(k2));
			if (iter != successorAdjacencyList[tag].end()) {	// if found
				iter->weight += weight;
				map<unsigned int, unsigned int>::iterator it2 = precursorIndex.find(k2);		// insert k2->k1
				if (it2 != precursorIndex.end()) {
					precursorAdjacencyList[it2->second].weight += weight;
					inserted = true;
					return;
				}
			}
		}
	}
	if (!inserted) {			// There is no k1->k2 in buffer, start kicking
		int kickNum = 0;
		//temp is the edge currently to be inserted
		unsigned int temp_addr1 = addr_src;
		unsigned int temp_addr2 = addr_dst;
		unsigned short temp_fp1 = fp_src;
		unsigned short temp_fp2 = fp_dst;
		weight_type temp_weight = weight;
		
		while (!inserted){
			// kickout the elements of first pos(the power of two choices)
			unsigned int pos_x = (temp_addr1 + temp_fp1) % depth;
			unsigned int pos_y = (temp_addr2 + temp_fp2) % width;
			unsigned int pos = pos_x * width + pos_y;
			unsigned int insertnum[SLOTNUM];
			for (int i = 0; i < SLOTNUM; i++){
				insertnum[i] = (value[pos].src[i] >> 14) * 4 + (value[pos].dst[i] >> 14);
			}
			int kickindex = getMinIndex(insertnum, SLOTNUM);
			unsigned int kick_i = (value[pos].src[kickindex] >> 14);
			unsigned int kick_j = (value[pos].dst[kickindex] >> 14);
			unsigned short kick_fpx = (value[pos].src[kickindex] & mask);
			unsigned short kick_fpy = (value[pos].dst[kickindex] & mask);
			weight_type kick_weight = value[pos].weight[kickindex];
			// insert the element
			value[pos].src[kickindex] = temp_fp1;
			value[pos].dst[kickindex] = temp_fp2;
			value[pos].weight[kickindex] = temp_weight;
			temp_fp1 = kick_fpx;
			temp_fp2 = kick_fpy;
			temp_weight = kick_weight;
			
			unsigned int shifterx = temp_fp1;
			for (int v = 0; v < kick_i; v++) {
				shifterx = (shifterx * multiplier + increment) % modulus;
			}
			int tmp_h1 = pos_x;
			while (tmp_h1 < shifterx)
				tmp_h1 += depth;
			tmp_h1 -= shifterx;
			temp_addr1 = tmp_h1;

			unsigned int shiftery = temp_fp2;
			for (int v = 0; v < (kick_j / 2); v++)
				shiftery = (shiftery * multiplier + increment) % modulus;
			int tmp_h2 = (kick_j % 2 == 0) ? pos_y : (pos_y ^ (temp_fp2 % 4) % width);
			while (tmp_h2 < shiftery)
				tmp_h2 += width;
			tmp_h2 -= shiftery;
			temp_addr2 = tmp_h2;

			int curtmp1[4] = { 0 };
			int curtmp2[2] = { 0 }; 
			curtmp1[0] = temp_fp1;
			curtmp1[1] = (curtmp1[0] * multiplier + increment) % modulus;
			curtmp1[2] = (curtmp1[1] * multiplier + increment) % modulus;
			curtmp1[3] = (curtmp1[2] * multiplier + increment) % modulus;
			curtmp2[0] = temp_fp2;
			curtmp2[1] = (curtmp2[0] * multiplier + increment) % modulus;
			
			unsigned int addr_no = (kick_i * 4) + (kick_j);

			for (int i = 0; i < 4; i++) {
				int row_addr = (temp_addr1 + curtmp1[i]) % depth;
				for (int j = 0; j < 2; j++) {
					int column_addr = (temp_addr2 + curtmp2[j]) % width;
					for (int k = 0; k < 2; k++) {
						if((i * 4 + j * 2 + k) <= addr_no)
							continue;
						int pos;
						if (k == 0) {
							pos = row_addr * width + column_addr;
						}
						else {
							int column_addr_alt = (column_addr ^ (temp_fp2 % 4)) % width;
							pos = row_addr * width + column_addr_alt;
						}
						for (int m = 0; m < SLOTNUM; m++) {
							if (value[pos].src[m] == 0 && value[pos].weight[m] == 0) {
								value[pos].src[m] = temp_fp1 + head * i;
								value[pos].dst[m] = temp_fp2 + head * (j * 2 + k);
								value[pos].weight[m] = temp_weight;
								inserted = true;
								return;
							}
						}
					}
				}
			}
			if(!inserted && kick_i == 0 && kick_j == 0){
				break;
			}
			kickNum++;
			if(kickNum > 10){
				break;
			}
		}
		unsigned int curk1 = (temp_addr1 << fingerprintLength) + temp_fp1;
		unsigned int curk2 = (temp_addr2 << fingerprintLength) + temp_fp2;
		if (!inserted) {
			//save to successor buffer k1->k2
			map<unsigned int, unsigned int>::iterator it = successorIndex.find(curk1);
			if (it != successorIndex.end()) {
				unsigned int tag = it->second;
				vector<node>::iterator iter = find_if(successorAdjacencyList[tag].begin(), successorAdjacencyList[tag].end(), findx(curk2));
				if (iter != successorAdjacencyList[tag].end()) {
					iter->weight += temp_weight;
				}
				else {
					node tmpnode;
					tmpnode.key = curk2;
					tmpnode.weight = temp_weight;
					successorAdjacencyList[tag].push_back(tmpnode);
				}
			}
			else {
				successorIndex[curk1] = n1;
				n1++;
				node tmpnode;
				tmpnode.key = curk1;
				tmpnode.weight = 0;
				vector<node> vc;
				vc.push_back(tmpnode);
				if (curk1 != curk2) {  //k1==k2 means loop
					node newnode;
					newnode.key = curk2;
					newnode.weight = temp_weight;
					vc.push_back(newnode);
				}
				else {
					vc[0].weight += temp_weight;
				}
				successorAdjacencyList.push_back(vc);
			}
			
			//save to precursor buffer k2<-k1
			map<unsigned int, unsigned int>::iterator it2 = precursorIndex.find(curk2);
			if (it2 != precursorIndex.end()) {
				precursorAdjacencyList[it2->second].weight += temp_weight;
			}
			else {
				precursorIndex[curk2] = n2;
				n2++;
				node tmpnode;
				tmpnode.key = curk2;
				tmpnode.weight = temp_weight;
				precursorAdjacencyList.push_back(tmpnode);
			}
		}
	}
	return;
}
// src is the ID of the source node, dst is the ID of the destination node, return the weight of the edge
weight_type Layer::edgeQuery(string src, string dst) {
	unsigned int hash_src = (*hfunc[0])((unsigned char*)(src.c_str()), src.length());
	unsigned int hash_dst = (*hfunc[0])((unsigned char*)(dst.c_str()), dst.length());
	unsigned int mask = pow(2, fingerprintLength) - 1;
	unsigned short fp_src = hash_src & mask;
	if (fp_src == 0) fp_src += 1;
	unsigned int addr_src = (hash_src >> fingerprintLength) % depth;
	unsigned short fp_dst = hash_dst & mask;
	if (fp_dst == 0) fp_dst += 1;
	unsigned int addr_dst = (hash_dst >> fingerprintLength) % width;
	// alternative addresses
	int seed1[4] = { 0 };
	int seed2[2] = { 0 }; 
	seed1[0] = fp_src;
	seed1[1] = (seed1[0] * multiplier + increment) % modulus;
	seed1[2] = (seed1[1] * multiplier + increment) % modulus;
	seed1[3] = (seed1[2] * multiplier + increment) % modulus;
	seed2[0] = fp_dst;
	seed2[1] = (seed2[0] * multiplier + increment) % modulus;
	
	for (int i = 0; i < 4; i++) {
		int row_addr = (addr_src + seed1[i]) % depth;
		for (int l = 0; l < 2; l++) {
			int column_addr = (addr_dst + seed2[l]) % width;
			for (int k = 0; k < 2; k++) {
				int pos;
				if (k == 0) {
					pos = row_addr * width + column_addr;
				}
				else {
					int column_addr_alt = (column_addr ^ (fp_dst % 4)) % width;
					pos = row_addr * width + column_addr_alt;
				}
				if(pos >= width * depth || pos < 0) {
					cout << pos << " out of range!" << endl;
					continue;
				}
				for (int j = 0; j < SLOTNUM; j++) {
					if (((value[pos].src[j] >> 14) == i) && ((value[pos].dst[j] >> 14) == (l*2+k)) && ((value[pos].src[j] & mask) == fp_src) && ((value[pos].dst[j] & mask) == fp_dst)) {
						return value[pos].weight[j];
					}
				}
			}
		}
	}
	
	unsigned int k1 = (addr_src << fingerprintLength) + fp_src;
	unsigned int k2 = (addr_dst << fingerprintLength) + fp_dst;
	map<unsigned int, unsigned int>::iterator it = successorIndex.find(k1);
	if (it != successorIndex.end()) {
		unsigned int tag = it->second;
		vector<node>::iterator iter = find_if(successorAdjacencyList[tag].begin(), successorAdjacencyList[tag].end(), findx(k2));
		if (iter != successorAdjacencyList[tag].end()) {
			return iter->weight;
		}
	}
	return 0;
}
// vertex is the ID of the queried node, function for node query. type 0 is for successor query, type 1 is for precusor query
weight_type Layer::nodeQuery(string vertex, int type) { // vertex is the ID of the queried node, function for node query.
	weight_type weight = 0;
	unsigned int hash_vertex = (*hfunc[0])((unsigned char*)(vertex.c_str()), vertex.length());
	unsigned int mask = pow(2, fingerprintLength) - 1;
	unsigned short fp = hash_vertex & mask;
	if (fp == 0) fp += 1;
		
	// alternative addresses
	int seed1[4] = { 0, 0, 0, 0 };
	seed1[0] = fp;
	seed1[1] = (seed1[0] * multiplier + increment) % modulus;
	if (type == 0) {
		seed1[2] = (seed1[1] * multiplier + increment) % modulus;
		seed1[3] = (seed1[2] * multiplier + increment) % modulus;
		unsigned int addr = (hash_vertex >> fingerprintLength) % depth;
		for (int i = 0; i < 4; i++)	{
			int row_addr = (addr + seed1[i]) % depth;
			for (int k = 0; k < width; k++)	{
				int pos = row_addr * width + k;
				for (int j = 0; j < SLOTNUM; ++j) {
					if (((value[pos].src[j] >> 14) == i) && ((value[pos].src[j] & mask) == fp)) {
						weight += value[pos].weight[j];
					}
				}
			}	
		}
		unsigned int k1 = (addr << fingerprintLength) + fp;
		map<unsigned int, unsigned int>::iterator it = successorIndex.find(k1);
		if (it != successorIndex.end())	{
			unsigned int tag = it->second;
			vector<node>::iterator iter;
			for (iter = successorAdjacencyList[tag].begin(); iter != successorAdjacencyList[tag].end(); iter++) {
				weight += iter->weight;
			}
		}
	}
	else if (type == 1) {
		int py[4];
		unsigned int addr = (hash_vertex >> fingerprintLength) % width;
		py[0] = (addr + seed1[0]) % width;
		py[1] = py[0] ^ (fp % 4) % width;
		py[2] = (addr + seed1[1]) % width;
		py[3] = py[2] ^ (fp % 4) % width;
		for (int k = 0; k < depth; k++) {
			for (int i = 0; i < 4; i++) {
				int pos = k * width + py[i];
				for (int j = 0; j < SLOTNUM; ++j) {
					if (((value[pos].dst[j] >> 14) == i) && ((value[pos].dst[j] & mask) == fp)) {
						weight += value[pos].weight[j];
					}
				}
			}
		}
		unsigned int k1 = (addr << fingerprintLength) + fp;
		map<unsigned int, unsigned int>::iterator it = precursorIndex.find(k1);
		if (it != precursorIndex.end()) {
			weight += precursorAdjacencyList[it->second].weight;
		}
	}
	return weight;
}
