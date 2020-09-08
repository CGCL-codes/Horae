#pragma once
#include "LAYER.h"
#include <mutex>

typedef unsigned int time_type;

struct TimeInterval {
    time_type start;
    time_type end;
};


class Horae {
private:
	struct window {
		int level;
		int number;
	};
	unsigned int timeslice_len;
	unsigned int width;
	unsigned int depth;
	time_type start_time;
	//mutex m;
//public:
	vector<Layer*> multi_layers;
	//Layer* setd[13];
private:
	int decompose(int L, int point, int type, window win[], int curlayer);
public:
	Horae(time_type startTime, int timesliceLength, unsigned int gl, unsigned int width, unsigned int depth, int fingerprintLength);
	~Horae();
	void insert(unsigned int s, unsigned int d, weight_type w, time_type t);
	void levelInsert(int level, unsigned int s, unsigned int d, weight_type w, time_type t);
	unsigned int edgeQuery(unsigned int s, unsigned int d, unsigned int start, unsigned int end);
	unsigned int nodeQuery(unsigned int v, int type, unsigned int start, unsigned int end);
	void setStartTime(time_type startTime);
	time_type getStartTime();
	void setTimesliceLength(int timesliceLength);
	int getTimesliceLength();


	Layer* getLayer(int i);	
	void addLayer(Layer *layer);
	int getMultilayerSize();
};
Layer* Horae::getLayer(int i) {
	if(i >= this->multi_layers.size()) {
		cout << "The index of the multi-layers is out of range!" << endl;
		exit(-1);
	}
	if(this->multi_layers[i] == NULL) {
		cout << "你丫个锤子" << endl;
		getchar();
		exit(-1);
	}
	return this->multi_layers[i];
}

int Horae::getMultilayerSize() {
	return this->multi_layers.size();
}
void Horae::addLayer(Layer *layer) {
	this->multi_layers.push_back(layer);
	printf("mul-layer p = %p\n", &this->multi_layers);
	//for(int i = 0; i < this->multi_layers.size(); i++) {
	//	printf("mul-layer %d, p = %p\n", i, this->getLayer(i));
	//}
}
Horae::Horae(time_type startTime, int timesliceLength, unsigned int gl, unsigned int width, unsigned int depth, int fingerprintLength) {
	this->start_time = startTime;	
	this->timeslice_len = timesliceLength;
	this->width = width;
	this->depth = depth;
	Layer *layer = new Layer(gl, width, depth, fingerprintLength);
	printf("level = 0, layer = %p\n", layer);
	if(layer == NULL) {
		cout << "ERROR NULL Pointer!" << endl;
		getchar();
		exit(-1);
	}
	multi_layers.reserve(100);
	multi_layers.push_back(layer);
}

Horae::~Horae() {
	for (vector<Layer*>::iterator it = multi_layers.begin(); it != multi_layers.end(); it++) {
		delete* it;
	}
	vector<Layer*>().swap(multi_layers);
}

void Horae::insert(unsigned int s, unsigned int d, weight_type w, time_type t) {
	unsigned int tt = ceil((double)(t - start_time) / (double)timeslice_len);
	if (tt > multi_layers[multi_layers.size() - 1]->getGranularity()) {
		Layer *layer = new Layer(*multi_layers[multi_layers.size() - 1]);
		this->multi_layers.push_back(layer);
	}
	for (unsigned int i = 0; i < multi_layers.size(); i++) {
		string sv = to_string(s) + "+" + to_string((unsigned int)ceil((double)tt / (double)multi_layers[i]->getGranularity()));
		string dv = to_string(d) + "+" + to_string((unsigned int)ceil((double)tt / (double)multi_layers[i]->getGranularity()));
		this->multi_layers[i]->insert(sv, dv, w);
	}
}

void Horae::levelInsert(int level, unsigned int s, unsigned int d, weight_type w, time_type t){
	Layer *layer = this->getLayer(level);
	unsigned int tg = layer->getGranularity();
	unsigned int tt = ceil((double)(t - start_time) / (double)timeslice_len);
	string sv = to_string(s) + "+" + to_string((unsigned int)ceil((double)tt / (double)tg));
	string dv = to_string(d) + "+" + to_string((unsigned int)ceil((double)tt / (double)tg));
	layer->insert(sv, dv, w);
	
	//string sv = to_string(s) + "+" + to_string((unsigned int)ceil((double)tt / (double)multi_layers[level]->getGranularity()));
	//string dv = to_string(d) + "+" + to_string((unsigned int)ceil((double)tt / (double)multi_layers[level]->getGranularity()));
	//m.lock();
	//this->multi_layers[level]->insert(sv, dv, w);
	//m.unlock();
}

void Horae::setStartTime(time_type startTime) {
	this->start_time = startTime;
}

unsigned int Horae::getStartTime() {
	return this->start_time;
}

void Horae::setTimesliceLength(int timesliceLength) {
	this->timeslice_len = timesliceLength;
}

int Horae::getTimesliceLength() {
	return this->timeslice_len;
}

