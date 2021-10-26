#ifndef QUERYFUNCTION_H
#define QUERYFUNCTION_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h> //_access fun

#include "Horae.h"


const int query_data_pairs = 100000;

struct HORAE_VAR { 
	time_type startTime;
	uint32_t granularityLength;
	uint32_t gl;
	uint32_t width;
	uint32_t depth;
	uint32_t fingerprintLen;
	uint32_t row_addrs;
	uint32_t col_addrs;
	bool kick;
	bool cache_align;
};

struct QueryPairData {
	uint64_t source;
	uint64_t destination;
	time_type start_time;
	time_type end_time;
};

/******************* static variables *************************/
static Layer* baseline_gss;
static Horae* pgss_sequential;
static Horae* pgss_parallel;
/******************* static variables *************************/
/***************** function declaration ***********************/
int isFolderExist(char* folder);
int createDirectory(char* sPathName);
int readRandomFileToDataArray(string file, QueryPairData dataArray[]);
// para/seq insert functions
int baselineInsert(HORAE_VAR var, string filename);
int pgssSequentialInsert(HORAE_VAR var, string filename);
int insert_pgss_parallel(Horae* pg, int64_t fpLength, int level, string filename, int line);
int pgssParallelInsert(HORAE_VAR var, string filename);
// para query functions
int64_t edgeFrequenceBaseline(Layer& layer, int64_t s, int64_t d, int64_t start, int64_t end);
int edgeFrequenceBaselineTest_para(string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write);
int edgeFrequenceBaselineTest_single(string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write);
int edgeExistenceBaselineTest_para(string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag);
int edgeExistenceBaselineTest_single(string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write, int flag);
int64_t nodeFrequenceBaseline(Layer& layer, int64_t v, int type, int64_t start, int64_t end);
int nodeFrequenceBaselineTest_para(string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line);
int nodeFrequenceBaselineTest_single(string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write, int flag, int line);

int edgeFrequencePgssTest_para(Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write);
int edgeFrequencePgssTest_single(Horae* horae, string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write);
int edgeExistencePgssTest_para(Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag);
int edgeExistencePgssTest_single(Horae* horae, string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write, int flag);
int nodeFrequencePgssTest_para(Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line);
int nodeFrequencePgssTest_single(Horae* horae, string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write, int flag, int line);
// seq query functions
int edgeFrequenceBaselineTest_seq(string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write);
int edgeExistenceBaselineTest_seq(string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag);
int nodeFrequenceBaselineTest_seq(string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line);
int edgeFrequencePgssTest_seq(Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write);
int edgeExistencePgssTest_seq(Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag);
int nodeFrequencePgssTest_seq(Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line);
// query functions that the main function called
void edgeFrequenceBaselineTest(bool para_query, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write);
void edgeExistenceBaselineTest(bool para_query, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag);
void nodeFrequenceBaselineTest(bool para_query, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line);
void edgeFrequencePgssTest(bool para_query, Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write);
void edgeExistencePgssTest(bool para_query, Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag);
void nodeFrequencePgssTest(bool para_query, Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line);
/***************** function declaration ***********************/
int isFolderExist(char* folder) {
	int ret = 0;
	ret = access(folder, R_OK | W_OK);
	if (ret == 0)
		ret = 1;
	else
		ret = 0;
	return ret;
}

int createDirectory(char* sPathName) {
	char DirName[256];
	strcpy(DirName, sPathName);
	int i, len = strlen(DirName);
	if (DirName[len - 1] != '/')
		strcat(DirName, "/");
	len = strlen(DirName);
	for (i = 1; i < len; i++) {
		if (DirName[i] == '/') {
			DirName[i] = 0;
			int a = access(DirName, F_OK);
			if (a == -1) {
				mkdir(DirName, 0755);
			}
			DirName[i] = '/';
		}
	}
	return 0;
}

uint64_t count_lines(string file) {  // count file lines
    ifstream readFile;
    uint64_t n = 0;
    char line[512];
    string temp;
    readFile.open(file, ios::in);	// ios::in means that open file with readonly 
    if(readFile.fail()) { 			// open file error, return 0
        cout << "error in opening file" << endl;
        return 0;
    }
    else { 							// the file exists
        while(getline(readFile,temp))
            n++;
    }
    readFile.close();
    return n;
}

