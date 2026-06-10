#include "utility.h"

bool lessEdge(const Edge& l, const Edge& r) {
	if (l.src < r.src)return true;
	if (l.src > r.src)return false;
	if (l.tar < r.tar)return true;
	return false;
}
bool lessPath(const Path& l, const Path& r) {
	if (l.weight < r.weight)return true;
	if (l.weight == r.weight) return l.labels.size() < r.labels.size();
	return false;
}


point::~point() {
	neighbors.clear();
	//cout << "delete point " << id << endl;
}

void point::deleteNei(int i) {

}

//添加路径
void addPath_pure(vector<Path>& tar, Path& p) {
	tar.emplace_back(p);
}
void addPath_pure(vector<Path>& tar, vector<Path>& p) {
	tar.insert(tar.end(), p.begin(), p.end());
}

void addPath(vector<Path>& tar, Path& p) {
	for (auto it = tar.begin(); it != tar.end();) {
		if (dominate2(*it, p)) return;
		if (dominate(p, *it)) it = tar.erase(it); else ++it;
	}
	tar.emplace_back(p);
}
void addPath(vector<Path>& tar, vector<Path>& p) {
	for (auto e : p) {
		auto it = tar.begin();
		for (; it != tar.end();) {
			if (dominate2(*it, e)) break;
			if (dominate(e, *it)) it = tar.erase(it); else ++it;
		}
		if (it == tar.end()) tar.emplace_back(e);
	}
}

void showPathSet(list<Path>& tar) {
	for (auto& e : tar) {
		cout << e;
	}
}
void sortPathSet(vector<Path>& tar) {
	sort(tar.begin(), tar.end(), lessPath);
}

//路径集拼接
list<Path> pathJoin(list<Path>& a, list<Path>& b,int sep) {
	list<Path> p;
	bitset<L> conLabel = {};

	for (auto& ai : a) {
		for (auto& bj : b) {
			conLabel = ai.labels | bj.labels;
			Path path = { ai.weight + bj.weight, conLabel/*,{sep,&ai,&bj}*/ };
			p.emplace_back(path);
		}
	}
	return p;
}

void pathJoin_(list<Path>& a, list<Path>& b, list<Path>& pathset) {
	bitset<L> conLabel = {};
	Path path;
	for (auto& ai : a) {
		for (auto& bj : b) {
			conLabel = ai.labels | bj.labels;
			path = { ai.weight + bj.weight, conLabel};
			pathset.emplace_back(path);
		}
	}

}

vector<Path> pathJoin2(list<Path>& a, list<Path>& b) {

	vector<Path> p;
	p.reserve(a.size() * b.size());
	bitset<L> conLabel = {};
	for (auto& ai : a) {
		for (auto& bj : b) {
			conLabel = ai.labels | bj.labels;
			Path path = { ai.weight + bj.weight, conLabel};
			p.emplace_back(path);
		}
	}

	return p;
}


vector<Path> pathUnion(vector<Path>& a, vector<Path>& b) {
	vector<Path> p;
	p.reserve(a.size() * b.size());
	for (auto e : a) p.emplace_back(e);
	for (auto e : b) p.emplace_back(e);
	return p;
}

//天际线prune
void PrunePath(list<Path>& p) {
	p.sort([](Path& a, Path& b) {return a.weight < b.weight; });
	for (list<Path>::iterator pre = p.begin(); pre != p.end(); ++pre) {
		for (list<Path>::iterator nex = next(pre); nex != p.end(); ) {
			if (LabelBelong(pre->labels, nex->labels)) {
				nex=p.erase(nex);
			}
			else nex++;
		}
		
	}
}
void PrunePath_path_(list<Path_>& p) {
	p.sort([](Path_& a, Path_& b) {return a.weight < b.weight; });

	for (list<Path_>::iterator pre = p.begin(); pre != p.end(); ++pre) {
		for (list<Path_>::iterator nex = next(pre); nex != p.end(); ) {
			if (LabelBelong(pre->labels, nex->labels)) {
				nex = p.erase(nex);
			}
			else nex++;
		}
	}

}
void PrunePath_(vector<Path>& p) {
	sortPathSet(p);

	vector<bool> to_delete(p.size(), false);
	for (int i = 0; i < p.size(); ++i) {
		if (to_delete[i]) continue;  // 跳过已经标记为删除的路径
		for (int j = i + 1; j < p.size(); ++j) {
			if (to_delete[j]) continue;
			if (LabelBelong(p[i].labels, p[j].labels)) {
				to_delete[j] = true;  // 标记为删除
			}
		}
	}

	// 批量删除所有标记的路径
	auto new_end = remove_if(p.begin(), p.end(), [&](const Path& path) {
		return to_delete[&path - &p[0]];
		});
	p.erase(new_end, p.end());
}

bool dominate(Path& a, Path& b) {//a是否支配b(相等不支配)
	if (a.weight < b.weight && LabelBelong(a.labels, b.labels)) return true;
	if (a.weight <= b.weight && LabelBelong_true(a.labels, b.labels)) return true;
	return false;
}

bool dominate2(Path& a, Path& b) {//a是否支配b(相等则支配)
	if (a.weight <= b.weight && LabelBelong(a.labels, b.labels)) return true;
	return false;
}

bool dominate(vector<Path>& p, Path& b) {
	for (auto& e : p) if (dominate(e, b)) return true;
	return false;
}


bool LabelBelong(bitset<L>& a, bitset<L>& b) {//判断a是否是b的子集
	return (a & b) == a;
}
bool LabelBelong_true(bitset<L>& a, bitset<L>& b) {//判断a是否是b的真子集
	return (a & b) == a ? a == b ? false : true : true;
}

ostream& operator<<(ostream& os, const Path& p) {
	cout << "<" << p.weight << ",";
	for (int i = 0; i < p.labels.size(); ++i) cout << p.labels[i];
	cout << "> ";
	return os;
}

ostream& operator<<(ostream& os, const bitset<L>& lables) {
	for (int i = 0; i < lables.size(); ++i) os << lables[i];
	return os;
}
