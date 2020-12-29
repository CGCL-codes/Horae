// horae - 2*8 alternative address kick out
// 删除map索引
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
#include <sys/time.h>
#include "../HashFunction.h"
using namespace std;
// the parameters of linear congruential method
#define increment 739
#define modulus 1048576
#define multiplier 5

#define SLOTNUM 2 	// This is the parameter to control the maximum number of rooms in a bucket.

typedef uint32_t weight_type;
typedef int64_t time_type;

struct basket {
	uint16_t src[SLOTNUM];
	uint16_t dst[SLOTNUM]; 
	weight_type weight[SLOTNUM];
};

struct node {
	uint32_t key;
	weight_type weight;
};

class findx {
public:
	findx(const uint32_t va) { value = va; }
	bool operator()(const vector<node>::value_type &nod) {
		if (nod.key == value)
			return true;
		else
			return false;
	}
private:
	uint32_t value;
};

class findv {
public:
	findv(uint32_t va) { value = va; }
	bool operator()(vector<node> &vc) {
		if (vc[0].key == value)
			return true;
		else
			return false;
	}
private:
	uint32_t value;
};

class Layer {
private:
	const uint32_t granularity;
	const uint32_t width;
	const uint32_t depth;
	const uint32_t fingerprintLength;
	basket* value;
	// vector<node> precursorAdjacencyList;
	vector<vector<node>> successorAdjacencyList;

	// uint32_t n1, n2;	//n1 for successor, n2 for precursor
	// map<uint32_t, uint32_t> precursorIndex;
	// map<uint32_t, uint32_t> successorIndex;
	int getMinIndex(uint32_t* a, int length);

public:
	Layer(uint32_t granularity, uint32_t width, uint32_t depth, uint32_t fingerprintLength);
	Layer(const Layer &layer);
	~Layer();
	void insert(string src, string dst, weight_type weight);
	weight_type edgeQuery(string src, string dst);
	weight_type nodeQuery(string vertex, int type);		//src_type = 0 dst_type = 1
	uint32_t getGranularity() const;
	/**************** test functions ****************/
	void bucketCounting();
	/**************** test functions ****************/
};
/**************** test functions ****************/
void Layer::bucketCounting() {
	int64_t room_count = 0;
	int64_t bucket_count = 0;
	for (int64_t i = 0; i < width * depth; i++) {
		if ((value[i].src[0] != 0) && (value[i].weight[0] != 0)) {
				bucket_count++;
		}
		for (int64_t j = 0; j < SLOTNUM; j++) {
			if ((value[i].src[j] != 0) && (value[i].weight[j] != 0)) {
				room_count++;
			}
		}
	}
	cout << "---------------------------------------" << endl;
	cout << "Layer room_count = " << room_count << ", total room = " << (width * depth * SLOTNUM) << ", space usage is " << 
			(double)room_count / (double)(width * depth * SLOTNUM) * 100 << "%" << endl;
	cout << "Layer bucket_count = " << bucket_count << ", total bucket = " << (width * depth) << ", space usage is " << 
			(double)bucket_count / (double)(width * depth) * 100 << "%" << endl;
	//print buffer size
	// int64_t total_preBuffer = this->precursorAdjacencyList.size();
	int64_t total_sucBuffer = 0;
	for(int64_t i = 0; i < this->successorAdjacencyList.size(); i++) {
		for(int64_t j = 0; j < this->successorAdjacencyList[i].size(); j++) {
			total_sucBuffer++;
		}
	}
	// cout << "total_preBuffer = " << total_preBuffer << endl;
	cout << "total_sucBuffer = " << total_sucBuffer << endl;	
	cout << "---------------------------------------" << endl;
	return;
}
/**************** test functions ****************/


