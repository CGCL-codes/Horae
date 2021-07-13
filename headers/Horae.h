#ifndef _Horae_H
#define _Horae_H
#include "params.h"
#include "LayerHeaders.h"

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

	bool cache_align = false;
	bool kick = false;
	const uint32_t row_addrs;			// the row addrs
	const uint32_t column_addrs;		// the column_addrs

	uint32_t timeslice_len;
	uint32_t width;
	uint32_t depth;
	time_type start_time;
	vector<Layer*> multi_layers;
private:
	int decompose(int L, int point, int type, window win[], int winlen, int curlayer);
	int secondPowerDecompose(int start, int end, window win[], int level);

	// memory improvement
	int newDecompose(int L, int point, int type, vector<window>& win, int curlayer);
	int newSecondPowerDecompose(int start, int end, vector<window>& win, int level);
public:
	Horae(time_type startTime, uint32_t timesliceLength, uint32_t gl, uint32_t width, uint32_t depth, uint32_t fingerprintLength, bool cache_align, bool kick, uint32_t row_addrs = 4, uint32_t column_addrs = 4);
	~Horae();
	void insert(uint32_t s, uint32_t d, weight_type w, time_type t);
	void levelInsert(int level, uint32_t s, uint32_t d, weight_type w, time_type t);
	
	uint32_t edgeQuery(uint32_t s, uint32_t d, time_type start, time_type end);
	uint32_t nodeQuery(uint32_t v, int type, time_type start, time_type end);

	// memory improvement
	void newInsert(uint32_t s, uint32_t d, weight_type w, time_type t);
	void newLevelInsert(int level, uint32_t s, uint32_t d, weight_type w, time_type t);
	uint32_t newEdgeQuery(uint32_t s, uint32_t d, time_type start, time_type end);
	uint32_t newNodeQuery(uint32_t v, int type, time_type start, time_type end);

	void setStartTime(time_type startTime);
	time_type getStartTime();
	void setTimesliceLength(uint32_t timesliceLength);
	uint32_t getTimesliceLength();


	Layer* getLayer(int i);	
	void addLayer(Layer *layer);
	int getMultilayerSize();

	/**************** test functions ****************/
	void bucketCounting();
	/**************** test functions ****************/
};
/**************** test functions ****************/
void Horae::bucketCounting() {
	for(int i = 0; i < multi_layers.size(); i++) {
		cout << "************ layer " << i << " ************" << endl;
		multi_layers[i]->bucketCounting();
		cout << "*********************************" << endl << endl;
	}
}
/**************** test functions ****************/
Layer* Horae::getLayer(int i) {
	if(i >= this->multi_layers.size()) {
		cout << "The index " << i << " of the multi-layers is out of range!" << endl;
		exit(-1);
	}
	if(this->multi_layers[i] == NULL) {
		cout << "Error in getLayer()!" << endl;
		exit(-1);
	}
	return this->multi_layers[i];
}

void Horae::addLayer(Layer *layer) {
	this->multi_layers.push_back(layer);
#ifdef DEBUG
	printf("mul-layer p = %p\n", &this->multi_layers);
	for(int i = 0; i < this->multi_layers.size(); i++) {
		printf("mul-layer %d, p = %p\n", i, this->getLayer(i));
	}
#endif
}

int Horae::getMultilayerSize() {
	return this->multi_layers.size();
}

