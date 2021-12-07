#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>
#include <ctime>
#include "TcmTimeslice.h"
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h> //_access fun

#define EPS 1e-6
const int query_data_pairs = 100000;

struct VAR { 
	unsigned int startTime;
	unsigned int width;
	unsigned int depth;
	unsigned int hash_num;
	unsigned int tg;
	unsigned int granularityLength;
};

struct QueryPairData {
	unsigned int source;
	unsigned int destination;
	unsigned int start_time;
	unsigned int end_time;
};

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

/******************* static variables *************************/
static TcmTimeslice* baseline_tcm;
/******************* static variables *************************/
//baseline test functions 
void insert_baseline(TcmTimeslice& btcm, int s, int d, int w, int t, int startT, int gl);
int baselineInsert(VAR var, string filename);
unsigned int edgeQuery_baseline(TcmTimeslice& btcm, int s, int d, int start, int end);
unsigned int nodeQuery_baseline(TcmTimeslice& btcm, int v, int type, int start, int end);
int readRandomFileToDataArray(string file, QueryPairData dataArray[]);
int baselineInsert(VAR var, string filename);
int64 edgeFrequenceBaseline(TcmTimeslice& btcm, int64 s, int64 d, int64 start, int64 end);


/***********************  tcm baseline  ***********************/
void insert_baseline(TcmTimeslice& btcm, int s, int d, int w, int t, int startT, int gl) {
	// insert directly
	string sv, dv;
	int tt = ceil((double)(t - startT) / (double)gl);
	sv = to_string(s) + "+" + to_string((int)ceil((double)tt));
	dv = to_string(d) + "+" + to_string((int)ceil((double)tt));
	btcm.insert(sv, dv, w);
}
unsigned int edgeQuery_baseline(TcmTimeslice& btcm, int s, int d, int start, int end) {
	unsigned int result = 0;
	for (int i = start; i <= end; i++) {
		string v1 = to_string(s) + "+" + to_string(i);
		string v2 = to_string(d) + "+" + to_string(i);
		result += btcm.edgeQuery(v1, v2);
	}
	return result;
}
unsigned int nodeQuery_baseline(TcmTimeslice& btcm, int v, int type, int start, int end) {
	unsigned int result = 0;
	for (int i = start; i <= end; i++) {
		string v1 = to_string(v) + "+" + to_string(i);
		result += btcm.nodeQuery(v1, type);
	}
	return result;
}

