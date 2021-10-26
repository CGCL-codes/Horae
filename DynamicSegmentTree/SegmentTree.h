#ifndef _SegmentTree_H
#define _SegmentTree_H
#include <stack>
#include "TreeNode.h"

class SegmentTree {
private:
	uint32_t edgeQueryRec(TreeNode* node, string s, string d, time_type start, time_type end);
	uint32_t nodeQueryRec(TreeNode* node, string v, int type, time_type start, time_type end);
	bool reachabilityQueryRec(TreeNode* node, string s, string d, time_type start, time_type end);

protected:
	struct mapnode {
        uint32_t addr;
        uint16_t fp;
    };
	bool cache_align = false;
	bool kick = false;
	const uint32_t row_addrs;			// the row addrs
	const uint32_t column_addrs;		// the column_addrs

	uint32_t width;
	uint32_t depth;
	uint32_t fingerprintLength;
    TreeNode* root;
	int node_nums;

public:
	SegmentTree(uint32_t width, uint32_t depth, uint32_t fingerprintLength, bool cache_align, bool kick, uint32_t row_addrs = 4, uint32_t column_addrs = 4);
	virtual ~SegmentTree();

	void preOrderTraversalWithStack();
	void inOrderTraversalWithStack();
	void postOrderTraversalWithStack();
	void levelOrderTraversalWithQueue();

	virtual void insert(string s, string d, weight_type w, time_type t) = 0;

	virtual uint32_t edgeQuery(string s, string d, time_type start, time_type end);
	virtual uint32_t nodeQuery(string v, int type, time_type start, time_type end);
	virtual bool reachabilityQuery(string s, string d, time_type start, time_type end);


	// /**************** test functions ****************/
	void bucketCounting();
	// /**************** test functions ****************/
};
/**************** test functions ****************/
void SegmentTree::bucketCounting() {
	stack<TreeNode*> s;
	TreeNode* treeNode = this->root;
	while (treeNode != nullptr || !s.empty()) {
		while (treeNode != nullptr) {
			if (treeNode->value != nullptr) {
				cout << "************ interval: [" << treeNode->interval_start << ", " << treeNode->interval_end << "] ************" << endl;
				treeNode->bucketCounting();
				cout << "*********************************" << endl << endl;
			}
			s.push(treeNode);
			treeNode = treeNode->left;
		}
		if(!s.empty()) {
			treeNode = s.top();
			s.pop();
			treeNode = treeNode->right;
		}
	}
}
/**************** test functions ****************/

SegmentTree::SegmentTree(uint32_t width, uint32_t depth, uint32_t fingerprintLength, bool cache_align, bool kick, uint32_t row_addrs, uint32_t column_addrs):
width(width), depth(depth), fingerprintLength(fingerprintLength), cache_align(cache_align), kick(kick), row_addrs(row_addrs), column_addrs(column_addrs)
{
	cout << "SegmentTree::SegmentTree(width: " << width << ", depth: " << depth << ", fplen: " << fingerprintLength 
		 << ", cache_align: " << cache_align << ", kick: " << kick
		 << ", row_addrs: " << row_addrs << ", column_addrs: " << column_addrs << ")" << endl;
	this->node_nums = 0;
	this->root = nullptr;
}

SegmentTree::~SegmentTree() {
	cout << "node_nums = " << node_nums << endl;
}

void SegmentTree::preOrderTraversalWithStack() {
	stack<TreeNode*> s;
	TreeNode* treeNode = this->root;
	while (treeNode != nullptr || !s.empty()) {
		while (treeNode != nullptr) {
			cout << "[" << treeNode->interval_start << ", " << treeNode->interval_end << "]" << endl;
			s.push(treeNode);
			treeNode = treeNode->left;
		}
		if(!s.empty()) {
			treeNode = s.top();
			s.pop();
			treeNode = treeNode->right;
		}
	}
}

void SegmentTree::inOrderTraversalWithStack() {
	stack<TreeNode*> s;
	TreeNode* treeNode = this->root;
	while (treeNode != nullptr || !s.empty()) {
		while (treeNode != nullptr) {
			s.push(treeNode);
			treeNode = treeNode->left;
		}
		if(!s.empty()) {
			treeNode = s.top();
			s.pop();
			if (treeNode->value != nullptr)
				cout << "[" << treeNode->interval_start << ", " << treeNode->interval_end << "]" << endl;
			treeNode = treeNode->right;
		}
	}
}

