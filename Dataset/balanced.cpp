#include <iostream>
#include <set>
#include <string>
#include <string.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

using namespace std;

int main(int argc, char* argv[]) {
	string source = "";
	string target = "";
	int num = 0;
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-dataset") == 0) {
			source = argv[++i];
		}
		if (strcmp(argv[i], "-output") == 0) {
			target = argv[++i];
		}
		if (strcmp(argv[i], "-num") == 0) {
			num = atoi(argv[++i]);
		}
	}
	ifstream ifs(source);
	if (!ifs.is_open()) {
		cout << "open source file error" << endl;
		return -1;
	}

	ofstream ofs(target);
	if (!ofs.is_open()) {
		cout << "open target file error" << endl;
		return -1;
	}
	int windows = 1;
	int datanum = 0;
	uint32_t s, d, w, t;
	while (!ifs.eof()) {
	ifs >> s >> d >> w >> t;
	if(ifs.fail())
		break;
		ofs << s << " " << d << " " << w << " " << windows << endl;
		datanum++;
		if (datanum == num) {
			windows++;
			datanum = 0;
		}
	}
	ofs.flush();
	ofs.close();
	ifs.close();
	cout << " finished!" << endl;
	return 0;
}
