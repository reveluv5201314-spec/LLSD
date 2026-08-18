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
	bitset<L> all_labels;//All labels
	unordered_map<int, unordered_map<int, list<Path>>> edges_neibr;//Path set from point to neighbor
	unordered_map<int, pair<int, int>> tolabel;//<Number,<Number of occurrences, Corresponding tags>>
	unordered_set<int> computed;//Number of nodes for statistical calculation

	
	vector<int> node_height;//Storage node height
	vector<vector<array<int, L>>> indexanc_LLSD;//The number on the array is INT-MAX, indicating no path, otherwise it represents the lower bound weight


	vector<unordered_map<int, list<Path>>> index;//LSD
	vector<unordered_map<int, list<Path>>> temp_index;//Temporary path set


	vector<int> exist_s, exist_t;//Temporary storage distance value

	int rho;
	double rhoavg;


	vector<mutex> node_mutex;
	vector<atomic<int>> flag;
	vector<atomic<bool>> cleaned;
	ThreadPool pool;
	atomic<int> counter;
	atomic<int> totals;
public:
	Graph() :edgeNum(0), ptsNum(0), delete_point_num(0),rho(0),rhoavg(0),
		cleaned(5000000), node_mutex(5000000), flag(5000000), pool(4), counter(0), totals(0){};
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

	void settingLLSD();//Generate a complete index
	void settingLLSD_p(int s);//Generate partial index
	void settingAstarParallel();
	void processNode(shared_ptr<TreeNode> p);
	void try_cleanup_upwards(shared_ptr<TreeNode> start, vector<atomic<int>>& flag, vector<mutex>& node_mutex, atomic<int>& totals);

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
