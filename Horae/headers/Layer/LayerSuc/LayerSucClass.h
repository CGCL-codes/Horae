#ifndef _LayerSucClass_H
#define _LayerSucClass_H
#include "LayerSuc.h"

class LayerSucClass: public LayerSuc {
private:
	bool cache_align = false;
	bool kick = false;
	struct mapnode
	{
		uint32_t addr;
		uint16_t fp;
	};
public:
	LayerSucClass(uint32_t granularity, uint32_t width, uint32_t depth, uint32_t fingerprintLength, bool cache_align, bool kick, uint32_t row_addrs = 4, uint32_t column_addrs = 4);
	LayerSucClass(const LayerSucClass *layer);

	// memory improvement
	LayerSucClass(const LayerSucClass *layer, int level);

	~LayerSucClass();
	void insert(string src, string dst, weight_type weight);
	weight_type edgeQuery(string src, string dst);
	weight_type nodeQuery(string vertex, int type);		//src_type = 0 dst_type = 1
	bool reachabilityQuery(string s, string d);
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
	uint32_t hash_src = (*hfunc[HASH])((unsigned char*)(src.c_str()), src.length());
	uint32_t hash_dst = (*hfunc[HASH])((unsigned char*)(dst.c_str()), dst.length());
	//uint32_t mask = pow(2, fingerprintLength) - 1;
	uint32_t mask = (1 << fingerprintLength) - 1;
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
				inserted = kickElement(addr_src, fp_src, addr_dst, fp_dst, weight);
			}
			else {
				inserted = kickElementCacheline(addr_src, fp_src, addr_dst, fp_dst, weight);
			}
			if (!inserted) {
				uint32_t curk1 = (addr_src << fingerprintLength) + fp_src;
				uint32_t curk2 = (addr_dst << fingerprintLength) + fp_dst;
				// save to successor buffer k1->k2
				insertSucBuffer(curk1, curk2, weight);
			}
		}
	}
	return;
}
// src is the ID of the source node, dst is the ID of the destination node, return the weight of the edge
weight_type LayerSucClass::edgeQuery(string src, string dst) {
	uint32_t hash_src = (*hfunc[HASH])((unsigned char*)(src.c_str()), src.length());
	uint32_t hash_dst = (*hfunc[HASH])((unsigned char*)(dst.c_str()), dst.length());
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
	uint32_t hash_vertex = (*hfunc[HASH])((unsigned char*)(vertex.c_str()), vertex.length());
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

// s is the ID of the source node, d is the ID of the destination node
bool LayerSucClass::reachabilityQuery(string s, string d) {
	uint32_t hash_s = (*hfunc[HASH])((unsigned char*)(s.c_str()), s.length());
	uint32_t hash_d = (*hfunc[HASH])((unsigned char*)(d.c_str()), d.length());
	uint32_t mask = (1 << fingerprintLength) - 1;
	
	uint32_t addr_s = (hash_s >> fingerprintLength) % depth;
	uint32_t addr_d = (hash_d >> fingerprintLength) % width;

	uint16_t fp_s = hash_s & mask;
	uint16_t fp_d = hash_d & mask;
	if(fp_s == 0) fp_s = 1;
	if(fp_d == 0) fp_d = 1;

	uint32_t key_s = (addr_s << fingerprintLength) + fp_s;
	uint32_t key_d = (addr_d << fingerprintLength) + fp_d;
	
	int pos;
	map<uint32_t, bool> checked;
	queue<mapnode> q;
	mapnode e;
	e.addr = addr_s;
	e.fp = fp_s;
	q.push(e);
	checked[key_s] = true;
	map<unsigned int, bool>::iterator IT;

	uint32_t temp_addr;
	uint16_t temp_fp;
	
	if (cache_align) 
	{
		while (!q.empty())
		{
			e = q.front();
			temp_addr = e.addr;
			temp_fp = e.fp;

			int* tmp1 = new int[row_addrs];
			int* tmp2 = new int[column_addrs / 2];
			tmp1[0] = temp_fp;
			tmp2[0] = fp_d;
			
			for (int i = 1; i < row_addrs; i++)
			{
				tmp1[i] = (tmp1[i - 1] * multiplier + increment) % modulus;
			}
			for (int i = 1; i < column_addrs / 2; i++) 
			{
				tmp2[i] = (tmp2[i - 1] * multiplier + increment) % modulus;
			}
			for (int i1 = 0; i1 < row_addrs; i1++)
			{
				int p1 = (temp_addr + tmp1[i1]) % depth;
				for (int i2 = 0; i2 < column_addrs / 2; i2++)
				{
					int p2 = (addr_d + tmp2[i2]) % width;
					for (int k = 0; k < 2; k++) {
						uint32_t pos;
						if (k == 0) {
							pos = p1 * width + p2;
						}
						else {
							uint32_t column_addr_alt = (p2 ^ (fp_d % CACHESLOT)) % width;
							pos = p1 * width + column_addr_alt;
						}
						for (int j = 0; j < SLOTNUM; j++) {
							if (((value[pos].src[j] >> 14) == i1) && ((value[pos].dst[j] >> 14) == (i2*2+k)) && ((value[pos].src[j] & mask) == temp_fp) && ((value[pos].dst[j] & mask) == fp_d)) {
								delete []tmp1;
								delete []tmp2;
								return true;
							}
						}
					}
				}
			}
			//	 find in buffer
			uint32_t temp_key = (temp_addr << fingerprintLength) + temp_fp;
			vector<vector<node> >::iterator it = find_if(successorAdjacencyList.begin(), successorAdjacencyList.end(), findv(temp_key));
			if (it != successorAdjacencyList.end())
			{
				for (vector<node>::iterator iter = it->begin(); iter != it->end(); iter++) 
				{
					if (iter->key != temp_key)
					{
						unsigned int val = iter->key;
						unsigned int temp_h = (val) >> fingerprintLength;
						unsigned int tmp = pow(2, fingerprintLength);
						unsigned short temp_g = (val % tmp);
						if ((temp_h == addr_d) && (temp_g == fp_d))
						{
							delete []tmp1;
							delete []tmp2;
							return true;
						}
			
						IT = checked.find(val);
						if (IT == checked.end())
						{
							mapnode temp_e;
							temp_e.addr = temp_h;
							temp_e.fp = temp_g;
							q.push(temp_e);
							checked[val] = true;;
						}
					}
				}
			}
			
			// find in matrix
			for (int i1 = 0; i1 < row_addrs; i1++)
			{
				int p1 = (temp_addr + tmp1[i1]) % depth;
				for (int i2 = 0; i2 < width; i2++)
				{
					int pos = p1 * width + i2;
					for (int i3 = 0; i3 < SLOTNUM; i3++)
					{
						if ((value[pos].src[i3] & mask) == temp_fp && ((value[pos].src[i3] >> 14) == i1))
						{
							uint32_t tmp_g = value[pos].dst[i3];
							int tmp_s = (value[pos].dst[i3] >> 14) & 0xff;
				
							uint32_t shifter = tmp_g;
							for (int v = 0; v < tmp_s; v++)
								shifter = (shifter * multiplier + increment) % modulus;
							uint32_t tmp_h = i2;
							while (tmp_h < shifter)
								tmp_h += width;				/////////
							tmp_h -= shifter;

							uint32_t val = (tmp_h << fingerprintLength) + tmp_g;
						
							IT = checked.find(val);
							if (IT == checked.end())
							{
								mapnode tmp_e;
								tmp_e.addr = tmp_h;
								tmp_e.fp = tmp_g;
								q.push(tmp_e);
								checked[val] = true;
							}
						}
					}
				}
			}
			delete[] tmp1;
			delete[] tmp2;
			q.pop();
		}
	}
	else 
	{
		while (!q.empty())
		{
			e = q.front();
			temp_addr = e.addr;
			temp_fp = e.fp;

			int* tmp1 = new int[row_addrs];
			int* tmp2 = new int[column_addrs];
			tmp1[0] = temp_fp;
			tmp2[0] = fp_d;
			
			for (int i = 1; i < row_addrs; i++)
			{
				tmp1[i] = (tmp1[i - 1] * multiplier + increment) % modulus;
			}
			for (int i = 1; i < column_addrs; i++) 
			{
				tmp2[i] = (tmp2[i - 1] * multiplier + increment) % modulus;
			}
			for (int i1 = 0; i1 < row_addrs; i1++)
			{
				int p1 = (temp_addr + tmp1[i1]) % depth;
				for (int i2 = 0; i2 < column_addrs; i2++)
				{
					int p2 = (addr_d + tmp2[i2]) % width;
					int pos = p1 * width + p2;
					for (int i3 = 0; i3 < SLOTNUM; i3++)
					{
						if (((value[pos].src[i3] >> 14) == i1) && ((value[pos].dst[i3] >> 14) == i2) && ((value[pos].src[i3] & mask) == temp_fp) && ((value[pos].dst[i3] & mask) == fp_d))
						{
							delete []tmp1;
							delete []tmp2;
							return true;
						}
					}
				}
			}
			//	 find in buffer
			uint32_t temp_key = (temp_addr << fingerprintLength) + temp_fp;
			vector<vector<node> >::iterator it = find_if(successorAdjacencyList.begin(), successorAdjacencyList.end(), findv(temp_key));
			if (it != successorAdjacencyList.end())
			{
				for (vector<node>::iterator iter = it->begin(); iter != it->end(); iter++) 
				{
					if (iter->key != temp_key)
					{
						unsigned int val = iter->key;
						unsigned int temp_h = (val) >> fingerprintLength;
						unsigned int tmp = pow(2, fingerprintLength);
						unsigned short temp_g = (val % tmp);
						if ((temp_h == addr_d) && (temp_g == fp_d))
						{
							delete []tmp1;
							delete []tmp2;
							return true;
						}
			
						IT = checked.find(val);
						if (IT == checked.end())
						{
							mapnode temp_e;
							temp_e.addr = temp_h;
							temp_e.fp = temp_g;
							q.push(temp_e);
							checked[val] = true;;
						}
					}
				}
			}
			
			// find in matrix
			for (int i1 = 0; i1 < row_addrs; i1++)
			{
				int p1 = (temp_addr + tmp1[i1]) % depth;
				for (int i2 = 0; i2 < width; i2++)
				{
					int pos = p1 * width + i2;
					for (int i3 = 0; i3 < SLOTNUM; i3++)
					{
						if ((value[pos].src[i3] & mask) == temp_fp && ((value[pos].src[i3] >> 14) == i1))
						{
							uint32_t tmp_g = (value[pos].dst[i3] & mask);
							int tmp_s = value[pos].dst[i3] >> 14;
				
							uint32_t shifter = tmp_g;
							for (int v = 0; v < tmp_s; v++)
								shifter = (shifter * multiplier + increment) % modulus;
							uint32_t tmp_h = i2;
							while (tmp_h < shifter)
								tmp_h += width;				/////////
							tmp_h -= shifter;

							uint32_t val = (tmp_h << fingerprintLength) + tmp_g;
						
							IT = checked.find(val);
							if (IT == checked.end())
							{
								mapnode tmp_e;
								tmp_e.addr = tmp_h;
								tmp_e.fp = tmp_g;
								q.push(tmp_e);
								checked[val] = true;
							}
						}
					}
				}
			}
			delete[] tmp1;
			delete[] tmp2;
			q.pop();
		}
	}
	return false;
}

#endif		// _LayerSucClass_H