#if defined(DEBUG) || defined(TINSTIME) || defined(BINSTIME) || defined(HINT)
void progress_bar(int n) {
	int i = 0;
	char bar[102];
	const char *lable = "|/-\\";
	bar[0] = 0;
	while (i < n) {
	    bar[i] = '#';
		i++;
		bar[i] = 0;
	}
	printf("\r[%-100s][%d%%][%c]", bar, i, lable[i%4]);
	fflush(stdout);
	return;
}
#endif
int baselineInsert(HORAE_VAR var, string filename) {
	//unsigned int bpoint = 10000000;
	ifstream ifs;
	ifs.open(filename);
	if (!ifs.is_open()) {
		cout << "Error in open file, Path = " << filename << endl;
		return -1;
	}
	int64_t s, d;
	weight_type w;
	time_type t;
#if defined(DEBUG) || defined(TINSTIME) || defined(BINSTIME) || defined(HINT)
	cout << "Inserting..." << endl;
	timeval matrix_s, matrix_e;
	gettimeofday( &matrix_s, NULL);
#endif
	baseline_gss = new LayerSucClass(var.gl, var.width, var.depth, var.fingerprintLen, var.cache_align, var.kick, var.row_addrs, var.col_addrs);
#if defined(DEBUG) || defined(TINSTIME)
	gettimeofday( &matrix_e, NULL);
	double matrix_time = (matrix_e.tv_sec - matrix_s.tv_sec) +  (matrix_e.tv_usec - matrix_s.tv_usec) / 1000000.0;
	cout << "Matrix Time = " << matrix_time << " s" << endl;
#endif
#if defined(DEBUG) || defined(TINSTIME) || defined(BINSTIME) || defined(HINT)
	timeval ins_start, ins_end;	
	gettimeofday( &ins_start, NULL);
#endif
#if defined(DEBUG) || defined(BINSTIME)
	timeval bpoint_start, bpoint_end;
	gettimeofday( &bpoint_start, NULL);
#endif
#if defined(DEBUG) || defined(HINT)
		double total = count_lines(filename);
		if(total == 0)
			cout << "ERROR--QueryFunction--178" <<endl;
#endif
	int datanum = 0;
	while (!ifs.eof()) {
		ifs >> s >> d >> w >> t;
		if(ifs.fail())
			break;
		uint32_t tt = ceil((double)(t - var.startTime) / (double)var.granularityLength);
		string sv = to_string(s) + "+" + to_string((uint32_t)ceil((double)tt / (double)baseline_gss->getGranularity()));
		string dv = to_string(d) + "+" + to_string((uint32_t)ceil((double)tt / (double)baseline_gss->getGranularity()));
		baseline_gss->insert(sv, dv, w);
		datanum++;
#if defined(DEBUG) || defined(BINSTIME)
		if (datanum % 10000000 == 0) {
			gettimeofday( &bpoint_end, NULL);
			double bpoint_time = (bpoint_end.tv_sec - bpoint_start.tv_sec) +  (bpoint_end.tv_usec - bpoint_start.tv_usec) / 1000000.0;
			cout << datanum << ", Break Point Time = " << bpoint_time << " s" << endl;
			gettimeofday( &bpoint_start, NULL);
		}
#endif
#if defined(DEBUG) || defined(HINT)
		if (datanum % 100000 == 0) {
			int n = (int) ((double) datanum / total * 100);
			progress_bar(n);
		}
		if (datanum == total) {
			progress_bar(100);
		}
#endif
	}
#if defined(DEBUG) || defined(HINT)
	cout << endl;
#endif
#if defined(DEBUG) || defined(BINSTIME)
	gettimeofday( &bpoint_end, NULL);
	double bpoint_time = (bpoint_end.tv_sec - bpoint_start.tv_sec) +  (bpoint_end.tv_usec - bpoint_start.tv_usec) / 1000000.0;
	cout << datanum << ", Break Point Time = " << bpoint_time << " s" << endl;
#endif
#if defined(DEBUG) || defined(TINSTIME) || defined(BINSTIME) || defined(HINT)
	gettimeofday( &ins_end, NULL);
	double ins_time = (ins_end.tv_sec - ins_start.tv_sec) +  (ins_end.tv_usec - ins_start.tv_usec) / 1000000.0;
	cout << "Insertion Time = " << ins_time << " s" << endl;
	cout << "Insertion Finished!" << endl;
	cout << "Datanum = " << datanum << endl;
#endif
	cout << "************************" << endl;
	baseline_gss->bucketCounting();
	cout << "************************" << endl << endl;
	ifs.close();
	return 0;
}
int pgssSequentialInsert(HORAE_VAR var, string filename) {
	ifstream ifs;
	ifs.open(filename);
	if (!ifs.is_open()) {
		cout << "Error in open file, Path = " << filename << endl;
		return -1;
	}
#if defined(DEBUG) || defined(TINSTIME) || defined(BINSTIME) || defined(HINT)
	cout << "Inserting..." << endl;
#endif
	pgss_sequential = new Horae(var.startTime, var.granularityLength, var.gl, var.width, var.depth, var.fingerprintLen, var.cache_align, var.kick, var.row_addrs, var.col_addrs);	
	int64_t s, d;
	weight_type w;
	time_type t;
	int flag = 1, level = 0, datanum = 0;
#if defined(DEBUG) || defined(TINSTIME) || defined(HINT)
	timeval ins_start, ins_end;	
	gettimeofday( &ins_start, NULL);
#endif
#if defined(DEBUG) || defined(BINSTIME)
	timeval bpoint_start, bpoint_end;
	gettimeofday( &bpoint_start, NULL);
#endif
#if defined(DEBUG) || defined(HINT)
		double total = count_lines(filename);
		if(total == 0)
			cout << "ERROR--QueryFunction--269" <<endl;
#endif
	while (!ifs.eof()) {
		ifs >> s >> d >> w >> t;
		if(ifs.fail())
			break;
		pgss_sequential->insert(s, d, w, t);
		datanum++;
#if defined(DEBUG) || defined(BINSTIME)
		if (datanum % 10000000 == 0) {
			gettimeofday( &bpoint_end, NULL);
			double bpoint_time = (bpoint_end.tv_sec - bpoint_start.tv_sec) +  (bpoint_end.tv_usec - bpoint_start.tv_usec) / 1000000.0;
			cout << datanum << ", Break Point Time = " << bpoint_time << " s" << endl;
			gettimeofday( &bpoint_start, NULL);
		}
#endif
#if defined(DEBUG) || defined(HINT)
		if (datanum % 100000 == 0) {
			int n = (int) ((double) datanum / total * 100);
			progress_bar(n);
		}
		if (datanum == total) {
			progress_bar(100);
		}
#endif
	}
#if defined(DEBUG) || defined(HINT)
	cout << endl;
#endif
#if defined(DEBUG) || defined(BINSTIME)
	gettimeofday( &bpoint_end, NULL);
	double bpoint_time = (bpoint_end.tv_sec - bpoint_start.tv_sec) +  (bpoint_end.tv_usec - bpoint_start.tv_usec) / 1000000.0;
	cout << datanum << ", Break Point Time = " << bpoint_time << " s" << endl;
#endif
	
#if defined(DEBUG) || defined(TINSTIME) || defined(HINT)
	gettimeofday( &ins_end, NULL);
	double ins_time = (ins_end.tv_sec - ins_start.tv_sec) +  (ins_end.tv_usec - ins_start.tv_usec) / 1000000.0;
	cout << "Insertion Time = " << ins_time << " s" << endl;
	cout << "Insertion Finished!" << endl;
	cout << "Datanum = " << datanum << endl;
	cout << "Level = " << pgss_sequential->getMultilayerSize() << endl;
#endif
	ifs.close();
	pgss_sequential->bucketCounting();
	return 0;
}
int insert_pgss_parallel(Horae* pg, int64_t fpLength, int level, string filename, int line) {
    ifstream ifs;
	ifs.open(filename);
	if (!ifs.is_open()) {
		cout << "error in open file " << endl;
		return -1;
	}
	ifs.seekg(fpLength);
	int64_t s, d;
	weight_type w;
	time_type t;
	int flag = 1;
#ifdef DEBUG
	int tag = 1;
#endif
	thread* child = NULL;
    int datanum = line;
#if defined(DEBUG) || defined(BINSTIME)
	timeval bpoint_start, bpoint_end;
	gettimeofday( &bpoint_start, NULL);
#endif
	while (!ifs.eof()) {
		ifs >> s >> d >> w >> t;
		if(ifs.fail())
			break;
#ifdef DEBUG
		if(tag == 1) {
			cout << "(" << s << ", " << d << ", " << w << ", " << t << ") ---- " << " level = " << level << endl;
			tag = 0;
		}
#endif
		int tt = ceil((double)(t - pg->getStartTime()) / (double)pg->getTimesliceLength());
#ifdef MEM
		if ((flag == 1) && (tt > 2 * pg->getLayer(level)->getGranularity())) {
			flag = 0;
			int line = datanum + 1;
	#if defined(DEBUG) || defined(TINSTIME)
			timeval matrix_s1, matrix_e1;
			gettimeofday( &matrix_s1, NULL);
	#endif
			// Layer* gs = new Layer(*(pg->getLayer(level)), level+1);
			Layer* gs = NULL;
	#ifdef SUCPRE
			gs = new LayerSucPreClass((LayerSucPreClass *)pg->getLayer(level), level + 1);
	#elif SUCPREMAP
			gs = new LayerSucPreMapClass((LayerSucPreMapClass *)pg->getLayer(level), level + 1);
	#else
			gs = new LayerSucClass((LayerSucClass *)pg->getLayer(level), level + 1);
	#endif
#else
		if ((flag == 1) && (tt > pg->getLayer(level)->getGranularity())) {
			flag = 0;
			int line = datanum + 1;
	#if defined(DEBUG) || defined(TINSTIME)
			timeval matrix_s1, matrix_e1;
			gettimeofday( &matrix_s1, NULL);
	#endif
			Layer* gs = NULL;
	#ifdef SUCPRE
			gs = new LayerSucPreClass((LayerSucPreClass *)pg->getLayer(level));
	#elif SUCPREMAP
			gs = new LayerSucPreMapClass((LayerSucPreMapClass *)pg->getLayer(level));
	#else
			gs = new LayerSucClass((LayerSucClass *)pg->getLayer(level));
	#endif
#endif
	#if defined(DEBUG) || defined(TINSTIME)
			gettimeofday( &matrix_e1, NULL);
			double matrix_time1 = (matrix_e1.tv_sec - matrix_s1.tv_sec) +  (matrix_e1.tv_usec - matrix_s1.tv_usec) / 1000000.0;
			cout << "Level " << (level + 1) << ", Line = " << line << ", Matrix Time = " << matrix_time1 << " s" << endl;
	#endif
			pg->addLayer(gs);
			int64_t length = ifs.tellg();
	#ifdef DEBUG
			cout << "(" << s << ", " << d << ", " << w << ", " << t << ") -- " << " level = " << level << endl;
	#endif
			pg->levelInsert(level + 1, s, d, w, t);
			child = new thread(insert_pgss_parallel, pg, length, level + 1, filename, line);
		}
		pg->levelInsert(level, s, d, w, t);
		datanum++;
#if defined(DEBUG) || defined(BINSTIME)
		if (datanum % 10000000 == 0) {
			gettimeofday( &bpoint_end, NULL);
			double bpoint_time = (bpoint_end.tv_sec - bpoint_start.tv_sec) +  (bpoint_end.tv_usec - bpoint_start.tv_usec) / 1000000.0;
			cout << "Level " << level << ", datanum = " << datanum << ", Break Point Time = " << bpoint_time << " s" << endl;
			gettimeofday( &bpoint_start, NULL);
		}
#endif
	}
#if defined(DEBUG) || defined(BINSTIME)
	gettimeofday( &bpoint_end, NULL);
	double bpoint_time = (bpoint_end.tv_sec - bpoint_start.tv_sec) +  (bpoint_end.tv_usec - bpoint_start.tv_usec) / 1000000.0;
	cout << "Level " << level << ", datanum = " << datanum << ", Break Point Time = " << bpoint_time << " s" << endl;
#endif
#if defined(DEBUG) || defined(HINT)
    cout << "level " << level << " data insert = " << datanum << endl;
    cout << "level " << level << "finished!" << endl;
#endif
	if(child != NULL) {
		child->join();
	}
#if defined(DEBUG) || defined(HINT)
    cout << "level " << level << "finished!!!" << endl;
#endif
	delete child;
	return 0;
}

int pgssParallelInsert(HORAE_VAR var, string filename) {
	ifstream ifs;
	ifs.open(filename);
	if (!ifs.is_open()) {
		cout << "Error in open file, Path = " << filename << endl;
		return -1;
	}
#if defined(DEBUG) || defined(HINT)
	cout << "Inserting..." << endl;
#endif
	pgss_parallel = new Horae(var.startTime, var.granularityLength, var.gl, var.width, var.depth, var.fingerprintLen, var.cache_align, var.kick, var.row_addrs, var.col_addrs);
	int64_t s, d;
	weight_type w;
	time_type t;
	int flag = 1, level = 0, datanum = 0;
	thread* child = NULL;
#if defined(DEBUG) || defined(TINSTIME) || defined(HINT)
	timeval t_start, t_end;
	gettimeofday( &t_start, NULL);
#endif
#if defined(DEBUG) || defined(BINSTIME)
	timeval bpoint_start, bpoint_end;
	gettimeofday( &bpoint_start, NULL);
#endif
#if defined(DEBUG) || defined(HINT)
		double total = count_lines(filename);
		if(total == 0)
			cout << "ERROR--QueryFunction--429" <<endl;
#endif
	while (!ifs.eof()) {
		ifs >> s >> d >> w >> t;
		if(ifs.fail())
			break;
		int tt = ceil((double)(t - var.startTime) / (double)var.granularityLength);

#ifdef MEM
		if ((flag == 1) && (tt > 2 * (pgss_parallel->getLayer(level)->getGranularity()))) {
			flag = 0;
			int line = datanum + 1;
	#if defined(DEBUG) || defined(TINSTIME)
			timeval matrix_s1, matrix_e1;
			gettimeofday( &matrix_s1, NULL);
	#endif
			// Layer* gs = new Layer(*(pgss_parallel->getLayer(level)), level+1); 
			Layer* gs = NULL;
	#ifdef SUCPRE
			gs = new LayerSucPreClass((LayerSucPreClass *)pgss_parallel->getLayer(level), level + 1);
	#elif SUCPREMAP
			gs = new LayerSucPreMapClass((LayerSucPreMapClass *)pgss_parallel->getLayer(level), level + 1);
	#else
			gs = new LayerSucClass((LayerSucClass *)pgss_parallel->getLayer(level), level + 1);
	#endif
#else
		if ((flag == 1) && (tt > (pgss_parallel->getLayer(level)->getGranularity()))) {
			flag = 0;
			int line = datanum + 1;
	#if defined(DEBUG) || defined(TINSTIME)
			timeval matrix_s1, matrix_e1;
			gettimeofday( &matrix_s1, NULL);
	#endif

			Layer* gs = NULL;
	#ifdef SUCPRE
			gs = new LayerSucPreClass((LayerSucPreClass *)pgss_parallel->getLayer(level));
	#elif SUCPREMAP
			gs = new LayerSucPreMapClass((LayerSucPreMapClass *)pgss_parallel->getLayer(level));
	#else
			gs = new LayerSucClass((LayerSucClass *)pgss_parallel->getLayer(level));
	#endif
#endif

	#if defined(DEBUG) || defined(TINSTIME)
			gettimeofday( &matrix_e1, NULL);
			double matrix_time1 = (matrix_e1.tv_sec - matrix_s1.tv_sec) +  (matrix_e1.tv_usec - matrix_s1.tv_usec) / 1000000.0;
			cout << "Level " << (level + 1) << ", Line = " << line << ", Matrix Time = " << matrix_time1 << " s" << endl;
	#endif
			pgss_parallel->addLayer(gs);
			int64_t length = ifs.tellg();
	#ifdef DEBUG
			cout << "(" << s << ", " << d << ", " << w << ", " << t << ") -- " << " level = " << level << endl;
	#endif
			pgss_parallel->levelInsert(level + 1, s, d, w, t);
            child = new thread(insert_pgss_parallel, pgss_parallel, length, level + 1, filename, line);	
		}
		pgss_parallel->levelInsert(level, s, d, w, t);

		datanum++;
#if defined(DEBUG) || defined(BINSTIME)
		if (datanum % 10000000 == 0) {
			gettimeofday( &bpoint_end, NULL);
			double bpoint_time = (bpoint_end.tv_sec - bpoint_start.tv_sec) +  (bpoint_end.tv_usec - bpoint_start.tv_usec) / 1000000.0;
			cout << "Level 0, datanum = " << datanum << ", Break Point Time = " << bpoint_time << " s" << endl;
			gettimeofday( &bpoint_start, NULL);
		}
#endif
#if defined(DEBUG) || defined(HINT)
		if (datanum % 100000 == 0) {
			int n = (int) ((double) datanum / total * 100) ;
			progress_bar(n);
		}
		if (datanum == total) {
			progress_bar(100);
		}
#endif
	}
#if defined(DEBUG) || defined(HINT)
	cout << endl;
#endif
#if defined(DEBUG) || defined(BINSTIME)
	gettimeofday( &bpoint_end, NULL);
	double bpoint_time = (bpoint_end.tv_sec - bpoint_start.tv_sec) +  (bpoint_end.tv_usec - bpoint_start.tv_usec) / 1000000.0;
	cout << "Level 0, datanum = " << datanum << ", Break Point Time = " << bpoint_time << " s" << endl;
#endif
#if defined(DEBUG) || defined(HINT)
    cout << "level " << level << " data insert = " << datanum << endl;
    cout << "level " << level << "finished!" << endl;
#endif
	if(child != NULL)
		child->join();
#if defined(DEBUG) || defined(HINT)
    cout << "level " << level << "finished!!!" << endl;
#endif
	delete child;
#if defined(DEBUG) || defined(HINT)
	cout << "Datanum = " << datanum << endl;
	cout << "Level = " << pgss_parallel->getMultilayerSize()<< endl;
#endif
	ifs.close();
#if defined(DEBUG) || defined(TINSTIME) || defined(HINT)
	gettimeofday( &t_end, NULL);
	cout << "Insertion Finished!" << endl;
	double ins_time = (t_end.tv_sec - t_start.tv_sec) +  (t_end.tv_usec - t_start.tv_usec) / 1000000.0;
	cout << "Insertion Time = " << ins_time << " s" << endl;
#endif
	pgss_parallel->bucketCounting();
	return 0;
}

int readRandomFileToDataArray(string file, QueryPairData dataArray[]) {
	ifstream randomFile;
	randomFile.open(file);
	if (!randomFile.is_open()) {
		cout << "Error in open file, Path = " << file << endl;
		return -1;
	}
	int datanum = 0;
	int64_t startPoint, endPoint, timeStart, timeEnd;
	while (!randomFile.eof()) {
		randomFile >> startPoint >> endPoint >> timeStart >> timeEnd;
		if(randomFile.fail())
			break;
		dataArray[datanum].source = startPoint;
		dataArray[datanum].destination = endPoint;
		dataArray[datanum].start_time = timeStart;
		dataArray[datanum].end_time = timeEnd;
		datanum++;
		// if(datanum > query_data_pairs) {
		// 	cout << "the input data is more than the range of the array" << endl;
		// 	break;
		// }
	}
	randomFile.close();
	return datanum;
}

int64_t edgeFrequenceBaseline(Layer& layer, int64_t s, int64_t d, int64_t start, int64_t end) {
	//layer.edgeQuery()
	int64_t result = 0;
	for (int i = start; i <= end; i++) {
		string v1 = to_string(s) + "+" + to_string(i);
		string v2 = to_string(d) + "+" + to_string(i);
		result += layer.edgeQuery(v1, v2);
	}
	return result;
}
int edgeFrequenceBaselineTest_single(string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write) {
	string input_file_prefix = dataset_name + "_random_edge_frequence_";
	string input_file_suffix = "_sorted.txt";
	string output_file_prefix = dataset_name + "_edge_frequence_baseline_";
	string output_file_suffix = "_res.txt";
	string time_file_suffix = "_time.txt";

	//edge query process
	QueryPairData* dataArray = new QueryPairData[query_data_pairs];
	int datanum = readRandomFileToDataArray(input_dir + input_file_prefix + to_string(num) + input_file_suffix, dataArray);
#if defined(DEBUG) || defined(HINT)
	cout << "****************** timeslot = " << num << " ******************" << endl;
#endif
	ofstream resultFile, timeFile;
	if (write) {
		char dir_path[FILENAME_MAX];
		strcpy(dir_path, output_dir.c_str());
		if (createDirectory(dir_path) != 0) {
			cout << "CreateDirectory Error, Path = " << dir_path << endl;
			return -1;
		}
		resultFile.open(output_dir + output_file_prefix + to_string(num) + output_file_suffix);
		if (!resultFile.is_open()) {
			cout << "Error in open file, Path = " << (output_dir + output_file_prefix + to_string(num) + output_file_suffix) << endl;
			return -1;
		}
		timeFile.open(output_dir + output_file_prefix + to_string(num) + time_file_suffix);
		if (!timeFile.is_open()) {
			cout << "Error in open file, Path = " << (output_dir + output_file_prefix + to_string(num) + time_file_suffix) << endl;
			return -1;
		}
	}

	double sumTime = 0, sumTime_perquery = 0;
	timeval tp1, tp2;
	for (int m = 0; m < query_times; m++) {
		sumTime_perquery = 0;
		for (int n = 0; n < datanum; n++) {
			gettimeofday( &tp1, NULL);
			int64_t res = edgeFrequenceBaseline(*baseline_gss, dataArray[n].source, dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
			gettimeofday( &tp2, NULL);
			double delta_t = (tp2.tv_sec - tp1.tv_sec) * 1000000 +  (tp2.tv_usec - tp1.tv_usec);
			sumTime_perquery += delta_t;
			if (write && m == 0) {
				if(n == (datanum - 1)) {
					resultFile << res;
					timeFile  << delta_t;
					break;
				}
				else {
					resultFile << res << endl;
					timeFile  << delta_t << endl;
				}
			}
		}
		sumTime += (sumTime_perquery / (double)datanum);
	}

	if (write) {
		resultFile.flush();
		timeFile.flush();
		resultFile.close();
		timeFile.close();
	}
	delete[] dataArray;
#if defined(DEBUG) || defined(HINT)
	double mseconds = (double)(sumTime / (double)query_times) / 1000;
	printf("Timeslot = %d, Query Times = %d, Query Avg Time = %lf ms\n\n", num, query_times, mseconds);
#endif
	return 0;
}
int edgeFrequenceBaselineTest_para(string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write) {
	vector<thread*> childs;
	thread* child = NULL;
	for(int i = 0; i < num.size(); i++) {
		child = new thread(edgeFrequenceBaselineTest_single, input_dir, output_dir, dataset_name, num[i], query_times, write);
		childs.push_back(child);

	}
	for(int i = 0; i < childs.size(); i++) {
		if(childs[i] != NULL)
			childs[i]->join();
	}
	return 0;
}
int edgeExistenceBaselineTest_single(string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write, int flag) {
	string input_file_prefix = "";
	string input_file_suffix = "";
	string output_file_prefix = "";
	string output_file_suffix = "";
	string time_file_suffix = "_time.txt";
	switch (flag)
	{
	case 1:
		input_file_prefix = "_random_edge_existence_";
		input_file_suffix = "_sorted.txt";
		output_file_prefix = "_edge_existence_baseline_";
		output_file_suffix = "_res.txt";
		break;
	case 2:
		input_file_prefix = "_bool_";
		input_file_suffix = ".txt";
		output_file_prefix = "_bool_baseline_";
		output_file_suffix = "_res.txt";
		break;
	default:
		break;
	}
	//edge query process
	QueryPairData* dataArray = new QueryPairData[query_data_pairs];
	int datanum = readRandomFileToDataArray(input_dir + dataset_name + input_file_prefix + to_string(num) + input_file_suffix, dataArray);
#if defined(DEBUG) || defined(HINT)
	cout << "****************** timeslot = " << num << " ******************" << endl;
#endif
	ofstream resultFile, timeFile;
	if (write) {
		char dir_path[FILENAME_MAX];
		strcpy(dir_path, output_dir.c_str());
		if (createDirectory(dir_path) != 0) {
			cout << "createDirectory error" << endl;
			return -1;
		}
		resultFile.open(output_dir + dataset_name + output_file_prefix + to_string(num) + output_file_suffix);
		if (!resultFile.is_open()) {
			cout << "error in open file " << (output_dir + dataset_name + output_file_prefix + to_string(num) + output_file_suffix) << endl;
			return -1;
		}
		timeFile.open(output_dir + dataset_name + output_file_prefix + to_string(num) + time_file_suffix);
		if (!timeFile.is_open()) {
			cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + to_string(num) + time_file_suffix) << endl;
			return -1;
		}
	}

	double sumTime = 0, sumTime_perquery = 0;
	int ones = 0;
	timeval tp1, tp2;
	for (int m = 0; m < query_times; m++) {
		sumTime_perquery = 0;
		for (int n = 0; n < datanum; n++) {
			gettimeofday( &tp1, NULL);
			int64_t res = edgeFrequenceBaseline(*baseline_gss, dataArray[n].source, dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
			gettimeofday( &tp2, NULL);
			double delta_t = (tp2.tv_sec - tp1.tv_sec) * 1000000 +  (tp2.tv_usec - tp1.tv_usec);
			sumTime_perquery += delta_t;
			if (write && m == 0) {
				if (res > 0)   
					ones++;
				if(n == (datanum - 1)) {
					resultFile << ((res > 0) ? 1 : 0);
					timeFile  << delta_t;
					break;
				}
				else {
					resultFile << ((res > 0) ? 1 : 0) << endl;
					timeFile  << delta_t << endl;
				}
			}
		}
		sumTime += (sumTime_perquery / (double)datanum);
	}
	
	if (write) {
		resultFile.flush();
		timeFile.flush();
		resultFile.close();
		timeFile.close();
	}
	delete[] dataArray;
#if defined(DEBUG) || defined(HINT)
	double mseconds = (double)(sumTime / (double)query_times) / 1000; 
	printf("Timeslot = %d, Query Times = %d, Query Avg Time = %lf ms\n\n", num, query_times, mseconds);
#endif
	return 0;
}
int edgeExistenceBaselineTest_para(string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag) {
	vector<thread*> childs;
	thread* child = NULL;
	for(int i = 0; i < num.size(); i++) {
		child = new thread(edgeExistenceBaselineTest_single, input_dir, output_dir, dataset_name, num[i], query_times, write, flag);
		childs.push_back(child);

	}
	for(int i = 0; i < childs.size(); i++) {
		if(childs[i] != NULL)
			childs[i]->join();
	}
	return 0;
}

int64_t nodeFrequenceBaseline(Layer& layer, int64_t v, int type, int64_t start, int64_t end) {
	int64_t result = 0;
	for (int i = start; i <= end; i++) {
		string vertex = to_string(v) + "+" + to_string(i);
		result += layer.nodeQuery(vertex, type);
	}
	return result;
}
int nodeFrequenceBaselineTest_single(string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write, int flag, int line) {
	string input_file_prefix = "";
	string input_file_suffix = "";
	string output_file_prefix = "";
	string output_file_suffix = "";
	string time_file_suffix = "_time.txt";
	switch (flag) {
		case 1:
			input_file_prefix = "_random_node_frequence_in_";
			input_file_suffix = "_sorted.txt";
			output_file_prefix = "_node_frequence_in_";
			output_file_suffix = "_res.txt";
			break;
		case 2:
			input_file_prefix = "_random_node_frequence_out_";
			input_file_suffix = "_sorted.txt";
			output_file_prefix = "_node_frequence_out_";
			output_file_suffix = "_res.txt";
			break;
		default:
			break;
	}
	//node query process
	QueryPairData* dataArray = new QueryPairData[query_data_pairs];
	int datanum = readRandomFileToDataArray(input_dir + dataset_name + input_file_prefix + to_string(num) + input_file_suffix, dataArray);
#if defined(DEBUG) || defined(HINT)
	cout << "****************** timeslot = " << num << " ******************" << endl;
#endif
	ofstream resultFile, timeFile;
	if (write) {
		char dir_path[FILENAME_MAX];
		strcpy(dir_path, output_dir.c_str());
		if (createDirectory(dir_path) != 0) {
			cout << "createDirectory error" << endl;
			return -1;
		}

		if(line == 0) {
			resultFile.open(output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num) + output_file_suffix);
			if (!resultFile.is_open()) {
				cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num) + output_file_suffix) << endl;
				return -1;
			}
			timeFile.open(output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num) + time_file_suffix);
			if (!timeFile.is_open()) {
				cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num) + time_file_suffix) << endl;
				return -1;
			}
		}
		else {	//	append
			resultFile.open(output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num) + output_file_suffix, ios::app);
			cout << "append " << (output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num) + output_file_suffix) << endl;
			if (!resultFile.is_open()) {
				cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num) + output_file_suffix) << endl;
				return -1;
			}
			timeFile.open(output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num) + time_file_suffix, ios::app);
			cout << "append " << (output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num) + time_file_suffix) << endl;
			if (!timeFile.is_open()) {
				cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num) + time_file_suffix) << endl;
				return -1;
			}
		}
	}

	double sumTime = 0, sumTime_perquery = 0;	
	timeval tp1, tp2;
	for (int m = 0; m < query_times; m++) {
		sumTime_perquery = 0;
		for (int n = 0; n < datanum; n++) {
			if(m == 0 && n < line) 
				continue;
			
			gettimeofday( &tp1, NULL);
			int64_t res = nodeFrequenceBaseline(*baseline_gss, dataArray[n].source, (int)dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
			gettimeofday( &tp2, NULL);
			double delta_t = (tp2.tv_sec - tp1.tv_sec) * 1000000 +  (tp2.tv_usec - tp1.tv_usec);
			sumTime_perquery += delta_t;
			
			if (write && m == 0) {
				if(n == (datanum - 1)) {
					resultFile << res;
					timeFile  << delta_t;
					break;
				}
				else {
					resultFile << res << endl;
					timeFile  << delta_t << endl;
				}
			}
		}
		sumTime += (sumTime_perquery / (double)datanum);
	}

	if (write) {
		resultFile.flush();
		timeFile.flush();
		resultFile.close();
		timeFile.close();
	}
	delete[] dataArray;
#if defined(DEBUG) || defined(HINT)
	double mseconds = (double)(sumTime / (double)query_times) / 1000; 
	printf("Timeslot = %d, Query Times = %d, Query Avg Time = %lf ms\n\n", num, query_times, mseconds);
#endif
	return 0;
}
int nodeFrequenceBaselineTest_para(string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line) {
	vector<thread*> childs;
	thread* child = NULL;
	for(int i = 0; i < num.size(); i++) {
		child = new thread(nodeFrequenceBaselineTest_single, input_dir, output_dir, dataset_name, num[i], query_times, write, flag, line);
		childs.push_back(child);

	}
	for(int i = 0; i < childs.size(); i++) {
		if(childs[i] != NULL)
			childs[i]->join();
	}
	return 0;
}


