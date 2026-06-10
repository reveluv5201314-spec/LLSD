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
	vector<int> val;//节点内序号
	vector<int> valanc;//存放祖先结点序号,顺序为从根节点到本节点（包括本节点）

	unordered_map<int, int> distance;//点到点的最短距离，dls


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