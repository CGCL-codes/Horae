// no map & no prelist
#ifndef _LayerSucClass_H
#define _LayerSucClass_H
#include "LayerSuc.h"

class LayerSucClass: public LayerSuc {
private:
	bool cache_align = false;
	bool kick = false;
public:
	LayerSucClass(uint32_t granularity, uint32_t width, uint32_t depth, uint32_t fingerprintLength, bool cache_align, bool kick, uint32_t row_addrs = 4, uint32_t column_addrs = 4);
	LayerSucClass(const LayerSucClass *layer);

	// memory improvement
	LayerSucClass(const LayerSucClass *layer, int level);

	~LayerSucClass();
	void insert(string src, string dst, weight_type weight);
	weight_type edgeQuery(string src, string dst);
	weight_type nodeQuery(string vertex, int type);		//src_type = 0 dst_type = 1
};

LayerSucClass::LayerSucClass(uint32_t granularity, uint32_t width, uint32_t depth, uint32_t fingerprintLength, bool cache_align, bool kick, uint32_t row_addrs, uint32_t column_addrs): 
LayerSuc(granularity, width, depth, fingerprintLength, row_addrs, column_addrs), cache_align(cache_align), kick(kick) {
	cout << "LayerSucClass::LayerSucClass(granularity: " << granularity 
		<< ", width: " << width <<", depth: " << depth << ", fplen: " << fingerprintLength
		<< ", cache_align: " << cache_align << ", kick: " << kick << ", row_addrs: " << row_addrs 
		<< ", column_addrs: " << column_addrs << ")" << endl;
}
LayerSucClass::LayerSucClass(const LayerSucClass *layer): LayerSuc(layer) {
	cout << "LayerSucClass::LayerSucClass(*layer)" << endl;
}
// memory improvement
LayerSucClass::LayerSucClass(const LayerSucClass *layer, int level): LayerSuc(layer, level) {
	cout << "LayerSucClass::LayerSucClass(*layer, level: " << level << ")" << endl;
}

LayerSucClass::~LayerSucClass() {
	cout << "LayerSucClass::~LayerSucClass()" << endl;
}

