#ifndef _LayerSucPreMap_H
#define _LayerSucPreMap_H
#include "../LayerSucPre/LayerSucPre.h"

class LayerSucPreMap: public LayerSucPre {
protected:
	// both map and prelist
	uint32_t n1 = 0, n2 = 0;	//n1 for successor, n2 for precursor
	map<uint32_t, uint32_t> precursorIndex;
	map<uint32_t, uint32_t> successorIndex;
public:
	LayerSucPreMap(uint32_t granularity, uint32_t width, uint32_t depth, uint32_t fingerprintLength, uint32_t row_addrs = 4, uint32_t column_addrs = 4);
	LayerSucPreMap(const LayerSucPreMap *layer);
	// memory improvement
	LayerSucPreMap(const LayerSucPreMap *layer, int level);

	~LayerSucPreMap();
	void bucketCounting();

	// from class LayerSucPre -> LayerSuc -> Layer
	// virtual void insert(string src, string dst, weight_type weight) = 0;
	// virtual weight_type edgeQuery(string src, string dst) = 0;
	// virtual weight_type nodeQuery(string vertex, int type) = 0;		//src_type = 0 dst_type = 1

protected:
	void insertSucPreMapBuffer(uint32_t k1, uint32_t k2, weight_type weight);
	weight_type edgeQuerySucMapBuffer(uint32_t k1, uint32_t k2);
	weight_type nodeQuerySucPreMapBuffer(uint32_t k, int type);

};
void LayerSucPreMap::bucketCounting() {
	LayerSucPre::bucketCounting();
}

LayerSucPreMap::LayerSucPreMap(uint32_t granularity, uint32_t width, uint32_t depth, uint32_t fingerprintLength, uint32_t row_addrs, uint32_t column_addrs): LayerSucPre(granularity, width, depth, fingerprintLength, row_addrs, column_addrs) {
	cout << "LayerSucPreMap::LayerSucPreMap(granularity: " << granularity 
		<< ", width: " << width <<", depth: " << depth << ", fplen: " << fingerprintLength
		<< ", row_addrs: " << row_addrs << ", column_addrs: " << column_addrs << ")" << endl;
	this->n1 = 0;
	this->n2 = 0;
}
LayerSucPreMap::LayerSucPreMap(const LayerSucPreMap *layer): LayerSucPre(layer) {
	cout << "LayerSucPreMap::LayerSucPreMap(*layer)" << endl;
	this->n1 = layer->n1;
	this->n2 = layer->n2;
}

// memory improvement
LayerSucPreMap::LayerSucPreMap(const LayerSucPreMap *layer, int level): LayerSucPre(layer, level) {
	cout << "LayerSucPreMap::LayerSucPreMap(*layer, level: " << level << ")" << endl;
}

LayerSucPreMap::~LayerSucPreMap() {
	cout << "LayerSucPreMap::~LayerSucPreMap()" << endl;
}

void LayerSucPreMap::insertSucPreMapBuffer(uint32_t k1, uint32_t k2, weight_type weight) {
	// save to successor buffer k1->k2
	map<uint32_t, uint32_t>::iterator it = successorIndex.find(k1);
	if (it != successorIndex.end()) {
		uint32_t tag = it->second;
		vector<node>::iterator iter = find_if(successorAdjacencyList[tag].begin(), successorAdjacencyList[tag].end(), findx(k2));

		if (iter != successorAdjacencyList[tag].end()) {
			iter->weight += weight;
		}
		else {
			node tmpnode;
			tmpnode.key = k2;
			tmpnode.weight = weight;
			successorAdjacencyList[tag].push_back(tmpnode);
		}
	}
	else {
		successorIndex[k1] = n1;
		n1++;
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
	//save to precursor buffer k2<-k1
	map<uint32_t, uint32_t>::iterator it2 = precursorIndex.find(k2);
	if (it2 != precursorIndex.end()) {
		precursorAdjacencyList[it2->second].weight += weight;
	}
	else {
		precursorIndex[k2] = n2;
		n2++;
		node newnode;
		newnode.key = k2;
		newnode.weight = weight;
		precursorAdjacencyList.push_back(newnode);
	}
}
weight_type LayerSucPreMap::edgeQuerySucMapBuffer(uint32_t k1, uint32_t k2) {
	// query the suc-buffer
	map<uint32_t, uint32_t>::iterator it = successorIndex.find(k1);
	if (it != successorIndex.end()) {
		uint32_t tag = it->second;
		vector<node>::iterator iter = find_if(successorAdjacencyList[tag].begin(), successorAdjacencyList[tag].end(), findx(k2));
		if (iter != successorAdjacencyList[tag].end()) {
			return iter->weight;
		}
	}
	return 0;
}
weight_type LayerSucPreMap::nodeQuerySucPreMapBuffer(uint32_t k, int type) {
	weight_type weight = 0;
	if (type == 0) {
		map<uint32_t, uint32_t>::iterator it = successorIndex.find(k);
		if (it != successorIndex.end())	{
			uint32_t tag = it->second;
			vector<node>::iterator iter;
			for (iter = successorAdjacencyList[tag].begin(); iter != successorAdjacencyList[tag].end(); iter++) {
				weight += iter->weight;
			}
		}
	}
	else if (type == 1) {
		map<uint32_t, uint32_t>::iterator it = precursorIndex.find(k);
		if (it != precursorIndex.end()) {
			weight += precursorAdjacencyList[it->second].weight;
		}
	}
	return weight;
}

#endif		  // _LayerSucPreMap_H