//测试边查、点查等结果
#include <iomanip>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>
#include <ctime>
#include "HORAE.h"
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h> //_access fun

const int query_data_pairs = 100000;

struct HORAE_VAR { 
	time_type startTime;
	int timesliceLength;
	unsigned int gl;
	unsigned int width;
	unsigned int depth;
	int fingerprintLength;
};

struct QueryPairData {
	unsigned int source;
	unsigned int destination;
	unsigned int start_time;
	unsigned int end_time;
};

static Layer* baseline_layer;
static Horae* horae_sequential;
static Horae* horae_parallel;
int isFolderExist(char* folder);
int createDirectory(char* sPathName);
int readRandomFileToDataArray(string file, QueryPairData dataArray[]);
unsigned int getDatasetStartTime(string datasetPath);
int baselineInsert(HORAE_VAR var, string filename);
int horaeSequentialInsert(HORAE_VAR var, string filename);
int insert_horae_parallel(Horae* pg, unsigned int fpLength, int level, string filename, int line);
int horaeParallelInsert(HORAE_VAR var, string filename);
int edgeFrequenceHoraeTest_seq(Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write);
int nodeFrequenceHoraeTest_seq(Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line);


int main(int argc, char* argv[]) {
	//强制使用小数,防止使用科学计数法
    cout << fixed;
    //控制显示的精度，控制小数点后面的位数
    cout << setprecision(7);

	//HORAE_VAR varibles  //int granularityLength = 43200;  //int granularityLength = 604800;// 2592000 one month

	unsigned int width, depth;
	
	string back_addr = "_20200906_HORAE_para";
	int query_times = 1;						//查询的次数

    string filename = "..//..//Dataset//lkml";
	string input_dir = "..//..//TestFiles//lkml-10w-86400//input//";
	string output_dir = "..//..//TestFiles//lkml-10w-86400//output//";
	string dataset_name = "lkml";
    string txt_name = "";
	vector<int> num = { 8, 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560 };
	
	bool parallel_insert = false;				//是否并行插入
	bool writeflag = true;						//是否将测试结果写入到文件
	int node_query_flag = 1;					//1-node_in_query, 2-node_out_query
    time_type startTime = getDatasetStartTime(filename);
    HORAE_VAR horae_var = { startTime, 86400, 1, 740, 739, 14 };

	string test_situation_dir = "lkml_gl_86400_740_739_14" + back_addr + +"//";
	output_dir += test_situation_dir;

	char dir_path[FILENAME_MAX];
	strcpy(dir_path, output_dir.c_str());
	if (createDirectory(dir_path) != 0) {
		cout << "createDirectory error" << endl;
		return -1;
	}
    horaeParallelInsert(horae_var, filename);
    //horaeSequentialInsert(horae_var, filename);
    //edgeFrequenceHoraeTest_seq(horae_parallel, input_dir, output_dir, dataset_name, num, query_times, writeflag);
    //edgeFrequenceHoraeTest_seq(horae_sequential, input_dir, output_dir, dataset_name, num, query_times, writeflag);
    nodeFrequenceHoraeTest_seq(horae_parallel, input_dir, output_dir, dataset_name, num, query_times, writeflag, 1, 0);
    nodeFrequenceHoraeTest_seq(horae_parallel, input_dir, output_dir, dataset_name, num, query_times, writeflag, 2, 0);
    //nodeFrequenceHoraeTest_seq(horae_sequential, input_dir, output_dir, dataset_name, num, query_times, writeflag, 1, 0);
    //nodeFrequenceHoraeTest_seq(horae_sequential, input_dir, output_dir, dataset_name, num, query_times, writeflag, 2, 0);

	return 0;
}

time_type getDatasetStartTime(string datasetPath) {
	ifstream ifs;
	ifs.open(datasetPath);
	if(!ifs.is_open()) {
		cout << "Open dataset error! Path = " << datasetPath << endl;
		return -1;
	}
	unsigned int s, d, startTime;
	weight_type w;
	ifs >> s >> d >> w >> startTime;
	ifs.close();
	if(startTime > 1) 
		return startTime - 1;
	else
		return -1;
}

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

