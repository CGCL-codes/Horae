#include "QueryFunction.h"
#include <iomanip>

time_type getDatasetStartTime(string datasetPath);

int main(int argc, char* argv[]) {
	cout << fixed;
#if defined(DEBUG) || defined(HINT)  
	cout << setprecision(7);
	timeval main_start, main_end;
	gettimeofday( &main_start, NULL);

	for (int i = 0; i < argc; i++) {
		cout << argv[i] << " ";
	}
	cout << endl << endl;
#endif
	// varibles
	time_type startTime;
	uint32_t width, depth;
	uint32_t granularityLength = 61200, gl = 1, slot = 2, fingerprintLen = 7;
	string back_addr = "";

	int dataset = 3;
	int query_times = 1;						// query times
	string filename, input_dir, output_dir;		// dataset filepath, test file folder path , output file folder path
	string dataset_name;
	vector<int> num;
	int efflag = 0, eeflag = 0, nfflag = 0; 	// edge frequence query, edge existence query, node frequence query
	bool writeflag = false;						// the flag of writing results to files
	int node_query_flag = 0;					// 1-node_in_query, 2-node_out_query
	int edge_existence_flag = 1;				// 1-edge_existence_query, 2-bool_query
	int line = 0;
	bool para_query = true;  					// 0-sequential query, 1-parallel query

	uint32_t row_addrs = 4, column_addrs = 4;
	bool edge_file_test = false;
	bool node_file_test = false;
	
	// Command-line parameters
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-dataset") == 0) {
			dataset = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-gl") == 0) {
			granularityLength = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-slot") == 0) {
			slot = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-fplength") == 0) {
			fingerprintLen = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-edgeweight") == 0) {
			efflag = 1;
		}
		if (strcmp(argv[i], "-edgeexistence") == 0) {
			eeflag = 1;
		}
		if (strcmp(argv[i], "-nodeinweight") == 0) {
			nfflag = 1;
			node_query_flag = 1;
		}
		if (strcmp(argv[i], "-nodeoutweight") == 0) {
			nfflag = 1;
			node_query_flag = 2;
		}
		if (strcmp(argv[i], "-bool") == 0) {
			edge_existence_flag = 2;
		}
		if (strcmp(argv[i], "-write") == 0) {
			writeflag = true;
		}
		if (strcmp(argv[i], "-para_query") == 0) {
			para_query = true;
		}
		if (strcmp(argv[i], "-seq_query") == 0) {
			para_query = false;
		}
		if (strcmp(argv[i], "-row_addrs") == 0) {
			row_addrs = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-col_addrs") == 0) {
			column_addrs = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-qtimes") == 0) {
			query_times = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-line") == 0) {		// used for the process of writing results to files
			line = atoi(argv[++i]);
		}
	}
	
	back_addr = "-gss-timeslice-" + to_string(row_addrs) + "x" + to_string(column_addrs) + "-res";
		
	switch (dataset) {
		case 1:
			filename = ".//Dataset//lkml";
			input_dir = ".//TestFiles//lkml-10w-86400//input//";
			output_dir = ".//TestFiles//lkml-10w-86400//output//";
			dataset_name = "lkml";
			num = { 8, 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560 };
			width = 2680;
			depth = 2680;
			break;
		case 2:
			filename = ".//Dataset//wiki-talk";
			input_dir = ".//TestFiles//wiki-talk//input//";
			output_dir = ".//TestFiles//wiki-talk//output//";
			dataset_name = "wiki-talk";
			num = { 32, 64, 128, 256, 512, 1024, 2048, 3072, 4096, 5120 };
			width = 13500;
			depth = 13500;
			break;
		case 3:
			filename = ".//Dataset//stackoverflow";
			input_dir = ".//TestFiles//stackoverflow//input//";
			output_dir = ".//TestFiles//stackoverflow//output//";
			dataset_name = "stackoverflow";
			num = { 8, 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560 };
			width = 20396;
			depth = 20396;
			break;
		case 4:
			filename = ".//Dataset//caida";
			input_dir = ".//TestFiles//caida//input//";
			output_dir = ".//TestFiles//caida//output//";
			dataset_name = "caida";
			num = { 1024, 2048, 3072, 4096, 5120, 6144, 7168, 8192, 9216, 10240, 11264, 12288 };
			width = 55000;
			depth = 55000;
			break;
		default:
			break;
	}

	// Command-line parameters
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-vector") == 0) {
			num.clear();
			while(true) {
				if((i + 1) >= argc) {
					break;
				}
				if(argv[i + 1][0] < '0' || argv[i + 1][0] > '9') {
					break;
				}
				else {
					num.push_back(atoi(argv[++i]));
				}
			}
		}
		if (strcmp(argv[i], "-input_dir") == 0) {
			input_dir = argv[++i];
			input_dir += "//";
		}
		if (strcmp(argv[i], "-output_dir") == 0) {
			output_dir = argv[++i];
			output_dir += "//";
		}
		if (strcmp(argv[i], "-filename") == 0) {
			filename = argv[++i];
		}
		if (strcmp(argv[i], "-w") == 0) {
			width = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-d") == 0) {
			depth = atoi(argv[++i]);
		}
	}
	startTime = getDatasetStartTime(filename);
	VAR gss_timeslice_var = { startTime, granularityLength, gl, width, depth, fingerprintLen, row_addrs, column_addrs };