int readRandomFileToDataArray(string file, QueryPairData dataArray[]) {
	ifstream randomFile;
	randomFile.open(file);
	if (!randomFile.is_open()) {
		cout << "Error in open file, Path = " << file << endl;
		return -1;
	}
	int datanum = 0;
	int64 startPoint, endPoint, timeStart, timeEnd;
	while (!randomFile.eof()) {
		randomFile >> startPoint >> endPoint >> timeStart >> timeEnd;
		if (randomFile.fail())
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
int baselineInsert(VAR var, string filename) {
	ifstream ifs;
	ifs.open(filename);
	if (!ifs.is_open()) {
		cout << "Error in open file, Path = " << filename << endl;
		return -1;
	}
	unsigned int s, d, w, t;
	cout << "Inserting..." << endl;
	timeval matrix_s, matrix_e;
	gettimeofday( &matrix_s, NULL);
	baseline_tcm = new TcmTimeslice(var.width, var.depth, var.hash_num, var.tg);
	gettimeofday( &matrix_e, NULL);
	double matrix_time = (matrix_e.tv_sec - matrix_s.tv_sec) +  (matrix_e.tv_usec - matrix_s.tv_usec) / 1000000.0;
	cout << "Matrix Time = " << matrix_time << " s" << endl;

	int datanum = 0;
	timeval ins_start, ins_end;
	gettimeofday( &ins_start, NULL);
	timeval bpoint_start, bpoint_end;
	gettimeofday( &bpoint_start, NULL);
	while (!ifs.eof()) {
		ifs >> s >> d >> w >> t;
		if(ifs.fail())
			break;
		
		unsigned int tt = ceil((double)(t - var.startTime) / (double)var.granularityLength);
		string sv = to_string(s) + "+" + to_string((int)ceil((double)tt / (double)baseline_tcm->getTimeGranularity()));
		string dv = to_string(d) + "+" + to_string((int)ceil((double)tt / (double)baseline_tcm->getTimeGranularity()));
		baseline_tcm->insert(sv, dv, w);

		datanum++;
		if (datanum % 10000000 == 0) {
			gettimeofday( &bpoint_end, NULL);
			double bpoint_time = (bpoint_end.tv_sec - bpoint_start.tv_sec) +  (bpoint_end.tv_usec - bpoint_start.tv_usec) / 1000000.0;
			cout << datanum << ", Break Point Time = " << bpoint_time << " s" << endl;
			gettimeofday( &bpoint_start, NULL);
		}
	}
	gettimeofday( &bpoint_end, NULL);
	double bpoint_time = (bpoint_end.tv_sec - bpoint_start.tv_sec) +  (bpoint_end.tv_usec - bpoint_start.tv_usec) / 1000000.0;
	cout << datanum << ", Break Point Time = " << bpoint_time << " s" << endl;
	gettimeofday( &ins_end, NULL);
	cout << "Insertion Finished!" << endl;
	cout << "Datanum = " << datanum << endl;
	ifs.close();

	double ins_time = (ins_end.tv_sec - ins_start.tv_sec) +  (ins_end.tv_usec - ins_start.tv_usec) / 1000000.0;
	cout << "Insertion Time = " << ins_time << " s" << endl;
	return 0;
}
// parallel queries
int64 edgeFrequenceBaseline(TcmTimeslice& btcm, int64 s, int64 d, int64 start, int64 end) {
	//btcm.edgeQuery()
	unsigned int result = 0;
	for (int i = start; i <= end; i++) {
		string v1 = to_string(s) + "+" + to_string(i);
		string v2 = to_string(d) + "+" + to_string(i);
		result += btcm.edgeQuery(v1, v2);
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
	cout << "****************** timeslot = " << num << " ******************" << endl;
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
			int64 res = edgeFrequenceBaseline(*baseline_tcm, dataArray[n].source, dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
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
	double mseconds = (double)(sumTime / (double)query_times) / 1000; 
	printf("Timeslot = %d, Query Times = %d, Query Avg Time = %lf ms\n\n", num, query_times, mseconds);
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
	cout << "****************** timeslot = " << num << " ******************" << endl;
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
			int64 res = edgeFrequenceBaseline(*baseline_tcm, dataArray[n].source, dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
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
	double mseconds = (double)(sumTime / (double)query_times) / 1000; 
	printf("Timeslot = %d, Query Times = %d, Query Avg Time = %lf ms\n\n", num, query_times, mseconds);
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
int64 nodeFrequenceBaseline(TcmTimeslice& btcm, int64 v, int type, int64 start, int64 end) {
	int64 result = 0;
	for (int i = start; i <= end; i++) {
		string vertex = to_string(v) + "+" + to_string(i);
		result += btcm.nodeQuery(vertex, type);
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
	cout << "****************** timeslot = " << num << " ******************" << endl;
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
		else {	// append
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
			int64 res = nodeFrequenceBaseline(*baseline_tcm, dataArray[n].source, (int)dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
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
	double mseconds = (double)(sumTime / (double)query_times) / 1000; 
	printf("Timeslot = %d, Query Times = %d, Query Avg Time = %lf ms\n\n", num, query_times, mseconds);
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
// sequential queries
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
		cout << "****************** timeslot = " << num[i] << " ******************" << endl;
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
				int64 res = edgeFrequenceBaseline(*baseline_tcm, dataArray[n].source, dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
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
		cout << "Query Times = " << query_times << endl;
		cout << "Query Avg Time = " << (double)(sumTime / (double)query_times) / 1000 << "ms" << endl;
		cout << endl << endl;
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
		cout << "****************** timeslot = " << num[i] << " ******************" << endl;
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
				int64 res = edgeFrequenceBaseline(*baseline_tcm, dataArray[n].source, dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
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
		cout << "ones: " << ones << endl;
		cout << "Query Times = " << query_times << endl;
		cout << "Query Avg Time = " << (double)(sumTime / (double)query_times) / 1000 << "ms" << endl;
		cout << endl << endl;
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
		cout << "****************** timeslot = " << num[i] << " ******************" << endl;
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
				int64 res = nodeFrequenceBaseline(*baseline_tcm, dataArray[n].source, (int)dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
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
		cout << "Query Times = " << query_times << endl;
		cout << "Query Avg Time = " << (double)(sumTime / (double)query_times) / 1000 << "ms" << endl;
		cout << endl << endl;
	}
	delete[] dataArray;
	return 0;
}

// the functions that called by main
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
/**********************************************************/