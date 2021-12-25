#ifndef _LayerSuc_H
#define _LayerSuc_H
#include "../Layer.h"

class LayerSuc: public Layer {
protected:
	// no map & no prelist
	vector<vector<node>> successorAdjacencyList;
public:
	LayerSuc(uint32_t granularity, uint32_t width, uint32_t depth, uint32_t fingerprintLength, uint32_t row_addrs = 4, uint32_t column_addrs = 4);
	LayerSuc(const LayerSuc *layer);
	// memory improvement
	LayerSuc(const LayerSuc *layer, int level);

	virtual ~LayerSuc();
	void bucketCounting();

	// from class Layer
	// virtual void insert(string src, string dst, weight_type weight) = 0;
	// virtual weight_type edgeQuery(string src, string dst) = 0;
	// virtual weight_type nodeQuery(string vertex, int type) = 0;		//src_type = 0 dst_type = 1

protected:
	void insertSucBuffer(uint32_t k1, uint32_t k2, weight_type weight);
	weight_type edgeQuerySucBuffer(uint32_t k1, uint32_t k2);
	weight_type nodeQuerySucBuffer(uint32_t k, int type);

};
void LayerSuc::bucketCounting() {
	Layer::bucketCounting();
	//print buffer size
	cout << "---------------------------------------" << endl;
	cout << "LayerSuc bucketCounting(): print successorAdjacencyList..." << endl;
	cout << "successorAdjacencyList.size() = " << successorAdjacencyList.size() << endl;
	cout << "successorAdjacencyList.capacity() = " << successorAdjacencyList.capacity() << endl;

	int64_t total_sucBuffer = 0;
	int64_t total_cap = 0, total_size = 0;
	for(int64_t i = 0; i < this->successorAdjacencyList.size(); i++) {
		total_cap += this->successorAdjacencyList[i].capacity();
		total_size += this->successorAdjacencyList[i].size();
	}
	cout << "total_size = " << total_size << endl;
	cout << "total_cap = " << total_cap << endl;
	for(int64_t i = 0; i < this->successorAdjacencyList.size(); i++) {
		 total_sucBuffer += this->successorAdjacencyList[i].size();
	}
	cout << "total_sucBuffer = " << total_sucBuffer << endl;
	cout << "---------------------------------------" << endl;
	return;
}

LayerSuc::LayerSuc(uint32_t granularity, uint32_t width, uint32_t depth, uint32_t fingerprintLength, uint32_t row_addrs, uint32_t column_addrs): Layer(granularity, width, depth, fingerprintLength, row_addrs, column_addrs) {
	cout << "LayerSuc::LayerSuc(granularity: " << granularity 
		<< ", width: " << width <<", depth: " << depth << ", fplen: " << fingerprintLength
		<< ", row_addrs: " << row_addrs << ", column_addrs: " << column_addrs << ")" << endl;
}
LayerSuc::LayerSuc(const LayerSuc *layer): Layer(layer) {
	cout << "LayerSuc::LayerSuc(*layer)" << endl;
	
	this->successorAdjacencyList.resize(layer->successorAdjacencyList.size());
	for (uint32_t i = 0; i < layer->successorAdjacencyList.size(); i++) {
		this->successorAdjacencyList[i].assign(layer->successorAdjacencyList[i].begin(), layer->successorAdjacencyList[i].end());
	}
}

// memory improvement
LayerSuc::LayerSuc(const LayerSuc *layer, int level): Layer(layer, level) {
	cout << "LayerSuc::LayerSuc(*layer, level: " << level << ")" << endl;
}

LayerSuc::~LayerSuc() {
	cout << "LayerSuc::~LayerSuc()" << endl;
	vector<vector<node>>().swap(successorAdjacencyList);
}

