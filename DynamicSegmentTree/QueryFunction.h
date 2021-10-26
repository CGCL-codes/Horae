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

#include "DynamicSegTree.h"


const int query_data_pairs = 100000;

struct VAR { 
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

/***************** function declaration ***********************/
int isFolderExist(char* folder);
int createDirectory(char* sPathName);
int readRandomFileToDataArray(string file, QueryPairData dataArray[]);
time_type getDatasetStartTime(string datasetPath);
// insert functions
int segmentTreeInsert(SegmentTree* segmentTree, string filename, int granularityLength, int bp);

// seq query functions
int edgeFrequenceSegTest_seq(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write);
int edgeExistenceSegTest_seq(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag);
int nodeFrequenceSegTest_seq(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line);

// para query functions
int edgeFrequenceSegTest_single(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write);
int edgeFrequenceSegTest_para(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write);
int edgeExistenceSegTest_single(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write, int flag);
int edgeExistenceSegTest_para(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag);
int nodeFrequenceSegTest_single(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write, int flag, int line);
int nodeFrequenceSegTest_para(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line);

// query functions that the main function called
void edgeFrequenceSegTest(bool para_query, SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write);
void edgeExistenceSegTest(bool para_query, SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag);
void nodeFrequenceSegTest(bool para_query, SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line);

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

time_type getDatasetStartTime(string datasetPath) {
	ifstream ifs;
	ifs.open(datasetPath);
	if(!ifs.is_open()) {
		cout << "Open dataset error! Path = " << datasetPath << endl;
		return -1;
	}
	int64_t s, d;
	weight_type w;
	time_type startTime;
	ifs >> s >> d >> w >> startTime;
	ifs.close();
	if(startTime > 0) 
		return startTime - 1;
	else
		return -1;
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
		if(datanum > query_data_pairs) {
			cout << "the input data is more than the range of the array" << endl;
			break;
		}
	}
	randomFile.close();
	return datanum;
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
int segmentTreeInsert(SegmentTree* segmentTree, string filename, int granularityLength, int bp) {
	int bpp = bp * 10000;
	ifstream ifs(filename);
	if (!ifs.is_open()) {
		cout << "Error in open file, Path = " << filename << endl;
		return -1;
	}
	time_type startTime = getDatasetStartTime(filename);
#if defined(DEBUG) || defined(TINSTIME) || defined(BINSTIME) || defined(HINT)
	cout << "Inserting..." << endl;
#endif
	int64_t s, d;
	weight_type w;
	time_type t;
	int datanum = 0;
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
			cout << "ERROR--QueryFunction--216" << endl;
#endif
	while (!ifs.eof()) {
		ifs >> s >> d >> w >> t;
		if(ifs.fail())
			break;
		int tt = ceil((double)(t - startTime) / (double)granularityLength);
		segmentTree->insert(to_string(s), to_string(d), w, tt);
		datanum++;
#if defined(DEBUG) || defined(BINSTIME)
		if (datanum % bpp == 0) {
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
#endif
	ifs.close();
	segmentTree->bucketCounting();
	return 0;
}


void edgeFrequenceSegTest(bool para_query, SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write) {
	if(para_query)
		edgeFrequenceSegTest_para(segmentTree, input_dir, output_dir, dataset_name, num, query_times, write);
	else 
		edgeFrequenceSegTest_seq(segmentTree, input_dir, output_dir, dataset_name, num, query_times, write);
}
void edgeExistenceSegTest(bool para_query, SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag) {
	if(para_query)
		edgeExistenceSegTest_para(segmentTree, input_dir, output_dir, dataset_name, num, query_times, write, flag);
	else 
		edgeExistenceSegTest_seq(segmentTree, input_dir, output_dir, dataset_name, num, query_times, write, flag);
}
void nodeFrequenceSegTest(bool para_query, SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line) {
	if(para_query)
		nodeFrequenceSegTest_para(segmentTree, input_dir, output_dir, dataset_name, num, query_times, write, flag, line);
	else 
		nodeFrequenceSegTest_seq(segmentTree, input_dir, output_dir, dataset_name, num, query_times, write, flag, line);
}

int edgeFrequenceSegTest_seq(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write) {
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
				res = segmentTree->edgeQuery(to_string(dataArray[n].source), to_string(dataArray[n].destination), dataArray[n].start_time, dataArray[n].end_time);
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
int edgeExistenceSegTest_seq(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag) {
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
				res = segmentTree->edgeQuery(to_string(dataArray[n].source), to_string(dataArray[n].destination), dataArray[n].start_time, dataArray[n].end_time);			
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
int nodeFrequenceSegTest_seq(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line) {
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
				res = segmentTree->nodeQuery(to_string(dataArray[n].source), (int)dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
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

int edgeFrequenceSegTest_single(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write) {
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
			res = segmentTree->edgeQuery(to_string(dataArray[n].source), to_string(dataArray[n].destination), dataArray[n].start_time, dataArray[n].end_time);
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
int edgeFrequenceSegTest_para(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write) {
	vector<thread*> childs;
	thread* child = NULL;
	for(int i = 0; i < num.size(); i++) {
		child = new thread(edgeFrequenceSegTest_single, segmentTree, input_dir, output_dir, dataset_name, num[i], query_times, write);
		childs.push_back(child);

	}
	for(int i = 0; i < childs.size(); i++) {
		if(childs[i] != NULL)
			childs[i]->join();
	}
	return 0;
}
int edgeExistenceSegTest_single(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write, int flag) {
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
			res = segmentTree->edgeQuery(to_string(dataArray[n].source), to_string(dataArray[n].destination), dataArray[n].start_time, dataArray[n].end_time);
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
int edgeExistenceSegTest_para(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag) {
	vector<thread*> childs;
	thread* child = NULL;
	for(int i = 0; i < num.size(); i++) {
		child = new thread(edgeExistenceSegTest_single, segmentTree, input_dir, output_dir, dataset_name, num[i], query_times, write, flag);
		childs.push_back(child);

	}
	for(int i = 0; i < childs.size(); i++) {
		if(childs[i] != NULL)
			childs[i]->join();
	}
	return 0;
}
int nodeFrequenceSegTest_single(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, int num, int query_times, bool write, int flag, int line) {
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
			res = segmentTree->nodeQuery(to_string(dataArray[n].source), (int)dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);			
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
int nodeFrequenceSegTest_para(SegmentTree* segmentTree, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line) {
	vector<thread*> childs;
	thread* child = NULL;
	for(int i = 0; i < num.size(); i++) {
		child = new thread(nodeFrequenceSegTest_single, segmentTree, input_dir, output_dir, dataset_name, num[i], query_times, write, flag, line);
		childs.push_back(child);

	}
	for(int i = 0; i < childs.size(); i++) {
		if(childs[i] != NULL)
			childs[i]->join();
	}
	return 0;
}

#endif // #ifndef QUERYFUNCTION_H