int edgeFrequencePgssTest_single(Horae* horae, string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write) {
	string input_file_prefix = dataset_name + "_random_edge_frequence_";
	string input_file_suffix = "_sorted.txt";
	string output_file_prefix = dataset_name + "_edge_frequence_pgss_";
	string output_file_suffix = "_res.txt";
	string time_file_suffix = "_time.txt";
	//edge query process
	QueryPairData* dataArray = new QueryPairData[query_data_pairs];
	int datanum = readRandomFileToDataArray(input_dir + input_file_prefix + to_string(num) + input_file_suffix, dataArray);
#if defined(DEBUG) || defined(HINT)
	cout << "****************** timeslot = " << num << " ******************" << endl;
#endif
	ofstream resultFile, timeFile;
	if (write) {
		char dir_path[FILENAME_MAX];
		strcpy(dir_path, output_dir.c_str());
		if (createDirectory(dir_path) != 0) {
			cout << "createDirectory error" << endl;
			return -1;
		}
		resultFile.open(output_dir + output_file_prefix + to_string(num) + output_file_suffix);
		if (!resultFile.is_open()) {
			cout << "Error in open file, Path = " << (output_dir + output_file_prefix + to_string(num) + output_file_suffix) << endl;
			return -1;
		}
		timeFile.open(output_dir + output_file_prefix + to_string(num) + time_file_suffix);
		if (!timeFile.is_open()) {
			cout << "Error in open file, Path = " << (output_dir + output_file_prefix + to_string(num) + time_file_suffix) << endl;
			return -1;
		}
	}

	double sumTime = 0, sumTime_perquery = 0;
	timeval tp1, tp2;
	for (int m = 0; m < query_times; m++) {
		sumTime_perquery = 0;
		for (int n = 0; n < datanum; n++) {
			int64_t res;
			gettimeofday( &tp1, NULL);
			res = horae->edgeQuery(dataArray[n].source, dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
			gettimeofday( &tp2, NULL);
			double delta_t = (tp2.tv_sec - tp1.tv_sec) * 1000000 +  (tp2.tv_usec - tp1.tv_usec);
			sumTime_perquery += delta_t;
			if (write && m == 0) {
				if(n == (datanum - 1)) {
					resultFile << res;
					timeFile  << delta_t;
					break;
				}
				else {
					resultFile << res << endl;
					timeFile  << delta_t << endl;
				}
			}
		}
		sumTime += (sumTime_perquery / (double)datanum);
	}
	if (write) {
		resultFile.flush();
		timeFile.flush();
		resultFile.close();
		timeFile.close();
	}
	delete[] dataArray;
#if defined(DEBUG) || defined(HINT)
	double mseconds = (double)(sumTime / (double)query_times) / 1000;
	printf("Timeslot = %d, Query Times = %d, Query Avg Time = %lf ms\n\n", num, query_times, mseconds);
#endif
	return 0;
}
int edgeFrequencePgssTest_para(Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write) {
	vector<thread*> childs;
	thread* child = NULL;
	for(int i = 0; i < num.size(); i++) {
		child = new thread(edgeFrequencePgssTest_single, horae, input_dir, output_dir, dataset_name, num[i], query_times, write);
		childs.push_back(child);

	}
	for(int i = 0; i < childs.size(); i++) {
		if(childs[i] != NULL)
			childs[i]->join();
	}
	return 0;
}
int edgeExistencePgssTest_single(Horae* horae, string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write, int flag) {
	string input_file_prefix = "";
	string input_file_suffix = "";
	string output_file_prefix = "";
	string output_file_suffix = "";
	string time_file_suffix = "_time.txt";
	switch (flag)
	{
	case 1:
		input_file_prefix = "_random_edge_existence_";
		input_file_suffix = "_sorted.txt";
		output_file_prefix = "_edge_existence_pgss_";
		output_file_suffix = "_res.txt";
		break;
	case 2:
		input_file_prefix = "_bool_";
		input_file_suffix = ".txt";
		output_file_prefix = "_bool_pgss_";
		output_file_suffix = "_res.txt";
		break;
	default:
		break;
	}
	//edge query process
	QueryPairData* dataArray = new QueryPairData[query_data_pairs];
	int datanum = readRandomFileToDataArray(input_dir + dataset_name + input_file_prefix + to_string(num) + input_file_suffix, dataArray);
#if defined(DEBUG) || defined(HINT)
	cout << "****************** timeslot = " << num << " ******************" << endl;
#endif
	ofstream resultFile, timeFile;
	if (write) {
		char dir_path[FILENAME_MAX];
		strcpy(dir_path, output_dir.c_str());
		if (createDirectory(dir_path) != 0) {
			cout << "CreateDirectory error, Path = " << dir_path << endl;
			return -1;
		}
		resultFile.open(output_dir + dataset_name + output_file_prefix + to_string(num) + output_file_suffix);
		if (!resultFile.is_open()) {
			cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + to_string(num) + output_file_suffix) << endl;
			return -1;
		}
		timeFile.open(output_dir + dataset_name + output_file_prefix + to_string(num) + time_file_suffix);
		if (!timeFile.is_open()) {
			cout << "Error in open file, Path = " << (output_dir + output_file_prefix + to_string(num) + time_file_suffix) << endl;
			return -1;
		}
	}

	double sumTime = 0, sumTime_perquery = 0;
	int ones = 0;
	timeval tp1, tp2;
	for (int m = 0; m < query_times; m++) {
		sumTime_perquery = 0;
		for (int n = 0; n < datanum; n++) {
			int64_t res;
			gettimeofday( &tp1, NULL);
			res = horae->edgeQuery(dataArray[n].source, dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
			gettimeofday( &tp2, NULL);
			double delta_t = (tp2.tv_sec - tp1.tv_sec) * 1000000 +  (tp2.tv_usec - tp1.tv_usec);
    		sumTime_perquery += delta_t;
			if (write && m == 0) {
				if (res > 0) ones++;
				if(n == (datanum - 1)) {
					resultFile << ((res > 0) ? 1 : 0);
					timeFile  << delta_t;
					break;
				}
				else {
					resultFile << ((res > 0) ? 1 : 0) << endl;
					timeFile  << delta_t << endl;
				}
			}
		}
		sumTime += (sumTime_perquery / (double)datanum);
	}
	if (write) {
		resultFile.flush();			
		resultFile.close();
		timeFile.flush();
		timeFile.close();
	}
	delete[] dataArray;
#if defined(DEBUG) || defined(HINT)
	double mseconds = (double)(sumTime / (double)query_times) / 1000;
	printf("Timeslot = %d, Query Times = %d, Query Avg Time = %lf ms\n\n", num, query_times, mseconds);
#endif
	return 0;
}
int edgeExistencePgssTest_para(Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag) {
	vector<thread*> childs;
	thread* child = NULL;
	for(int i = 0; i < num.size(); i++) {
		child = new thread(edgeExistencePgssTest_single, horae, input_dir, output_dir, dataset_name, num[i], query_times, write, flag);
		childs.push_back(child);

	}
	for(int i = 0; i < childs.size(); i++) {
		if(childs[i] != NULL)
			childs[i]->join();
	}
	return 0;
}
int nodeFrequencePgssTest_single(Horae* horae, string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write, int flag, int line) {
	string input_file_prefix = "";
	string input_file_suffix = "";
	string output_file_prefix = "";
	string output_file_suffix = "";
	string time_file_suffix = "_time.txt";
	switch (flag)
	{
	case 1:
		input_file_prefix = "_random_node_frequence_in_";
		input_file_suffix = "_sorted.txt";
		output_file_prefix = "_node_frequence_in_";
		output_file_suffix = "_res.txt";
		break;
	case 2:
		input_file_prefix = "_random_node_frequence_out_";
		input_file_suffix = "_sorted.txt";
		output_file_prefix = "_node_frequence_out_";
		output_file_suffix = "_res.txt";
		break;
	default:
		break;
	}
	//node query process
	QueryPairData* dataArray = new QueryPairData[query_data_pairs];
	int datanum = readRandomFileToDataArray(input_dir + dataset_name + input_file_prefix + to_string(num) + input_file_suffix, dataArray);
#if defined(DEBUG) || defined(HINT)
	cout << "****************** timeslot = " << num << " ******************" << endl;
#endif
	ofstream resultFile, timeFile;
	if (write) {
		char dir_path[FILENAME_MAX];
		strcpy(dir_path, output_dir.c_str());
		if (createDirectory(dir_path) != 0) {
			cout << "CreateDirectory error, Path = " << dir_path << endl;
			return -1;
		}
		
		if(line == 0) {
			resultFile.open(output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num) + output_file_suffix);
			if (!resultFile.is_open()) {
				cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num) + output_file_suffix) << endl;
				return -1;
			}
			timeFile.open(output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num) + time_file_suffix);
			if (!timeFile.is_open()) {
				cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num) + time_file_suffix) << endl;
				return -1;
			}
		}
		else {	// append
			resultFile.open(output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num) + output_file_suffix, ios::app);
			cout << "append " << (output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num) + output_file_suffix) << endl;
			if (!resultFile.is_open()) {
				cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num) + output_file_suffix) << endl;
				return -1;
			}
			timeFile.open(output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num) + time_file_suffix, ios::app);
			cout << "append " << (output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num) + time_file_suffix) << endl;
			if (!timeFile.is_open()) {
				cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num) + time_file_suffix) << endl;
				return -1;
			}
		}
	}

	double sumTime = 0, sumTime_perquery = 0;
	timeval tp1, tp2;
	for (int m = 0; m < query_times; m++) {
		sumTime_perquery = 0;
		for (int n = 0; n < datanum; n++) {
			if(m == 0 && n < line) 
				continue;
			int64_t res;
			gettimeofday( &tp1, NULL);
			res = horae->nodeQuery(dataArray[n].source, (int)dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);			
			gettimeofday( &tp2, NULL);
			double delta_t = (tp2.tv_sec - tp1.tv_sec) * 1000000 +  (tp2.tv_usec - tp1.tv_usec);
    		sumTime_perquery += delta_t;
			if (write && m == 0) {
				if(n == (datanum - 1)) {
					resultFile << res;
					timeFile  << delta_t;
					break;
				}
				else {
					resultFile << res << endl;
					timeFile  << delta_t << endl;
				}
			}
		}
		sumTime += (sumTime_perquery / (double)datanum);
	}

	if (write) {
		resultFile.flush();
		timeFile.flush();
		resultFile.close();
		timeFile.close();
	}
	delete[] dataArray;
