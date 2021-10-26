#include "QueryFunction.h"
#include <iomanip>

time_type getDatasetStartTime(string datasetPath);

int TreeNode::nodes = 0;

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
	//HORAE_VAR varibles  //int granularityLength = 43200;  //int granularityLength = 604800;// 2592000 one month
	time_type startTime;
	uint32_t width, depth;
	uint32_t granularityLength = 61200, gl = 1, fingerprintLen = 7;
	string back_addr = "";
	string cl = "-no-map-no-prelist";

	int dataset = 3;
	int query_times = 1;						// query times
	string filename, input_dir, output_dir;		// dataset filepath, test file folder path , output file folder path
	string dataset_name, txt_name = "";
	vector<int> num;
	int efflag = 0, eeflag = 0, nfflag = 0; 	//  edge frequence query,  edge existence query,  node frequence query
	bool writeflag = false;						// write result to file flag
	int node_query_flag = 0;					// 1-node_in_query, 2-node_out_query
	int edge_existence_flag = 1;				// 1-edge_existence_query, 2-bool_query
	int line = 0;
	bool para_query = true;  					// 0-seq result query, 1-para result query

	uint32_t row_addrs = 4, column_addrs = 4;
	bool kick = false, cache_align = false;
	int end_time = 2775;
	int bp = 100;
	bool query = false;
	
	//命令行参数
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-line") == 0) {
			line = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-dataset") == 0) {
			dataset = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-gl") == 0) {
			granularityLength = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-bp") == 0) {
			bp = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-fplength") == 0) {
			fingerprintLen = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-edgefrequence") == 0) {
			efflag = 1;
		}
		if (strcmp(argv[i], "-edgeexistence") == 0) {
			eeflag = 1;
		}
		if (strcmp(argv[i], "-nodefrequence") == 0) {
			nfflag = 1;
		}
		if (strcmp(argv[i], "-in") == 0) {
			node_query_flag = 1;
		}
		if (strcmp(argv[i], "-out") == 0) {
			node_query_flag = 2;
		}
		if (strcmp(argv[i], "-write") == 0) {
			writeflag = true;
		}
		if (strcmp(argv[i], "-qtimes") == 0) {
			query_times = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-bool") == 0) {
			edge_existence_flag = 2;
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
		if (strcmp(argv[i], "-kick") == 0) {
			kick = true;
		}
		if (strcmp(argv[i], "-cache_align") == 0) {
			cache_align = true;
		}
		if (strcmp(argv[i], "-query") == 0) {
			query = true;
		}
	}
	back_addr += "_GSS_DynSegTree-";
	if (cache_align) {
		back_addr += to_string(row_addrs) + "x" + to_string(column_addrs / 2) + "x" + "2-cache";
	}
	else {
		back_addr += to_string(row_addrs) + "x" + to_string(column_addrs);
	}
	if (kick) {
		back_addr += ("-kick" + cl + "-res");
	}
	else {
		back_addr += (cl + "-res");
	}

	switch (dataset) {
		case 1:
			filename = "..//..//Dataset//out";
			input_dir = "..//..//TestFiles//out//input//";
			output_dir = "..//..//TestFiles//out//output//";
			dataset_name = "out";
			num = { 8, 16, 32, 64, 128, 256, 384 };
			width = 1000;
			depth = 1000;
			break;
		case 2:
			filename = "..//..//Dataset//lastfm_song";
			input_dir = "..//..//TestFiles//lastfm_song//input//";
			output_dir = "..//..//TestFiles//lastfm_song//output//";
			dataset_name = "lastfm_song";
			num = { 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560, 3072};
			width = 15000;
			depth = 600;
			break;
		case 3:
			filename = "..//..//Dataset//stackoverflow";
			input_dir = "..//..//TestFiles//stackoverflow//input//";
			output_dir = "..//..//TestFiles//stackoverflow//output//";
			dataset_name = "stackoverflow";
			//num = { 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560, 3072, 3584 };
			num = { 8, 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560 };
			width = 5655;///////////
			depth = 5659;///////////
			end_time = 2775;
			break;
		case 4:
			filename = "..//..//Dataset//caida";
			input_dir = "..//..//TestFiles//caida//input//";
			output_dir = "..//..//TestFiles//caida//output//";
			dataset_name = "caida";
			num = { 1024, 2048, 3072, 4096, 5120, 6144, 7168, 8192, 9216, 10240, 11264, 12288 };
			width = 15000;
			depth = 15000;
			end_time = 13200;
			break;
		case 5:
			filename = "..//..//Dataset//wiki";
			input_dir = "..//..//TestFiles//wiki//input//";
			output_dir = "..//..//TestFiles//wiki//output//";
			dataset_name = "wiki";
			num = { 64, 128, 256, 512, 1024, 1536, 2048, 2560, 3072, 3584, 4096, 4608 };
			width = 1750;
			depth = 1800;
			end_time = 5363;
			break;
		case 6:
			filename = "..//..//Dataset//lkml";
			input_dir = "..//..//TestFiles//lkml-10w-86400//input//";
			output_dir = "..//..//TestFiles//lkml-10w-86400//output//";
			dataset_name = "lkml";
			num = { 8, 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560 };
			width = 1750;
			depth = 1800;
			end_time = 2922;
			break;
		case 7:
			filename = "..//..//Dataset//wiki-talk";
			input_dir = "..//..//TestFiles//wiki-talk//input//";
			output_dir = "..//..//TestFiles//wiki-talk//output//";
			dataset_name = "wiki-talk";
			num = { 32, 64, 128, 256, 512, 1024, 2048, 3072, 4096, 5120 };
			width = 3536;
			depth = 3536;
			end_time = 5363;
			break;
		case 8:
			filename = "..//..//Dataset//dblp";
			input_dir = "..//..//TestFiles//dblp//input//";
			output_dir = "..//..//TestFiles//dblp//output//";
			dataset_name = "dblp";
			num = { 8, 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560 };
			width = 3840;
			depth = 3840;
			break;
		case 9:
			filename = "..//..//Dataset//stk-balanced";
			input_dir = "..//..//TestFiles//stk-balanced//input//";
			output_dir = "..//..//TestFiles//stk-balanced//output//";
			dataset_name = "stk-balanced";
			//num = { 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560, 3072, 3584 };
			num = { 8, 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560 };
			width = 5655;///////////
			depth = 5659;///////////
			break;
		case 10:
			filename = "..//..//Dataset//stk-time";
			input_dir = "..//..//TestFiles//stackoverflow//input//";
			output_dir = "..//..//TestFiles//stackoverflow//output//";
			dataset_name = "stackoverflow";
			//num = { 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560, 3072, 3584 };
			num = { 8, 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560 };
			width = 5655;///////////
			depth = 5659;///////////
			break;
		default:
			break;
	}

	// command parameters
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
	VAR seg_var = { startTime, granularityLength, gl, width, depth, fingerprintLen, row_addrs, column_addrs, kick, cache_align };