uint32_t Layer::getGranularity() const{
	if(this == NULL) {
		cout << "NULL pointer!!" << endl;
		getchar();
		exit(-1);
	}
	return this->granularity;
}
Layer::Layer(uint32_t granularity, uint32_t width, uint32_t depth, uint32_t fingerprintLength)
: granularity(granularity), width(width), depth(depth), fingerprintLength(fingerprintLength) {
	// this->n1 = 0;									// precursor index
	// this->n2 = 0;									// successor index
	uint32_t msize = width * depth;
	posix_memalign((void**)&value, 64, sizeof(basket) * msize); 	// 64-byte alignment of the requested space
	memset(this->value, 0, sizeof(basket) * msize);
}
Layer::Layer(const Layer &layer)
: granularity(2 * layer.getGranularity()), width(layer.width), depth(layer.depth), fingerprintLength(layer.fingerprintLength) {
	// this->n1 = layer.n1;
	// this->n2 = layer.n2;
	// this->precursorIndex = layer.precursorIndex;
	// this->successorIndex = layer.successorIndex;
	// this->precursorAdjacencyList.resize(layer.precursorAdjacencyList.size());
	this->successorAdjacencyList.resize(layer.successorAdjacencyList.size());
	
	// this->precursorAdjacencyList.assign(layer.precursorAdjacencyList.begin(), layer.precursorAdjacencyList.end());
	for (uint32_t i = 0; i < layer.successorAdjacencyList.size(); i++) {
		this->successorAdjacencyList[i].assign(layer.successorAdjacencyList[i].begin(), layer.successorAdjacencyList[i].end());
	}
	uint32_t msize = width * depth;
	posix_memalign((void**)&value, 64, sizeof(basket) * msize);		//// 64-byte alignment of the requested space
	for(uint32_t i = 0; i < msize; i++) {
		for(uint32_t j = 0; j < SLOTNUM; j++) {
			this->value[i].src[j] = layer.value[i].src[j];
			this->value[i].dst[j] = layer.value[i].dst[j];
			this->value[i].weight[j] = layer.value[i].weight[j];
		}
	}
}
Layer::~Layer() {
	delete[] value;
	// vector<node>().swap(precursorAdjacencyList);
	vector<vector<node>>().swap(successorAdjacencyList);
}
int Layer::getMinIndex(uint32_t* a, int length) {
	uint32_t min = a[0];
	int index = 0;
	for(int i = 1; i < length; i++) {
		if(a[i] < min) {
			min = a[i];
			index = i;
		}
	}
	return index;
}
// src is the ID of the source node, dst is the ID of the destination node, weight is the weight of the edge.
void Layer::insert(string src, string dst, weight_type weight) {
	uint32_t hash_src = (*hfunc[0])((unsigned char*)(src.c_str()), src.length());
	uint32_t hash_dst = (*hfunc[0])((unsigned char*)(dst.c_str()), dst.length());
	//uint32_t mask = pow(2, fingerprintLength) - 1;
	uint32_t mask = (1 << fingerprintLength) - 1;
	uint32_t head = 16384; //pow(2, 14);
	uint16_t fp_src = hash_src & mask;
	if (fp_src == 0) fp_src += 1;
	uint32_t addr_src = (hash_src >> fingerprintLength) % depth;
	uint16_t fp_dst = hash_dst & mask;
	if (fp_dst == 0) fp_dst += 1;
	uint32_t addr_dst = (hash_dst >> fingerprintLength) % width;

	uint32_t k1 = (addr_src << fingerprintLength) + fp_src;
	uint32_t k2 = (addr_dst << fingerprintLength) + fp_dst;

	// Alternative address -- 4 * 4
	uint32_t seed1[4] = { 0, 0, 0, 0 };		// row
	uint32_t seed2[4] = { 0, 0, 0, 0 };		//column
	seed1[0] = fp_src;
	seed1[1] = (seed1[0] * multiplier + increment) % modulus;
	seed1[2] = (seed1[1] * multiplier + increment) % modulus;
	seed1[3] = (seed1[2] * multiplier + increment) % modulus;
	seed2[0] = fp_dst;
	seed2[1] = (seed2[0] * multiplier + increment) % modulus;
	seed2[2] = (seed2[1] * multiplier + increment) % modulus;
	seed2[3] = (seed2[2] * multiplier + increment) % modulus;

	bool inserted = false;
	for (int i = 0; i < 4; i++) {
		int depth_addr = (addr_src + seed1[i]) % depth;
		for (int j = 0; j < 4; j++) {
			int width_addr = (addr_dst + seed2[j]) % width;
			int pos = depth_addr * width + width_addr;
			for (int m = 0; m < SLOTNUM; m++) {
				if (((value[pos].src[m] >> 14) == i) && ((value[pos].dst[m] >> 14) == j) && ((value[pos].src[m] & mask) == fp_src) && ((value[pos].dst[m] & mask) == fp_dst)) {
					value[pos].weight[m] += weight;
					inserted = true;
					return;
				}
				if (value[pos].src[m] == 0 && value[pos].weight[m] == 0) {
					value[pos].src[m] = fp_src + head * i;
					value[pos].dst[m] = fp_dst + head * j;
					value[pos].weight[m] = weight;
					inserted = true;
					return;
				}
			}
			
		}
	}
	if (!inserted) {
		// map<uint32_t, uint32_t>::iterator it = successorIndex.find(k1);
		vector<vector<node> >::iterator it = find_if(successorAdjacencyList.begin(), successorAdjacencyList.end(), findv(k1));
		if (it != successorAdjacencyList.end()) {			//在successorBuffer中查找k1->k2
			vector<node>::iterator iter = find_if(it->begin(), it->end(), findx(k2));
			if (iter != it->end()) {	//找到k1->k2
				iter->weight += weight;
				inserted = true;
				return;
				// map<uint32_t, uint32_t>::iterator it2 = precursorIndex.find(k2);		//将k2->k1插入
				// vector<node>::iterator it2 = find_if(precursorAdjacencyList.begin(), precursorAdjacencyList.end(), findx(k2));
				// if (it2 != precursorAdjacencyList.end()) {
				// 	it2->weight += weight;
				// 	inserted = true;
				// 	return;
				// }
			}
		}
	}
	if (!inserted) {			// There is no k1->k2 in buffer, start kicking
		int kickNum = 0;
		//temp is the edge currently to be inserted
		uint32_t temp_addr1 = addr_src;
		uint32_t temp_addr2 = addr_dst;
		uint16_t temp_fp1 = fp_src;
		uint16_t temp_fp2 = fp_dst;
		weight_type temp_weight = weight;
		int kickflag = 1; //
		while (!inserted){
			// 找到当前边(e1)的第kickflag个地址，然后判断两个slot中序号较小的那个剔出
			// kickout the elements of the pos(the power of two choices)
			uint32_t pos_x = (temp_addr1 + temp_fp1) % depth; 
			uint32_t pos_y;
			if(kickflag == 1){
				pos_y = (temp_addr2 + temp_fp2) % width;
			}else if(kickflag == 2){
				uint32_t tmp2 = (temp_fp2 * multiplier + increment) % modulus;
				pos_y = (temp_addr2 + tmp2) % width;

			}
			uint32_t pos = pos_x * width + pos_y;
			uint32_t insertnum[SLOTNUM];
			for (int i = 0; i < SLOTNUM; i++){
				insertnum[i] = (value[pos].src[i] >> 14) * 4 + (value[pos].dst[i] >> 14);
			}
			int kickindex = getMinIndex(insertnum, SLOTNUM);					//找到要踢的那个slot
			uint32_t kick_i = (value[pos].src[kickindex] >> 14);			//将要踢出的边(e2)信息存储到kick_变量中
			uint32_t kick_j = (value[pos].dst[kickindex] >> 14);
			uint16_t kick_fpx = (value[pos].src[kickindex] & mask);
			uint16_t kick_fpy = (value[pos].dst[kickindex] & mask);
			weight_type kick_weight = value[pos].weight[kickindex];
			// insert the element
			value[pos].src[kickindex] = temp_fp1;
			if(kickflag == 1){
				value[pos].dst[kickindex] = temp_fp2;
			}
			else if(kickflag == 2){
				value[pos].dst[kickindex] = temp_fp2 + head;
			}
			value[pos].weight[kickindex] = temp_weight;
			//temp_为边e2
			temp_fp1 = kick_fpx;
			temp_fp2 = kick_fpy;
			temp_weight = kick_weight;
			
			//根据e2存储在矩阵中的当前位置以及下标信息还原边e2的addr1、addr2
			uint32_t shifterx = temp_fp1;
			for (int v = 0; v < kick_i; v++) {
				shifterx = (shifterx * multiplier + increment) % modulus;
			}
			uint32_t tmp_h1 = pos_x;
			while (tmp_h1 < shifterx)
				tmp_h1 += depth;
			tmp_h1 -= shifterx;
			temp_addr1 = tmp_h1;

			uint32_t shiftery = temp_fp2;
			for (int v = 0; v < kick_j; v++)
				shiftery = (shiftery * multiplier + increment) % modulus;
			uint32_t tmp_h2 = pos_y;
			while (tmp_h2 < shiftery)
				tmp_h2 += width;
			tmp_h2 -= shiftery;
			temp_addr2 = tmp_h2;

			uint32_t curtmp1[4] = { 0 };
			uint32_t curtmp2[4] = { 0 }; 
			curtmp1[0] = temp_fp1;
			curtmp1[1] = (curtmp1[0] * multiplier + increment) % modulus;
			curtmp1[2] = (curtmp1[1] * multiplier + increment) % modulus;
			curtmp1[3] = (curtmp1[2] * multiplier + increment) % modulus;
			curtmp2[0] = temp_fp2;
			curtmp2[1] = (curtmp2[0] * multiplier + increment) % modulus;
			curtmp2[2] = (curtmp2[1] * multiplier + increment) % modulus;
			curtmp2[3] = (curtmp2[2] * multiplier + increment) % modulus;
			
			uint32_t addr_no = (kick_i * 4) + (kick_j);

			for (int i = 0; i < 4; i++) {
				uint32_t depth_addr = (temp_addr1 + curtmp1[i]) % depth;
				for (int j = 0; j < 4; j++) {
					if((i * 4 + j) <= addr_no)
						continue;
					int width_addr = (temp_addr2 + curtmp2[j]) % width;
					int pos = depth_addr * width + width_addr;
					
					for (int m = 0; m < SLOTNUM; m++) {
						if (value[pos].src[m] == 0 && value[pos].weight[m] == 0) {
							value[pos].src[m] = temp_fp1 + head * i;
							value[pos].dst[m] = temp_fp2 + head * j;
							value[pos].weight[m] = temp_weight;
							inserted = true;
							return;
						}
					}
					
				}
			}
			if(addr_no == 0){
				kickflag = 2;
			}else{
				kickflag = 1;
			}//avoid kick-loop
			kickNum++;
			if(kickNum > 10){
				break;
			}
		}
	
		uint32_t curk1 = (temp_addr1 << fingerprintLength) + temp_fp1;
		uint32_t curk2 = (temp_addr2 << fingerprintLength) + temp_fp2;
		if (!inserted) {
			//save to successor buffer k1->k2
			// map<uint32_t, uint32_t>::iterator it = successorIndex.find(curk1);
			vector<vector<node> >::iterator it = find_if(successorAdjacencyList.begin(), successorAdjacencyList.end(), findv(curk1));
			if (it != successorAdjacencyList.end()) {
				vector<node>::iterator iter = find_if(it->begin(), it->end(), findx(curk2));
				if (iter != it->end()) {
					iter->weight += temp_weight;
				}
				else {
					node tmpnode;
					tmpnode.key = curk2;
					tmpnode.weight = temp_weight;
					it->push_back(tmpnode);
				}
			}
			else {
				// successorIndex[k1] = n1;
				// n1++;
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
			// map<uint32_t, uint32_t>::iterator it2 = precursorIndex.find(curk2);
			// vector<node>::iterator it2 = find_if(precursorAdjacencyList.begin(), precursorAdjacencyList.end(), findx(curk2));
			// if (it2 != precursorAdjacencyList.end()) {
			// 	it2->weight += temp_weight;
			// }
			// else {
			// 	// precursorIndex[curk2] = n2;
			// 	// n2++;
			// 	node newnode;
			// 	newnode.key = curk2;
			// 	newnode.weight = temp_weight;
			// 	precursorAdjacencyList.push_back(newnode);
			// }
		}
	}
	return;
}
// src is the ID of the source node, dst is the ID of the destination node, return the weight of the edge
weight_type Layer::edgeQuery(string src, string dst) {
	uint32_t hash_src = (*hfunc[0])((unsigned char*)(src.c_str()), src.length());
	uint32_t hash_dst = (*hfunc[0])((unsigned char*)(dst.c_str()), dst.length());
	uint32_t mask = pow(2, fingerprintLength) - 1;
	uint16_t fp_src = hash_src & mask;
	if (fp_src == 0) fp_src += 1;
	uint32_t addr_src = (hash_src >> fingerprintLength) % depth;
	uint16_t fp_dst = hash_dst & mask;
	if (fp_dst == 0) fp_dst += 1;
	uint32_t addr_dst = (hash_dst >> fingerprintLength) % width;
	// alternative addresses
	uint32_t seed1[4] = { 0, 0, 0, 0 };
	uint32_t seed2[4] = { 0, 0, 0, 0 }; 
	seed1[0] = fp_src;
	seed1[1] = (seed1[0] * multiplier + increment) % modulus;
	seed1[2] = (seed1[1] * multiplier + increment) % modulus;
	seed1[3] = (seed1[2] * multiplier + increment) % modulus;
	seed2[0] = fp_dst;
	seed2[1] = (seed2[0] * multiplier + increment) % modulus;
	seed2[2] = (seed2[1] * multiplier + increment) % modulus;
	seed2[3] = (seed2[2] * multiplier + increment) % modulus;
	
	bool inserted = false;

	for (int i = 0; i < 4; i++) {
		int p1 = (addr_src + seed1[i]) % depth;
		for (int l = 0; l < 4; l++) {
			int p2 = (addr_dst + seed2[l]) % width;
			
			int pos = p1 * width + p2;
			if(pos >= width * depth || pos < 0) {
				cout << pos << " out of range!" << endl;
				continue;
			}
			for (int j = 0; j < SLOTNUM; j++) {
				if (((value[pos].src[j] >> 14) == i) && ((value[pos].dst[j] >> 14) == l) && ((value[pos].src[j] & mask) == fp_src) && ((value[pos].dst[j] & mask) == fp_dst)) {
					return value[pos].weight[j];
				}
			}
			
		}
	}
	
	uint32_t k1 = (addr_src << fingerprintLength) + fp_src;
	uint32_t k2 = (addr_dst << fingerprintLength) + fp_dst;
	// map<uint32_t, uint32_t>::iterator it = successorIndex.find(k1);
	vector<vector<node> >::iterator it = find_if(successorAdjacencyList.begin(), successorAdjacencyList.end(), findv(k1));
	if (it != successorAdjacencyList.end()) {
		vector<node>::iterator iter = find_if(it->begin(), it->end(), findx(k2));
		if (iter != it->end()) {
			return iter->weight;
		}
	}
	return 0;
}
// vertex is the ID of the queried node, function for node query. type 0 is for successor query, type 1 is for precusor query
weight_type Layer::nodeQuery(string vertex, int type) { // vertex is the ID of the queried node, function for node query.
	weight_type weight = 0;
	uint32_t hash_vertex = (*hfunc[0])((unsigned char*)(vertex.c_str()), vertex.length());
	uint32_t mask = pow(2, fingerprintLength) - 1;
	uint16_t fp = hash_vertex & mask;
	if (fp == 0) fp += 1;
		
	// alternative addresses
	uint32_t seed1[4] = { 0, 0, 0, 0 };
	seed1[0] = fp;
	seed1[1] = (seed1[0] * multiplier + increment) % modulus;
	seed1[2] = (seed1[1] * multiplier + increment) % modulus;
	seed1[3] = (seed1[2] * multiplier + increment) % modulus;
	if (type == 0) {
		uint32_t addr = (hash_vertex >> fingerprintLength) % depth;
		for (int i = 0; i < 4; i++)	{
			uint32_t row_addr = (addr + seed1[i]) % depth;
			for (int k = 0; k < width; k++)	{
				uint32_t pos = row_addr * width + k;
				for (int j = 0; j < SLOTNUM; ++j) {
					if (((value[pos].src[j] >> 14) == i) && ((value[pos].src[j] & mask) == fp)) {
						weight += value[pos].weight[j];
					}
				}
			}	
		}
		uint32_t k1 = (addr << fingerprintLength) + fp;
		//map<uint32_t, uint32_t>::iterator it = successorIndex.find(k1);
		vector<vector<node> >::iterator it = find_if(successorAdjacencyList.begin(), successorAdjacencyList.end(), findv(k1));
		if (it != successorAdjacencyList.end())	{
			//uint32_t tag = it->second;
			vector<node>::iterator iter;
			for (iter = it->begin(); iter != it->end(); iter++) {
				weight += iter->weight;
			}
		}
	}
	else if (type == 1) {
		uint32_t addr = (hash_vertex >> fingerprintLength) % width;
		for (int i = 0; i < 4; i++) {
			uint32_t col_addr = (addr + seed1[i]) % width;
			for (int k = 0; k < depth; k++) {
				uint32_t pos = k * width + col_addr;
				for (int j = 0; j < SLOTNUM; ++j) {
					if (((value[pos].dst[j] >> 14) == i) && ((value[pos].dst[j] & mask) == fp)) {
						weight += value[pos].weight[j];
					}
				}
			}
		}
		uint32_t k1 = (addr << fingerprintLength) + fp;
		//map<uint32_t, uint32_t>::iterator it = precursorIndex.find(k1);
		// vector<node>::iterator it = find_if(precursorAdjacencyList.begin(), precursorAdjacencyList.end(), findx(k1));
		// if (it != precursorAdjacencyList.end()) {
		// 	weight += it->weight;
		// }
		for(vector<vector<node> >::iterator it = successorAdjacencyList.begin(); it != successorAdjacencyList.end(); it++){
			for(vector<node>::iterator iter = it->begin(); iter!= it->end(); iter++){
				if(iter->key == k1){
					weight += iter->weight;
				}
			}
		}
	}
	return weight;
}
