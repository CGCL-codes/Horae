#ifndef _TreeNode_H
#define _TreeNode_H
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
#include "params.h"
#include <sys/time.h>

using namespace std;

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

class TreeNode {
private:
	bool cache_align = false;
	bool kick = false;
	uint32_t width;						// the width of the matrix
	uint32_t depth;						// the depth of the matrix
	const uint32_t fingerprintLength;	// the fingerprint length
	const uint32_t row_addrs;			// the row addrs
	const uint32_t column_addrs;		// the column_addrs
	struct mapnode {
        uint32_t addr;
        uint16_t fp;
    };
public:
	static int nodes;					// 矩阵实例化的节点数量
	time_type interval_start;
	time_type interval_end;
	TreeNode* left;
	TreeNode* right;
	basket* value;									// the matrix
	vector<vector<node>> successorAdjacencyList;	// the buffer

private:
	int getMinIndex(uint32_t* a, int length);
	bool insertMatrix(uint32_t addr_src, uint16_t fp_src, uint32_t addr_dst, uint16_t fp_dst, weight_type weight);
	bool kickElement(uint32_t& addr_src, uint16_t& fp_src, uint32_t& addr_dst, uint16_t& fp_dst, weight_type& weight);
	weight_type edgeQueryMatrix(uint32_t addr_src, uint16_t fp_src, uint32_t addr_dst, uint16_t fp_dst);
	weight_type nodeQueryMatrix(uint32_t addr_v, uint16_t fp_v, int type);
	
	bool insertMatrixCacheline(uint32_t addr_src, uint16_t fp_src, uint32_t addr_dst, uint16_t fp_dst, weight_type weight);
	bool kickElementCacheline(uint32_t& addr_src, uint16_t& fp_src, uint32_t& addr_dst, uint16_t& fp_dst, weight_type& weight);
	weight_type edgeQueryMatrixCacheline(uint32_t addr_src, uint16_t fp_src, uint32_t addr_dst, uint16_t fp_dst);
	weight_type nodeQueryMatrixCacheline(uint32_t addr_v, uint16_t fp_v, int type);

	void insertSucBuffer(uint32_t k1, uint32_t k2, weight_type weight);
	weight_type edgeQuerySucBuffer(uint32_t k1, uint32_t k2);
	weight_type nodeQuerySucBuffer(uint32_t k, int type);

public:
	TreeNode(time_type interval_start, time_type interval_end, uint32_t width, uint32_t depth, uint32_t fingerprintLength, bool cache_align, bool kick, uint32_t row_addrs = 4, uint32_t column_addrs = 4);
	// copy & build
	TreeNode(time_type interval_start, time_type interval_end, TreeNode* node);

	~TreeNode();
	void bucketCounting();

	void insert(string src, string dst, weight_type weight);
	weight_type edgeQuery(string src, string dst);
	weight_type nodeQuery(string vertex, int type);		//src_type = 0 dst_type = 1
	bool reachabilityQuery(string s, string d);
};

// copy & build
TreeNode::TreeNode(time_type interval_start, time_type interval_end, TreeNode* node):
interval_start(interval_start), interval_end(interval_end), width(node->width), depth(node->depth), fingerprintLength(node->fingerprintLength), cache_align(node->cache_align), kick(node->kick), row_addrs(node->row_addrs), column_addrs(node->column_addrs) {
	// cout << "TreeNode::TreeNode([" << interval_start << ", " << interval_end << "], width: " 
	// 	 << width <<", depth: " << depth << ", fplen: " << fingerprintLength
	// 	 << ", cache_align: " << cache_align << ", kick: " << kick 
	// 	 <<", row_addrs: " << row_addrs << ", column_addrs: " << column_addrs << ")" << endl;

	if (node->value == nullptr) {
		cout << "COPY ON BUILD ERROR" << endl;
	} else {
		nodes++;
		uint32_t msize = width * depth;
		posix_memalign((void**)&value, 64, sizeof(basket) * msize); 	// 64-byte alignment of the requested space
		for(uint32_t i = 0; i < msize; i++) {
			for(uint32_t j = 0; j < SLOTNUM; j++) {
				this->value[i].src[j] = node->value[i].src[j];
				this->value[i].dst[j] = node->value[i].dst[j];
				this->value[i].weight[j] = node->value[i].weight[j];
			}
		}
		// memset(this->value, 0, sizeof(basket) * msize);
		this->successorAdjacencyList.resize(node->successorAdjacencyList.size());
		for (uint32_t i = 0; i < node->successorAdjacencyList.size(); i++) {
			this->successorAdjacencyList[i].assign(node->successorAdjacencyList[i].begin(), node->successorAdjacencyList[i].end());
		}
	}
	this->left = nullptr;
	this->right = nullptr;
}

TreeNode::TreeNode(time_type interval_start, time_type interval_end, uint32_t width, uint32_t depth, uint32_t fingerprintLength, bool cache_align, bool kick, uint32_t row_addrs, uint32_t column_addrs):
interval_start(interval_start), interval_end(interval_end), width(width), depth(depth), fingerprintLength(fingerprintLength), cache_align(cache_align), kick(kick), row_addrs(row_addrs), column_addrs(column_addrs) {
	// cout << "TreeNode::TreeNode([" << interval_start << ", " << interval_end << "], width: " 
	// 	 << width <<", depth: " << depth << ", fplen: " << fingerprintLength
	// 	 << ", cache_align: " << cache_align << ", kick: " << kick 
	// 	 <<", row_addrs: " << row_addrs << ", column_addrs: " << column_addrs << ")" << endl;

	// uint32_t msize = width * depth;
	// posix_memalign((void**)&value, 64, sizeof(basket) * msize); 	// 64-byte alignment of the requested space
	// memset(this->value, 0, sizeof(basket) * msize);
	
	this->value = nullptr;
	this->left = nullptr;
	this->right = nullptr;
}