void SegmentTree::postOrderTraversalWithStack() {
	stack<TreeNode*> s;
	TreeNode* treeNode = this->root;
	TreeNode* lastVisit = nullptr;
	while (treeNode != nullptr || !s.empty()) {
		while (treeNode != nullptr) {
			s.push(treeNode);
			treeNode = treeNode->left;
		}
		if (!s.empty()) {
			treeNode = s.top();
			s.pop();
			if (treeNode->right == nullptr || treeNode->right == lastVisit) {
				cout << "[" << treeNode->interval_start << ", " << treeNode->interval_end << "]" << endl;
				lastVisit = treeNode;
				treeNode = nullptr;
			}
			else {
				s.push(treeNode);
				treeNode = treeNode->right;
			}
		}
	}
}

void SegmentTree::levelOrderTraversalWithQueue() {
	queue<TreeNode*> q;
	q.push(this->root);
	while (!q.empty()) {
		TreeNode* temp = q.front();
		q.pop();
		cout << "[" << temp->interval_start << ", " << temp->interval_end << "]" << endl;
		if (temp->left != nullptr) {
			q.push(temp->left);
		}
		if (temp->right != nullptr) {
			q.push(temp->right);
		}
	}
}

uint32_t SegmentTree::edgeQuery(string s, string d, time_type start, time_type end) {
	return edgeQueryRec(this->root, s, d, start, end);
}

uint32_t SegmentTree::edgeQueryRec(TreeNode* node, string s, string d, time_type start, time_type end) {
	// cout << "node->interval_start = " << node->interval_start << ", node->interval_end = " << node->interval_end << endl;
	if(node == nullptr || start > end || start > node->interval_end || end < node->interval_start)
		return 0;
	if (start <= node->interval_start && end >= node->interval_end) {
		return node->edgeQuery(s, d);
	}

	int mid = (node->interval_start + node->interval_end) / 2;
	int result = 0;
	if (start <= mid)
		result += edgeQueryRec(node->left, s, d, start, end);
	if (end > mid)
		result += edgeQueryRec(node->right, s, d, start, end);
	return result;

	// return edgeQueryRec(node->left, s, d, start, end) + edgeQueryRec(node->right, s, d, start, end);
}

uint32_t SegmentTree::nodeQuery(string v, int type, time_type start, time_type end) {
	return nodeQueryRec(this->root, v, type, start, end);
}

uint32_t SegmentTree::nodeQueryRec(TreeNode* node, string v, int type, time_type start, time_type end) {
	if(node == nullptr || start > end || start > node->interval_end || end < node->interval_start)
		return 0;
	if (start <= node->interval_start && end >= node->interval_end) {
		return node->nodeQuery(v, type);
	}

	int mid = (node->interval_start + node->interval_end) / 2;
	int result = 0;
	if (start <= mid)
		result += nodeQueryRec(node->left, v, type, start, end);
	if (end > mid)
		result += nodeQueryRec(node->right, v, type, start, end);
	return result;

	// return nodeQueryRec(node->left, v, type, start, end) + edgeQueryRec(node->right, v, type, start, end);
}

bool SegmentTree::reachabilityQuery(string s, string d, time_type start, time_type end) {
	return reachabilityQueryRec(this->root, s, d, start, end);
}

bool SegmentTree::reachabilityQueryRec(TreeNode* node, string s, string d, time_type start, time_type end) {
	if(node == nullptr || start > end || start > node->interval_end || end < node->interval_start)
		return false;
	if (start <= node->interval_start && end >= node->interval_end) {
		return node->reachabilityQuery(s, d);
	}

	//bool res = false;
	int mid = (node->interval_start + node->interval_end) / 2;
	if (start <= mid) {
		if (!reachabilityQueryRec(node->left, s, d, start, end))
			return false;
		//res = res || (reachabilityQueryRec(node->left, s, d, start, end));
	}
	if (end > mid) {
		if (!reachabilityQueryRec(node->right, s, d, start, end)) 
			return false;
		//res = res || (reachabilityQueryRec(node->right, s, d, start, end));
	}
	//return res;
	return true;
}

#endif		// _SegmentTree_H