int Horae::decompose(int L, int point, int type, window win[], int curlayer) {
	int i, y;
	int j = 0;

	for (i = 0; i <= curlayer; ++i) {
		int tmp = ((L >> i) & 0x1) << i;
		if (tmp != 0) {
			win[j].level = i + 1;
			if (type == 0) {//left alignment
				win[j].number = point / pow(2, (win[j].level - 1));
				point -= tmp;
			}
			else if (type == 1)//right alignment
			{
				if (j == 0) {
					point += tmp - 1;
					win[j].number = point / pow(2, (win[j].level - 1));
				}
				else {
					point += tmp;
					win[j].number = point / pow(2, (win[j].level - 1));
				}
				
			}
			j++;
		}
	}
	return j;
}

unsigned int Horae::edgeQuery(unsigned int s, unsigned int d, unsigned int start, unsigned int end) {
	unsigned int result = 0;
	int length = end - start + 1;
	int level = 0;
	while (length) {
		length = length >> 1;
		level++;
	}
	int gl = (1 << (level - 1));
	//cout << endl;
	//vector<window> vc;

	window *win = new window[level];

	if (start % gl == 1 && end % gl == 0) {
		unsigned int win_number = end / pow(2, level - 1);
		string v1 = to_string(s) + "+" + to_string(win_number);
		string v2 = to_string(d) + "+" + to_string(win_number);
		result += multi_layers[level - 1]->edgeQuery(v1, v2);
		delete[] win;
		return result;
	}

	if (start % gl == 1)//left alignment
	{
		int n = decompose(end - start + 1, end, 0, win, level - 1);
		for (int i = 0; i < n; i++) {
			string v1 = to_string(s) + "+" + to_string(win[i].number);
			string v2 = to_string(d) + "+" + to_string(win[i].number);
			result += multi_layers[win[i].level - 1]->edgeQuery(v1, v2);
		}
		delete[] win;
		return result;
	}

	if (end % gl == 0)//right alignment
	{
		int n = decompose(end - start + 1, start, 1, win, level - 1);
		for (int i = 0; i < n; i++) {
			string v1 = to_string(s) + "+" + to_string(win[i].number);
			string v2 = to_string(d) + "+" + to_string(win[i].number);
			result += multi_layers[win[i].level - 1]->edgeQuery(v1, v2);
		}
		delete[] win;
		return result;
	}

	int breakpoint = floor(end / gl) * gl;
	int L1 = breakpoint - start + 1;
	int L2 = end - breakpoint;
	int n1 = decompose(L1, start, 1, win, level- 1);
	for (int i = 0; i < n1; i++) {
		string v1 = to_string(s) + "+" + to_string(win[i].number);
		string v2 = to_string(d) + "+" + to_string(win[i].number);
		result += multi_layers[win[i].level - 1]->edgeQuery(v1, v2);
	}
	
	window *win2 = new window[level];
	int n2 = decompose(L2, end, 0, win2, level - 1);
	for (int i = 0; i < n2; i++) {
		string v1 = to_string(s) + "+" + to_string(win2[i].number);
		string v2 = to_string(d) + "+" + to_string(win2[i].number);
		result += multi_layers[win2[i].level - 1]->edgeQuery(v1, v2);
	}
	delete[] win;
	delete[] win2;
	return result;
}

unsigned int Horae::nodeQuery(unsigned int v, int type, unsigned int start, unsigned int end) {
	unsigned int result = 0;
	int length = end - start + 1;
	int level = 0;
	while (length) {
		length = length >> 1;
		level++;
	}
	int gl = (1 << (level - 1));
	// cout << endl;
	//vector<window> vc;

	window *win = new window[level];

	if (start % gl == 1 && end % gl == 0) {
		unsigned int win_number = end / pow(2, level - 1);
		string v1 = to_string(v) + "+" + to_string(win_number);
		result += multi_layers[level - 1]->nodeQuery(v1, type);
		delete[] win;
		return result;
	}

	if (start % gl == 1) //left alignment
	{
		int n = decompose(end - start + 1, end, 0, win, level - 1);
		for (int i = 0; i < n; i++) {
			string v1 = to_string(v) + "+" + to_string(win[i].number);
			result += multi_layers[win[i].level - 1]->nodeQuery(v1, type);
		}
		delete[] win;
		return result;
	}

	if (end % gl ==0) //right alignment
	{
		int n = decompose(end - start + 1, start, 1, win, level - 1);
		for (int i = 0; i < n; i++) {
			string v1 = to_string(v) + "+" + to_string(win[i].number);
			result += multi_layers[win[i].level - 1]->nodeQuery(v1, type);
		}
		delete[] win;
		return result;
	}

	int breakpoint = floor(end / gl) * gl;
	int L1 = breakpoint - start + 1;
	int L2 = end - breakpoint;
	int n1 = decompose(L1, start, 1, win, level - 1);
	for (int i = 0; i < n1; i++) {
		string v1 = to_string(v) + "+" + to_string(win[i].number);
		result += multi_layers[win[i].level - 1]->nodeQuery(v1, type);
	}
	
	window *win2 = new window[level];
	int n2 = decompose(L2, end, 0, win2, level - 1);
	for (int i = 0; i < n2; i++) {
		string v1 = to_string(v) + "+" + to_string(win2[i].number);
		result += multi_layers[win2[i].level - 1]->nodeQuery(v1, type);
	}
	delete[] win;
	delete[] win2;
	return result;

}