// src is the ID of the source node, dst is the ID of the destination node, weight is the weight of the edge.
void LayerSucClass::insert(string src, string dst, weight_type weight) {
	uint32_t hash_src = (*hfunc[0])((unsigned char*)(src.c_str()), src.length());
	uint32_t hash_dst = (*hfunc[0])((unsigned char*)(dst.c_str()), dst.length());
	//uint32_t mask = pow(2, fingerprintLength) - 1;
	uint32_t mask = (1 << fingerprintLength) - 1;
	uint32_t head = 16384; //pow(2, 14);
	uint16_t fp_src = hash_src & mask;
	if (fp_src == 0) fp_src += 1;
	uint32_t addr_src = (hash_src >> fingerprintLength) % depth;
	uint16_t fp_dst = hash_dst & mask;
	if (fp_dst == 0) fp_dst += 1;
	uint32_t addr_dst = (hash_dst >> fingerprintLength) % width;

	uint32_t k1 = (addr_src << fingerprintLength) + fp_src;
	uint32_t k2 = (addr_dst << fingerprintLength) + fp_dst;

	bool inserted = false;
	if (cache_align) 
		inserted = insertMatrixCacheline(addr_src, fp_src, addr_dst, fp_dst, weight);
	else
		inserted = insertMatrix(addr_src, fp_src, addr_dst, fp_dst, weight);
	
	if (!inserted) {
		// insert to the suc-buffer or kick-out
		if (!kick) 
			insertSucBuffer(k1, k2, weight);
		else {
			// map<uint32_t, uint32_t>::iterator it = successorIndex.find(k1);
			vector<vector<node> >::iterator it = find_if(successorAdjacencyList.begin(), successorAdjacencyList.end(), findv(k1));
			if (it != successorAdjacencyList.end()) {			// find k1->k2 in successorAdjacencyList
				vector<node>::iterator iter = find_if(it->begin(), it->end(), findx(k2));
				if (iter != it->end()) {	// if found
					iter->weight += weight;
					return;
					// map<uint32_t, uint32_t>::iterator it2 = precursorIndex.find(k2);		// insert k2->k1
					// vector<node>::iterator it2 = find_if(precursorAdjacencyList.begin(), precursorAdjacencyList.end(), findx(k2));
					// if (it2 != precursorAdjacencyList.end()) {
					// 	it2->weight += weight;
					// 	inserted = true;
					// 	return;
					// }
				}
			}
			if (!cache_align) {			// There is no k1->k2 in buffer, start kicking
				int kickNum = 0;
				//temp is the edge currently to be inserted
				uint32_t temp_addr1 = addr_src;
				uint32_t temp_addr2 = addr_dst;
				uint16_t temp_fp1 = fp_src;
				uint16_t temp_fp2 = fp_dst;
				weight_type temp_weight = weight;
				int kickflag = 1; //
				while (!inserted){
					// 找到当前边(e1)的第kickflag个地址，然后判断两个slot中序号较小的那个剔出
					// kickout the elements of the first pos(the power of two choices)
					uint32_t pos_x = (temp_addr1 + temp_fp1) % depth; 
					uint32_t pos_y;
					if(kickflag == 1){
						pos_y = (temp_addr2 + temp_fp2) % width;
					}else if(kickflag == 2){
						uint32_t tmp2 = (temp_fp2 * multiplier + increment) % modulus;
						pos_y = (temp_addr2 + tmp2) % width;

					}
					uint32_t pos = pos_x * width + pos_y;
					uint32_t insertnum[SLOTNUM];
					for (int i = 0; i < SLOTNUM; i++){
						insertnum[i] = (value[pos].src[i] >> 14) * 4 + (value[pos].dst[i] >> 14);
					}
					int kickindex = getMinIndex(insertnum, SLOTNUM);					//找到要踢的那个slot
					uint32_t kick_i = (value[pos].src[kickindex] >> 14);			//将要踢出的边(e2)信息存储到kick_变量中
					uint32_t kick_j = (value[pos].dst[kickindex] >> 14);
					uint16_t kick_fpx = (value[pos].src[kickindex] & mask);
					uint16_t kick_fpy = (value[pos].dst[kickindex] & mask);
					weight_type kick_weight = value[pos].weight[kickindex];
					// insert the element
					value[pos].src[kickindex] = temp_fp1;
					if(kickflag == 1){
						value[pos].dst[kickindex] = temp_fp2;
					}
					else if(kickflag == 2){
						value[pos].dst[kickindex] = temp_fp2 + head;
					}
					value[pos].weight[kickindex] = temp_weight;
					//temp_为边e2
					temp_fp1 = kick_fpx;
					temp_fp2 = kick_fpy;
					temp_weight = kick_weight;
					
					//根据e2存储在矩阵中的当前位置以及下标信息还原边e2的addr1、addr2
					uint32_t shifterx = temp_fp1;
					for (int v = 0; v < kick_i; v++) {
						shifterx = (shifterx * multiplier + increment) % modulus;
					}
					uint32_t tmp_h1 = pos_x;
					while (tmp_h1 < shifterx)
						tmp_h1 += depth;
					tmp_h1 -= shifterx;
					temp_addr1 = tmp_h1;

					uint32_t shiftery = temp_fp2;
					for (int v = 0; v < kick_j; v++)
						shiftery = (shiftery * multiplier + increment) % modulus;
					uint32_t tmp_h2 = pos_y;
					while (tmp_h2 < shiftery)
						tmp_h2 += width;
					tmp_h2 -= shiftery;
					temp_addr2 = tmp_h2;

					uint32_t curtmp1[4] = { 0 };
					uint32_t curtmp2[4] = { 0 }; 
					curtmp1[0] = temp_fp1;
					curtmp1[1] = (curtmp1[0] * multiplier + increment) % modulus;
					curtmp1[2] = (curtmp1[1] * multiplier + increment) % modulus;
					curtmp1[3] = (curtmp1[2] * multiplier + increment) % modulus;
					curtmp2[0] = temp_fp2;
					curtmp2[1] = (curtmp2[0] * multiplier + increment) % modulus;
					curtmp2[2] = (curtmp2[1] * multiplier + increment) % modulus;
					curtmp2[3] = (curtmp2[2] * multiplier + increment) % modulus;
					
					uint32_t addr_no = (kick_i * 4) + (kick_j);

					for (int i = 0; i < 4; i++) {
						uint32_t depth_addr = (temp_addr1 + curtmp1[i]) % depth;
						for (int j = 0; j < 4; j++) {
							if((i * 4 + j) <= addr_no)
								continue;
							int width_addr = (temp_addr2 + curtmp2[j]) % width;
							int pos = depth_addr * width + width_addr;
							
							for (int m = 0; m < SLOTNUM; m++) {
								if (value[pos].src[m] == 0 && value[pos].weight[m] == 0) {
									value[pos].src[m] = temp_fp1 + head * i;
									value[pos].dst[m] = temp_fp2 + head * j;
									value[pos].weight[m] = temp_weight;
									inserted = true;
									return;
								}
							}
							
						}
					}
					if(addr_no == 0){
						kickflag = 2;
					}else{
						kickflag = 1;
					}//avoid kick-loop
					kickNum++;
					if(kickNum > 10){
						break;
					}
				}
			
				uint32_t curk1 = (temp_addr1 << fingerprintLength) + temp_fp1;
				uint32_t curk2 = (temp_addr2 << fingerprintLength) + temp_fp2;
				if (!inserted) {
					//save to successor buffer k1->k2
					// map<uint32_t, uint32_t>::iterator it = successorIndex.find(curk1);
					vector<vector<node> >::iterator it = find_if(successorAdjacencyList.begin(), successorAdjacencyList.end(), findv(curk1));
					if (it != successorAdjacencyList.end()) {
						vector<node>::iterator iter = find_if(it->begin(), it->end(), findx(curk2));
						if (iter != it->end()) {
							iter->weight += temp_weight;
						}
						else {
							node tmpnode;
							tmpnode.key = curk2;
							tmpnode.weight = temp_weight;
							it->push_back(tmpnode);
						}
					}
					else {
						// successorIndex[k1] = n1;
						// n1++;
						node tmpnode;
						tmpnode.key = curk1;
						tmpnode.weight = 0;
						vector<node> vc;
						vc.push_back(tmpnode);
						if (curk1 != curk2) {  //k1==k2 means loop
							node newnode;
							newnode.key = curk2;
							newnode.weight = temp_weight;
							vc.push_back(newnode);
						}
						else {
							vc[0].weight += temp_weight;
						}
						successorAdjacencyList.push_back(vc);
					}
					//save to precursor buffer k2<-k1
					// map<uint32_t, uint32_t>::iterator it2 = precursorIndex.find(curk2);
					// vector<node>::iterator it2 = find_if(precursorAdjacencyList.begin(), precursorAdjacencyList.end(), findx(curk2));
					// if (it2 != precursorAdjacencyList.end()) {
					// 	it2->weight += temp_weight;
					// }
					// else {
					// 	// precursorIndex[curk2] = n2;
					// 	// n2++;
					// 	node newnode;
					// 	newnode.key = curk2;
					// 	newnode.weight = temp_weight;
					// 	precursorAdjacencyList.push_back(newnode);
					// }
				}
			}
			else {
				int kickNum = 0;
				//temp is the edge currently to be inserted
				uint32_t temp_addr1 = addr_src;
				uint32_t temp_addr2 = addr_dst;
				uint16_t temp_fp1 = fp_src;
				uint16_t temp_fp2 = fp_dst;
				weight_type temp_weight = weight;
				int kickflag = 1; //
				while (!inserted){
					// 找到当前边(e1)的第kickflag个地址，然后判断两个slot中序号较小的那个剔出
					// kickout the elements of first pos(the power of two choices)
					uint32_t pos_x = (temp_addr1 + temp_fp1) % depth; 
					uint32_t pos_y = (temp_addr2 + temp_fp2) % width;
					if(kickflag == 2){
						pos_y =  pos_y ^ (temp_fp2 % 4) % width;
					}
					uint32_t pos = pos_x * width + pos_y;
					uint32_t insertnum[SLOTNUM];
					for (int i = 0; i < SLOTNUM; i++){
						insertnum[i] = (value[pos].src[i] >> 14) * 4 + (value[pos].dst[i] >> 14);
					}
					int kickindex = getMinIndex(insertnum, SLOTNUM);
					uint32_t kick_i = (value[pos].src[kickindex] >> 14);
					uint32_t kick_j = (value[pos].dst[kickindex] >> 14);
					uint16_t kick_fpx = (value[pos].src[kickindex] & mask);
					uint16_t kick_fpy = (value[pos].dst[kickindex] & mask);
					weight_type kick_weight = value[pos].weight[kickindex];
					// insert the element
					value[pos].src[kickindex] = temp_fp1;
					value[pos].dst[kickindex] = temp_fp2;
					if(kickflag == 1){
						value[pos].dst[kickindex] = temp_fp2;
					}else if(kickflag == 2){
						value[pos].dst[kickindex] = temp_fp2 + head;
					}
					value[pos].weight[kickindex] = temp_weight;
					//temp_为边e2
					temp_fp1 = kick_fpx;
					temp_fp2 = kick_fpy;
					temp_weight = kick_weight;
					
					//根据e2存储在矩阵中的当前位置以及下标信息还原边e2的addr1、addr2
					uint32_t shifterx = temp_fp1;
					for (int v = 0; v < kick_i; v++) {
						shifterx = (shifterx * multiplier + increment) % modulus;
					}
					uint32_t tmp_h1 = pos_x;
					while (tmp_h1 < shifterx)
						tmp_h1 += depth;
					tmp_h1 -= shifterx;
					temp_addr1 = tmp_h1;

					uint32_t shiftery = temp_fp2;
					for (int v = 0; v < (kick_j / 2); v++)
						shiftery = (shiftery * multiplier + increment) % modulus;
					uint32_t tmp_h2 = (kick_j % 2 == 0) ? pos_y : (pos_y ^ (temp_fp2 % 4) % width);
					while (tmp_h2 < shiftery)
						tmp_h2 += width;
					tmp_h2 -= shiftery;
					temp_addr2 = tmp_h2;

					uint32_t curtmp1[4] = { 0, 0, 0, 0 };
					uint32_t curtmp2[2] = { 0, 0 }; 
					curtmp1[0] = temp_fp1;
					curtmp1[1] = (curtmp1[0] * multiplier + increment) % modulus;
					curtmp1[2] = (curtmp1[1] * multiplier + increment) % modulus;
					curtmp1[3] = (curtmp1[2] * multiplier + increment) % modulus;
					curtmp2[0] = temp_fp2;
					curtmp2[1] = (curtmp2[0] * multiplier + increment) % modulus;
					
					uint32_t addr_no = (kick_i * 4) + (kick_j);

					for (int i = 0; i < 4; i++) {
						uint32_t row_addr = (temp_addr1 + curtmp1[i]) % depth;
						for (int j = 0; j < 2; j++) {
							uint32_t column_addr = (temp_addr2 + curtmp2[j]) % width;
							for (int k = 0; k < 2; k++) {
								if((i * 4 + j * 2 + k) <= addr_no)
									continue;
								uint32_t pos;
								if (k == 0) {
									pos = row_addr * width + column_addr;
								}
								else {
									uint32_t column_addr_alt = (column_addr ^ (temp_fp2 % 4)) % width;
									pos = row_addr * width + column_addr_alt;
								}
								for (int m = 0; m < SLOTNUM; m++) {
									if (value[pos].src[m] == 0 && value[pos].weight[m] == 0) {
										value[pos].src[m] = temp_fp1 + head * i;
										value[pos].dst[m] = temp_fp2 + head * (j * 2 + k);
										value[pos].weight[m] = temp_weight;
										inserted = true;
										return;
									}
								}
							}
						}
					}
					if(addr_no == 0){
						kickflag = 2;
					}else{
						kickflag = 1;
					}//avoid kick-loop
					kickNum++;
					if(kickNum > 10) {
						break;
					}
				}
				uint32_t curk1 = (temp_addr1 << fingerprintLength) + temp_fp1;
				uint32_t curk2 = (temp_addr2 << fingerprintLength) + temp_fp2;
				if (!inserted) {
					//save to successor buffer k1->k2
					//map<uint32_t, uint32_t>::iterator it = successorIndex.find(curk1);
					vector<vector<node> >::iterator it = find_if(successorAdjacencyList.begin(), successorAdjacencyList.end(), findv(curk1));
					if (it != successorAdjacencyList.end()) {
					
						vector<node>::iterator iter = find_if(it->begin(), it->end(), findx(curk2));
						if (iter != it->end()) {
							iter->weight += temp_weight;
						}
						else {
							node tmpnode;
							tmpnode.key = curk2;
							tmpnode.weight = temp_weight;
							it->push_back(tmpnode);
						}
					}
					else {
						// successorIndex[curk1] = n1;
						// n1++;
						node tmpnode;
						tmpnode.key = curk1;
						tmpnode.weight = 0;
						vector<node> vc;
						vc.push_back(tmpnode);
						if (curk1 != curk2) {  //k1==k2 means loop
							node newnode;
							newnode.key = curk2;
							newnode.weight = temp_weight;
							vc.push_back(newnode);
						}
						else {
							vc[0].weight += temp_weight;
						}
						successorAdjacencyList.push_back(vc);
					}
					
					//save to precursor buffer k2<-k1
					//map<uint32_t, uint32_t>::iterator it2 = precursorIndex.find(curk2);
					// vector<node>::iterator it2 = find_if(precursorAdjacencyList.begin(), precursorAdjacencyList.end(), findx(curk2));
					// if (it2 != precursorAdjacencyList.end()) {
					// 	it2->weight += temp_weight;
					// }
					// else {
					// 	node tmpnode;
					// 	tmpnode.key = curk2;
					// 	tmpnode.weight = temp_weight;
					// 	precursorAdjacencyList.push_back(tmpnode);
					// }
				}
			}
		}
	}
	return;
}
// src is the ID of the source node, dst is the ID of the destination node, return the weight of the edge
weight_type LayerSucClass::edgeQuery(string src, string dst) {
	uint32_t hash_src = (*hfunc[0])((unsigned char*)(src.c_str()), src.length());
	uint32_t hash_dst = (*hfunc[0])((unsigned char*)(dst.c_str()), dst.length());
	uint32_t mask = pow(2, fingerprintLength) - 1;
	uint16_t fp_src = hash_src & mask;
	if (fp_src == 0) fp_src += 1;
	uint32_t addr_src = (hash_src >> fingerprintLength) % depth;
	uint16_t fp_dst = hash_dst & mask;
	if (fp_dst == 0) fp_dst += 1;
	uint32_t addr_dst = (hash_dst >> fingerprintLength) % width;
	
	// query the matrix
	weight_type res = 0;
	if (cache_align) 
		res = edgeQueryMatrixCacheline(addr_src, fp_src, addr_dst, fp_dst);
	else
		res = edgeQueryMatrix(addr_src, fp_src, addr_dst, fp_dst);
	if (res != 0)	// find in the matrix
		return res;
	else {
		// query the suc-buffer
		uint32_t k1 = (addr_src << fingerprintLength) + fp_src;
		uint32_t k2 = (addr_dst << fingerprintLength) + fp_dst;
		return edgeQuerySucBuffer(k1, k2);
	}
	return 0;
}
// vertex is the ID of the queried node, function for node query. type 0 is for successor query, type 1 is for precusor query
weight_type LayerSucClass::nodeQuery(string vertex, int type) { // vertex is the ID of the queried node, function for node query.
	weight_type weight = 0;
	uint32_t hash_vertex = (*hfunc[0])((unsigned char*)(vertex.c_str()), vertex.length());
	uint32_t mask = pow(2, fingerprintLength) - 1;
	uint16_t fp = hash_vertex & mask;
	if (fp == 0) fp += 1;
	
	if (type == 0) {
		uint32_t addr = (hash_vertex >> fingerprintLength) % depth;
		if (cache_align)
			weight += nodeQueryMatrixCacheline(addr, fp, type);
		else
			weight += nodeQueryMatrix(addr, fp, type);

		// query the suc-buffer
		uint32_t k1 = (addr << fingerprintLength) + fp;
		weight += nodeQuerySucBuffer(k1, type);
	}
	else if (type == 1) {
		uint32_t addr = (hash_vertex >> fingerprintLength) % width;
		if (cache_align)
			weight += nodeQueryMatrixCacheline(addr, fp, type);
		else
			weight += nodeQueryMatrix(addr, fp, type);

		// query the suc-buffer
		uint32_t k1 = (addr << fingerprintLength) + fp;
		weight += nodeQuerySucBuffer(k1, type);
	}
	return weight;
}

#endif		// _LayerSucClass_H