int baselineInsert(HORAE_VAR var, string filename) {
	ifstream ifs;
	ifs.open(filename);
	if (!ifs.is_open()) {
		cout << "Error in open file, Path = " << filename << endl;
		return -1;
	}
	unsigned int s, d, t;
	weight_type w;
	cout << "Inserting..." << endl;
	baseline_layer = new Layer(var.gl, var.width, var.depth, var.fingerprintLength);

	int datanum = 0;
	while (!ifs.eof()) {
		ifs >> s >> d >> w >> t;
		int tt = ceil((double)(t - var.startTime) / (double)var.timesliceLength);
		string sv = to_string(s) + "+" + to_string((int)ceil((double)tt / (double)baseline_layer->granularity));
		string dv = to_string(d) + "+" + to_string((int)ceil((double)tt / (double)baseline_layer->granularity));
		baseline_layer->insert(sv, dv, w);

		datanum++;
		if (datanum % 10000000 == 0) {
			cout << datanum << endl;
		}
	}
	cout << "Insertion Finished!" << endl;
	cout << "Datanum = " << datanum << endl;
	ifs.close();
	return 0;
}
int horaeSequentialInsert(HORAE_VAR var, string filename) {
	ifstream ifs;
	ifs.open(filename);
	if (!ifs.is_open()) {
		cout << "Error in open file, Path = " << filename << endl;
		return -1;
	}
	cout << "Inserting..." << endl;
	horae_sequential = new Horae(var.startTime, var.timesliceLength, var.gl, var.width, var.depth, var.fingerprintLength);
	
	unsigned int s, d, t;
	weight_type w;
	int flag = 1, level = 0, datanum = 0;
	
	while (!ifs.eof()) {
		ifs >> s >> d >> w >> t;
		horae_sequential->insert(s, d, w, t);

		datanum++;
		if (datanum % 10000000 == 0) {
			cout << datanum << endl;
		}
	}
	cout << "Insertion Finished!" << endl;
	cout << "Datanum = " << datanum << endl;
	cout << "Level = " << horae_sequential->multi_layers.size() << endl;
	ifs.close();

	return 0;
}
int insert_horae_parallel(Horae* pg, unsigned int fpLength, int level, string filename, int line) {
    ifstream ifs;
	ifs.open(filename);
	if (!ifs.is_open()) {
		cout << "error in open file " << endl;
		return -1;
	}
	ifs.seekg(fpLength);
	unsigned int s, d, t;
	weight_type w;
	int flag = 1, tag = 1;
	thread* child = NULL;
    int datanum = line;

	while (!ifs.eof()) {
		ifs >> s >> d >> w >> t;
        
		if(tag == 1) {
			cout << "(" << s << ", " << d << ", " << w << ", " << t << ") ---- " << " level = " << level << endl;
			tag = 0;
		}
		int tt = ceil((double)(t - pg->getStartTime()) / (double)pg->getTimesliceLength());
		
		if ((flag == 1) && (tt > pg->multi_layers[level]->granularity)) {
			flag = 0;
			int line = datanum + 1;
			Layer* gs = new Layer(*(pg->multi_layers[level]));
			pg->multi_layers.push_back(gs);
			unsigned int length = ifs.tellg();
			cout << "(" << s << ", " << d << ", " << w << ", " << t << ") -- " << " level = " << level << endl;
			pg->levelInsert(level + 1, s, d, w, t);
			child = new thread(insert_horae_parallel, pg, length, level + 1, filename, line);
		}
		pg->levelInsert(level, s, d, w, t);
		
		datanum++;
    }
    cout << "level " << level << " data insert = " << datanum << endl;
    cout << "level " << level << "finished!" << endl;
	if(child != NULL) {
		child->join();
	}
    //sleep(60);
    cout << "level " << level << "finished!!!" << endl;
	//delete child;
	return 0;
}
int horaeParallelInsert(HORAE_VAR var, string filename) {
	ifstream ifs;
	ifs.open(filename);
	if (!ifs.is_open()) {
		cout << "Error in open file, Path = " << filename << endl;
		return -1;
	}
	cout << "Inserting..." << endl;
	horae_parallel = new Horae(var.startTime, var.timesliceLength, var.gl, var.width, var.depth, var.fingerprintLength);

	unsigned int s, d, t;
	weight_type w;
	int flag = 1, level = 0, datanum = 0;
	thread* child = NULL;

	
	while (!ifs.eof()) {
		ifs >> s >> d >> w >> t;
		
		int tt = ceil((double)(t - var.startTime) / (double)var.timesliceLength);
		
		if ((flag == 1) && (tt > (horae_parallel->multi_layers[level]->granularity))) {
			flag = 0;
			int line = datanum + 1;
			
			Layer* gs = new Layer(*(horae_parallel->multi_layers[level]));
			horae_parallel->multi_layers.push_back(gs);
			unsigned int length = ifs.tellg();
		
			cout << "(" << s << ", " << d << ", " << w << ", " << t << ") -- " << " level = " << level << endl;
			horae_parallel->levelInsert(level + 1, s, d, w, t);
            child = new thread(insert_horae_parallel, horae_parallel, length, level + 1, filename, line);
			
		}
		horae_parallel->levelInsert(level, s, d, w, t);

		datanum++;
		if (datanum % 10000000 == 0) {
			cout << "Datanum = " << datanum << endl;
		}
	}
	
    cout << "level " << level << " data insert = " << datanum << endl;
    cout << "level " << level << "finished!" << endl;
	if(child != NULL)
		child->join();
    cout << "level " << level << "finished!!!" << endl;
	//delete child;
	cout << "Insertion Finished!" << endl;
	cout << "Datanum = " << datanum << endl;
	cout << "Level = " << horae_parallel->multi_layers.size() << endl;
	ifs.close();
	
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
	unsigned int startPoint, endPoint, timeStart, timeEnd;
	while (!randomFile.eof()) {
		randomFile >> startPoint >> endPoint >> timeStart >> timeEnd;
		dataArray[datanum].source = startPoint;
		dataArray[datanum].destination = endPoint;
		dataArray[datanum].start_time = timeStart;
		dataArray[datanum].end_time = timeEnd;
		datanum++;
		if(datanum > query_data_pairs) {
			cout << "输入数据 > 数组范围" << endl;
			break;
		}
	}
	randomFile.close();
	return datanum;
}