#if defined(DEBUG) || defined(HINT)
	double mseconds = (double)(sumTime / (double)query_times) / 1000;
	printf("Timeslot = %d, Query Times = %d, Query Avg Time = %lf ms\n\n", num, query_times, mseconds);
#endif
	return 0;
}
int nodeFrequencePgssTest_para(Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line) {
	vector<thread*> childs;
	thread* child = NULL;
	for(int i = 0; i < num.size(); i++) {
		child = new thread(nodeFrequencePgssTest_single, horae, input_dir, output_dir, dataset_name, num[i], query_times, write, flag, line);
		childs.push_back(child);

	}
	for(int i = 0; i < childs.size(); i++) {
		if(childs[i] != NULL)
			childs[i]->join();
	}
	return 0;
}


int edgeFrequenceBaselineTest_seq(string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write) {
	string input_file_prefix = dataset_name + "_random_edge_frequence_";
	string input_file_suffix = "_sorted.txt";
	string output_file_prefix = dataset_name + "_edge_frequence_baseline_";
	string output_file_suffix = "_res.txt";
	string time_file_suffix = "_time.txt";
	QueryPairData* dataArray = new QueryPairData[query_data_pairs];
	for (int i = 0; i < num.size(); i++) {
		//edge query process
		int datanum = readRandomFileToDataArray(input_dir + input_file_prefix + to_string(num[i]) + input_file_suffix, dataArray);
#if defined(DEBUG) || defined(HINT)
		cout << "****************** timeslot = " << num[i] << " ******************" << endl;
#endif
		ofstream resultFile, timeFile;
		if (write) {
			char dir_path[FILENAME_MAX];
			strcpy(dir_path, output_dir.c_str());
			if (createDirectory(dir_path) != 0) {
				cout << "CreateDirectory Error, Path = " << dir_path << endl;
				return -1;
			}
			resultFile.open(output_dir + output_file_prefix + to_string(num[i]) + output_file_suffix);
			if (!resultFile.is_open()) {
				cout << "Error in open file, Path = " << (output_dir + output_file_prefix + to_string(num[i]) + output_file_suffix) << endl;
				return -1;
			}
			timeFile.open(output_dir + output_file_prefix + to_string(num[i]) + time_file_suffix);
			if (!timeFile.is_open()) {
				cout << "Error in open file, Path = " << (output_dir + output_file_prefix + to_string(num[i]) + output_file_suffix) << endl;
				return -1;
			}
		}
		double sumTime = 0, sumTime_perquery = 0;
		timeval tp1, tp2;
		for (int m = 0; m < query_times; m++) {
			sumTime_perquery = 0;
			for (int n = 0; n < datanum; n++) {
				gettimeofday( &tp1, NULL);
				int64_t res = edgeFrequenceBaseline(*baseline_gss, dataArray[n].source, dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
				gettimeofday( &tp2, NULL);
    			double delta_t = (tp2.tv_sec - tp1.tv_sec) * 1000000 +  (tp2.tv_usec - tp1.tv_usec);
    			sumTime_perquery += delta_t;
				if (write && (m == 0)) {
					if(n == (datanum - 1)) {
						resultFile << res;
						timeFile  << delta_t;
						break;
					}
					else {
						resultFile << res << endl;
						timeFile  << delta_t << endl;
					}
				}
			}
			sumTime += (sumTime_perquery / (double)datanum);
		}
		if (write) {
			resultFile.flush();
			timeFile.flush();
			resultFile.close();
			timeFile.close();
		}
#if defined(DEBUG) || defined(HINT)
		cout << "Query Times = " << query_times << endl;
		cout << "Query Avg Time = " << (double)(sumTime / (double)query_times) / 1000 << "ms" << endl;
		cout << endl << endl;
#endif
	}
	delete[] dataArray;
	return 0;
}
int edgeExistenceBaselineTest_seq(string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag) {
	string input_file_prefix = "";
	string input_file_suffix = "";
	string output_file_prefix = "";
	string output_file_suffix = "";
	string time_file_suffix = "_time.txt";
	switch (flag)
	{
	case 1:
		input_file_prefix = "_random_edge_existence_";
		input_file_suffix = "_sorted.txt";
		output_file_prefix = "_edge_existence_baseline_";
		output_file_suffix = "_res.txt";
		break;
	case 2:
		input_file_prefix = "_bool_";
		input_file_suffix = ".txt";
		output_file_prefix = "_bool_baseline_";
		output_file_suffix = "_res.txt";
		break;
	default:
		break;
	}
	QueryPairData* dataArray = new QueryPairData[query_data_pairs];
	for (int i = 0; i < num.size(); i++) {
		//edge query process
		int datanum = readRandomFileToDataArray(input_dir + dataset_name + input_file_prefix + to_string(num[i]) + input_file_suffix, dataArray);
#if defined(DEBUG) || defined(HINT)
		cout << "****************** timeslot = " << num[i] << " ******************" << endl;
#endif
		ofstream resultFile, timeFile;
		if (write) {
			char dir_path[FILENAME_MAX];
			strcpy(dir_path, output_dir.c_str());
			if (createDirectory(dir_path) != 0) {
				cout << "createDirectory error" << endl;
				return -1;
			}
			resultFile.open(output_dir + dataset_name + output_file_prefix + to_string(num[i]) + output_file_suffix);
			if (!resultFile.is_open()) {
				cout << "error in open file " << (output_dir + dataset_name + output_file_prefix + to_string(num[i]) + output_file_suffix) << endl;
				return -1;
			}
			timeFile.open(output_dir + dataset_name + output_file_prefix + to_string(num[i]) + time_file_suffix);
			if (!timeFile.is_open()) {
				cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + to_string(num[i]) + time_file_suffix) << endl;
				return -1;
			}
		}

		double sumTime = 0, sumTime_perquery = 0;
		int ones = 0;
		timeval tp1, tp2;
		for (int m = 0; m < query_times; m++) {
			sumTime_perquery = 0;
			for (int n = 0; n < datanum; n++) {
				gettimeofday( &tp1, NULL);
				int64_t res = edgeFrequenceBaseline(*baseline_gss, dataArray[n].source, dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
				gettimeofday( &tp2, NULL);
				double delta_t = (tp2.tv_sec - tp1.tv_sec) * 1000000 +  (tp2.tv_usec - tp1.tv_usec);
    			sumTime_perquery += delta_t;
				if (write && (m == 0)) {
					if (res > 0)   
						ones++;
					if(n == (datanum - 1)) {
						resultFile << ((res > 0) ? 1 : 0);
						timeFile  << delta_t;
						break;
					}
					else {
						resultFile << ((res > 0) ? 1 : 0) << endl;
						timeFile  << delta_t << endl;
					}
				}
			}
			sumTime += (sumTime_perquery / (double)datanum);
		}
		if (write) {
			resultFile.flush();
			timeFile.flush();
			resultFile.close();
			timeFile.close();
		}
#if defined(DEBUG) || defined(HINT)
		cout << "ones: " << ones << endl;
		cout << "Query Times = " << query_times << endl;
		cout << "Query Avg Time = " << (double)(sumTime / (double)query_times) / 1000 << "ms" << endl;
		cout << endl << endl;
#endif
	}
	delete[] dataArray;
	return 0;
}
int nodeFrequenceBaselineTest_seq(string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line) {
	string input_file_prefix = "";
	string input_file_suffix = "";
	string output_file_prefix = "";
	string output_file_suffix = "";
	string time_file_suffix = "_time.txt";
	switch (flag)
	{
	case 1:
		input_file_prefix = "_random_node_frequence_in_";
		input_file_suffix = "_sorted.txt";
		output_file_prefix = "_node_frequence_in_";
		output_file_suffix = "_res.txt";
		break;
	case 2:
		input_file_prefix = "_random_node_frequence_out_";
		input_file_suffix = "_sorted.txt";
		output_file_prefix = "_node_frequence_out_";
		output_file_suffix = "_res.txt";
		break;
	default:
		break;
	}
	QueryPairData* dataArray = new QueryPairData[query_data_pairs];
	for (int i = 0; i < num.size(); i++) {
		//node query process
		int datanum = readRandomFileToDataArray(input_dir + dataset_name + input_file_prefix + to_string(num[i]) + input_file_suffix,dataArray);
#if defined(DEBUG) || defined(HINT)
		cout << "****************** timeslot = " << num[i] << " ******************" << endl;
#endif
		ofstream resultFile, timeFile;
		if (write) {
			char dir_path[FILENAME_MAX];
			strcpy(dir_path, output_dir.c_str());
			if (createDirectory(dir_path) != 0) {
				cout << "createDirectory error" << endl;
				return -1;
			}

			if(line == 0) {
				resultFile.open(output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num[i]) + output_file_suffix);
				if (!resultFile.is_open()) {
					cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num[i]) + output_file_suffix) << endl;
					return -1;
				}
				timeFile.open(output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num[i]) + time_file_suffix);
				if (!timeFile.is_open()) {
					cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num[i]) + time_file_suffix) << endl;
					return -1;
				}
			}
			else {	// append
				resultFile.open(output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num[i]) + output_file_suffix, ios::app);
				cout << "append " << (output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num[i]) + output_file_suffix) << endl;
				if (!resultFile.is_open()) {
					cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num[i]) + output_file_suffix) << endl;
					return -1;
				}
				timeFile.open(output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num[i]) + time_file_suffix, ios::app);
				cout << "append " << (output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num[i]) + time_file_suffix) << endl;
				if (!timeFile.is_open()) {
					cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + "baseline_" + to_string(num[i]) + time_file_suffix) << endl;
					return -1;
				}
			}
		}

		double sumTime = 0, sumTime_perquery = 0;
		timeval tp1, tp2;
		for (int m = 0; m < query_times; m++) {
			sumTime_perquery = 0;
			for (int n = 0; n < datanum; n++) {
				if((m == 0) && (n < line)) 
					continue;
				gettimeofday( &tp1, NULL);
				int64_t res = nodeFrequenceBaseline(*baseline_gss, dataArray[n].source, (int)dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
				gettimeofday( &tp2, NULL);
    			double delta_t = (tp2.tv_sec - tp1.tv_sec) * 1000000 +  (tp2.tv_usec - tp1.tv_usec);
    			sumTime_perquery += delta_t;
				if (write && m == 0) {
					if(n == (datanum - 1)) {
						resultFile << res;
						timeFile  << delta_t;
						break;
					}
					else {
						resultFile << res << endl;
						timeFile  << delta_t << endl;
					}
				}
			}
			sumTime += (sumTime_perquery / (double)datanum);
		}
		if (write) {
			resultFile.flush();
			timeFile.flush();
			resultFile.close();
			timeFile.close();
		}
#if defined(DEBUG) || defined(HINT)
		cout << "Query Times = " << query_times << endl;
		cout << "Query Avg Time = " << (double)(sumTime / (double)query_times) / 1000 << "ms" << endl;
		cout << endl << endl;
#endif
	}
	delete[] dataArray;
	return 0;
}
int edgeFrequencePgssTest_seq(Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write) {
	string input_file_prefix = dataset_name + "_random_edge_frequence_";
	string input_file_suffix = "_sorted.txt";
	string output_file_prefix = dataset_name + "_edge_frequence_pgss_";
	string output_file_suffix = "_res.txt";
	string time_file_suffix = "_time.txt";
	
	QueryPairData* dataArray = new QueryPairData[query_data_pairs];
	for (int i = 0; i < num.size(); i++) {
		//edge query process
		int datanum = readRandomFileToDataArray(input_dir + input_file_prefix + to_string(num[i]) + input_file_suffix,dataArray);
#if defined(DEBUG) || defined(HINT)
		cout << "****************** timeslot = " << num[i] << " ******************" << endl;
#endif
		ofstream resultFile, timeFile;
		if (write) {
			char dir_path[FILENAME_MAX];
			strcpy(dir_path, output_dir.c_str());
			if (createDirectory(dir_path) != 0) {
				cout << "createDirectory error" << endl;
				return -1;
			}
			resultFile.open(output_dir + output_file_prefix + to_string(num[i]) + output_file_suffix);
			if (!resultFile.is_open()) {
				cout << "Error in open file, Path = " << (output_dir + output_file_prefix + to_string(num[i]) + output_file_suffix) << endl;
				return -1;
			}
			timeFile.open(output_dir + output_file_prefix + to_string(num[i]) + time_file_suffix);
			if (!timeFile.is_open()) {
				cout << "Error in open file, Path = " << (output_dir + output_file_prefix + to_string(num[i]) + output_file_suffix) << endl;
				return -1;
			}
		}
		double sumTime = 0, sumTime_perquery = 0;
		timeval tp1, tp2;
		for (int m = 0; m < query_times; m++) {
			sumTime_perquery = 0;
			for (int n = 0; n < datanum; n++) {
				int64_t res;
				gettimeofday( &tp1, NULL);
				res = horae->edgeQuery(dataArray[n].source, dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
				gettimeofday( &tp2, NULL);
				double delta_t = (tp2.tv_sec - tp1.tv_sec) * 1000000 +  (tp2.tv_usec - tp1.tv_usec);
    			sumTime_perquery += delta_t;
				if (write && (m == 0)) {
					if(n == (datanum - 1)) {
						resultFile << res;
						timeFile  << delta_t;
						break;
					}
					else {
						resultFile << res << endl;
						timeFile  << delta_t << endl;
					}
				}
			}
			sumTime += (sumTime_perquery / (double)datanum);
		}
		if (write) {
			resultFile.flush();
			timeFile.flush();
			resultFile.close();
			timeFile.close();
		}
#if defined(DEBUG) || defined(HINT)
		cout << "Query Times = " << query_times << endl;
		cout << "Query Avg Time = " << (double)(sumTime / (double)query_times) / 1000 << "ms" << endl;
		cout << endl << endl;
#endif
	}
	delete[] dataArray;
	return 0;
}
int edgeExistencePgssTest_seq(Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag) {
	string input_file_prefix = "";
	string input_file_suffix = "";
	string output_file_prefix = "";
	string output_file_suffix = "";
	string time_file_suffix = "_time.txt";
	switch (flag)
	{
	case 1:
		input_file_prefix = "_random_edge_existence_";
		input_file_suffix = "_sorted.txt";
		output_file_prefix = "_edge_existence_pgss_";
		output_file_suffix = "_res.txt";
		break;
	case 2:
		input_file_prefix = "_bool_";
		input_file_suffix = ".txt";
		output_file_prefix = "_bool_pgss_";
		output_file_suffix = "_res.txt";
		break;
	default:
		break;
	}
	QueryPairData* dataArray = new QueryPairData[query_data_pairs];
	for (int i = 0; i < num.size(); i++) {
		//edge query process
		int datanum = readRandomFileToDataArray(input_dir + dataset_name + input_file_prefix + to_string(num[i]) + input_file_suffix, dataArray);
#if defined(DEBUG) || defined(HINT)
		cout << "****************** timeslot = " << num[i] << " ******************" << endl;
#endif
		ofstream resultFile, timeFile;
		if (write) {
			char dir_path[FILENAME_MAX];
			strcpy(dir_path, output_dir.c_str());
			if (createDirectory(dir_path) != 0) {
				cout << "CreateDirectory error, Path = " << dir_path << endl;
				return -1;
			}
			resultFile.open(output_dir + dataset_name + output_file_prefix + to_string(num[i]) + output_file_suffix);
			if (!resultFile.is_open()) {
				cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + to_string(num[i]) + output_file_suffix) << endl;
				return -1;
			}
			timeFile.open(output_dir + dataset_name + output_file_prefix + to_string(num[i]) + time_file_suffix);
			if (!timeFile.is_open()) {
				cout << "Error in open file, Path = " << (output_dir + output_file_prefix + to_string(num[i]) + time_file_suffix) << endl;
				return -1;
			}
		}

		double sumTime = 0, sumTime_perquery = 0;
		int ones = 0;
		timeval tp1, tp2;
		for (int m = 0; m < query_times; m++) {
			sumTime_perquery = 0;
			for (int n = 0; n < datanum; n++) {
				int64_t res;
				gettimeofday( &tp1, NULL);
				res = horae->edgeQuery(dataArray[n].source, dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);			
				gettimeofday( &tp2, NULL);
				double delta_t = (tp2.tv_sec - tp1.tv_sec) * 1000000 +  (tp2.tv_usec - tp1.tv_usec);
    			sumTime_perquery += delta_t;
				if (write && (m == 0)) {
					if (res > 0) ones++;
					if(n == (datanum - 1)) {
						resultFile << ((res > 0) ? 1 : 0);
						timeFile  << delta_t;
						break;
					}
					else {
						resultFile << ((res > 0) ? 1 : 0) << endl;
						timeFile  << delta_t << endl;
					}
				}
			}
			sumTime += (sumTime_perquery / (double)datanum);
		}
		if (write) {
			resultFile.flush();			
			resultFile.close();
			timeFile.flush();
			timeFile.close();
		}
#if defined(DEBUG) || defined(HINT)
		cout << "ones: " << ones << endl;
		cout << "Query Times = " << query_times << endl;
		cout << "Query Avg Time = " << (double)(sumTime / (double)query_times) / 1000 << "ms" << endl;
		cout << endl << endl;
#endif
	}
	delete[] dataArray;
	return 0;
}
int nodeFrequencePgssTest_seq(Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line) {
	string input_file_prefix = "";
	string input_file_suffix = "";
	string output_file_prefix = "";
	string output_file_suffix = "";
	string time_file_suffix = "_time.txt";
	switch (flag)
	{
	case 1:
		input_file_prefix = "_random_node_frequence_in_";
		input_file_suffix = "_sorted.txt";
		output_file_prefix = "_node_frequence_in_";
		output_file_suffix = "_res.txt";
		break;
	case 2:
		input_file_prefix = "_random_node_frequence_out_";
		input_file_suffix = "_sorted.txt";
		output_file_prefix = "_node_frequence_out_";
		output_file_suffix = "_res.txt";
		break;
	default:
		break;
	}
	QueryPairData* dataArray = new QueryPairData[query_data_pairs];
	for (int i = 0; i < num.size(); i++) {
		//node query process
		int datanum = readRandomFileToDataArray(input_dir + dataset_name + input_file_prefix + to_string(num[i]) + input_file_suffix, dataArray);
#if defined(DEBUG) || defined(HINT)
		cout << "****************** timeslot = " << num[i] << " ******************" << endl;
#endif
		ofstream resultFile, timeFile;
		if (write) {
			char dir_path[FILENAME_MAX];
			strcpy(dir_path, output_dir.c_str());
			if (createDirectory(dir_path) != 0) {
				cout << "CreateDirectory error, Path = " << dir_path << endl;
				return -1;
			}
			
			if(line == 0) {
				resultFile.open(output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num[i]) + output_file_suffix);
				if (!resultFile.is_open()) {
					cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num[i]) + output_file_suffix) << endl;
					return -1;
				}
				timeFile.open(output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num[i]) + time_file_suffix);
				if (!timeFile.is_open()) {
					cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num[i]) + time_file_suffix) << endl;
					return -1;
				}
			}
			else {	// append
				resultFile.open(output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num[i]) + output_file_suffix, ios::app);
				cout << "append " << (output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num[i]) + output_file_suffix) << endl;
				if (!resultFile.is_open()) {
					cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num[i]) + output_file_suffix) << endl;
					return -1;
				}
				timeFile.open(output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num[i]) + time_file_suffix, ios::app);
				cout << "append " << (output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num[i]) + time_file_suffix) << endl;
				if (!timeFile.is_open()) {
					cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num[i]) + time_file_suffix) << endl;
					return -1;
				}
			}
		}

		double sumTime = 0, sumTime_perquery = 0;
		timeval tp1, tp2;
		for (int m = 0; m < query_times; m++) {
			sumTime_perquery = 0;
			for (int n = 0; n < datanum; n++) {
				if((m == 0) && (n < line)) 
					continue;
				int64_t res;
				gettimeofday( &tp1, NULL);
				res = horae->nodeQuery(dataArray[n].source, (int)dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
				gettimeofday( &tp2, NULL);
				double delta_t = (tp2.tv_sec - tp1.tv_sec) * 1000000 +  (tp2.tv_usec - tp1.tv_usec);
    			sumTime_perquery += delta_t;
				if (write && (m == 0)) {
					if(n == (datanum - 1)) {
						resultFile << res;
						timeFile  << delta_t;
						break;
					}
					else {
						resultFile << res << endl;
						timeFile  << delta_t << endl;
					}
				}
			}
			sumTime += (sumTime_perquery / (double)datanum);
		}
		if (write) {
			resultFile.flush();
			timeFile.flush();
			resultFile.close();
			timeFile.close();
		}
#if defined(DEBUG) || defined(HINT)
		cout << "Query Times = " << query_times << endl;
		cout << "Query Avg Time = " << (double)(sumTime / (double)query_times) / 1000 << "ms" << endl;
		cout << endl << endl;
#endif
	}
	delete[] dataArray;
	return 0;
}