TreeNode::~TreeNode() {
    // cout << "TreeNode::~TreeNode([" << interval_start << ", " << interval_end << "])" << endl;
	vector<vector<node>>().swap(successorAdjacencyList);
	if (this->value != nullptr)
		delete[] this->value;
}

int TreeNode::getMinIndex(uint32_t* a, int length) {
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

void TreeNode::bucketCounting() {
	if (this->value != nullptr) {
		cout << "---------------------------------------" << endl;
		cout << "TreeNode bucketCounting(): print bucket..." << endl;
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
		cout << "TreeNode (" << interval_start << ", " << interval_end << "), room_count = " << room_count << ", total room = " << (width * depth * SLOTNUM) << ", space usage is " << 
				(double)room_count / (double)(width * depth * SLOTNUM) * 100 << "%" << endl;
		cout << "TreeNode (" << interval_start << ", " << interval_end << "), bucket_count = " << bucket_count << ", total bucket = " << (width * depth) << ", space usage is " << 
				(double)bucket_count / (double)(width * depth) * 100 << "%" << endl;
		cout << "---------------------------------------" << endl;
		//print buffer size
		cout << "TreeNode bucketCounting(): print successorAdjacencyList..." << endl;
		int64_t total_sucBuffer = 0;
		int64_t total_cap = 0;
		for(int64_t i = 0; i < this->successorAdjacencyList.size(); i++) {
			total_cap += this->successorAdjacencyList[i].capacity();
		}
		cout << "successorAdjacencyList.capacity() = " << successorAdjacencyList.capacity() << endl;
		cout << "total_cap = " << total_cap << endl;
		for(int64_t i = 0; i < this->successorAdjacencyList.size(); i++) {
			total_sucBuffer += this->successorAdjacencyList[i].size();
		}
		cout << "total_sucBuffer = " << total_sucBuffer << endl;
		cout << "---------------------------------------" << endl;
	}
	return;
}

// src is the ID of the source node, dst is the ID of the destination node, weight is the weight of the edge.
void TreeNode::insert(string src, string dst, weight_type weight) {
	if (this->value == nullptr) {
		nodes++;
		uint32_t msize = width * depth;
		posix_memalign((void**)&value, 64, sizeof(basket) * msize); 	// 64-byte alignment of the requested space
		memset(this->value, 0, sizeof(basket) * msize);
	}

	// cout << "insert--[" << this->interval_start << ", " << this->interval_end << "]" << endl;
	uint32_t hash_src = (*hfunc[0])((unsigned char*)(src.c_str()), src.length());
	uint32_t hash_dst = (*hfunc[0])((unsigned char*)(dst.c_str()), dst.length());
	//uint32_t mask = pow(2, fingerprintLength) - 1;
	uint32_t mask = (1 << fingerprintLength) - 1;
	uint16_t fp_src = hash_src & mask;
	if (fp_src == 0) fp_src += 1;
	uint32_t addr_src = (hash_src >> fingerprintLength) % depth;
	uint16_t fp_dst = hash_dst & mask;
	if (fp_dst == 0) fp_dst += 1;
	uint32_t addr_dst = (hash_dst >> fingerprintLength) % width;

	uint32_t k1 = (addr_src << fingerprintLength) + fp_src;
	uint32_t k2 = (addr_dst << fingerprintLength) + fp_dst;

	bool inserted = false;
	if (cache_align) 
		inserted = insertMatrixCacheline(addr_src, fp_src, addr_dst, fp_dst, weight);
	else
		inserted = insertMatrix(addr_src, fp_src, addr_dst, fp_dst, weight);
	
	if (!inserted) {
		// insert to the suc-buffer or kick-out
		if (!kick) 
			insertSucBuffer(k1, k2, weight);
		else {
			// map<uint32_t, uint32_t>::iterator it = successorIndex.find(k1);
			vector<vector<node> >::iterator it = find_if(successorAdjacencyList.begin(), successorAdjacencyList.end(), findv(k1));
			if (it != successorAdjacencyList.end()) {			// find k1->k2 in successorAdjacencyList
				vector<node>::iterator iter = find_if(it->begin(), it->end(), findx(k2));
				if (iter != it->end()) {	// if found
					iter->weight += weight;
					return;
				}
			}
			if (!cache_align) {			// There is no k1->k2 in buffer, start kicking
				inserted = kickElement(addr_src, fp_src, addr_dst, fp_dst, weight);
			}
			else {
				inserted = kickElementCacheline(addr_src, fp_src, addr_dst, fp_dst, weight);
			}
			if (!inserted) {
				uint32_t curk1 = (addr_src << fingerprintLength) + fp_src;
				uint32_t curk2 = (addr_dst << fingerprintLength) + fp_dst;
				// save to successor buffer k1->k2
				insertSucBuffer(curk1, curk2, weight);
			}
		}
	}
	return;
}
// src is the ID of the source node, dst is the ID of the destination node, return the weight of the edge
weight_type TreeNode::edgeQuery(string src, string dst) {
	if (this->value == nullptr)
		return 0;

	// cout << "edge query: (" << this->interval_start << ", " << this->interval_end << ")-[" << src << ", " << dst << "]" << endl;
	uint32_t hash_src = (*hfunc[0])((unsigned char*)(src.c_str()), src.length());
	uint32_t hash_dst = (*hfunc[0])((unsigned char*)(dst.c_str()), dst.length());
	uint32_t mask = pow(2, fingerprintLength) - 1;
	uint16_t fp_src = hash_src & mask;
	if (fp_src == 0) fp_src += 1;
	uint32_t addr_src = (hash_src >> fingerprintLength) % depth;
	uint16_t fp_dst = hash_dst & mask;
	if (fp_dst == 0) fp_dst += 1;
	uint32_t addr_dst = (hash_dst >> fingerprintLength) % width;
	
	// query the matrix
	weight_type res = 0;
	if (cache_align) 
		res = edgeQueryMatrixCacheline(addr_src, fp_src, addr_dst, fp_dst);
	else
		res = edgeQueryMatrix(addr_src, fp_src, addr_dst, fp_dst);
	if (res != 0)	// find in the matrix
		return res;
	else {
		// query the suc-buffer
		uint32_t k1 = (addr_src << fingerprintLength) + fp_src;
		uint32_t k2 = (addr_dst << fingerprintLength) + fp_dst;
		return edgeQuerySucBuffer(k1, k2);
	}
	return 0;
}
// vertex is the ID of the queried node, function for node query. type 0 is for successor query, type 1 is for precusor query
weight_type TreeNode::nodeQuery(string vertex, int type) { // vertex is the ID of the queried node, function for node query.
	if (this->value == nullptr)
		return 0;
	
	weight_type weight = 0;
	uint32_t hash_vertex = (*hfunc[0])((unsigned char*)(vertex.c_str()), vertex.length());
	uint32_t mask = pow(2, fingerprintLength) - 1;
	uint16_t fp = hash_vertex & mask;
	if (fp == 0) fp += 1;
	
	if (type == 0) {
		uint32_t addr = (hash_vertex >> fingerprintLength) % depth;
		if (cache_align)
			weight += nodeQueryMatrixCacheline(addr, fp, type);
		else
			weight += nodeQueryMatrix(addr, fp, type);

		// query the suc-buffer
		uint32_t k1 = (addr << fingerprintLength) + fp;
		weight += nodeQuerySucBuffer(k1, type);
	}
	else if (type == 1) {
		uint32_t addr = (hash_vertex >> fingerprintLength) % width;
		if (cache_align)
			weight += nodeQueryMatrixCacheline(addr, fp, type);
		else
			weight += nodeQueryMatrix(addr, fp, type);

		// query the suc-buffer
		uint32_t k1 = (addr << fingerprintLength) + fp;
		weight += nodeQuerySucBuffer(k1, type);
	}
	return weight;
}

// s is the ID of the source node, d is the ID of the destination node
bool TreeNode::reachabilityQuery(string s, string d) {
	if (this->value == nullptr)
		return false;
	
	uint32_t hash_s = (*hfunc[0])((unsigned char*)(s.c_str()), s.length());
    uint32_t hash_d = (*hfunc[0])((unsigned char*)(d.c_str()), d.length());
    uint32_t mask = (1 << fingerprintLength) - 1;
    
    uint32_t addr_s = (hash_s >> fingerprintLength) % depth;
    uint32_t addr_d = (hash_d >> fingerprintLength) % width;

    uint16_t fp_s = hash_s & mask;
    uint16_t fp_d = hash_d & mask;
    if(fp_s == 0) fp_s = 1;
    if(fp_d == 0) fp_d = 1;

    uint32_t key_s = (addr_s << fingerprintLength) + fp_s;
    uint32_t key_d = (addr_d << fingerprintLength) + fp_d;
    
	int pos;
	map<uint32_t, bool> checked;
	queue<mapnode> q;
	mapnode e;
	e.addr = addr_s;
	e.fp = fp_s;
	q.push(e);
	checked[key_s] = true;
	map<unsigned int, bool>::iterator IT;

    uint32_t temp_addr;
    uint16_t temp_fp;
	
	if (cache_align) 
	{
		while (!q.empty())
		{
			e = q.front();
			temp_addr = e.addr;
			temp_fp = e.fp;

			int* tmp1 = new int[row_addrs];
			int* tmp2 = new int[column_addrs / 2];
			tmp1[0] = temp_fp;
			tmp2[0] = fp_d;
			
			for (int i = 1; i < row_addrs; i++)
			{
				tmp1[i] = (tmp1[i - 1] * multiplier + increment) % modulus;
			}
			for (int i = 1; i < column_addrs / 2; i++) 
			{
				tmp2[i] = (tmp2[i - 1] * multiplier + increment) % modulus;
			}
			for (int i1 = 0; i1 < row_addrs; i1++)
			{
				int p1 = (temp_addr + tmp1[i1]) % depth;
				for (int i2 = 0; i2 < column_addrs / 2; i2++)
				{
					int p2 = (addr_d + tmp2[i2]) % width;
					for (int k = 0; k < 2; k++) {
						uint32_t pos;
						if (k == 0) {
							pos = p1 * width + p2;
						}
						else {
							uint32_t column_addr_alt = (p2 ^ (fp_d % CACHESLOT)) % width;
							pos = p1 * width + column_addr_alt;
						}
						for (int j = 0; j < SLOTNUM; j++) {
							if (((value[pos].src[j] >> 14) == i1) && ((value[pos].dst[j] >> 14) == (i2*2+k)) && ((value[pos].src[j] & mask) == temp_fp) && ((value[pos].dst[j] & mask) == fp_d)) {
								delete []tmp1;
								delete []tmp2;
								return true;
							}
						}
					}
				}
			}
			//	 find in buffer
			uint32_t temp_key = (temp_addr << fingerprintLength) + temp_fp;
			vector<vector<node> >::iterator it = find_if(successorAdjacencyList.begin(), successorAdjacencyList.end(), findv(temp_key));
			if (it != successorAdjacencyList.end())
			{
				for (vector<node>::iterator iter = it->begin(); iter != it->end(); iter++) 
				{
					if (iter->key != temp_key)
					{
						unsigned int val = iter->key;
						unsigned int temp_h = (val) >> fingerprintLength;
						unsigned int tmp = pow(2, fingerprintLength);
						unsigned short temp_g = (val % tmp);
						if ((temp_h == addr_d) && (temp_g == fp_d))
						{
							delete []tmp1;
							delete []tmp2;
							return true;
						}
			
						IT = checked.find(val);
						if (IT == checked.end())
						{
							mapnode temp_e;
							temp_e.addr = temp_h;
							temp_e.fp = temp_g;
							q.push(temp_e);
							checked[val] = true;;
						}
					}
				}
			}
			
			// find in matrix
			for (int i1 = 0; i1 < row_addrs; i1++)
			{
				int p1 = (temp_addr + tmp1[i1]) % depth;
				for (int i2 = 0; i2 < width; i2++)
				{
					int pos = p1 * width + i2;
					for (int i3 = 0; i3 < SLOTNUM; i3++)
					{
						if ((value[pos].src[i3] & mask) == temp_fp && ((value[pos].src[i3] >> 14) == i1))
						{
							uint32_t tmp_g = value[pos].dst[i3];
							int tmp_s = (value[pos].dst[i3] >> 14) & 0xff;
				
							uint32_t shifter = tmp_g;
							for (int v = 0; v < tmp_s; v++)
								shifter = (shifter * multiplier + increment) % modulus;
							uint32_t tmp_h = i2;
							while (tmp_h < shifter)
								tmp_h += width;				/////////
							tmp_h -= shifter;

							uint32_t val = (tmp_h << fingerprintLength) + tmp_g;
						
							IT = checked.find(val);
							if (IT == checked.end())
							{
								mapnode tmp_e;
								tmp_e.addr = tmp_h;
								tmp_e.fp = tmp_g;
								q.push(tmp_e);
								checked[val] = true;
							}
						}
					}
				}
			}
			delete[] tmp1;
			delete[] tmp2;
			q.pop();
		}
	}
	else 
	{
		while (!q.empty())
		{
			e = q.front();
			temp_addr = e.addr;
			temp_fp = e.fp;

			int* tmp1 = new int[row_addrs];
			int* tmp2 = new int[column_addrs];
			tmp1[0] = temp_fp;
			tmp2[0] = fp_d;
			
			for (int i = 1; i < row_addrs; i++)
			{
				tmp1[i] = (tmp1[i - 1] * multiplier + increment) % modulus;
			}
			for (int i = 1; i < column_addrs; i++) 
			{
				tmp2[i] = (tmp2[i - 1] * multiplier + increment) % modulus;
			}
			for (int i1 = 0; i1 < row_addrs; i1++)
			{
				int p1 = (temp_addr + tmp1[i1]) % depth;
				for (int i2 = 0; i2 < column_addrs; i2++)
				{
					int p2 = (addr_d + tmp2[i2]) % width;
					int pos = p1 * width + p2;
					for (int i3 = 0; i3 < SLOTNUM; i3++)
					{
						if (((value[pos].src[i3] >> 14) == i1) && ((value[pos].dst[i3] >> 14) == i2) && ((value[pos].src[i3] & mask) == temp_fp) && ((value[pos].dst[i3] & mask) == fp_d))
						{
							delete []tmp1;
							delete []tmp2;
							return true;
						}
					}
				}
			}
			//	 find in buffer
			uint32_t temp_key = (temp_addr << fingerprintLength) + temp_fp;
			vector<vector<node> >::iterator it = find_if(successorAdjacencyList.begin(), successorAdjacencyList.end(), findv(temp_key));
			if (it != successorAdjacencyList.end())
			{
				for (vector<node>::iterator iter = it->begin(); iter != it->end(); iter++) 
				{
					if (iter->key != temp_key)
					{
						unsigned int val = iter->key;
						unsigned int temp_h = (val) >> fingerprintLength;
						unsigned int tmp = pow(2, fingerprintLength);
						unsigned short temp_g = (val % tmp);
						if ((temp_h == addr_d) && (temp_g == fp_d))
						{
							delete []tmp1;
							delete []tmp2;
							return true;
						}
			
						IT = checked.find(val);
						if (IT == checked.end())
						{
							mapnode temp_e;
							temp_e.addr = temp_h;
							temp_e.fp = temp_g;
							q.push(temp_e);
							checked[val] = true;;
						}
					}
				}
			}
			
			// find in matrix
			for (int i1 = 0; i1 < row_addrs; i1++)
			{
				int p1 = (temp_addr + tmp1[i1]) % depth;
				for (int i2 = 0; i2 < width; i2++)
				{
					int pos = p1 * width + i2;
					for (int i3 = 0; i3 < SLOTNUM; i3++)
					{
						if ((value[pos].src[i3] & mask) == temp_fp && ((value[pos].src[i3] >> 14) == i1))
						{
							uint32_t tmp_g = (value[pos].dst[i3] & mask);
							int tmp_s = value[pos].dst[i3] >> 14;
				
							uint32_t shifter = tmp_g;
							for (int v = 0; v < tmp_s; v++)
								shifter = (shifter * multiplier + increment) % modulus;
							uint32_t tmp_h = i2;
							while (tmp_h < shifter)
								tmp_h += width;				/////////
							tmp_h -= shifter;

							uint32_t val = (tmp_h << fingerprintLength) + tmp_g;
						
							IT = checked.find(val);
							if (IT == checked.end())
							{
								mapnode tmp_e;
								tmp_e.addr = tmp_h;
								tmp_e.fp = tmp_g;
								q.push(tmp_e);
								checked[val] = true;
							}
						}
					}
				}
			}
			delete[] tmp1;
			delete[] tmp2;
			q.pop();
		}
	}
	return false;
}

bool TreeNode::insertMatrix(uint32_t addr_src, uint16_t fp_src, uint32_t addr_dst, uint16_t fp_dst, weight_type weight) {
	uint32_t mask = (1 << fingerprintLength) - 1;
	uint32_t head = 16384; //pow(2, 14);

	// Alternative address -- row * column
	uint32_t* seed1 = new uint32_t[row_addrs];			// row address seeds
	uint32_t* seed2 = new uint32_t[column_addrs];		// column address seeds
	seed1[0] = fp_src;
	seed2[0] = fp_dst;
	for (int i = 1; i < row_addrs; i++)	
		seed1[i] =  (seed1[i - 1] * multiplier + increment) % modulus;
	for (int i = 1; i < column_addrs; i++)	
		seed2[i] =  (seed2[i - 1] * multiplier + increment) % modulus;

	for (int i = 0; i < row_addrs; i++) {
		uint32_t row_addr = (addr_src + seed1[i]) % depth;
		for (int j = 0; j < column_addrs; j++) {
			uint32_t column_addr = (addr_dst + seed2[j]) % width;
			uint32_t pos = row_addr * width + column_addr;
			for (int m = 0; m < SLOTNUM; m++) {
				if (((value[pos].src[m] >> 14) == i) && ((value[pos].dst[m] >> 14) == j) && ((value[pos].src[m] & mask) == fp_src) && ((value[pos].dst[m] & mask) == fp_dst)) {
					value[pos].weight[m] += weight;
					delete[] seed1;
					delete[] seed2;
					return true;
				}
				if (value[pos].src[m] == 0 && value[pos].weight[m] == 0) {
					value[pos].src[m] = fp_src + head * i;
					value[pos].dst[m] = fp_dst + head * j;
					value[pos].weight[m] = weight;
					delete[] seed1;
					delete[] seed2;
					return true;
				}
			}
		}
	}
	delete[] seed1;
	delete[] seed2;
	return false;
}
bool TreeNode::kickElement(uint32_t& addr_src, uint16_t& fp_src, uint32_t& addr_dst, uint16_t& fp_dst, weight_type& weight) {
	int kickNum = 0;
	
	uint32_t mask = (1 << fingerprintLength) - 1;
	uint32_t head = 16384; //pow(2, 14);

	uint32_t* curtmp1 = new uint32_t[row_addrs];			// row address seeds
	uint32_t* curtmp2 = new uint32_t[column_addrs];			// column address seeds

	int kickflag = 1;
	while (true){
		// 找到当前边(e1)的第kickflag个地址，然后判断两个slot中序号较小的那个剔出
		// kickout the elements of the first pos(the power of two choices)
		uint32_t pos_x = (addr_src + fp_src) % depth; 
		uint32_t pos_y;
		if (kickflag == 1) {
			pos_y = (addr_dst + fp_dst) % width;
		}
		else if (kickflag == 2) {
			uint32_t tmp2 = (fp_dst * multiplier + increment) % modulus;
			pos_y = (fp_dst + tmp2) % width;

		}
		uint32_t pos = pos_x * width + pos_y;
		uint32_t insertnum[SLOTNUM];
		for (int i = 0; i < SLOTNUM; i++){
			insertnum[i] = (value[pos].src[i] >> 14) * 4 + (value[pos].dst[i] >> 14);
		}
		int kickindex = getMinIndex(insertnum, SLOTNUM);					// 找到要踢的那个slot
		uint32_t kick_i = (value[pos].src[kickindex] >> 14);				// 将要踢出的边(e2)信息存储到kick_变量中
		uint32_t kick_j = (value[pos].dst[kickindex] >> 14);
		uint16_t kick_fpx = (value[pos].src[kickindex] & mask);
		uint16_t kick_fpy = (value[pos].dst[kickindex] & mask);
		weight_type kick_weight = value[pos].weight[kickindex];
		// insert the element
		value[pos].src[kickindex] = fp_src;
		if(kickflag == 1) {
			value[pos].dst[kickindex] = fp_dst;
		}
		else if (kickflag == 2) {
			value[pos].dst[kickindex] = fp_dst + head;
		}
		value[pos].weight[kickindex] = weight;

		// 此时为边e2
		fp_src = kick_fpx;
		fp_dst = kick_fpy;
		weight = kick_weight;
		
		// 根据e2存储在矩阵中的当前位置以及下标信息还原边e2的addr1、addr2
		uint32_t shifterx = fp_src;
		for (int v = 0; v < kick_i; v++) {
			shifterx = (shifterx * multiplier + increment) % modulus;
		}
		uint32_t tmp_h1 = pos_x;
		while (tmp_h1 < shifterx)
			tmp_h1 += depth;
		tmp_h1 -= shifterx;
		addr_src = tmp_h1;

		uint32_t shiftery = fp_dst;
		for (int v = 0; v < kick_j; v++)
			shiftery = (shiftery * multiplier + increment) % modulus;
		uint32_t tmp_h2 = pos_y;
		while (tmp_h2 < shiftery)
			tmp_h2 += width;
		tmp_h2 -= shiftery;
		addr_dst = tmp_h2;

		////////
		curtmp1[0] = fp_src;
		curtmp2[0] = fp_dst;
		for (int i = 1; i < row_addrs; i++)	
			curtmp1[i] =  (curtmp1[i - 1] * multiplier + increment) % modulus;
		for (int i = 1; i < column_addrs; i++)	
			curtmp2[i] =  (curtmp2[i - 1] * multiplier + increment) % modulus;
		
		uint32_t addr_no = (kick_i * 4) + (kick_j);

		for (int i = 0; i < row_addrs; i++) {
			uint32_t depth_addr = (addr_src + curtmp1[i]) % depth;
			for (int j = 0; j < column_addrs; j++) {
				if((i * 4 + j) <= addr_no)
					continue;
				int width_addr = (addr_dst + curtmp2[j]) % width;
				int pos = depth_addr * width + width_addr;
				
				for (int m = 0; m < SLOTNUM; m++) {
					if (value[pos].src[m] == 0 && value[pos].weight[m] == 0) {
						value[pos].src[m] = fp_src + head * i;
						value[pos].dst[m] = fp_dst + head * j;
						value[pos].weight[m] = weight;
						delete[] curtmp1;
						delete[] curtmp2;
						return true;
					}
				}
				
			}
		}
		if(addr_no == 0) {
			kickflag = 2;
		}
		else {
			kickflag = 1;
		} //avoid kick-loop
		kickNum++;
		if(kickNum > 10){
			break;
		}
	}
	delete[] curtmp1;
	delete[] curtmp2;
	return false;
}
weight_type TreeNode::edgeQueryMatrix(uint32_t addr_src, uint16_t fp_src, uint32_t addr_dst, uint16_t fp_dst) {
	uint32_t mask = (1 << fingerprintLength) - 1;
	// Alternative address -- row * column
	uint32_t* seed1 = new uint32_t[row_addrs];			// row address seeds
	uint32_t* seed2 = new uint32_t[column_addrs];		// column address seeds
	seed1[0] = fp_src;
	seed2[0] = fp_dst;
	for (int i = 1; i < row_addrs; i++)	
		seed1[i] =  (seed1[i - 1] * multiplier + increment) % modulus;
	for (int i = 1; i < column_addrs; i++)	
		seed2[i] =  (seed2[i - 1] * multiplier + increment) % modulus;

	for (int i = 0; i < row_addrs; i++) {
		uint32_t row_addr = (addr_src + seed1[i]) % depth;
		for (int j = 0; j < column_addrs; j++) {
			uint32_t column_addr = (addr_dst + seed2[j]) % width;
			uint32_t pos = row_addr * width + column_addr;
			if(pos >= width * depth || pos < 0) {
				cout << "matrix pos: " << pos << " out of range!" << endl;
				continue;
			}
			for (int m = 0; m < SLOTNUM; m++) {
				if (((value[pos].src[m] >> 14) == i) && ((value[pos].dst[m] >> 14) == j) && ((value[pos].src[m] & mask) == fp_src) && ((value[pos].dst[m] & mask) == fp_dst)) {
					delete[] seed1;
					delete[] seed2;
					return value[pos].weight[m];
				}
			}
		}
	}
	delete[] seed1;
	delete[] seed2;
	return 0;
}
weight_type TreeNode::nodeQueryMatrix(uint32_t addr_v, uint16_t fp_v, int type) {
	weight_type weight = 0;
	uint32_t mask = pow(2, fingerprintLength) - 1;
	int addrs = (type == 0) ? row_addrs : column_addrs;
	// Alternative address
	uint32_t* seeds = new uint32_t[addrs];			// address seeds
	seeds[0] = fp_v;
	for (int i = 1; i < addrs; i++)	
		seeds[i] =  (seeds[i - 1] * multiplier + increment) % modulus;
	
	if (type == 0) {
		for (int i = 0; i < row_addrs; i++)	{
			uint32_t row_addr = (addr_v + seeds[i]) % depth;
			for (int k = 0; k < width; k++)	{
				uint32_t pos = row_addr * width + k;
				for (int j = 0; j < SLOTNUM; ++j) {
					if (((value[pos].src[j] >> 14) == i) && ((value[pos].src[j] & mask) == fp_v)) {
						weight += value[pos].weight[j];
					}
				}
			}	
		}
	}
	else if (type == 1) {
		for (int i = 0; i < column_addrs; i++) {
			uint32_t col_addr = (addr_v + seeds[i]) % width;
			for (int k = 0; k < depth; k++) {
				uint32_t pos = k * width + col_addr;
				for (int j = 0; j < SLOTNUM; ++j) {
					if (((value[pos].dst[j] >> 14) == i) && ((value[pos].dst[j] & mask) == fp_v)) {
						weight += value[pos].weight[j];
					}
				}
			}
		}
	}
	delete[] seeds;
	return weight;
}

bool TreeNode::insertMatrixCacheline(uint32_t addr_src, uint16_t fp_src, uint32_t addr_dst, uint16_t fp_dst, weight_type weight) {
	uint32_t mask = (1 << fingerprintLength) - 1;
	uint32_t head = 16384; //pow(2, 14);

	// Alternative address -- row * (column / 2) * 2 cache
	// the column_addrs must be even
	uint32_t* seed1 = new uint32_t[row_addrs];				// row address seeds
	uint32_t* seed2 = new uint32_t[column_addrs / 2];		// column address seeds
	seed1[0] = fp_src;
	seed2[0] = fp_dst;
	for (int i = 1; i < row_addrs; i++)	
		seed1[i] =  (seed1[i - 1] * multiplier + increment) % modulus;
	for (int i = 1; i < column_addrs / 2; i++)	
		seed2[i] =  (seed2[i - 1] * multiplier + increment) % modulus;

	for (int i = 0; i < row_addrs; i++) {
		uint32_t row_addr = (addr_src + seed1[i]) % depth;
		for (int j = 0; j < column_addrs / 2; j++) {
			uint32_t column_addr = (addr_dst + seed2[j]) % width;
			uint32_t column_addr_alt;
			for (int k = 0; k < 2; k++) {
				uint32_t pos;
				if (k == 0) {
					pos = row_addr * width + column_addr;
				}
				else {
					column_addr_alt = (column_addr ^ (fp_dst % CACHESLOT)) % width;
					pos = row_addr * width + column_addr_alt;
				}
				for (int m = 0; m < SLOTNUM; m++) {
					if (((value[pos].src[m] >> 14) == i) && ((value[pos].dst[m] >> 14) == (j * 2 + k)) && ((value[pos].src[m] & mask) == fp_src) && ((value[pos].dst[m] & mask) == fp_dst)) {
						value[pos].weight[m] += weight;
						delete[] seed1;
						delete[] seed2;
						return true;
					}
					if (value[pos].src[m] == 0 && value[pos].weight[m] == 0) {
						value[pos].src[m] = fp_src + head * i;
						value[pos].dst[m] = fp_dst + head * (j * 2 + k);
						value[pos].weight[m] = weight;
						delete[] seed1;
						delete[] seed2;
						return true;
					}
				}
			}
		}
	}
	delete[] seed1;
	delete[] seed2;
	return false;
}
bool TreeNode::kickElementCacheline(uint32_t& addr_src, uint16_t& fp_src, uint32_t& addr_dst, uint16_t& fp_dst, weight_type& weight) {
	int kickNum = 0;

	uint32_t mask = (1 << fingerprintLength) - 1;
	uint32_t head = 16384; //pow(2, 14);
	
	uint32_t* curtmp1 = new uint32_t[row_addrs];			// row address seeds
	uint32_t* curtmp2 = new uint32_t[column_addrs / 2];			// column address seeds

	int kickflag = 1;
	while (true){
		// 找到当前边(e1)的第kickflag个地址，然后判断两个slot中序号较小的那个剔出
		// kickout the elements of first pos(the power of two choices)
		uint32_t pos_x = (addr_src + fp_src) % depth; 
		uint32_t pos_y = (addr_dst + fp_dst) % width;
		if(kickflag == 2) {
			pos_y =  pos_y ^ (fp_dst % CACHESLOT) % width;
		}
		uint32_t pos = pos_x * width + pos_y;
		uint32_t insertnum[SLOTNUM];
		for (int i = 0; i < SLOTNUM; i++){
			insertnum[i] = (value[pos].src[i] >> 14) * 4 + (value[pos].dst[i] >> 14);
		}
		int kickindex = getMinIndex(insertnum, SLOTNUM);					// 找到要踢的那个slot
		uint32_t kick_i = (value[pos].src[kickindex] >> 14);				// 将要踢出的边(e2)信息存储到kick_变量中
		uint32_t kick_j = (value[pos].dst[kickindex] >> 14);
		uint16_t kick_fpx = (value[pos].src[kickindex] & mask);
		uint16_t kick_fpy = (value[pos].dst[kickindex] & mask);
		weight_type kick_weight = value[pos].weight[kickindex];
		// insert the element
		value[pos].src[kickindex] = fp_src;
		if (kickflag == 1) {
			value[pos].dst[kickindex] = fp_dst;
		}
		else if (kickflag == 2) {
			value[pos].dst[kickindex] = fp_dst + head;
		}
		value[pos].weight[kickindex] = weight;
		
		// 此时为边e2
		fp_src = kick_fpx;
		fp_dst = kick_fpy;
		weight = kick_weight;
		
		//根据e2存储在矩阵中的当前位置以及下标信息还原边e2的addr1、addr2
		uint32_t shifterx = fp_src;
		for (int v = 0; v < kick_i; v++) {
			shifterx = (shifterx * multiplier + increment) % modulus;
		}
		uint32_t tmp_h1 = pos_x;
		while (tmp_h1 < shifterx)
			tmp_h1 += depth;
		tmp_h1 -= shifterx;
		addr_src = tmp_h1;

		uint32_t shiftery = fp_dst;
		for (int v = 0; v < (kick_j / 2); v++)
			shiftery = (shiftery * multiplier + increment) % modulus;
		uint32_t tmp_h2 = (kick_j % 2 == 0) ? pos_y : (pos_y ^ (fp_dst % CACHESLOT) % width);
		while (tmp_h2 < shiftery)
			tmp_h2 += width;
		tmp_h2 -= shiftery;
		addr_dst = tmp_h2;

		curtmp1[0] = fp_src;
		curtmp2[0] = fp_dst;
		for (int i = 1; i < row_addrs; i++)	
			curtmp1[i] =  (curtmp1[i - 1] * multiplier + increment) % modulus;
		for (int i = 1; i < column_addrs / 2; i++)	
			curtmp2[i] =  (curtmp2[i - 1] * multiplier + increment) % modulus;
		
		uint32_t addr_no = (kick_i * 4) + (kick_j);

		for (int i = 0; i < row_addrs; i++) {
			uint32_t row_addr = (addr_src + curtmp1[i]) % depth;
			for (int j = 0; j < column_addrs / 2; j++) {
				uint32_t column_addr = (addr_dst + curtmp2[j]) % width;
				for (int k = 0; k < 2; k++) {
					if((i * 4 + j * 2 + k) <= addr_no)
						continue;
					uint32_t pos;
					if (k == 0) {
						pos = row_addr * width + column_addr;
					}
					else {
						uint32_t column_addr_alt = (column_addr ^ (fp_dst % CACHESLOT)) % width;
						pos = row_addr * width + column_addr_alt;
					}
					for (int m = 0; m < SLOTNUM; m++) {
						if (value[pos].src[m] == 0 && value[pos].weight[m] == 0) {
							value[pos].src[m] = fp_src + head * i;
							value[pos].dst[m] = fp_dst + head * (j * 2 + k);
							value[pos].weight[m] = weight;
							delete[] curtmp1;
							delete[] curtmp2;
							return true;
						}
					}
				}
			}
		}
		if(addr_no == 0){
			kickflag = 2;
		}
		else{
			kickflag = 1;
		}	//avoid kick-loop
		kickNum++;
		if(kickNum > 10) {
			break;
		}
	}
	delete[] curtmp1;
	delete[] curtmp2;
	return false;
}
weight_type TreeNode::edgeQueryMatrixCacheline(uint32_t addr_src, uint16_t fp_src, uint32_t addr_dst, uint16_t fp_dst) {
	uint32_t mask = (1 << fingerprintLength) - 1;

	// Alternative address -- row * (column / 2) * 2 cache
	// the column_addrs must be even
	uint32_t* seed1 = new uint32_t[row_addrs];			// row address seeds
	uint32_t* seed2 = new uint32_t[column_addrs / 2];		// column address seeds
	seed1[0] = fp_src;
	seed2[0] = fp_dst;
	for (int i = 1; i < row_addrs; i++)	
		seed1[i] =  (seed1[i - 1] * multiplier + increment) % modulus;
	for (int i = 1; i < column_addrs / 2; i++)	
		seed2[i] =  (seed2[i - 1] * multiplier + increment) % modulus;

	for (int i = 0; i < row_addrs; i++) {
		uint32_t row_addr = (addr_src + seed1[i]) % depth;
		for (int l = 0; l < column_addrs / 2; l++) {
			uint32_t column_addr = (addr_dst + seed2[l]) % width;
			for (int k = 0; k < 2; k++) {
				uint32_t pos;
				if (k == 0) {
					pos = row_addr * width + column_addr;
				}
				else {
					uint32_t column_addr_alt = (column_addr ^ (fp_dst % CACHESLOT)) % width;
					pos = row_addr * width + column_addr_alt;
				}
				if(pos >= width * depth || pos < 0) {
					cout << pos << " out of range!" << endl;
					continue;
				}
				for (int j = 0; j < SLOTNUM; j++) {
					if (((value[pos].src[j] >> 14) == i) && ((value[pos].dst[j] >> 14) == (l*2+k)) && ((value[pos].src[j] & mask) == fp_src) && ((value[pos].dst[j] & mask) == fp_dst)) {
						delete[] seed1;
						delete[] seed2;
						return value[pos].weight[j];
					}
				}
			}
		}
	}
	delete[] seed1;
	delete[] seed2;
	return 0;
}
weight_type TreeNode::nodeQueryMatrixCacheline(uint32_t addr_v, uint16_t fp_v, int type) {
	weight_type weight = 0;
	uint32_t mask = pow(2, fingerprintLength) - 1;
	int addrs = (type == 0) ? row_addrs : column_addrs / 2;
	// Alternative address
	uint32_t* seeds = new uint32_t[addrs];			// address seeds
	seeds[0] = fp_v;
	for (int i = 1; i < addrs; i++)	
		seeds[i] =  (seeds[i - 1] * multiplier + increment) % modulus;
	
	if (type == 0) {
		for (int i = 0; i < row_addrs; i++)	{
			uint32_t row_addr = (addr_v + seeds[i]) % depth;
			for (int k = 0; k < width; k++)	{
				uint32_t pos = row_addr * width + k;
				for (int j = 0; j < SLOTNUM; ++j) {
					if (((value[pos].src[j] >> 14) == i) && ((value[pos].src[j] & mask) == fp_v)) {
						weight += value[pos].weight[j];
					}
				}
			}	
		}
	}
	else if (type == 1) {
		// uint32_t py[4];
		// uint32_t addr = (hash_vertex >> fingerprintLength) % width;
		// py[0] = (addr + seed1[0]) % width;
		// py[1] = py[0] ^ (fp % 4) % width;
		// py[2] = (addr + seed1[1]) % width;
		// py[3] = py[2] ^ (fp % 4) % width;
		uint32_t* py = new uint32_t[column_addrs];
		for (int i = 0; i < column_addrs; i++) {
			if (i % 2 == 0) {
				py[i] = (addr_v + seeds[i / 2]) % width;
			}
			else {
				py[i] = py[i - 1] ^ (fp_v % CACHESLOT) % width;
			}
		}

		for (int k = 0; k < depth; k++) {
			for (int i = 0; i < column_addrs; i++) {
				uint32_t pos = k * width + py[i];
				for (int j = 0; j < SLOTNUM; ++j) {
					if (((value[pos].dst[j] >> 14) == i) && ((value[pos].dst[j] & mask) == fp_v)) {
						weight += value[pos].weight[j];
					}
				}
			}
		}
		delete[] py;
	}
	delete[] seeds;
	return weight;
}

void TreeNode::insertSucBuffer(uint32_t k1, uint32_t k2, weight_type weight) {
	// save to successor buffer k1->k2
	// map<uint32_t, uint32_t>::iterator it = successorIndex.find(k1);
	vector<vector<node> >::iterator it = find_if(successorAdjacencyList.begin(), successorAdjacencyList.end(), findv(k1));
	if (it != successorAdjacencyList.end()) {		// find k1->k2 in successorAdjacencyList
		// uint32_t tag = it->second;
		// vector<node>::iterator iter = find_if(successorAdjacencyList[tag].begin(), successorAdjacencyList[tag].end(), findx(k2));
		vector<node>::iterator iter = find_if(it->begin(), it->end(), findx(k2));
		if (iter != it->end()) {		// if found
			iter->weight += weight;
		}
		else {
			node tmpnode;
			tmpnode.key = k2;
			tmpnode.weight = weight;
			it->push_back(tmpnode);
		}
	}
	else {
		// successorIndex[k1] = n1;
		// n1++;
		node tmpnode;
		tmpnode.key = k1;
		tmpnode.weight = 0;
		vector<node> vc;
		vc.push_back(tmpnode);
		if (k1 != k2) {  //k1==k2 means loop
			node newnode;
			newnode.key = k2;
			newnode.weight = weight;
			vc.push_back(newnode);
		}
		else {
			vc[0].weight += weight;
		}
		successorAdjacencyList.push_back(vc);
	}
	// save to precursor buffer k2<-k1
	// map<uint32_t, uint32_t>::iterator it2 = precursorIndex.find(k2);
	// vector<node>::iterator it2 = find_if(precursorAdjacencyList.begin(), precursorAdjacencyList.end(), findx(k2));
	// if (it2 != precursorAdjacencyList.end()) {		// find k2->k1 in precursorAdjacencyList
	// 	it2->weight += weight;
	// }
	// else {
	// 	// precursorIndex[k2] = n2;
	// 	// n2++;
	// 	node newnode;
	// 	newnode.key = k2;
	// 	newnode.weight = weight;
	// 	precursorAdjacencyList.push_back(newnode);
	// }
}
weight_type TreeNode::edgeQuerySucBuffer(uint32_t k1, uint32_t k2) {
	// query the suc-buffer
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
weight_type TreeNode::nodeQuerySucBuffer(uint32_t k, int type) {
	weight_type weight = 0;
	if (type == 0) {
		// map<uint32_t, uint32_t>::iterator it = successorIndex.find(k1);
		vector<vector<node> >::iterator it = find_if(successorAdjacencyList.begin(), successorAdjacencyList.end(), findv(k));
		if (it != successorAdjacencyList.end())	{
			vector<node>::iterator iter;
			for (iter = it->begin(); iter != it->end(); iter++) {
				weight += iter->weight;
			}
		}
	}
	else if (type == 1) {
		// map<uint32_t, uint32_t>::iterator it = precursorIndex.find(k1);
		// vector<node>::iterator it = find_if(precursorAdjacencyList.begin(), precursorAdjacencyList.end(), findx(k1));
		// if (it != precursorAdjacencyList.end()) {
		// 		weight += it->weight;
		// }
		for(vector<vector<node> >::iterator it = successorAdjacencyList.begin(); it != successorAdjacencyList.end(); it++){
			for(vector<node>::iterator iter = it->begin(); iter!= it->end(); iter++){
				if(iter->key == k){
					weight += iter->weight;
				}
			}
		}
	}
	return weight;
}

#endif // _TreeNode_H