#pragma once
#ifndef Tree_h
#define Tree_h
#include "utility.h"
class TreeNode {
public:
	shared_ptr<TreeNode> parent;
	shared_ptr<TreeNode> prev1;
	shared_ptr<TreeNode> prev2;
	vector<shared_ptr<TreeNode>> children;
	vector<int> val;//Node internal serial number
	vector<int> valanc;//Store ancestor node numbers in order from the root node to the current node (including the current node)

	unordered_map<int, int> distance;//The shortest distance from point to point, dls


public:
	TreeNode() :parent(NULL) {};
	TreeNode(shared_ptr<TreeNode> p, vector<int> val) : parent(p), val(val) {};
	void showNode(unordered_map<int, list<Path>> index);
};



class Tree {
public:
	shared_ptr<TreeNode> root;
	vector<vector<shared_ptr<TreeNode>>> nodes;
	vector<shared_ptr<TreeNode>> node_list;
	vector<int> node_index;
public:
	Tree() : root(NULL) {};
};

#endif