void LayerSuc::insertSucBuffer(uint32_t k1, uint32_t k2, weight_type weight) {
	// save to successor buffer k1->k2
	// map<uint32_t, uint32_t>::iterator it = successorIndex.find(k1);
	vector<vector<node> >::iterator it = find_if(successorAdjacencyList.begin(), successorAdjacencyList.end(), findv(k1));
	if (it != successorAdjacencyList.end()) {		// find k1->k2 in successorAdjacencyList
		// uint32_t tag = it->second;
		// vector<node>::iterator iter = find_if(successorAdjacencyList[tag].begin(), successorAdjacencyList[tag].end(), findx(k2));
		vector<node>::iterator iter = find_if(it->begin(), it->end(), findx(k2));
		if (iter != it->end()) {		// if found
			iter->weight += weight;
		}
		else {
			node tmpnode;
			tmpnode.key = k2;
			tmpnode.weight = weight;
			it->push_back(tmpnode);
		}
	}
	else {
		// successorIndex[k1] = n1;
		// n1++;
		node tmpnode;
		tmpnode.key = k1;
		tmpnode.weight = 0;
		vector<node> vc;
		vc.push_back(tmpnode);
		if (k1 != k2) {  //k1==k2 means loop
			node newnode;
			newnode.key = k2;
			newnode.weight = weight;
			vc.push_back(newnode);
		}
		else {
			vc[0].weight += weight;
		}
		successorAdjacencyList.push_back(vc);
	}
	// save to precursor buffer k2<-k1
	// map<uint32_t, uint32_t>::iterator it2 = precursorIndex.find(k2);
	// vector<node>::iterator it2 = find_if(precursorAdjacencyList.begin(), precursorAdjacencyList.end(), findx(k2));
	// if (it2 != precursorAdjacencyList.end()) {		// find k2->k1 in precursorAdjacencyList
	// 	it2->weight += weight;
	// }
	// else {
	// 	// precursorIndex[k2] = n2;
	// 	// n2++;
	// 	node newnode;
	// 	newnode.key = k2;
	// 	newnode.weight = weight;
	// 	precursorAdjacencyList.push_back(newnode);
	// }
}
weight_type LayerSuc::edgeQuerySucBuffer(uint32_t k1, uint32_t k2) {
	// query the suc-buffer
	// map<uint32_t, uint32_t>::iterator it = successorIndex.find(k1);
	vector<vector<node> >::iterator it = find_if(successorAdjacencyList.begin(), successorAdjacencyList.end(), findv(k1));
	if (it != successorAdjacencyList.end()) {
		vector<node>::iterator iter = find_if(it->begin(), it->end(), findx(k2));
		if (iter != it->end()) {
			return iter->weight;
		}
	}
	return 0;
}
weight_type LayerSuc::nodeQuerySucBuffer(uint32_t k, int type) {
	weight_type weight = 0;
	if (type == 0) {
		// map<uint32_t, uint32_t>::iterator it = successorIndex.find(k1);
		vector<vector<node> >::iterator it = find_if(successorAdjacencyList.begin(), successorAdjacencyList.end(), findv(k));
		if (it != successorAdjacencyList.end())	{
			vector<node>::iterator iter;
			for (iter = it->begin(); iter != it->end(); iter++) {
				weight += iter->weight;
			}
		}
	}
	else if (type == 1) {
		// map<uint32_t, uint32_t>::iterator it = precursorIndex.find(k1);
		// vector<node>::iterator it = find_if(precursorAdjacencyList.begin(), precursorAdjacencyList.end(), findx(k1));
		// if (it != precursorAdjacencyList.end()) {
		// 		weight += it->weight;
		// }
		for(vector<vector<node> >::iterator it = successorAdjacencyList.begin(); it != successorAdjacencyList.end(); it++){
			for(vector<node>::iterator iter = it->begin(); iter!= it->end(); iter++){
				if(iter->key == k){
					weight += iter->weight;
				}
			}
		}
	}
	return weight;
}

#endif	  	// _LayerSuc_H