#if defined(DEBUG) || defined(HINT)
	cout << "*******************************************************" << endl;
	cout << "Print Infomation" << endl;
	cout << "GssTimeslice_VAR: startTime = " << gss_timeslice_var.startTime 
		 << ", granularityLength = " << gss_timeslice_var.granularityLength 
		 << ", gl = " << gss_timeslice_var.gl 
		 << ", width = " << gss_timeslice_var.width 
		 << ", depth = " << gss_timeslice_var.depth 
		 << ", fingerprintLen = " << gss_timeslice_var.fingerprintLen << endl;
#endif
	string test_situation_dir = dataset_name + "_gl_" + to_string(gss_timeslice_var.granularityLength) + "_" + to_string(gss_timeslice_var.width) + "_" + 
		to_string(gss_timeslice_var.depth) + "_" + to_string(SLOTNUM) + "_" + to_string(gss_timeslice_var.fingerprintLen) + back_addr + "//";
	
	output_dir += test_situation_dir;
	// cout << "test_situation_dir = " << test_situation_dir << endl;
	// cout << "output_dir = " << output_dir << endl;

	char dir_path[FILENAME_MAX];
	strcpy(dir_path, output_dir.c_str());
	if (createDirectory(dir_path) != 0) {
		cout << "Create Directory error" << endl;
		return -1;
	}
#if defined(DEBUG) || defined(HINT)
	cout << "dataset: " << filename << endl;
	cout << "input_dir: " << input_dir << endl;
	cout << "output_dir: " << output_dir << endl;
	cout << "write flag = " << writeflag << endl;
	cout << "vector num: ";
	for (int i = 0; i < num.size(); i++) {
		cout << num[i] << " ";
	}
	cout << endl;
	cout << "*******************************************************" << endl << endl;
#endif

	cout << "****************** GSS + Timeslice insert start *****************" << endl;
	baselineInsert(gss_timeslice_var, filename);
	cout << "****************** GSS + Timeslice insert end *******************" << endl << endl;
	if (efflag == 1) {
		cout << "**************** GSS + Timeslice frequence start ****************" << endl;
		edgeFrequenceBaselineTest(para_query, input_dir, output_dir, dataset_name, num, query_times, writeflag);
		cout << "***************** GSS + Timeslice frequence end *****************" << endl << endl;
	}
	if (eeflag == 1) {
		cout << "**************** GSS + Timeslice existence start ****************" << endl;
		edgeExistenceBaselineTest(para_query, input_dir, output_dir, dataset_name, num, query_times, writeflag, edge_existence_flag);
		cout << "***************** GSS + Timeslice existence end *****************" << endl << endl;
	}
	if (nfflag == 1) {
		cout << "************* GSS + Timeslice node frequence start **************" << endl;
		nodeFrequenceBaselineTest(para_query, input_dir, output_dir, dataset_name, num, query_times, writeflag, node_query_flag, line);
	}

	gettimeofday( &main_end, NULL);
	double main_time = (main_end.tv_sec - main_start.tv_sec) + (main_end.tv_usec - main_start.tv_usec) / 1000000.0;
	cout << endl << "This program lasts for " << main_time / 60.0 << " min" << endl;
	return 0;
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