void edgeFrequenceBaselineTest(bool para_query, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write) {
	if(para_query)
		edgeFrequenceBaselineTest_para(input_dir, output_dir, dataset_name, num, query_times, write);
	else 
		edgeFrequenceBaselineTest_seq(input_dir, output_dir, dataset_name, num, query_times, write);
}
void edgeExistenceBaselineTest(bool para_query, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag) {
	if(para_query)
		edgeExistenceBaselineTest_para(input_dir, output_dir, dataset_name, num, query_times, write, flag);
	else 
		edgeExistenceBaselineTest_seq(input_dir, output_dir, dataset_name, num, query_times, write, flag);
}
void nodeFrequenceBaselineTest(bool para_query, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line) {
	if(para_query)
		nodeFrequenceBaselineTest_para(input_dir, output_dir, dataset_name, num, query_times, write, flag, line);
	else 
		nodeFrequenceBaselineTest_seq(input_dir, output_dir, dataset_name, num, query_times, write, flag, line);
}
void edgeFrequencePgssTest(bool para_query, Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write) {
	if(para_query)
		edgeFrequencePgssTest_para(horae, input_dir, output_dir, dataset_name, num, query_times, write);
	else 
		edgeFrequencePgssTest_seq(horae, input_dir, output_dir, dataset_name, num, query_times, write);
}
void edgeExistencePgssTest(bool para_query, Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag) {
	if(para_query)
		edgeExistencePgssTest_para(horae, input_dir, output_dir, dataset_name, num, query_times, write, flag);
	else 
		edgeExistencePgssTest_seq(horae, input_dir, output_dir, dataset_name, num, query_times, write, flag);
}
void nodeFrequencePgssTest(bool para_query, Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line) {
	if(para_query)
		nodeFrequencePgssTest_para(horae, input_dir, output_dir, dataset_name, num, query_times, write, flag, line);
	else 
		nodeFrequencePgssTest_seq(horae, input_dir, output_dir, dataset_name, num, query_times, write, flag, line);
}

