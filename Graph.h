#pragma once
#ifndef Graph_H
#define Graph_H
#include "utility.h"
#include "Tree.h"
class Graph {
public:
	int edgeNum;
	int ptsNum;
	int delete_point_num;
	int height;
	int width;
	vector<Edge> edgelist;
	vector<point> pts;
	vector<int> node_index;
	bitset<L> all_labels;//所有标签
	unordered_map<int, unordered_map<int, list<Path>>> edges_neibr;//点到邻居的路径集
	unordered_map<int, pair<int, int>> tolabel;//<编号，<出现次数，对应标签>>
	unordered_set<int> computed;//统计计算的节点数量


	vector<int> node_height;//存储节点高度
	vector<vector<array<int, L>>> indexanc_LLSD;//array上的数字为INT_MAX，表示没路，否则表示下界weight


	vector<unordered_map<int, list<Path>>> index;//LSD
	vector<unordered_map<int, list<Path>>> temp_index;//暂存路径集


	vector<int> exist_s, exist_t;//暂存距离值

	int rho;
	double rhoavg;

public:
	Graph() :edgeNum(0), ptsNum(0), delete_point_num(0),rho(0),rhoavg(0){};
	void readGraph(string graphname);
	void organizeGraph();
	void sortPaths();
	// -------our method--------//
	void treeNodeContraction();
	void treeFormation();
	void labelAssignment();
};


class Graph1 : public Graph {
public:
	shared_ptr<TreeNode> root;
	vector<shared_ptr<TreeNode>> node_arr;
public:
	Graph1() :Graph(), root(NULL) {};
	//void showTreeinformation();
	void outputTree();
	void showLabel();
	void outputLabel(string datasetname);
	void addAnc();
	// -------our method--------//
	void treeNodeContraction();
	void treeFormation();
	void labelAssignment();

	shared_ptr<TreeNode> findLca(int s, int t);

	int searchDist(int& u, int& v, bitset<L>& labels);
	int LLSD_METHOD(int s, int t, bitset<L> labels,int step);


	int LLSD(int s, int t, bitset<L> labels);
	int LLSD_single(int s, int t, bitset<L> labels, int step);
	int LLSD_layering(int s, int t, bitset<L> labels,int step);

	inline bool has_direct_index(int a, int b);
	inline int Graph1::compute_weight(const int& src, const int& tar, const vector<int>& valid_labels);

	void settingLLSD();//生成完全索引
	void settingLLSD_p(int s);//生成部分索引

	vector<Path> LLSDJoin(int v, int u, int w);

	void query(int s, int t, bitset<L> labels, int step);
	void queryx(string name, int step);
	void queryx2(string name);
	
	

	bitset<L> label_find(int src, int tar);
	bitset<L> label_find_min(int src, int tar, bitset<L>, double d);

	void get_h_and_w();
	void saveTree(string name);
	void saveTree_bin(string name);
	void saveLLSD(string name,int step);


	void readTree(string name);
	void readLLSD(string name);

};

void generateGraph();

#endif