Horae::Horae(time_type startTime, uint32_t timesliceLength, uint32_t gl, uint32_t width, uint32_t depth, uint32_t fingerprintLength, bool cache_align, bool kick, uint32_t row_addrs, uint32_t column_addrs):
row_addrs(row_addrs), column_addrs(column_addrs), start_time(startTime), timeslice_len(timesliceLength), width(width), depth(depth), cache_align(cache_align), kick(kick)
{
	cout << "Horae::Horae(startTime: " << start_time << ", timesliceLength: " << timeslice_len << ", gl: " << gl
		<< ", width: " << width <<", depth: " << depth << ", fplen: " << fingerprintLength << ", cache_align: " << cache_align
		<< ", kick: " << kick << ", row_addrs: " << row_addrs << ", column_addrs: " << column_addrs << ")" << endl;
#if defined(DEBUG) || defined(TINSTIME)
	timeval matrix_s, matrix_e;
	gettimeofday( &matrix_s, NULL);
#endif
	// Layer *layer = new Layer(gl, width, depth, fingerprintLength, row_addrs, column_addrs);
	// Layer *layer1 = new LayerSucClass(gl, width, depth, fingerprintLength, cache_align, kick, row_addrs, column_addrs);
	// Layer *layer2 = new LayerSucPreClass(gl, width, depth, fingerprintLength, cache_align, kick, row_addrs, column_addrs);
	// Layer *layer3 = new LayerSucPreMapClass(gl, width, depth, fingerprintLength, cache_align, kick, row_addrs, column_addrs);
	Layer *layer = NULL;
#ifdef SUCPRE
		layer = new LayerSucPreClass(gl, width, depth, fingerprintLength, cache_align, kick, row_addrs, column_addrs);
#elif SUCPREMAP
		layer = new LayerSucPreMapClass(gl, width, depth, fingerprintLength, cache_align, kick, row_addrs, column_addrs);
#else
		layer = new LayerSucClass(gl, width, depth, fingerprintLength, cache_align, kick, row_addrs, column_addrs);
#endif


#if defined(DEBUG) || defined(TINSTIME)
	gettimeofday( &matrix_e, NULL);
	double matrix_time = (matrix_e.tv_sec - matrix_s.tv_sec) +  (matrix_e.tv_usec - matrix_s.tv_usec) / 1000000.0;
	cout << "Level 0, Line = 0, Matrix Time = " << matrix_time << " s" << endl;
#endif
	
#ifdef DEBUG
	printf("level = 0, layer = %p\n", layer);
#endif
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

void Horae::insert(uint32_t s, uint32_t d, weight_type w, time_type t) {
	uint32_t tt = ceil((double)(t - start_time) / (double)timeslice_len);
	if (tt > multi_layers[multi_layers.size() - 1]->getGranularity()) {
#if defined(DEBUG) || defined(TINSTIME)
		timeval matrix_s, matrix_e;
		gettimeofday( &matrix_s, NULL);
#endif
		// Layer *layer = new Layer(*multi_layers[multi_layers.size() - 1]);
		// Layer *layer = new LayerSucClass((LayerSucClass *)multi_layers[multi_layers.size() - 1]);
		Layer *layer = nullptr;
#ifdef SUCPRE
		layer = new LayerSucPreClass((LayerSucPreClass *)multi_layers[multi_layers.size() - 1]);
#elif SUCPREMAP
		layer = new LayerSucPreMapClass((LayerSucPreMapClass *)multi_layers[multi_layers.size() - 1]);
#else
		layer = new LayerSucClass((LayerSucClass *)multi_layers[multi_layers.size() - 1]);
#endif

#if defined(DEBUG) || defined(TINSTIME)
		gettimeofday( &matrix_e, NULL);
		double matrix_time = (matrix_e.tv_sec - matrix_s.tv_sec) +  (matrix_e.tv_usec - matrix_s.tv_usec) / 1000000.0;
		cout << "Level " << multi_layers.size() << ", Matrix Time = " << matrix_time << " s" << endl;
#endif	
		this->multi_layers.push_back(layer);
	}
	for (uint32_t i = 0; i < multi_layers.size(); i++) {
		string sv = to_string(s) + "+" + to_string((uint32_t)ceil((double)tt / (double)multi_layers[i]->getGranularity()));
		string dv = to_string(d) + "+" + to_string((uint32_t)ceil((double)tt / (double)multi_layers[i]->getGranularity()));
		this->multi_layers[i]->insert(sv, dv, w);
	}
}

void Horae::levelInsert(int level, uint32_t s, uint32_t d, weight_type w, time_type t){
	Layer *layer = this->getLayer(level);
	uint32_t tg = layer->getGranularity();
	uint32_t tt = ceil((double)(t - start_time) / (double)timeslice_len);
	string sv = to_string(s) + "+" + to_string((uint32_t)ceil((double)tt / (double)tg));
	string dv = to_string(d) + "+" + to_string((uint32_t)ceil((double)tt / (double)tg));
	layer->insert(sv, dv, w);
}

void Horae::setStartTime(time_type startTime) {
	this->start_time = startTime;
}

time_type Horae::getStartTime() {
	return this->start_time;
}

void Horae::setTimesliceLength(uint32_t timesliceLength) {
	this->timeslice_len = timesliceLength;
}

uint32_t Horae::getTimesliceLength() {
	return this->timeslice_len;
}
int Horae::secondPowerDecompose(int start, int end, window win[], int level){
	int length = end - start + 1;
	int gl = (1 << level);
	if (start % gl == 1) {
		if (end % gl == 0) {//perfect alignment
			win[0].number = end / gl;
			win[0].level = level + 1;
			return 1;
		}else{//left alignment
			int n = decompose(length, end, 0, win, 0, level);
			return n;
		}	
	}

	if (end % gl ==0)//right alignment
	{
		int n = decompose(length, start, 1, win, 0, level);
		return n;	
	}

	int breakpoint = floor(end / gl) * gl;
	int L1 = breakpoint - start + 1;
	int L2 = end - breakpoint;
	int n1 = decompose(L1, start, 1, win, 0, level);
	int n2 = decompose(L2, end, 0, win, n1, level);
	return n2;
}
int Horae::decompose(int L, int point, int type, window win[], int winlen, int curlayer) {
	int i;
	int j = winlen;

	for (i = 0; i <= curlayer; ++i) {
		if(L >> i == 0)
			break;
		int tmp = ((L >> i) & 0x1) << i;
		if (tmp != 0) {
			win[j].level = i + 1;
			if (type == 0) {		//left alignment
				win[j].number = point / pow(2, (win[j].level - 1));
				point -= tmp;
			}
			else if (type == 1) {	//right alignment
				if (i == 0) {
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

uint32_t Horae::edgeQuery(uint32_t s, uint32_t d, time_type start, time_type end) {
	uint32_t result = 0;
	int length = end - start + 1;
	int level = 0;
	while (length) {
		length = length >> 1;
		level++;
	}
	int gl = (1 << (level - 1));
	window *win = new window[2 * level];

	int n = secondPowerDecompose(start, end, win, level-1);
	for (int i = 0; i < n; i++) {
			string v1 = to_string(s) + "+" + to_string(win[i].number);
			string v2 = to_string(d) + "+" + to_string(win[i].number);
			result += multi_layers[win[i].level - 1]->edgeQuery(v1, v2);
	}
	delete[] win;
	return result;
}

uint32_t Horae::nodeQuery(uint32_t v, int type, time_type start, time_type end) {
	uint32_t result = 0;
	int length = end - start + 1;
	int level = 0;
	while (length) {
		length = length >> 1;
		level++;
	}
	int gl = (1 << (level - 1));

	window *win = new window[2*level];

	int n = secondPowerDecompose(start, end, win, level-1);
	for (int i = 0; i < n; i++) {
			string v1 = to_string(v) + "+" + to_string(win[i].number);
			result += multi_layers[win[i].level - 1]->nodeQuery(v1, type);
	}
	delete[] win;
	return result;
}


// memory improvement
void Horae::newInsert(uint32_t s, uint32_t d, weight_type w, time_type t) {
	uint32_t tt = ceil((double)(t - start_time) / (double)timeslice_len);
	if (tt > 2 * multi_layers[multi_layers.size() - 1]->getGranularity()) {
#if defined(DEBUG) || defined(TINSTIME)
		timeval matrix_s, matrix_e;
		gettimeofday( &matrix_s, NULL);
#endif
		// Layer *layer = new Layer(*multi_layers[multi_layers.size() - 1], multi_layers.size());
		Layer *layer = nullptr;
#ifdef SUCPRE
		layer = new LayerSucPreClass((LayerSucPreClass *)multi_layers[multi_layers.size() - 1], multi_layers.size());
#elif SUCPREMAP
		layer = new LayerSucPreMapClass((LayerSucPreMapClass *)multi_layers[multi_layers.size() - 1], multi_layers.size());
#else
		layer = new LayerSucClass((LayerSucClass *)multi_layers[multi_layers.size() - 1], multi_layers.size());
#endif

#if defined(DEBUG) || defined(TINSTIME)
		gettimeofday( &matrix_e, NULL);
		double matrix_time = (matrix_e.tv_sec - matrix_s.tv_sec) +  (matrix_e.tv_usec - matrix_s.tv_usec) / 1000000.0;
		cout << "Level " << multi_layers.size() << ", Matrix Time = " << matrix_time << " s" << endl;
#endif	
		this->multi_layers.push_back(layer);
	}
	for (uint32_t i = 0; i < 1; i++) {
		string sv = to_string(s) + "+" + to_string((uint32_t)ceil((double)tt / (double)multi_layers[i]->getGranularity()));
		string dv = to_string(d) + "+" + to_string((uint32_t)ceil((double)tt / (double)multi_layers[i]->getGranularity()));
		this->multi_layers[i]->insert(sv, dv, w);
	}
	for (uint32_t i = 1; i < multi_layers.size(); i++) {
		uint32_t winNum = ceil((double)tt / (double)multi_layers[i]->getGranularity());
		if(winNum % 2 ==1){
			continue;
		}else{
			string sv = to_string(s) + "+" + to_string(winNum);
			string dv = to_string(d) + "+" + to_string(winNum);
			this->multi_layers[i]->insert(sv, dv, w);
		}
	}
}
void Horae::newLevelInsert(int level, uint32_t s, uint32_t d, weight_type w, time_type t) {
	Layer *layer = this->getLayer(level);
	uint32_t tg = layer->getGranularity();
	uint32_t tt = ceil((double)(t - start_time) / (double)timeslice_len);
	int winNum = ceil((double)tt / (double)tg);
	if (level != 0 && winNum % 2 == 1)
		return;
	else {
		string sv = to_string(s) + "+" + to_string(winNum);
		string dv = to_string(d) + "+" + to_string(winNum);
		layer->insert(sv, dv, w);
	}
}

int Horae::newSecondPowerDecompose(int start, int end, vector<window>& win, int level){
	int length = end - start + 1;
	int gl = (1 << level);
	if (start % gl == 1) {
		if (end % gl == 0) {//perfect alignment
			// win[0].number = end / gl;
			// win[0].level = level + 1;
			window temp;
			temp.number = end / gl;
			temp.level = level + 1;
			if (temp.number % 2 == 1 && temp.level != 1) {
				window win1;
				win1.level = 1;
				win1.number = start;
				win.push_back(win1);
				newSecondPowerDecompose(start + 1, end, win, level - 1);
			}
			else {
				win.push_back(temp);
			}
			return 1;
		}else{//left alignment
			int n = newDecompose(length, end, 0, win, level);
			return n;
		}	
	}

	if (end % gl ==0)//right alignment
	{
		int n = newDecompose(length, start, 1, win, level);
		return n;	
	}

	int breakpoint = floor(end / gl) * gl;
	int L1 = breakpoint - start + 1;
	int L2 = end - breakpoint;
	int n1 = newDecompose(L1, start, 1, win, level);
	int n2 = newDecompose(L2, end, 0, win, level);
	return n2;
}
int Horae::newDecompose(int L, int point, int type, vector<window>& win, int curlayer) {
	int i;

	for (i = 0; i <= curlayer; ++i) {
		if(L>>i==0){break;}
		int tmp = ((L >> i) & 0x1) << i;
		if (tmp != 0) {
			//win[j].level = i + 1;
			window temp;
			temp.level = i + 1;
			if (type == 0) {//left alignment
				//win[j].number = point / pow(2, (win[j].level - 1));
				temp.number = point / pow(2, (temp.level - 1));
				if (temp.number % 2 == 1 && temp.level != 1) {
					window win1;
					win1.level = 1;
					win1.number = point - pow(2, i) + 1;
					win.push_back(win1);
					newSecondPowerDecompose(point - pow(2, i) + 2, point, win, i - 1);
				}
				else {
					win.push_back(temp);
				}
				point -= tmp;
			}
			else if (type == 1)//right alignment
			{
				if (i == 0) {
					point += tmp - 1;
					//win[j].number = point / pow(2, (win[j].level - 1));
					//win[j].number = point / pow(2, (win[j].level - 1));
					temp.number = point / pow(2, (temp.level - 1));
					win.push_back(temp);
				}
				else {
					point += tmp;
					//win[j].number = point / pow(2, (win[j].level - 1));
					temp.number = point / pow(2, (temp.level - 1));
					if (temp.number % 2 == 1 && temp.level != 1) {
						window win1;
						win1.level = 1;
						win1.number = point - pow(2, i) + 1;
						win.push_back(win1);
						newSecondPowerDecompose(point - pow(2, i) + 2, point, win, i - 1);
					}
					else {
						win.push_back(temp);
					}
				}
				
			}
		}
	}
	return win.size();
}

uint32_t Horae::newEdgeQuery(uint32_t s, uint32_t d, time_type start, time_type end) {		
	uint32_t result = 0;
	int length = end - start + 1;
	int level = 0;
	while (length) {
		length = length >> 1;
		level++;
	}
	int gl = (1 << (level - 1));
	//window *win = new window[2 * level];
	vector<window> win;
	newSecondPowerDecompose(start, end, win, level-1);
	for (int i = 0; i < win.size(); i++) {
		string v1 = to_string(s) + "+" + to_string(win[i].number);
		string v2 = to_string(d) + "+" + to_string(win[i].number);
		result += multi_layers[win[i].level - 1]->edgeQuery(v1, v2);
	}
	//delete[] win;
	return result;
}

uint32_t Horae::newNodeQuery(uint32_t v, int type, time_type start, time_type end) {
	uint32_t result = 0;
	int length = end - start + 1;
	int level = 0;
	while (length) {
		length = length >> 1;
		level++;
	}
	int gl = (1 << (level - 1));
	vector<window> win;		
	newSecondPowerDecompose(start, end, win, level-1);
	for (int i = 0; i < win.size(); i++) {
			string v1 = to_string(v) + "+" + to_string(win[i].number);
			result += multi_layers[win[i].level - 1]->nodeQuery(v1, type);
	}
	//delete[] win;
	return result;
}


#endif		// _Horae_H