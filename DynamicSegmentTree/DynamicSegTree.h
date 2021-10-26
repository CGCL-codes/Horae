#ifndef _DynamicSegTree_H
#define _DynamicSegTree_H
#include "SegmentTree.h"

class DynamicSegTree: public SegmentTree {
private:
	TreeNode* dynamicBuildSegmentTree(TreeNode* oldRoot, time_type start, time_type end);
	TreeNode* buildSegmentTree(time_type start, time_type end);
public:
	DynamicSegTree(uint32_t width, uint32_t depth, uint32_t fingerprintLength, bool cache_align, bool kick, uint32_t row_addrs = 4, uint32_t column_addrs = 4);
	~DynamicSegTree();
	void insert(string s, string d, weight_type w, time_type t);
};

TreeNode* DynamicSegTree::buildSegmentTree(time_type start, time_type end) {
	if (start > end)
		return nullptr;

	TreeNode* root = new TreeNode(start, end, width, depth, fingerprintLength, cache_align, kick, row_addrs, column_addrs);
	this->node_nums++;
	if (start < end) {
		root->left = buildSegmentTree(start, (start + end) / 2);
		root->right = buildSegmentTree((start + end) / 2 + 1, end);
	}
	return root;
}

TreeNode* DynamicSegTree::dynamicBuildSegmentTree(TreeNode* oldRoot, time_type start, time_type end) {
	if (oldRoot == nullptr)
		return buildSegmentTree(start, end);
	if (start > end)
		return nullptr;
	if (end <= oldRoot->interval_end) {
		return oldRoot;
	}

	TreeNode* newRoot = nullptr;
	if (start == oldRoot->interval_start && end > oldRoot->interval_end) {
		newRoot = new TreeNode(start, end, oldRoot);
	} else {
		newRoot = new TreeNode(start, end, width, depth, fingerprintLength, cache_align, kick, row_addrs, column_addrs);
	}
	
	this->node_nums++;
	if (start < end) {
		int mid = (start + end) / 2;
		if ((start == oldRoot->interval_start) && (end == mid)) {
			newRoot->left = oldRoot;
		} else {
			newRoot->left = dynamicBuildSegmentTree(oldRoot, start, mid);
		}

		newRoot->right = dynamicBuildSegmentTree(oldRoot, mid + 1, end);
	}
	return newRoot;
}

DynamicSegTree::DynamicSegTree(uint32_t width, uint32_t depth, uint32_t fingerprintLength, bool cache_align, bool kick, uint32_t row_addrs, uint32_t column_addrs):
SegmentTree(width, depth, fingerprintLength, cache_align, kick, row_addrs, column_addrs)
{
	cout << "DynamicSegTree::DynamicSegTree(width: " << width << ", depth: " << depth << ", fplen: " << fingerprintLength
		 << ", cache_align: " << cache_align  << ", kick: " << kick 
		 << ", row_addrs: " << row_addrs << ", column_addrs: " << column_addrs << ")" << endl;
	// this->node_nums = 0;
	// this->root = nullptr;
}

DynamicSegTree::~DynamicSegTree() {
	// delete the interval tree
	stack<TreeNode*> s;
	// vector<TreeNode*> nodes;
	TreeNode* treeNode = this->root;
	TreeNode* temp = nullptr;
	while (treeNode != nullptr || !s.empty()) {
		while (treeNode != nullptr) {
			s.push(treeNode);
			treeNode = treeNode->left;
		}
		if(!s.empty()) {
			treeNode = s.top();
			s.pop();
			temp = treeNode;
			// if (treeNode->value != nullptr)
			// 	nodes.push_back(treeNode);
			treeNode = treeNode->right;
			delete temp;
		}
	}
	// cout << "nodes.size() = " << nodes.size() << endl;
	// for (TreeNode* treeNode: nodes) {
	// 	delete treeNode;
	// }
}

void DynamicSegTree::insert(string s, string d, weight_type w, time_type t) {
	if (this->root == nullptr || t > this->root->interval_end) {
		time_type upper = 1 << ((int)ceil((log(t) / log(2))));
		this->root = this->dynamicBuildSegmentTree(this->root, 1, upper);
	}
	stack<TreeNode*> st;
	TreeNode* treeNode = this->root;
	while (treeNode != nullptr || !st.empty()) {
		while (treeNode != nullptr) {
			// cout << "[" << treeNode->interval_start << ", " << treeNode->interval_end << "]" << endl;
			if (t >= treeNode->interval_start && t <= treeNode->interval_end) {
				// insert process
				treeNode->insert(s, d, w);
				st.push(treeNode);
				treeNode = treeNode->left;
			} else {
				treeNode = nullptr;
				break;
			}
		}
		if(!st.empty()) {
			treeNode = st.top();
			st.pop();
			treeNode = treeNode->right;
		}
	}
	return;
}

#endif		// _DynamicSegTree_H