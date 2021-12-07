#pragma once
#include "HashFunction.h"
#include <math.h>
#include <string>
#include <iostream>
#include <memory.h>
#include <queue>
using namespace std;
typedef long long int64;
typedef unsigned int weight_type;

struct MatrixSize {
	unsigned int width;
	unsigned int depth;
};

class TcmTimeslice {
private:
	unsigned int width;
	unsigned int depth;
	unsigned int hashNum;
	unsigned int timeGranularity;
	MatrixSize* matrix_size;
	unsigned int** value; // the matrix

public:
	TcmTimeslice(unsigned int w, unsigned int d, unsigned int h, unsigned int tg);
	TcmTimeslice(const TcmTimeslice& tcm);
	~TcmTimeslice() {
		delete matrix_size;
		for (unsigned int i = 0; i < hashNum; ++i) {
			delete[] value[i];
		}
		delete[] value;
	}
	void setTimeGranularity(unsigned int tg) {
		timeGranularity = tg;
	}
	unsigned int getTimeGranularity() {
		return timeGranularity;
	}
	void insert(string v1, string v2, unsigned int weight);
	unsigned int edgeQuery(string v1, string v2);
	unsigned int nodeQuery(string v1, int type);
	bool reachabilityQuery(string v1, string v2);
	unsigned int getMaxValue(unsigned int hashnumber);
	unsigned int bucketCounting(unsigned int hashnumber);
	void spaceUsage();
	unsigned int getMatrixMaxValue();
};

bool TcmTimeslice::reachabilityQuery(string v1, string v2) {
	for (unsigned int i = 0; i < hashNum; i++) {
		bool found = false;
		bool* checked = new bool[width];
		for (unsigned int j = 0; j < width; j++)
			checked[j] = false;
		unsigned int src = ((*hfunc[i])((unsigned char*)(v1.c_str()), v1.length())) % depth;
		unsigned int dest = ((*hfunc[i])((unsigned char*)(v2.c_str()), v2.length())) % width;
		queue<int> q;
		unsigned int v1 = src;
		unsigned int v2 = dest;
		unsigned int hash = v1 * width + v2;
		if (value[i][hash] > 0)//
			continue;
		else {
			q.push(v1);
			checked[v1] = true;
			while (!q.empty()) {
				v1 = q.front();
				if (value[i][v1 * width + dest] > 0) {
					// cout << "found" << endl;
					found = true;
					break;
				}
				for (unsigned int j = 0; j < width; j++) {
					hash = v1 * width + j;
					if (value[i][hash] > 0 && checked[j] == false) {
						q.push(j);
						// cout << "in!";
						checked[j] = true;
					}
				}
				q.pop();
			}
			if (!found)
				return false;
		}
		delete[] checked;
	}
	return true;
}

TcmTimeslice::TcmTimeslice(unsigned int w, unsigned int d, unsigned int h, unsigned int tg) {
	/*********** initial varibles *************/
	width = w;
	depth = d;
	hashNum = h;
	timeGranularity = tg;
	matrix_size = new MatrixSize[hashNum];
	value = new unsigned int* [hashNum];
	MatrixSize data[8] = { {width, depth}, {width, depth}, {width, depth}, {width, depth},
						   {width, depth}, {width, depth}, {width, depth}, {width, depth} };
	for (unsigned int i = 0; i < hashNum; i++) {
		matrix_size[i].depth = data[i].depth;
		matrix_size[i].width = data[i].width;
	}
	for (unsigned int i = 0; i < hashNum; ++i) {
		unsigned int size = matrix_size->depth * matrix_size->width;
		value[i] = new unsigned int[size];
		memset(value[i], 0, sizeof(unsigned int) * size);
	}

	for (unsigned int i = 0; i < hashNum; i++) {
		cout << "TCM-Hash" << i << " size : " << matrix_size[i].width << " * " << matrix_size[i].depth << endl;
	}
}

