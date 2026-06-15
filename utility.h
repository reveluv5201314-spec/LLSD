#ifndef UTILITY_H
#define UTILITY_H
#include<iostream>
#include<vector>
#include<set>
#include<fstream>
#include<algorithm>
#include<string>
#include<unordered_map>
#include<map>
#include<queue>
#include<iterator>
#include<chrono>
#include<random>
#include<stack>
#include<unordered_set>
#include <array>
#include <bitset>
#include <list>
#include <functional>
#include <atomic>
#include <shared_mutex>

using namespace std;

#define L 10




struct Edge {
	int src;
	int tar;
	int weight;
	bitset<L> labels;
};
struct Path {
	int weight;
	bitset<L> labels;

	bool operator==(const Path& p) {
		if (weight == p.weight && labels == p.labels) return true;
		return false;
	}
	bool operator!=(const Path& p) {
		if (weight == p.weight && labels == p.labels) return false; 
		return true;
	}
};

class Path_  {
public:
	int weight;
	bitset<L> labels;
	int tar;
	Path_(const int w, const bitset<L>& l,const int tar) : weight(w), labels(l), tar(tar) {}
	bool operator==(const Path_& p) {
		if (weight == p.weight && labels == p.labels) return true;
		return false;
	}
	bool operator!=(const Path_& p) {
		if (weight == p.weight && labels == p.labels) return false;
		return true;
	}
};
ostream& operator<<(ostream& os, const bitset<L>& lables);
ostream& operator<<(ostream& os, const Path& p);

void showPathSet(list<Path>& tar);
void addPath_pure(vector<Path>& tar, Path& p);
void addPath_pure(vector<Path>& tar, vector<Path>& p);
void addPath(vector<Path>& tar, Path& p);
void addPath(vector<Path>& tar, vector<Path>& p);
void sortPathSet(vector<Path>& tar);

struct Mytuple {
	int src;
	int estimate;
	int real_dis;
	
	Mytuple() :src(0), estimate(0), real_dis(0) {};
	Mytuple(int src,int estimate, int real_dis) :
		src(src), estimate(estimate), real_dis(real_dis){};
};


class point {
public:
	int id;
	int du;//度
	int du2;
	int _du;
	int _du2;
	bool changed;
	map<int, int> neighbors;//点的邻居及在这个点的edge_neibr中是否存了边
public:
	point(int id) : id(id), du(0), du2(0), _du(0), _du2(0), changed(false) {};
	~point();
	void deleteNei(int i);
	bool operator< (const point p) const {
		if (du != p.du)
			return du < p.du; // 比较 DD 数组中的度数
		if (du2 != p.du2)
			return du2 < p.du2; // 如果度数相等，比较 DD2 数组中的度数
		return id < p.id; // 如果度数都相等，比较节点编号大小
	}
};
struct Point_co {
	int id;
	int x, y;
};


bool lessEdge(const Edge& l, const Edge& r);
bool lessPath(const Path& l, const Path& r);
list<Path> pathJoin(list<Path>& a, list<Path>& b,int sep);
void pathJoin_(list<Path>& a, list<Path>& b, list<Path>& pathset);
vector<Path> pathJoin2(list<Path>& a, list<Path>& b);
vector<Path> pathUnion(vector<Path>& a, vector<Path>& b);
void PrunePath(list<Path>& p);
void PrunePath_path_(list<Path_>& p);
void PrunePath_(vector<Path>& p);
bool dominate(Path& a, Path& b);
bool dominate2(Path& a, Path& b);
bool dominate(vector<Path>& p, Path& b);
bool LabelBelong(bitset<L>& a, bitset<L>& b);
bool LabelBelong_true(bitset<L>& a, bitset<L>& b);

#endif