#if defined(DEBUG) || defined(HINT)
	cout << "*******************************************************" << endl;
	cout << "Print Infomation" << endl;
	cout << "GSS+SegmentTree: startTime = " << seg_var.startTime 
		 << ", granularityLength = " << seg_var.granularityLength 
		 << ", gl = " << seg_var.gl 
		 << ", width = " << seg_var.width 
		 << ", depth = " << seg_var.depth 
		 << ", fingerprintLen = " << seg_var.fingerprintLen
		 << ", breakpoints = " << bp << endl;
#endif
	SegmentTree* segmentTree = new DynamicSegTree(width, depth, fingerprintLen, cache_align, kick, row_addrs, column_addrs);
	string test_situation_dir = dataset_name + "_gl_" + to_string(seg_var.granularityLength) + "_" + to_string(seg_var.width) + "_" + 
		to_string(seg_var.depth) + "_" + to_string(seg_var.fingerprintLen) + back_addr + "//";
	
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


#if defined(DEBUG) || defined(HINT)
		cout << "****************** Segment tree sequential insert start *****************" << endl;
#endif
		segmentTreeInsert(segmentTree, filename, granularityLength, bp);
#if defined(DEBUG) || defined(HINT)
		cout << "****************** Segment tree sequential insert end *******************" << endl << endl;
#endif
	cout << "TreeNode::nodes = " << TreeNode::nodes << endl;
	// }
	if (query) {
		cout << "**************** Segment tree edge frequence start ****************" << endl;
		edgeFrequenceSegTest(para_query, segmentTree, input_dir, output_dir, dataset_name, num, query_times, writeflag);
		cout << "***************** Segment tree edge frequence end *****************" << endl << endl;

		cout << "**************** Segment tree edge existence start ****************" << endl;
		edgeExistenceSegTest(para_query, segmentTree, input_dir, output_dir, dataset_name, num, query_times, writeflag, 2);
		cout << "***************** Segment tree edge existence end *****************" << endl << endl;

		cout << "************* Segment tree node out frequence start **************" << endl;
		nodeFrequenceSegTest(para_query, segmentTree, input_dir, output_dir, dataset_name, num, query_times, writeflag, 2, line);
		cout << "************** Segment tree node out frequence end ***************" << endl << endl;

		cout << "************* Segment tree node in frequence start **************" << endl;
		nodeFrequenceSegTest(para_query, segmentTree, input_dir, output_dir, dataset_name, num, query_times, writeflag, 1, line);
		cout << "************** Segment tree node in frequence end ***************" << endl << endl;
	}

// 	if (efflag == 1) {
// #if defined(DEBUG) || defined(HINT)
// 		cout << "**************** Segment tree edge frequence start ****************" << endl;
// #endif
// 		edgeFrequenceSegTest(para_query, segmentTree, input_dir, output_dir, dataset_name, num, query_times, writeflag);
// #if defined(DEBUG) || defined(HINT)
// 		cout << "***************** Segment tree edge frequence end *****************" << endl << endl;
// #endif
// 	}
// 	if (eeflag == 1) {
// #if defined(DEBUG) || defined(HINT)
// 		cout << "**************** Segment tree edge existence start ****************" << endl;
// #endif
// 		edgeExistenceSegTest(para_query, segmentTree, input_dir, output_dir, dataset_name, num, query_times, writeflag, edge_existence_flag);
// #if defined(DEBUG) || defined(HINT)
// 		cout << "***************** Segment tree edge existence end *****************" << endl << endl;
// #endif
// 	}
// 	if (nfflag == 1) {
// #if defined(DEBUG) || defined(HINT)
// 		cout << "************* Segment tree node frequence start **************" << endl;
// #endif
// 		nodeFrequenceSegTest(para_query, segmentTree, input_dir, output_dir, dataset_name, num, query_times, writeflag, node_query_flag, line);
// #if defined(DEBUG) || defined(HINT)
// 		cout << "************** Segment tree node frequence end ***************" << endl << endl;
// #endif
// 	}
	delete segmentTree;
#if defined(DEBUG) || defined(HINT)
	gettimeofday( &main_end, NULL);
	double main_time = (main_end.tv_sec - main_start.tv_sec) + (main_end.tv_usec - main_start.tv_usec) / 1000000.0;
	cout << endl << "This program lasts for " << main_time / 60.0 << " min" << endl;
#endif
	return 0;
}