int edgeFrequenceFileTest(Horae* horae, string test_file, string output_dir, int query_times, bool write) {
	string output_file = "output_res.txt";
	string time_file = "output_time.txt";
	uint64_t lines = count_lines(test_file);
	QueryPairData* dataArray = new QueryPairData[lines + 10];
	
	//edge query process
	int datanum = readRandomFileToDataArray(test_file, dataArray);
	cout << "datanum = " << datanum << endl;
#if defined(DEBUG) || defined(HINT)
	cout << "****************** test_file = " << test_file << " ******************" << endl;
#endif
	ofstream resultFile, timeFile;
	if (write) {
		char dir_path[FILENAME_MAX];
		strcpy(dir_path, output_dir.c_str());
		if (createDirectory(dir_path) != 0) {
			cout << "createDirectory error" << endl;
			return -1;
		}
		resultFile.open(output_dir + "//" + output_file);
		if (!resultFile.is_open()) {
			cout << "Error in open file, Path = " << (output_dir + "//" + output_file) << endl;
			return -1;
		}
		timeFile.open(output_dir + "//" + time_file);
		if (!timeFile.is_open()) {
			cout << "Error in open file, Path = " << (output_dir + "//" + time_file) << endl;
			return -1;
		}
	}
	double sumTime = 0, sumTime_perquery = 0;
	timeval tp1, tp2;
	for (int m = 0; m < query_times; m++) {
		sumTime_perquery = 0;
		for (int n = 0; n < datanum; n++) {
			int64_t res;
			gettimeofday( &tp1, NULL);
			res = horae->edgeQuery(dataArray[n].source, dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
			gettimeofday( &tp2, NULL);
			double delta_t = (tp2.tv_sec - tp1.tv_sec) * 1000000 +  (tp2.tv_usec - tp1.tv_usec);
			sumTime_perquery += delta_t;
			if (write && (m == 0)) {
				if(n == (datanum - 1)) {
					resultFile << res;
					timeFile  << delta_t;
					break;
				}
				else {
					resultFile << res << endl;
					timeFile  << delta_t << endl;
				}
			}
		}
		sumTime += (sumTime_perquery / (double)datanum);
	}
	if (write) {
		resultFile.flush();
		timeFile.flush();
		resultFile.close();
		timeFile.close();
	}
#if defined(DEBUG) || defined(HINT)
	cout << "Query Times = " << query_times << endl;
	cout << "Query Avg Time = " << (double)(sumTime / (double)query_times) / 1000 << "ms" << endl;
	cout << endl << endl;
#endif
	delete[] dataArray;
	return 0;
}

int nodeFrequenceFileTest(Horae* horae, string test_file, string output_dir, int query_times, bool write, int flag) {
	string tag = "node-";
	if (flag == 1) {
		tag += "in-";
	}
	if (flag == 2) {
		tag += "out-";
	}
	string output_file = tag + "output_res.txt";
	string time_file = tag + "output_time.txt";
	uint64_t lines = count_lines(test_file);
	QueryPairData* dataArray = new QueryPairData[lines + 10];
	
	// node query process
	int datanum = readRandomFileToDataArray(test_file, dataArray);
	cout << "datanum = " << datanum << endl;
#if defined(DEBUG) || defined(HINT)
	cout << "****************** test_file = " << test_file << " ******************" << endl;
#endif
	ofstream resultFile, timeFile;
	if (write) {
		char dir_path[FILENAME_MAX];
		strcpy(dir_path, output_dir.c_str());
		if (createDirectory(dir_path) != 0) {
			cout << "createDirectory error" << endl;
			return -1;
		}
		resultFile.open(output_dir + "//" + output_file);
		if (!resultFile.is_open()) {
			cout << "Error in open file, Path = " << (output_dir + "//" + output_file) << endl;
			return -1;
		}
		timeFile.open(output_dir + "//" + time_file);
		if (!timeFile.is_open()) {
			cout << "Error in open file, Path = " << (output_dir + "//" + time_file) << endl;
			return -1;
		}
	}
	double sumTime = 0, sumTime_perquery = 0;
	timeval tp1, tp2;
	for (int m = 0; m < query_times; m++) {
		sumTime_perquery = 0;
		for (int n = 0; n < datanum; n++) {
			int64_t res;
			gettimeofday( &tp1, NULL);
			res = horae->nodeQuery(dataArray[n].source, (int)dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
			gettimeofday( &tp2, NULL);
			double delta_t = (tp2.tv_sec - tp1.tv_sec) * 1000000 +  (tp2.tv_usec - tp1.tv_usec);
			sumTime_perquery += delta_t;
			if (write && (m == 0)) {
				if(n == (datanum - 1)) {
					resultFile << res;
					timeFile  << delta_t;
					break;
				}
				else {
					resultFile << res << endl;
					timeFile  << delta_t << endl;
				}
			}
		}
		sumTime += (sumTime_perquery / (double)datanum);
	}
	if (write) {
		resultFile.flush();
		timeFile.flush();
		resultFile.close();
		timeFile.close();
	}
#if defined(DEBUG) || defined(HINT)
	cout << "Query Times = " << query_times << endl;
	cout << "Query Avg Time = " << (double)(sumTime / (double)query_times) / 1000 << "ms" << endl;
	cout << endl << endl;
#endif
	delete[] dataArray;
	return 0;
}



#endif // #ifndef QUERYFUNCTION_H