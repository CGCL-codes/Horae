#ifndef _LayerSucPreMapClass_H
#define _LayerSucPreMapClass_H
#include "LayerSucPreMap.h"

class LayerSucPreMapClass: public LayerSucPreMap {
private:
	bool cache_align = false;
	bool kick = false;
public:
	LayerSucPreMapClass(uint32_t granularity, uint32_t width, uint32_t depth, uint32_t fingerprintLength, bool cache_align, bool kick, uint32_t row_addrs = 4, uint32_t column_addrs = 4);
	LayerSucPreMapClass(const LayerSucPreMapClass *layer);
		
	// memory improvement
	LayerSucPreMapClass(const LayerSucPreMapClass *layer, int level);

	~LayerSucPreMapClass();
	void insert(string src, string dst, weight_type weight);
	weight_type edgeQuery(string src, string dst);
	weight_type nodeQuery(string vertex, int type);		//src_type = 0 dst_type = 1
};

LayerSucPreMapClass::LayerSucPreMapClass(uint32_t granularity, uint32_t width, uint32_t depth, uint32_t fingerprintLength, bool cache_align, bool kick, uint32_t row_addrs, uint32_t column_addrs): 
LayerSucPreMap(granularity, width, depth, fingerprintLength , row_addrs, column_addrs), cache_align(cache_align), kick(kick) {
	cout << "LayerSucPreMapClass::LayerSucPreMapClass(granularity: " << granularity 
		<< ", width: " << width <<", depth: " << depth << ", fplen: " << fingerprintLength
		<< ", cache_align: " << cache_align << ", kick: " << kick << ", row_addrs: " << row_addrs 
		<< ", column_addrs: " << column_addrs << ")" << endl;
}
LayerSucPreMapClass::LayerSucPreMapClass(const LayerSucPreMapClass *layer): LayerSucPreMap(layer) {
	cout << "LayerSucPreMapClass::LayerSucPreMapClass(*layer)" << endl;
}

// memory improvement
LayerSucPreMapClass::LayerSucPreMapClass(const LayerSucPreMapClass *layer, int level): LayerSucPreMap(layer, level) {
	cout << "LayerSucPreMapClass::LayerSucPreMapClass(*layer, level: " << level << ")" << endl;
}

LayerSucPreMapClass::~LayerSucPreMapClass() {
	cout << "LayerSucPreMapClass::~LayerSucPreMapClass()" << endl;
}

// src is the ID of the source node, dst is the ID of the destination node, weight is the weight of the edge.
void LayerSucPreMapClass::insert(string src, string dst, weight_type weight) {
	uint32_t hash_src = (*hfunc[HASH])((unsigned char*)(src.c_str()), src.length());
	uint32_t hash_dst = (*hfunc[HASH])((unsigned char*)(dst.c_str()), dst.length());
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
		// insert to buffers or kick-out
		if (!kick) {
			// save to successor buffer k1->k2 & precursor buffer k2<-k1
			insertSucPreMapBuffer(k1, k2, weight);
		}
		else {
			map<uint32_t, uint32_t>::iterator it = successorIndex.find(k1);
			if (it != successorIndex.end()) {			// find k1->k2 in successorAdjacencyList
				uint32_t tag = it->second;
				vector<node>::iterator iter = find_if(successorAdjacencyList[tag].begin(), successorAdjacencyList[tag].end(), findx(k2));
				if (iter != successorAdjacencyList[tag].end()) {	// if found
					iter->weight += weight;
					map<uint32_t, uint32_t>::iterator it2 = precursorIndex.find(k2);		// insert k2->k1
					if (it2 != precursorIndex.end()) {
						precursorAdjacencyList[it2->second].weight += weight;
						inserted = true;
						return;
					}
				}
			}
			if(!cache_align) {			// There is no k1->k2 in buffer, start kicking
				inserted = kickElement(addr_src, fp_src, addr_dst, fp_dst, weight);
			}
			else {
				inserted = kickElementCacheline(addr_src, fp_src, addr_dst, fp_dst, weight);
			}
			if (!inserted) {
				uint32_t curk1 = (addr_src << fingerprintLength) + fp_src;
				uint32_t curk2 = (addr_dst << fingerprintLength) + fp_dst;
				// save to successor buffer k1->k2 & precursor buffer k2<-k1
				insertSucPreMapBuffer(curk1, curk2, weight);
			}
		}
	}
	return;
}
// src is the ID of the source node, dst is the ID of the destination node, return the weight of the edge
weight_type LayerSucPreMapClass::edgeQuery(string src, string dst) {
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
		return edgeQuerySucMapBuffer(k1, k2);
	}
	return 0;
}
// vertex is the ID of the queried node, function for node query. type 0 is for successor query, type 1 is for precusor query
weight_type LayerSucPreMapClass::nodeQuery(string vertex, int type) { // vertex is the ID of the queried node, function for node query.
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
		weight += nodeQuerySucPreMapBuffer(k1, type);
	}
	else if (type == 1) {
		uint32_t addr = (hash_vertex >> fingerprintLength) % width;
		if (cache_align)
			weight += nodeQueryMatrixCacheline(addr, fp, type);
		else
			weight += nodeQueryMatrix(addr, fp, type);
		
		// query the pre-buffer
		uint32_t k1 = (addr << fingerprintLength) + fp;
		weight += nodeQuerySucPreMapBuffer(k1, type);
	}
	return weight;
}

#endif		// _LayerSucPreMapClass_H