TcmTimeslice::TcmTimeslice(const TcmTimeslice& tcm) {
	width = tcm.width;
	depth = tcm.depth;
	hashNum = tcm.hashNum;
	timeGranularity = tcm.timeGranularity;
	matrix_size = new MatrixSize[hashNum];
	value = new unsigned int* [hashNum];

	memcpy(this->matrix_size, tcm.matrix_size, sizeof(MatrixSize) * hashNum);
	for (unsigned int i = 0; i < hashNum; ++i) {
		unsigned int size = matrix_size[i].depth * matrix_size[i].width;
		value[i] = new unsigned int[size];
		memset(value[i], 0, sizeof(unsigned int) * size);
		memcpy(value[i], tcm.value[i], sizeof(unsigned int) * size);
	}
}

unsigned int TcmTimeslice::getMaxValue(unsigned int hashnumber) {
	unsigned int max = 0;
	for (unsigned int i = 0; i < matrix_size[hashnumber].width * matrix_size[hashnumber].depth; i++) {
		if (value[hashnumber][i] > max) {
			max = value[hashnumber][i];
		}
	}
	return max;
}

unsigned int TcmTimeslice::bucketCounting(unsigned int hashnumber) {
	unsigned int count = 0;
	for (unsigned int i = 0; i < matrix_size[hashnumber].width * matrix_size[hashnumber].depth; i++) {
		if (value[hashnumber][i] != 0) {
			count++;
		}
	}
	return count;
}

void TcmTimeslice::spaceUsage() {
	for (unsigned int i = 0; i < hashNum; i++) {
		cout << "TCM " << i << " space usage is " <<
			(double)bucketCounting(i) / (double)(matrix_size[i].depth * matrix_size[i].width) * 100 << "%" << endl;
		cout << "TCM " << i << " max value is " << getMaxValue(i) << endl << endl;
	}
}

unsigned int TcmTimeslice::getMatrixMaxValue() {
	unsigned int max = 0;
	for (unsigned int i = 0; i < this->hashNum; i++) {
		unsigned int value = getMaxValue(i);
		if (value > max)
			max = value;
	}
	return max;
}

void TcmTimeslice::insert(string v1, string v2, unsigned int weight) {
	for (unsigned int i = 0; i < hashNum; i++) {
		unsigned int hash1 = ((*hfunc[i])((unsigned char*)(v1.c_str()), v1.length()));
		unsigned int hash2 = ((*hfunc[i])((unsigned char*)(v2.c_str()), v2.length()));

		hash1 = hash1 % (matrix_size[i].depth);
		hash2 = hash2 % (matrix_size[i].width);
		value[i][hash1 * (matrix_size[i].width) + hash2] += weight;
	}
}

unsigned int TcmTimeslice::edgeQuery(string v1, string v2) {
	unsigned int min = UINT32_MAX;
	for (unsigned int i = 0; i < hashNum; i++) {
		unsigned int hash1 = ((*hfunc[i])((unsigned char*)(v1.c_str()), v1.length())) % (matrix_size[i].depth);
		unsigned int hash2 = ((*hfunc[i])((unsigned char*)(v2.c_str()), v2.length())) % (matrix_size[i].width);
		unsigned int v = value[i][hash1 * (matrix_size[i].width) + hash2];
		if (v < min)
			min = v;
	}
	return min;
}

unsigned int TcmTimeslice::nodeQuery(string v1, int type) {
	if (type == 0) {
		unsigned int min = UINT32_MAX;
		for (unsigned int i = 0; i < hashNum; i++) {
			unsigned int hash1 = ((*hfunc[i])((unsigned char*)(v1.c_str()), v1.length())) % (matrix_size[i].depth);
			hash1 = hash1 * (matrix_size[i].width);
			unsigned int sum = 0;
			for (unsigned int j = 0; j < (matrix_size[i].width); j++) {
				sum += value[i][hash1 + j];
			}
			if (sum < min)
				min = sum;
			/*if (min == 0)
				break;*/
		}
		return min;
	}
	else {
		unsigned int min = UINT32_MAX;
		for (unsigned int i = 0; i < hashNum; i++) {
			unsigned int hash1 = ((*hfunc[i])((unsigned char*)(v1.c_str()), v1.length())) % (matrix_size[i].width);
			unsigned int sum = 0;
			for (unsigned int j = 0; j < (matrix_size[i].depth); j++)
				sum += value[i][j * (matrix_size[i].width) + hash1];
			if (sum < min)
				min = sum;
			/*if (min == 0)
				break;*/
		}
		return min;
	}
}