int edgeFrequenceHoraeTest_seq(Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write) {
	string input_file_prefix = dataset_name + "_random_edge_frequence_";
	string input_file_suffix = "_sorted.txt";
	string output_file_prefix = dataset_name + "_edge_frequence_pgss_";
	string output_file_suffix = "_res.txt";
	
	QueryPairData* dataArray = new QueryPairData[query_data_pairs];
	for (int i = 0; i < num.size(); i++) {
		//edge query process
		int datanum = readRandomFileToDataArray(input_dir + input_file_prefix + to_string(num[i]) + input_file_suffix,dataArray);
		cout << "****************** timeslot = " << num[i] << " ******************" << endl;
		ofstream resultFile;
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
		}
		for (int m = 0; m < query_times; m++) {
			for (int n = 0; n < datanum; n++) {
				unsigned int res = horae->edgeQuery(dataArray[n].source, dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
				if (write && (m == 0)) {
					if(n == (datanum - 1)) {
						resultFile << res;
						break;
					}
					else {
						resultFile << res << endl;
					}
				}
			}
		}
		if (write) {
			resultFile.flush();
			resultFile.close();
		}
		cout << "Query Times = " << query_times << endl;
		cout << endl << endl;
	}
	delete[] dataArray;
	return 0;
}
int nodeFrequenceHoraeTest_seq(Horae* horae, string input_dir, string output_dir, string dataset_name, vector<int> num, int query_times, bool write, int flag, int line) {
	string input_file_prefix = "";
	string input_file_suffix = "";
	string output_file_prefix = "";
	string output_file_suffix = "";
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
		cout << "****************** timeslot = " << num[i] << " ******************" << endl;
		ofstream resultFile;
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
			}
			else {	//在原文件后面追加写入
				resultFile.open(output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num[i]) + output_file_suffix, ios::app);
				cout << "append " << (output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num[i]) + output_file_suffix) << endl;
				if (!resultFile.is_open()) {
					cout << "Error in open file, Path = " << (output_dir + dataset_name + output_file_prefix + "pgss_" + to_string(num[i]) + output_file_suffix) << endl;
					return -1;
				}
			}
		}
		for (int m = 0; m < query_times; m++) {
			for (int n = 0; n < datanum; n++) {
				if((m == 0) && (n < line)) 
					continue;
				unsigned int res = horae->nodeQuery(dataArray[n].source, (int)dataArray[n].destination, dataArray[n].start_time, dataArray[n].end_time);
				if (write && (m == 0)) {
					if(n == (datanum - 1)) {
						resultFile << res;
						break;
					}
					else {
						resultFile << res << endl;
					}
				}
			}
		}
		if (write) {
			resultFile.flush();
			resultFile.close();
		}
		cout << "Query Times = " << query_times << endl;
		cout << endl << endl;
	}
	delete[] dataArray;
	return 0;
}
