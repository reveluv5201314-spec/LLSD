#include "Graph.h"


//Read the graph
void Graph::readGraph(string graphname) {
	cout << "read Graph" << endl;
	ifstream ifs(graphname);
	ifs >> edgeNum;
	int src, tar, label_i; int x = 0;
	double weight;
	vector < pair<int, int>> temp;// <ID, occurrence count>
	unordered_map<int, int> temp2;// <ID, Label>
	for (int i = 0; i < edgeNum; i++) {
		ifs >> src >> tar >> weight >> label_i;
		if (temp2.find(label_i) == temp2.end()) {
			temp.push_back(make_pair(label_i, 1));
			temp2[label_i] = x;
			++x;
		}
		else {
			temp[temp2[label_i]].second++;
		}
	}
	sort(temp.begin(), temp.end(), [](const auto& a, const auto& b) {
		return a.second > b.second;
		});
	temp2.clear();
	int i = 0;
	for (auto& t : temp) {
		tolabel.insert(make_pair(i, make_pair(t.second, t.first)));
		temp2.insert(make_pair(t.first, i));
		++i;

	}

	ifs.close();
	ifs.open(graphname);

	ifs >> edgeNum;
	for (int i = 0; i < edgeNum; i++) {
		ifs >> src >> tar >> weight >> label_i;
		if (src != tar) {
			bitset<L> arr = {}; arr[temp2[label_i]] = 1;
			Edge e = { src, tar, weight,arr };
			edgelist.emplace_back(e);
			tar = src > tar ? src : tar;
			ptsNum = ptsNum > tar ? ptsNum : tar;//Number of points		
		}
	}

	for (int i = 0; i <= ptsNum; i++) {
		node_index.emplace_back(-1);//0,1,2,,,n
	}

	indexanc_LLSD.resize(ptsNum + 1);
	node_height.resize(ptsNum + 1);
	index.resize(ptsNum + 1);
	
	ifs.close();
	cout << edgeNum << " edges have been read" << endl;
	cout << ptsNum << " pts have been read" << endl;
	//sort edge
	organizeGraph();
	sortPaths();
}

//Initialization graph
void Graph::organizeGraph() {
	for (int i = 0; i <= ptsNum; i++) {
		point p(i);
		pts.emplace_back(p);
	}
	for (auto e : edgelist) {
		Path path = Path{ e.weight,e.labels };
		vector<Path> p;
		p.emplace_back(path);
		edges_neibr[e.src][e.tar].emplace_back(path);
		pts[e.src].neighbors.insert(make_pair(e.tar, 1));
		pts[e.src].du++;
		pts[e.src].du2++;
		pts[e.src]._du++;
		pts[e.src]._du2++;

		pts[e.tar].neighbors.insert(make_pair(e.src, 0));
		pts[e.tar].du++;
		pts[e.tar].du2++;
		pts[e.tar]._du++;
		pts[e.tar]._du2++;
	}
}

//Sort the neighbor paths of all points
void Graph::sortPaths() {
	for (auto p : pts) {
		for (auto n : p.neighbors) {
			if (n.second == 1) {
				edges_neibr[p.id][n.first].sort([](Path& a, Path& b) {return a.weight < b.weight; });
			}
			else {
				edges_neibr[n.first][p.id].sort([](Path& a, Path& b) {return a.weight < b.weight; });
			}
		}
	}
}

//Tree decomposition
void Graph1::treeNodeContraction() {
	cout << "treeNodeContraction" << endl;
	double dr_us12 = 0, dr_us23 = 0, dr_us34 = 0, dr_us45 = 0, dr_usjoin = 0, dr_usprune = 0; int i = 0, j = 0;
	set<point> deg;
	for (int i = 1; i <= ptsNum; ++i) {
		deg.insert(pts[i]);
	}
	while (!deg.empty()) {

		//Obtain the minimum degree point
		point* _p = &pts[(*deg.begin()).id];

		// If the degree of a node changes, update it
		while (true) {
			if (_p->changed) {
				deg.erase(*_p);
				_p->du = _p->_du;
				_p->du2 = _p->_du2;
				_p->changed = false;
				deg.insert(*_p);
				_p = &pts[(*deg.begin()).id];
			}
			else break;
		}
		deg.erase(deg.begin()); // Delete point
		++i; ++j;
		if (i == 1000) {
			cout << j << endl;
			i = 0;
		}
		point p = *_p;

		//生成树节点
		auto nod = make_shared<TreeNode>();
		node_index[p.id] = node_arr.size();
		node_arr.emplace_back(nod);
		nod->val.emplace_back(p.id);
		//nod->index.insert(make_pair(p.id, vector<Path>()));

		for (auto e : p.neighbors) {
			nod->val.emplace_back(e.first);
			if (e.second == 1)//If delete_point_id stores edges for e.first
				index[p.id][e.first] = edges_neibr[p.id][e.first];
			else
				index[p.id][e.first] = edges_neibr[e.first][p.id];

		}

		//Delete point
		vector<pair<int, int>> neighbors_list(p.neighbors.begin(), p.neighbors.end());
		for (auto& i : neighbors_list) {//Traverse the neighbors of the minimum degree point
			auto& neighbor = pts[i.first];

			neighbor.neighbors.erase(p.id);
			neighbor._du--;
			if (neighbor._du < (*prev(deg.end())).du) {
				deg.erase(neighbor);
				neighbor.du = neighbor._du;
				neighbor.du2 = neighbor._du2;
				deg.insert(neighbor);
			}
			else neighbor.changed = true;
		}

		int i_first, j_first;
		//Generate neighbor edges
		for (auto i = p.neighbors.begin(); i != p.neighbors.end(); ++i) {
			for (auto j = next(i, 1); j != p.neighbors.end(); ++j) {
				list<Path>* p1, * p2;
				i_first = i->first;
				j_first = j->first;
				if (i->second == 1) p1 = &edges_neibr[p.id][i_first];
				else p1 = &edges_neibr[i_first][p.id];

				if (j->second == 1) p2 = &edges_neibr[p.id][j_first];
				else p2 = &edges_neibr[j_first][p.id];

				auto& I = pts[i_first];
				auto& J = pts[j_first];
				list<Path> paths = pathJoin(*p1, *p2,p.id);
				if (I.neighbors.find(j_first) == I.neighbors.end()) {//If there is no edge between i and j yet
					edges_neibr[i_first][j_first] = paths;
					I.neighbors.insert(make_pair(j_first, 1));
					J.neighbors.insert(make_pair(i_first, 0));//Newly formed edge, storing neighbor information
					I._du++;
					J._du++;
					I._du2++;
					J._du2++;
					PrunePath(edges_neibr[i_first][j_first]);
					I.changed = true;
					J.changed = true;
				}
				else {
					if (I.neighbors[j_first] == 1) {
						edges_neibr[i_first][j_first].insert(edges_neibr[i_first][j_first].end(), paths.begin(), paths.end());
						PrunePath(edges_neibr[i_first][j_first]);
					}
					else {
						edges_neibr[j_first][i_first].insert(edges_neibr[j_first][i_first].end(), paths.begin(), paths.end());
						PrunePath(edges_neibr[j_first][i_first]);
					}
				}
			}
		}
	}
}

//Generate decomposition tree
void Graph1::treeFormation() {
	cout << "treeFormation" << endl;
	//pts.clear();
	for (int i = 0; i < node_arr.size() - 1; i++) {
		int min_index = node_index[node_arr[i]->val[1]];

		for (int j = 1; j < node_arr[i]->val.size(); j++) {
			min_index = min_index < node_index[node_arr[i]->val[j]] ? min_index : node_index[node_arr[i]->val[j]];
		}
		node_arr[i]->parent = node_arr[min_index];
		node_arr[min_index]->children.emplace_back(node_arr[i]);
	}

	(*(node_arr.end() - 1))->parent = NULL;
	cout << "node " << (*(node_arr.end() - 1))->val[0] << " is root" << endl;

	root = node_arr[node_arr.size() - 1];
	root->parent = root;
	cout << "treeFormation-over" << endl;
}

//Tracing back ancestral information
void Graph1::labelAssignment() {
	auto t1 = chrono::steady_clock::now();

	cout << "labelAssignment" << endl;
	list<Path> p; int a = 0, b = 0;
	for (int i = node_arr.size() - 2; i >= 0; --i) {
		if (b >= 1000) {
			cout << "Assign" << a << "/" << ptsNum << endl;
			b = 0;
		}

		++a; ++b;
		auto& vNode = node_arr[i];
		int v = vNode->val[0];
		for (int x = 1; x < vNode->val.size(); ++x) {
			int u = vNode->val[x];
			auto& uNode = index[u];
			for (int y = 1; y < vNode->val.size(); ++y) {
				if (x != y) {
					int w = vNode->val[y];

					if (uNode.find(w) != uNode.end())
						p = pathJoin(index[v][w], uNode[w],w);
					else
						p = pathJoin(index[v][w], index[w][u],w);

					index[v][u].insert(index[v][u].end(), p.begin(), p.end());
					PrunePath(index[v][u]);

				}
			}
		}
	}
	cout << "labelAssignment-over" << endl;

	auto t2 = chrono::steady_clock::now();
	double dr_us = chrono::duration<float, std::ratio<1>>(t2 - t1).count();
	cout << "***labelAssignment time cost:" << dr_us << endl;
	addAnc();
	auto t3 = chrono::steady_clock::now();
	dr_us = chrono::duration<float, std::ratio<1>>(t3 - t2).count();
	cout << "***addAnc time cost:" << dr_us << endl;
	cout << "addAnc-over" << endl;
}

//Output tree
void Graph1::outputTree() {
	cout << "*********Tree :*********" << endl;
	queue<shared_ptr<TreeNode>> q;
	q.push(root);
	while (!q.empty()) {
		shared_ptr<TreeNode> p = q.front();
		q.pop();
		p->showNode(index[p->val[0]]);
		for (auto e : p->children) {
			q.push(e);
		}
	}
	cout << "*********Tree :*********" << endl;
}

//Save ancestor information for nodes and sort the pathset index
void Graph1::addAnc() {
	queue<shared_ptr<TreeNode>> q;
	q.push(root);
	root->valanc.emplace_back(root->val[0]);//The root node's valanc is only its own
	node_height[root->val[0]] = 1;
	while (!q.empty()) {
		shared_ptr<TreeNode> p = q.front();
		q.pop();
		for (auto& c : p->children) {

			for (auto x : p->valanc) c->valanc.emplace_back(x);//Insert the parent node's valanc into the child nodes sequentially
			c->valanc.emplace_back(c->val[0]);//Finally, insert the child node's own val
			node_height[c->val[0]] = c->valanc.size();
			//Sort the path groups of each point in the index, that is, the paths in the index are sorted in ascending order of weight
			for (auto& x : index[c->val[0]])
				x.second.sort([](Path& a, Path& b) {return a.weight < b.weight; });

			q.push(c);
		}
	}
}

//Searching for LCA of s and t
shared_ptr<TreeNode> Graph1::findLca(int s, int t) {
	shared_ptr<TreeNode> S = node_arr[node_index[s]], T = node_arr[node_index[t]];
	auto si = S->valanc.begin(), ti = T->valanc.begin();
	while (*si == *ti) {
		++si; ++ti;
		if (si == S->valanc.end()) return S;
		if (ti == T->valanc.end()) return T;
	}
	return node_arr[node_index[*(--si)]];
}


//LLSDD
int Graph1::LLSD_METHOD(int s, int t, bitset<L> labels,int step) {
	shared_ptr<TreeNode> S = node_arr[node_index[s]], T = node_arr[node_index[t]], LCA = findLca(s, t);
	int lca = LCA->val[0];
	if (LCA == T) {
		return LLSD_single(s, t, labels,step);
	}
	else if (LCA == S) {
		return LLSD_single(t, s, labels,step);
	}
	else {
		return LLSD_layering(s, t, labels,step);
	}

}

inline bool Graph1::has_direct_index(int a, int b) {
	return index[a].count(b) || index[b].count(a);
}


inline int Graph1::compute_weight(const int& src, const int& tar, const vector<int>& valid_labels) {
	if (src == tar) return 0;
	int weight = INT_MAX;
	if (node_height[src] > node_height[tar]) {
		int tar_idx = node_height[tar] - 1;
		auto& i = indexanc_LLSD[src][tar_idx];
		for (int l : valid_labels)
			weight = min(weight, i[l]);
	}
	else {
		int src_idx = node_height[src] - 1;
		auto& i = indexanc_LLSD[tar][src_idx];
		for (int l : valid_labels)
			weight = min(weight, i[l]);
	}
	return weight;
}

template<typename T, typename Container = std::vector<T>, typename Compare = std::less<T>>
struct ExposedPQ : public std::priority_queue<T, Container, Compare> {
	using std::priority_queue<T, Container, Compare>::c; // Expose the bottom container
	// Constructor passed in comparator
	ExposedPQ(const Compare& comp = Compare()) : std::priority_queue<T, Container, Compare>(comp) {}

	void clear() noexcept {
		Container empty;
		this->c.swap(empty);
	}
};

int Graph1::LLSD(int s, int t, bitset<L> labels) {
	if (node_height[s] > node_height[t]) swap(s, t);
	vector<int> valid_labels;
	for (int l = 0; l < L; ++l) {
		if (labels[l] != 0) valid_labels.push_back(l);
	}

	int index_h = 0;
	int min_dis = INT_MAX, weight1 = INT_MAX, weight2 = INT_MAX;
	auto t_1 = chrono::steady_clock::now(), t_2 = chrono::steady_clock::now(), t_3 = chrono::steady_clock::now(), t_4 = chrono::steady_clock::now();
	double dr_us_1 = 0, dr_us_2 = 0, dr_us_3 = 0;
	
	auto compare = [](const Mytuple& a, const Mytuple& b) {
		return a.estimate + a.real_dis > b.estimate + b.real_dis;
		};
	using PQType = ExposedPQ<Mytuple, std::vector<Mytuple>, decltype(compare)>;

	PQType qs(compare), qt(compare);
	vector<Mytuple> qs_, qt_, temp_;//candidate array
	
	shared_ptr<TreeNode> S = node_arr[node_index[s]], T = node_arr[node_index[t]], LCA = findLca(s, t), SRC, PAR;
	int lca = LCA->val[0];
	fill(exist_s.begin(), exist_s.end(), INT_MAX);
	fill(exist_t.begin(), exist_t.end(), INT_MAX);
	unordered_map<int, int> dis_s, dis_t;//temporary storage
	vector<tuple<int, int, int>> hs;//<h,weight>
	unordered_set<int>vals;

	hs.reserve(LCA->val.size());
	for (auto& u : LCA->val) {
		weight1 = compute_weight(s, u, valid_labels);
		weight2 = compute_weight(t, u, valid_labels);


		if (weight1 != INT_MAX && weight2 != INT_MAX) {
			hs.emplace_back(make_tuple(u, weight1, weight2));
			dis_s[u] = INT_MAX; dis_t[u] = INT_MAX;
		}
	};

	if (hs.empty()) return INT_MAX;
	//All the shortest distance lower bounds have been calculated, and then sorted
	sort(hs.begin(), hs.end(), [](tuple<int, int, int>& a, tuple<int, int, int>& b) {return get<1>(a) + get<2>(a) < get<1>(b) + get<2>(b); });
	int now_h = get<0>(hs[index_h]),tar;
	int s_height_diff = node_height[s] - node_height[lca], t_height_diff = node_height[t] - node_height[lca];

	qs.push({ s, get<1>(hs[index_h]), 0 });
	qt.push({ t, get<2>(hs[index_h++]), 0 });
	int next_h = index_h >= hs.size() ? -1 : get<0>(hs[index_h++]);
	int height_diff = max(s_height_diff, t_height_diff);
	int max_size_s = 0,count_s=0, max_size_t = 0, count_t = 0;
	int count1 = 0, remain_size = 0, skip_time = 0; int src;
	Mytuple tup; int real_dis, temp_dis, dis_s_now_h, dis_t_now_h;
	unordered_set<int> par_child_vals;

	while (now_h != -1) {
		//When the minimum distance is less than the next estimated value, end
		if (get<1>(hs[index_h - 2]) + get<2>(hs[index_h - 2]) >= min_dis)
			break;

		//Calculate from s to now_h
		while (qs.size()) {
			
			tup = qs.top();
			src = tup.src;
			real_dis = tup.real_dis;

			if (tup.estimate + real_dis > dis_s[now_h])
				break;

			qs.pop();

			if (exist_s[node_height[src]-1] < real_dis) continue;

			if (has_direct_index(src, now_h)) {
				weight1 = searchDist(src, now_h, labels);
				if (weight1 != INT_MAX) {
					dis_s[now_h] = min(real_dis + weight1, dis_s[now_h]);
					if (dis_t[now_h] != INT_MAX) min_dis = min(min_dis, dis_s[now_h] + dis_t[now_h]);
				}
				//Directly add to the candidate array
				qs_.emplace_back(tup);
				continue;
			}
			SRC = node_arr[node_index[src]], PAR = SRC->parent;
			auto& SRC_val = SRC->val, PAR_val = PAR->val;
			par_child_vals.clear();
			par_child_vals.reserve(SRC_val.size());
			for (auto& e : SRC_val) par_child_vals.insert(e);

			for (auto& u : PAR_val) {
				if (par_child_vals.find(u) != par_child_vals.end()) {
					weight1 = searchDist(src, u, labels);
					if (weight1 == INT_MAX) continue;
					temp_dis = real_dis + weight1;
					if (exist_s[node_height[u] - 1] > temp_dis) {
						weight2 = compute_weight(u, now_h, valid_labels);

						if (weight2 == INT_MAX) {
							qs_.emplace_back(u, weight2, temp_dis);
							continue;
						}

						if (dis_s.find(u) != dis_s.end()) {
							dis_s[u] = min(dis_s[u], temp_dis);
							if (dis_t[u] != INT_MAX) min_dis = min(min_dis, dis_s[u] + dis_t[u]);
						}

						qs.emplace(u, weight2, temp_dis);
						exist_s[node_height[u] - 1] = temp_dis;
					}
					
				}
			}
		}
		dis_s_now_h = dis_s[now_h];

		//Calculate t to now_h, if s cannot reach now_h, then t will not calculate on this side
		if (dis_s_now_h != INT_MAX) while (qt.size()) {
			
			tup = qt.top();
			src = tup.src;
			real_dis = tup.real_dis;

			if (tup.estimate + real_dis > dis_t[now_h])
				break;
			qt.pop();

			if (exist_t[node_height[src]-1] < real_dis) continue;
			if (has_direct_index(src, now_h)) {
				weight1 = searchDist(src, now_h, labels);

				if (weight1 != INT_MAX) {
					dis_t[now_h] = min(real_dis + weight1, dis_t[now_h]);
					if (dis_s[now_h] != INT_MAX) min_dis = min(min_dis, dis_s[now_h] + dis_t[now_h]);
				}

				qt_.emplace_back(tup);
				
				continue;
			}
			SRC = node_arr[node_index[src]], PAR = SRC->parent;
			auto& SRC_val = SRC->val,PAR_val= PAR->val;
			par_child_vals.clear();
			par_child_vals.reserve(SRC_val.size());
			for (auto& e : SRC_val) par_child_vals.insert(e);

			for (auto& u : PAR_val) {
				if (par_child_vals.find(u) != par_child_vals.end()) {
					weight1 = searchDist(src, u, labels);
					if (weight1 == INT_MAX) continue;

					temp_dis = real_dis + weight1;
					
					if (exist_t[node_height[u] - 1] > temp_dis) {
						weight2 = compute_weight(u, now_h, valid_labels);

						if (weight2 == INT_MAX) {
							qt_.emplace_back(u, weight2, temp_dis);
							continue;
						}

						if (dis_t.find(u) != dis_t.end()) {
							dis_t[u] = min(dis_t[u], temp_dis);
							if (dis_s[u] != INT_MAX) min_dis = min(min_dis, dis_s[u] + dis_t[u]);
						}
					
						qt.emplace(u, weight2, temp_dis);
						exist_t[node_height[u]-1] = temp_dis;
					}
					
				}
			}
		}
		dis_t_now_h = dis_t[now_h];

		//sum
		if (dis_s_now_h != INT_MAX && dis_t_now_h != INT_MAX) if (min_dis > dis_s_now_h + dis_t_now_h)
			min_dis = dis_s_now_h + dis_t_now_h;

		//qs switches to the next h
		if (next_h != -1) {
			move(qs.c.begin(), qs.c.end(), std::back_inserter(qs_));
			qs.clear();
			temp_.clear();
			
			for (auto& tup : qs_) {
				src = tup.src;
				real_dis = tup.real_dis;

				if (src == next_h) {
					dis_s[next_h] = min(dis_s[next_h], real_dis);
					temp_.emplace_back(tup);
					continue;
				}
				if (exist_s[node_height[src] - 1] >= real_dis) {
					weight2 = compute_weight(src, next_h, valid_labels);
					if (weight2 == INT_MAX) {
						temp_.emplace_back(tup);
						continue;
					}
					qs.emplace(src, weight2, real_dis);
					exist_s[node_height[src]-1] = real_dis;
				}
			}
			qs_ = temp_;
		}
		if (next_h != -1) {
			move(qt.c.begin(), qt.c.end(), std::back_inserter(qt_));
			qt.clear();
			temp_.clear();
			
			for (auto& tup : qt_) {
				src = tup.src;
				real_dis = tup.real_dis;

				if (src == next_h) {
					dis_t[next_h] = min(dis_t[next_h], real_dis);
					temp_.emplace_back(tup);
					continue;
				}
				if (exist_t[node_height[src] - 1] >= real_dis) {
					weight2 = compute_weight(src, next_h, valid_labels);
					
					if (weight2 == INT_MAX) {
						temp_.emplace_back(tup);
						continue;
					}

					qt.emplace(src, weight2, real_dis);
					exist_t[node_height[src]-1] = real_dis;
				}
			}
			qt_ = temp_;
		}

		now_h = next_h;
		next_h = index_h >= hs.size() ? -1 : get<0>(hs[index_h++]);
	
	}

	return min_dis;
}

//Unilateral situation
int Graph1::LLSD_single(int s, int t, bitset<L> labels,int step) {
	vector<int> valid_labels;
	for (int l = 0; l < L; ++l) {
		if (labels[l] != 0) valid_labels.push_back(l);
	}

	auto compare = [](const Mytuple& a, const Mytuple& b) {
		return a.estimate + a.real_dis > b.estimate + b.real_dis;
		};
	using PQType = ExposedPQ<Mytuple, std::vector<Mytuple>, decltype(compare)>;

	PQType qs(compare);
	vector<Mytuple> qs_, temp_;

	shared_ptr<TreeNode> S = node_arr[node_index[s]], LCA = node_arr[node_index[t]], S_TAR = S, SRC, PAR;
	fill(exist_s.begin(), exist_s.end(), INT_MAX);

	unordered_map<int, int> dis_s;//Record this round
	unordered_map<int, int> dis_s_all;//Record all

	int weight1 = INT_MAX, weight2 = INT_MAX;
	int max_size = 0;
	int src, s_tar = s, tar_h, tar_n, lca = t;
	int s_height_diff = node_height[s] - node_height[lca];
	int first_step_s = s_height_diff % step;
	Mytuple tup; int real_dis, temp_dis, dis_s_now_h, dis_t_now_h;
	unordered_set<int> par_child_vals;
	bool flag_s = 1;
	vector<int> hs;
	dis_s_all[s] = 0;
	//Keep counting until the last layer
	while (flag_s) {
		qs.clear();
		qs_.clear();
		hs.clear();

		//Set the starting point based on the endpoint of the previous round
		for (auto& x : S_TAR->val) if (dis_s_all.find(x) != dis_s_all.end() && dis_s_all[x] != INT_MAX) qs.push({ x, 0, dis_s_all[x] });
		//Update endpoint
		if (node_height[s_tar] - node_height[lca] <= step) {
			flag_s = 0;
			continue;
		}
		else if (first_step_s != 0) {
			s_tar = *(S_TAR->valanc.rbegin() + first_step_s);
			first_step_s = 0;
		}
		else s_tar = *(S_TAR->valanc.rbegin() + step);
		S_TAR = node_arr[node_index[s_tar]];

		//Set the endpoint and record position for this round
		dis_s.clear();
		for (auto& x : S_TAR->val) if (dis_s_all.find(x) == dis_s_all.end()) {
			dis_s[x] = INT_MAX;
			hs.emplace_back(x);
		}

		tar_n = hs.size();

		for (auto& x : hs) {
			tar_h = x;
			//switch h
			move(qs.c.begin(), qs.c.end(), back_inserter(qs_));
			qs.clear();
			temp_.clear();


			for (auto& tup : qs_) {
				src = tup.src;
				real_dis = tup.real_dis;

				if (src == tar_h) {
					dis_s[tar_h] = min(dis_s[tar_h], real_dis);
					temp_.emplace_back(tup);
					continue;
				}
				if (exist_s[node_height[src] - 1] >= real_dis) {
					weight2 = compute_weight(src, tar_h, valid_labels);

					if (weight2 == INT_MAX) {
						temp_.emplace_back(tup);
						continue;
					}
					qs.emplace(src, weight2, real_dis);
					exist_s[node_height[src] - 1] = real_dis;
				}
			}
			qs_ = temp_;

			//Calculate s to s_tar
			while (qs.size()) {
				tup = qs.top();
				src = tup.src;
				real_dis = tup.real_dis;
				if (tup.estimate + real_dis > dis_s[tar_h]) break;
				qs.pop();

				if (exist_s[node_height[src] - 1] < real_dis) continue;
				computed.insert(src);
				if (has_direct_index(src, tar_h)) {
					weight1 = searchDist(src, tar_h, labels);
					if (weight1 != INT_MAX) dis_s[tar_h] = min(real_dis + weight1, dis_s[tar_h]);
					
					qs_.emplace_back(tup);
					continue;
				}

				SRC = node_arr[node_index[src]], PAR = SRC->parent;
				par_child_vals.clear();
				par_child_vals.reserve(SRC->val.size());
				for (auto& e : SRC->val) par_child_vals.insert(e);

				for (auto& u : PAR->val) {
					if (par_child_vals.find(u) != par_child_vals.end()) {
						weight1 = searchDist(src, u, labels);
						if (weight1 == INT_MAX) continue;
						temp_dis = real_dis + weight1;
						if (exist_s[node_height[u] - 1] > temp_dis) {
							weight2 = compute_weight(u, tar_h, valid_labels);

							if (weight2 == INT_MAX) {
								qs_.emplace_back(u, weight2, temp_dis);
								continue;
							}

							if (dis_s.find(u) != dis_s.end()) dis_s[u] = min(dis_s[u], temp_dis);

							qs.emplace(u, weight2, temp_dis);
							exist_s[node_height[u] - 1] = temp_dis;
						}

					}
				}
			}
		}
		//Save the records of this round
		for (auto& x : dis_s) dis_s_all[x.first] = x.second;
	}

	int index_h = 0, min_dis = INT_MAX;
	int tar;

	qs.clear();
	qs_.clear();
	for (auto& x : S_TAR->val) if (dis_s_all.find(x) != dis_s_all.end() && dis_s_all[x] != INT_MAX) {
		qs.emplace(x, compute_weight(x, t, valid_labels), dis_s_all[x]);
		if (x==t) min_dis = min(min_dis, dis_s_all[x]);	
	}

	while (!qs.empty()) {
		tup = qs.top();
		src = tup.src;
		real_dis = tup.real_dis;

		if (tup.estimate + real_dis > min_dis)
			break;
		qs.pop();

		if (exist_s[node_height[src] - 1] < real_dis) continue;
		computed.insert(src);
		if (has_direct_index(src, t)) {
			weight1 = searchDist(src, t, labels);
			if (weight1 != INT_MAX) {
				min_dis = min(real_dis + weight1, min_dis);
			}
			
			qs_.emplace_back(tup);
			continue;
		}
		SRC = node_arr[node_index[src]], PAR = SRC->parent;
		auto& SRC_val = SRC->val, PAR_val = PAR->val;
		par_child_vals.clear();
		par_child_vals.reserve(SRC_val.size());
		for (auto& e : SRC_val) par_child_vals.insert(e);

		for (auto& u : PAR_val) {
			if (par_child_vals.find(u) != par_child_vals.end()) {
				weight1 = searchDist(src, u, labels);
				if (weight1 == INT_MAX) continue;
				temp_dis = real_dis + weight1;

				if (exist_s[node_height[u] - 1] > temp_dis) {
					weight2 = compute_weight(u, t, valid_labels);

					if (weight2 == INT_MAX) {
						qs_.emplace_back(u, weight2, temp_dis);
						continue;
					}
					if (u==t) {
						min_dis = min(min_dis, temp_dis);
					}

					qs.emplace(u, weight2, temp_dis);
					exist_s[node_height[u] - 1] = temp_dis;
				}
			}
		}
	}

	return min_dis;
}
//hierarchical
int Graph1::LLSD_layering(int s, int t, bitset<L> labels,int step) {
	if (node_height[s] > node_height[t]) swap(s, t);
	vector<int> valid_labels;
	for (int l = 0; l < L; ++l) {
		if (labels[l] != 0) valid_labels.push_back(l);
	}

	auto compare = [](const Mytuple& a, const Mytuple& b) {
		return a.estimate + a.real_dis > b.estimate + b.real_dis;
		};
	using PQType = ExposedPQ<Mytuple, std::vector<Mytuple>, decltype(compare)>;

	PQType qs(compare), qt(compare);
	vector<Mytuple> qs_, qt_, temp_;

	shared_ptr<TreeNode> S = node_arr[node_index[s]], T = node_arr[node_index[t]], LCA = findLca(s, t), S_TAR = S, T_TAR = T, SRC, PAR;
	fill(exist_s.begin(), exist_s.end(), INT_MAX);
	fill(exist_t.begin(), exist_t.end(), INT_MAX);
	unordered_map<int, int> dis_s, dis_t;//Record this round
	unordered_map<int, int> dis_s_all, dis_t_all;//Record all

	int weight1 = INT_MAX, weight2 = INT_MAX;
	int max_size = 0;
	int src, s_tar = s, t_tar = t, tar_h, tar_n, lca = LCA->val[0];
	int s_height_diff = node_height[s] - node_height[lca], t_height_diff = node_height[t] - node_height[lca];
	int first_step_s = s_height_diff % step, first_step_t = t_height_diff % step;
	Mytuple tup; int real_dis, temp_dis, dis_s_now_h, dis_t_now_h;
	unordered_set<int> par_child_vals;
	bool flag_s = 1, flag_t = 1;
	vector<int> hs;
	dis_s_all[s] = 0; dis_t_all[t] = 0;
	int final_mid = 0;
	//Keep counting until the last layer
	while (flag_s || flag_t) {
		//s side
		if (flag_s) {
			qs.clear();
			qs_.clear();
			hs.clear();

			//Set the starting point based on the endpoint of the previous round
			for (auto& x : S_TAR->val) if (dis_s_all.find(x) != dis_s_all.end() && dis_s_all[x] != INT_MAX) qs.push({ x, 0, dis_s_all[x] });
			//Update endpoint
			if (node_height[s_tar] - node_height[lca] <= step) {
				flag_s = 0;
				continue;
			}
			else if (first_step_s != 0) {
				s_tar = *(S_TAR->valanc.rbegin() + first_step_s);
				first_step_s = 0;
			}
			else s_tar = *(S_TAR->valanc.rbegin() + step);
			S_TAR = node_arr[node_index[s_tar]];

			//Set the endpoint and record position for this round
			dis_s.clear();
			for (auto& x : S_TAR->val) if (dis_s_all.find(x) == dis_s_all.end()) {
				dis_s[x] = INT_MAX;
				hs.emplace_back(x);
			}

			tar_n = hs.size();

			int ind = 0;
			for (auto& x : hs) {
				tar_h = x;
				//qs switching h
				move(qs.c.begin(), qs.c.end(), back_inserter(qs_));
				qs.clear();
				temp_.clear();

				for (auto& tup : qs_) {
					src = tup.src;
					real_dis = tup.real_dis;
					
					if (src == tar_h) {
						dis_s[tar_h] = min(dis_s[tar_h], real_dis);
						temp_.emplace_back(tup);
						continue;
					}
					if (exist_s[node_height[src] - 1] >= real_dis) {
						weight2 = compute_weight(src, tar_h, valid_labels);

						if (weight2 == INT_MAX) {
							temp_.emplace_back(tup);
							continue;
						}
						if (weight2 + real_dis >= dis_s[tar_h]) {
							temp_.emplace_back(tup);
							continue;
						}

						qs.emplace(src, weight2, real_dis);
						exist_s[node_height[src] - 1] = real_dis;
					}
				}
				qs_ = temp_;

				//Calculate s to s_tar
				while (qs.size()) {
					tup = qs.top();
					src = tup.src;
					real_dis = tup.real_dis;
					if (tup.estimate + real_dis > dis_s[tar_h]) break;
					qs.pop();
					

					if (exist_s[node_height[src] - 1] < real_dis) continue;
					computed.insert(src);

					if (has_direct_index(src, tar_h)) {
						weight1 = searchDist(src, tar_h, labels);
						if (weight1 != INT_MAX) {
							dis_s[tar_h] = min(real_dis + weight1, dis_s[tar_h]);
						}
						
						qs_.emplace_back(tup);
						continue;
					}

					SRC = node_arr[node_index[src]], PAR = SRC->parent;
					par_child_vals.clear();
					par_child_vals.reserve(SRC->val.size());
					for (auto& e : SRC->val) par_child_vals.insert(e);

					for (auto& u : PAR->val) {
						if (par_child_vals.find(u) != par_child_vals.end()) {
							weight1 = searchDist(src, u, labels);
							
							if (weight1 == INT_MAX) continue;
							temp_dis = real_dis + weight1;
							if (exist_s[node_height[u] - 1] > temp_dis) {
								weight2 = compute_weight(u, tar_h, valid_labels);

								if (weight2 == INT_MAX) {
									qs_.emplace_back(u, weight2, temp_dis);
									continue;
								}

								if (dis_s.find(u) != dis_s.end()) {
									dis_s[u] = min(dis_s[u], temp_dis);
								}

								qs.emplace(u, weight2, temp_dis);
								exist_s[node_height[u] - 1] = temp_dis;
							}
						}
					}
				}
				++ind;
			}
			//Save the records of this round
			for (auto& x : dis_s) dis_s_all[x.first] = x.second;
		}

		//t side
		if (flag_t) {
			qt.clear();
			qt_.clear();
			hs.clear();

			//Set the starting point based on the endpoint of the previous round
			for (auto& x : T_TAR->val) if (dis_t_all.find(x) != dis_t_all.end() && dis_t_all[x] != INT_MAX) qt.push({ x, 0, dis_t_all[x] });
			//Update endpoint
			if (node_height[t_tar] - node_height[lca] <= step) {
				flag_t = 0;
				continue;
			}
			else if (first_step_t != 0) {
				t_tar = *(T_TAR->valanc.rbegin() + first_step_t);
				first_step_t = 0;
			}
			else t_tar = *(T_TAR->valanc.rbegin() + step);
			T_TAR = node_arr[node_index[t_tar]];

			//Set the endpoint and record position for this round
			dis_t.clear();
			for (auto& x : T_TAR->val) if (dis_t_all.find(x) == dis_t_all.end()) {
				dis_t[x] = INT_MAX;
				hs.emplace_back(x);
			}

			tar_n = hs.size();
			int ind = 0;
			for (auto& x : hs) {
				tar_h = x;
				//qt switch h
				move(qt.c.begin(), qt.c.end(), back_inserter(qt_));
				qt.clear();
				temp_.clear();
				
				for (auto& tup : qt_) {
					src = tup.src;
					real_dis = tup.real_dis;

					if (src == tar_h) {
						dis_t[tar_h] = min(dis_t[tar_h], real_dis);
						temp_.emplace_back(tup);
						continue;
					}
					if (exist_t[node_height[src] - 1] >= real_dis) {
						weight2 = compute_weight(src, tar_h, valid_labels);

						if (weight2 == INT_MAX) {
							temp_.emplace_back(tup);
							continue;
						}
						if (weight2 + real_dis >= dis_t[tar_h]) {
							temp_.emplace_back(tup);
							continue;
						}
						qt.emplace(src, weight2, real_dis);
						exist_t[node_height[src] - 1] = real_dis;
					}
				}
				qt_ = temp_;

				//Calculate t to t_tar
				while (qt.size()) {
					tup = qt.top();
					src = tup.src;
					real_dis = tup.real_dis;
					if (tup.estimate + real_dis > dis_t[tar_h]) break;
					qt.pop();

					if (exist_t[node_height[src] - 1] < real_dis) continue;
					
					computed.insert(src);
					if (has_direct_index(src, tar_h)) {
						weight1 = searchDist(src, tar_h, labels);
						if (weight1 != INT_MAX) {
							dis_t[tar_h] = min(real_dis + weight1, dis_t[tar_h]);

						}
						
						qt_.emplace_back(tup);
						continue;
					}

					SRC = node_arr[node_index[src]], PAR = SRC->parent;
					par_child_vals.clear();
					par_child_vals.reserve(SRC->val.size());
					for (auto& e : SRC->val) par_child_vals.insert(e);

					for (auto& u : PAR->val) {
						if (par_child_vals.find(u) != par_child_vals.end()) {
							weight1 = searchDist(src, u, labels);
							if (weight1 == INT_MAX) continue;
							temp_dis = real_dis + weight1;
							if (exist_t[node_height[u] - 1] > temp_dis) {
								weight2 = compute_weight(u, tar_h, valid_labels);

								if (weight2 == INT_MAX) {
									qt_.emplace_back(u, weight2, temp_dis);
									continue;
								}

								if (dis_t.find(u) != dis_t.end()) {
									dis_t[u] = min(dis_t[u], temp_dis);
								}
								qt.emplace(u, weight2, temp_dis);
								exist_t[node_height[u] - 1] = temp_dis;
							}
						}
					}
				}
				++ind;
			}

			//Save the records of this round
			for (auto& x : dis_t) dis_t_all[x.first] = x.second;
		}
	}

	//The last layer is sorted
	vector<pair<int, int>> hs_;//<h,weight>
	hs_.reserve(LCA->val.size());
	dis_s.clear();
	dis_t.clear();
	vector<int> S_VAL, T_VAL;
	for (auto& x : S_TAR->val) if (dis_s_all.find(x) != dis_s_all.end() && dis_s_all[x] != INT_MAX) S_VAL.emplace_back(x);
	for (auto& x : T_TAR->val) if (dis_t_all.find(x) != dis_t_all.end() && dis_t_all[x] != INT_MAX) T_VAL.emplace_back(x);

	for (auto& u : LCA->val) {
		weight1 = INT_MAX;
		for (auto& x : S_VAL) weight1 = min(weight1, compute_weight(x, u, valid_labels) + dis_s_all[x]);
		if (weight1 == INT_MAX) continue;
		weight2 = INT_MAX;
		for (auto& x : T_VAL) weight2 = min(weight2, compute_weight(x, u, valid_labels) + dis_t_all[x]);

		if (weight2 != INT_MAX) {
			hs_.emplace_back(u, weight1 + weight2);
			dis_s[u] = INT_MAX; dis_t[u] = INT_MAX;
		}
	};

	if (hs_.empty())return INT_MAX;

	
	sort(hs_.begin(), hs_.end(), [](pair<int, int>& a, pair<int, int>& b) {return a.second < b.second; });
	int index_h = 0, min_dis = INT_MAX;
	int now_h = hs_[index_h++].first, next_h = index_h >= hs_.size() ? -1 : hs_[index_h++].first, tar;

	qs.clear();
	qs_.clear();
	qt.clear();
	qt_.clear();
	//Pre calculate a mindis
	for (auto& x : S_VAL) {
		qs.emplace(x, compute_weight(x, now_h, valid_labels), dis_s_all[x]);
		if (dis_s.find(x) != dis_s.end()) {
			dis_s[x] = min(dis_s_all[x], dis_s[x]);
			if (dis_t[x] != INT_MAX) {
				min_dis = min(min_dis, dis_s[x] + dis_t[x]);
				final_mid = x;
			}
		}
	}
	for (auto& x : T_VAL) {
		qt.emplace(x, compute_weight(x, now_h, valid_labels), dis_t_all[x]);
		if (dis_t.find(x) != dis_t.end()) {
			dis_t[x] = min(dis_t_all[x], dis_t[x]);
			if (dis_s[x] != INT_MAX) {
				min_dis = min(min_dis, dis_s[x] + dis_t[x]);
				final_mid = x;
			}
		}
	}

	while (now_h != -1) {
		//When the minimum distance is less than the next estimated value, end
		if (hs_[index_h - 2].second >= min_dis)
			break;

		//Calculate from s to now_h
		while (qs.size()) {
			tup = qs.top();
			src = tup.src;
			real_dis = tup.real_dis;

			if (tup.estimate + real_dis > dis_s[now_h])
				break;
			qs.pop();

			if (exist_s[node_height[src] - 1] < real_dis) continue;
			computed.insert(src);

			if (has_direct_index(src, now_h)) {
				weight1 = searchDist(src, now_h, labels);
				if (weight1 != INT_MAX) {
					temp_dis = real_dis + weight1;
					if (exist_s[node_height[now_h] - 1] > temp_dis) {

					dis_s[now_h] = min(real_dis + weight1, dis_s[now_h]);
					if (dis_t[now_h] != INT_MAX) {
						min_dis = min(min_dis, dis_s[now_h] + dis_t[now_h]);
						final_mid = now_h;
					}
					node_arr[node_index[now_h]]->prev1 = node_arr[node_index[src]];
					exist_s[node_height[now_h] - 1] = temp_dis;
					}
				}
				
				qs_.emplace_back(tup);
				continue;
			}
			SRC = node_arr[node_index[src]], PAR = SRC->parent;
			auto& SRC_val = SRC->val, PAR_val = PAR->val;
			par_child_vals.clear();
			par_child_vals.reserve(SRC_val.size());
			for (auto& e : SRC_val) par_child_vals.insert(e);
			for (auto& u : PAR_val) {
				if (par_child_vals.find(u) != par_child_vals.end()) {
					weight1 = searchDist(src, u, labels);
					if (weight1 == INT_MAX) continue;
					temp_dis = real_dis + weight1;
					
					if (exist_s[node_height[u] - 1] > temp_dis) {
						weight2 = compute_weight(u, now_h, valid_labels);

						if (weight2 == INT_MAX) {
							qs_.emplace_back(u, weight2, temp_dis);
							continue;
						}

						if (dis_s.find(u) != dis_s.end()) {
							dis_s[u] = min(dis_s[u], temp_dis);
							if (dis_t[u] != INT_MAX) {
								min_dis = min(min_dis, dis_s[u] + dis_t[u]);
								final_mid = u;
							}
						}

						qs.emplace(u, weight2, temp_dis);
						node_arr[node_index[u]]->prev1 = node_arr[node_index[src]];
						exist_s[node_height[u] - 1] = temp_dis;
					}

				}
			}

		}
		dis_s_now_h = dis_s[now_h];

		//Calculate t to now_h, if s cannot reach now_h, then t will not calculate on this side
		if (dis_s_now_h != INT_MAX) while (qt.size()) {
			tup = qt.top();
			src = tup.src;
			real_dis = tup.real_dis;

			if (tup.estimate + real_dis > dis_t[now_h])
				break;

			qt.pop();



			if (exist_t[node_height[src] - 1] < real_dis) continue;
			computed.insert(src);

			if (has_direct_index(src, now_h)) {
				weight1 = searchDist(src, now_h, labels);
				if (weight1 != INT_MAX) {
					temp_dis = real_dis + weight1;
					if (exist_t[node_height[now_h] - 1] > temp_dis) {

					dis_t[now_h] = min(real_dis + weight1, dis_t[now_h]);
					if (dis_s[now_h] != INT_MAX) {
						min_dis = min(min_dis, dis_s[now_h] + dis_t[now_h]);
						final_mid = now_h;
					}

					node_arr[node_index[now_h]]->prev2 = node_arr[node_index[src]];
					exist_t[node_height[now_h] - 1] = temp_dis;
					
					}
				}
				
				qt_.emplace_back(tup);
				continue;
			}
			SRC = node_arr[node_index[src]], PAR = SRC->parent;
			auto& SRC_val = SRC->val, PAR_val = PAR->val;
			par_child_vals.clear();
			par_child_vals.reserve(SRC_val.size());
			for (auto& e : SRC_val) par_child_vals.insert(e);
			for (auto& u : PAR_val) {
				if (par_child_vals.find(u) != par_child_vals.end()) {
					weight1 = searchDist(src, u, labels);
					if (weight1 == INT_MAX) continue;
					
					temp_dis = real_dis + weight1;

					if (exist_t[node_height[u] - 1] > temp_dis) {
						weight2 = compute_weight(u, now_h, valid_labels);

						if (weight2 == INT_MAX) {
							qt_.emplace_back(u, weight2, temp_dis);
							continue;
						}

						if (dis_t.find(u) != dis_t.end()) {
							dis_t[u] = min(dis_t[u], temp_dis);
							if (dis_s[u] != INT_MAX) {
								min_dis = min(min_dis, dis_s[u] + dis_t[u]);
								final_mid = u;
							}
						}

						qt.emplace(u, weight2, temp_dis);
						node_arr[node_index[u]]->prev2 = node_arr[node_index[src]];
						exist_t[node_height[u] - 1] = temp_dis;
					}
				}
			}
		}
		dis_t_now_h = dis_t[now_h];

		//sum
		if (dis_s_now_h != INT_MAX && dis_t_now_h != INT_MAX) if (min_dis > dis_s_now_h + dis_t_now_h) {
			min_dis = dis_s_now_h + dis_t_now_h;
			final_mid = now_h;
		}

		//Switch h
		if (next_h != -1) {
			std::move(qs.c.begin(), qs.c.end(), std::back_inserter(qs_));
			qs.clear();
			temp_.clear();
			for (auto& tup : qs_) {
				src = tup.src;
				real_dis = tup.real_dis;

				if (src == next_h) {
					dis_s[next_h] = min(dis_s[next_h], real_dis);
					temp_.emplace_back(tup);
					continue;
				}
				if (exist_s[node_height[src] - 1] >= real_dis) {
					weight2 = compute_weight(src, next_h, valid_labels);

					if (weight2 == INT_MAX) {
						temp_.emplace_back(tup);
						continue;
					}
					if (weight2 + real_dis >= dis_s[next_h]) {
						//temp++;
						temp_.emplace_back(tup);
						continue;
					}
					qs.emplace(src, weight2, real_dis);
					exist_s[node_height[src] - 1] = real_dis;
				}
			}
			qs_ = temp_;
		}
		if (next_h != -1) {
			std::move(qt.c.begin(), qt.c.end(), std::back_inserter(qt_));
			qt.clear();
			temp_.clear();
			for (auto& tup : qt_) {
				src = tup.src;
				real_dis = tup.real_dis;

				if (src == next_h) {
					dis_t[next_h] = min(dis_t[next_h], real_dis);
					temp_.emplace_back(tup);
					continue;
				}
				if (exist_t[node_height[src] - 1] >= real_dis) {
					weight2 = compute_weight(src, next_h, valid_labels);

					if (weight2 == INT_MAX) {
						temp_.emplace_back(tup);
						continue;
					}

					if (weight2 + real_dis >= dis_t[next_h]) {
						temp_.emplace_back(tup);
						continue;
					}

					qt.emplace(src, weight2, real_dis);
					exist_t[node_height[src] - 1] = real_dis;
				}
			}
			qt_ = temp_;
		}
		now_h = next_h;
		next_h = index_h >= hs_.size() ? -1 : hs_[index_h++].first;
	}

	return min_dis;

}


//Update the minimum value for atomic variables
inline void atomic_min(std::atomic<int>& target, int value) {
	int old_val = target.load();
	while (old_val > value && !target.compare_exchange_weak(old_val, value)) {}
}

//Find the shortest distance between u and v under L
int Graph1::searchDist(int& u, int& v, bitset<L>& labels) {
	if (u == v) return 0;
	if (index[u].find(v) != index[u].end()) {
		for (auto& x : index[u][v]) if ((x.labels & labels) == x.labels) return x.weight; return INT_MAX;//The index path increases weight incrementally
	}
	else {
		for (auto& x : index[v][u]) if ((x.labels & labels) == x.labels) return x.weight; return INT_MAX;
	}
	return INT_MAX;
}



//Divide the tag length into levels and calculate the average time for each level
void Graph1::queryx(string name,int step) {
	ifstream query(name);
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> lab(0, 9);
	uniform_int_distribution<int> points(1, ptsNum);

	string out_name = "time_result_" + name;
	ofstream record_output(out_name);

	vector<double>Q(5);

	int s, t; shared_ptr<TreeNode> LCA;

	int counts = 1000;

	vector<int>weights(L);
	for (int i = 1; i < L;++i) {
		weights[i] = tolabel[i].first;
	}

	unordered_map<int, pair<int, int>> co; char ch;

	vector<int> visited(5);
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < counts; ++j) {
			double ratio = 0;
			computed.clear();

			bitset<L> labels;
			query >> s >> t >> labels;

			auto t1 = chrono::steady_clock::now();
			int dis_LLSD = LLSD_METHOD(s, t, labels, step);
			auto t2 = chrono::steady_clock::now();
			auto dr_us = chrono::duration<double, milli>(t2 - t1).count();
			visited[i] += computed.size();
			cout << "qurey " << i + 1 << "-" << j + 1 << ", "; cout << "s=" << s << " ,t=" << t << " ,label=" << labels << ", dis_LLSD=" << dis_LLSD << "m" << endl;
			Q[i] += dr_us;
		}
	}
	query.close();

	double total = 0;
	for (int i = 0; i < 5; ++i) {
		Q[i] /= counts;
		total += Q[i];;
	}

	cout << "LLSDD :average query time is " << endl;
	record_output << "LLSDD :average query time is " << endl;
	for (int i = 0; i < 5; ++i) {
		cout << Q[i] << " ";
		record_output << Q[i] << " ";
	}
	cout << endl; record_output << endl;

	cout << "LLSDD visited nodes:" << endl;
	record_output << "LLSDD visited nodes:" << endl;
	for (int i = 0; i < 5; ++i) {
		cout << (double)visited[i]/1000 << " ";
		record_output << (double)visited[i] / 1000 << " ";
	}
	cout << endl; record_output << endl;
	cout << "LLSDD average query time is " << total / 5 << "ms" << endl;
	record_output << "LLSDD average query time is " << total / 5 << "ms" << endl;
}

//Fix q and then change beta, i.e. step
void Graph1::queryx2(string name) {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> lab(0, 9);
	uniform_int_distribution<int> points(1, ptsNum);

	string out_name = "beta_time_result_" + name;
	ofstream record_output(out_name);

	vector<double>Q(10);

	int s, t; shared_ptr<TreeNode> LCA;
	int counts = 1000;

	vector<int>weights(L);
	for (int i = 1; i < L; ++i) {
		weights[i] = 1; tolabel[i].first;
	}

	unordered_map<int, pair<int, int>> co; char ch;
	
	for (int i = 0; i < 5; ++i) {
		
		ifstream query(name);
		int step = ((i + 1) / (double)10) * height;
		bitset<L> labels;
		for (int x = 0; x < 2000; ++x) {
			query >> s >> t >> labels;
		}
		for (int j = 0; j < counts; ++j) {
			double ratio = 0;

			bitset<L> labels;
			query >> s >> t >> labels;

			auto t1 = chrono::steady_clock::now();
			int dis_LLSD = LLSD_METHOD(s, t, labels,step);
			auto t2 = chrono::steady_clock::now();
			auto dr_us = chrono::duration<double, milli>(t2 - t1).count();


			cout << "qurey " << i + 1 << "-" << j + 1 << ", "; cout << "s=" << s << " ,t=" << t << " ,label=" << labels << ", dis_LLSD=" << dis_LLSD << "m" << endl;

			Q[i] += dr_us;

		}
		query.close();
	}
	double total = 0;
	for (int i = 0; i < 5; ++i) {
		Q[i] /= counts;
		total += Q[i];
	}

	cout << "LLSDD :average query time is " << endl;
	record_output << "LLSDD :average query time is " << endl;
	for (int i = 0; i < 5; ++i) {
		cout << Q[i] << " ";
		record_output << Q[i] << " ";
	}
	cout << endl; record_output << endl;
	cout << "LLSDD average query time is " << total / 5 << "ms" << endl;
	record_output << "LLSDD average query time is " << total / 5 << "ms" << endl;
}


void Graph1::query(int s, int t, bitset<L> labels, int step) {
	cout << "*********************************" << endl;
	
	auto t1 = chrono::steady_clock::now();	
	int dis_LLSD = LLSD_METHOD(s, t, labels, step);
	auto t2 = chrono::steady_clock::now();
	auto dr_us = chrono::duration<double, micro>(t2 - t1).count();
	cout << "dis_LLSD=" << dis_LLSD << "m" << endl;
	cout << "time cost:" << dr_us << "us" << endl;

	cout << "*********************************" << endl << endl;
}


vector<Path> Graph1::LLSDJoin(int v, int u, int w) {
	vector<Path> p; array<int, L> vu = indexanc_LLSD[v][node_height[u] - 1], uw;
	if (node_height[u] > node_height[w])
		uw = indexanc_LLSD[u][node_height[w] - 1];
	else uw = indexanc_LLSD[w][node_height[u] - 1];

	p.reserve(vu.size() * uw.size());
	int i = 0;
	bitset<L> arr;
	for (auto p1 : vu) {
		if (p1 != INT_MAX) {
			int j = 0;
			for (auto p2 : uw) {
				if (p2 != INT_MAX) {
					arr = {};
					arr[i] = 1;
					arr[j] = 1;
					Path path = { p1 + p2,arr };
					p.emplace_back(path);
				}
				++j;
			}
		}
		++i;
	}
	return p;
}

void Graph1::settingLLSD() {
	vector<int> flag(ptsNum + 1, 0);
	int counts = 0,n;
	stack<shared_ptr<TreeNode>> q;

	array<int, L> entry;
	n = indexanc_LLSD.size();
	int total_n = 0;

	int nums = 0;
	temp_index.resize(ptsNum + 1);
	q.push(root);
	int progress = 0, step = 0,p_val_i;
	list<Path> Paths, temp;
	list<Path_> temp_;
	while (!q.empty()) {
		shared_ptr<TreeNode> p = q.top();
		int v = p->val[0],n_v= p->valanc.size(),height_v = node_height[v];
		q.pop();

		indexanc_LLSD[v].resize(n_v - 1);

		shared_ptr<TreeNode> t = p;
		unordered_set<int> anc2(p->valanc.begin(), p->valanc.end());//Temporarily stored for the construction of indexanc_LLSD
		anc2.erase(v);


		
		for (auto x : index[v]) {//Save the index first
			if (x.first == v)continue;
			entry.fill(INT_MAX);

			//From back to front, because the index is ranked from small to large by weight, we will first enlarge it and then cover it with the smaller one
			for (list<Path>::reverse_iterator pre = x.second.rbegin(); pre != x.second.rend(); ++pre) {
				for (int j = L - 1; j >= 0; --j) if (pre->labels[j] == 1) {
					entry[j] = pre->weight;
					break;
				}
			}
			anc2.erase(x.first);
			indexanc_LLSD[v][node_height[x.first] - 1] = move(entry);
		}

		temp_index[v] = index[v];

		for (auto w : anc2) {
			Paths.clear();
			for (int i = 1; i < p->val.size(); ++i) {
				p_val_i = p->val[i];
				if (temp_index[p_val_i].find(w) != temp_index[p_val_i].end())
					pathJoin_(temp_index[v][p_val_i], temp_index[p_val_i][w], Paths);
				else
					pathJoin_(temp_index[v][p_val_i], temp_index[w][p_val_i], Paths);
				//PrunePath(temp);
			}
			PrunePath(Paths);
			
			entry.fill(INT_MAX);

			for (auto _path : Paths) {
				for (int j = L - 1; j >= 0; --j) if (_path.labels[j] == 1) {
					entry[j] = min(entry[j], _path.weight);
					break;
				}
			}
			//PrunePath(Paths);
			temp_index[v][w] = move(Paths);
			indexanc_LLSD[v][node_height[w] - 1] = move(entry);
		}
		

		counts++;
		//If it has reached the leaf node, delete the temporary index that is no longer in use
		flag[v] = p->children.size();
		if (flag[v] == 0) {
			
			shared_ptr<TreeNode> now = p;
			while (flag[now->val[0]] == 0) {
				temp_index[now->val[0]].clear();
				now = now->parent;
				flag[now->val[0]]--;
				counts--;
			}

		}
		
		if (++step % 1000 == 0) cout << ++progress << "/" << ptsNum/1000 <<" -- "<< counts << endl;
		for (auto& c : p->children) {
			q.push(c);
		}
	}

	cout << total_n << endl;
}
void Graph1::settingLLSD_p(int s) {
	vector<int> flag(ptsNum + 1, 0);
	int counts = 0, n;
	stack<shared_ptr<TreeNode>> q;

	array<int, L> entry;
	n = indexanc_LLSD.size();

	int nums = 0;
	temp_index.resize(ptsNum + 1);
	q.push(root);
	int progress = 0, step = 0, p_val_i;
	list<Path> Paths, temp;
	list<Path_> temp_;

	while (!q.empty()) {
		shared_ptr<TreeNode> p = q.top();
		int v = p->val[0], n_v = p->valanc.size(), height_v = node_height[v];
		q.pop();


		indexanc_LLSD[v].resize(n_v - 1);
		vector<bool> flags(1500);
		shared_ptr<TreeNode> t = p;
		unordered_set<int> anc2(p->valanc.begin(), p->valanc.end());
		anc2.erase(v);

		int N = min(s, (int)p->valanc.size() - 1);
		shared_ptr<TreeNode> TMP = p;
		fill(flags.begin(), flags.end(), 0);
		for (int i = 0; i < N; ++i) {

			for (auto& v_ : TMP->val) {
				flags[node_height[v_]] = 1;
			}
			TMP = TMP->parent;
		}

		for (auto x : index[v]) {
			if (x.first == v)continue;
			entry.fill(INT_MAX);

			
			for (list<Path>::reverse_iterator pre = x.second.rbegin(); pre != x.second.rend(); ++pre) {
				for (int j = L - 1; j >= 0; --j) if (pre->labels[j] == 1) {
					entry[j] = pre->weight;
					break;
				}
			}
			anc2.erase(x.first);
			indexanc_LLSD[v][node_height[x.first] - 1] = move(entry);
		}

		temp_index[v] = index[v];

		for (auto w : anc2) {
			if (flags[node_height[w]] == 0)continue;
			Paths.clear();
			for (int i = 1; i < p->val.size(); ++i) {
				temp.clear();
				p_val_i = p->val[i];
				if (temp_index[p_val_i].find(w) != temp_index[p_val_i].end())
					temp = pathJoin(temp_index[v][p_val_i], temp_index[p_val_i][w], p_val_i);
				else
					temp = pathJoin(temp_index[v][p_val_i], temp_index[w][p_val_i], p_val_i);

				//PrunePath(temp);
				Paths.splice(Paths.end(), temp);
			}
			PrunePath(Paths);

			entry.fill(INT_MAX);
			for (auto _path : Paths) {
				for (int j = L - 1; j >= 0; --j) if (_path.labels[j] == 1) {
					entry[j] = min(entry[j], _path.weight);
					break;
				}
			}
			//PrunePath(Paths);
			temp_index[v][w] = move(Paths);
			indexanc_LLSD[v][node_height[w] - 1] = move(entry);

		}


		counts++;
		
		flag[v] = p->children.size();
		if (flag[v] == 0) {

			shared_ptr<TreeNode> now = p;
			while (flag[now->val[0]] == 0) {
				temp_index[now->val[0]].clear();
				now = now->parent;
				flag[now->val[0]]--;
				counts--;
			}

		}

		if (++step % 1000 == 0) cout << ++progress << "/" << ptsNum / 1000 << " -- " << counts << endl;

		for (auto& c : p->children) {
			q.push(c);
		}
	}

}

void Graph1::processNode(shared_ptr<TreeNode> p)
{
	int v = p->val[0];
	int n_v = static_cast<int>(p->valanc.size());
	int height_v = node_height[v];

	indexanc_ASTAR[v].resize(n_v - 1);

	unordered_set<int> anc2(p->valanc.begin(), p->valanc.end());
	anc2.erase(v);

	// Process the existing index[v]
	for (auto& x : index[v]) {
		if (x.first == v)
			continue;

		array<int, L> entry;
		entry.fill(INT_MAX);

		for (auto pre = x.second.rbegin(); pre != x.second.rend(); ++pre) {
			for (int j = L - 1; j >= 0; --j) {
				if (pre->labels[j] == 1) {
					entry[j] = pre->weight;
					break;
				}
			}
		}

		anc2.erase(x.first);
		indexanc_ASTAR[v][node_height[x.first] - 1] = move(entry);
	}
	temp_index[v] = index[v];

	for (auto w : anc2) {
		list<Path> Paths;

		for (size_t ii = 1; ii < p->val.size(); ++ii) {
			int p_val_i = p->val[ii];

			auto it = temp_index[p_val_i].find(w);

			list<Path> temp;
			if (it != temp_index[p_val_i].end()) {
				temp = pathJoin(
					temp_index[v][p_val_i],
					it->second,
					p_val_i
				);
			}
			else {
				auto it2 = temp_index[w].find(p_val_i);
				if (it2 != temp_index[w].end()) {
					temp = pathJoin(
						temp_index[v][p_val_i],
						it2->second,
						p_val_i
					);
				}
			}

			Paths.splice(Paths.end(), temp);
		}

		PrunePath(Paths);

		array<int, L> entry;
		entry.fill(INT_MAX);

		for (auto& path : Paths) {
			for (int j = L - 1; j >= 0; --j) {
				if (path.labels[j] == 1) {
					entry[j] = min(entry[j], path.weight);
					break;
				}
			}
		}

		temp_index[v][w] = move(Paths);
		indexanc_ASTAR[v][node_height[w] - 1] = move(entry);
	}

	totals.fetch_add(1, memory_order_relaxed);

	int current_counter =
		counter.fetch_add(1, memory_order_relaxed) + 1;

	if (current_counter % 1000 == 0) {
		cout << current_counter << " / " << ptsNum << " -- valid paths: "
			<< totals.load(memory_order_relaxed) << endl;
	}

	// Submit the child nodes only after the current node is constructed
	for (auto& c : p->children) {
		pool.enqueue([this, c]() {
			this->processNode(c);

			if (c->children.empty()) {
				this->try_cleanup_upwards( c, flag, node_mutex, totals
				);
			}
			});
	}
}


void Graph1::try_cleanup_upwards(shared_ptr<TreeNode> start,vector<atomic<int>>& flag,vector<mutex>& node_mutex,atomic<int>& totals){
	shared_ptr<TreeNode> now = start;

	while (now) {
		int nv = now->val[0];
		auto parent = now->parent;

		bool expected = false;

		if (!cleaned[nv].compare_exchange_strong( expected, true, memory_order_acq_rel, memory_order_acquire)) {
			break;
		}

		if (!temp_index[nv].empty()) temp_index[nv].clear();

		totals.fetch_sub(1, memory_order_relaxed);

		if (!parent) break;

		int pv = parent->val[0];
		int old = flag[pv].fetch_sub( 1, memory_order_acq_rel );
		if (old != 1) break;

		now = parent;
	}
}


void Graph1::settingLLSD_parallel()
{
	temp_index.clear();
	temp_index.resize(ptsNum + 1);

	indexanc_ASTAR.clear();
	indexanc_ASTAR.resize(ptsNum + 1);

	if (!root) return;

	stack<shared_ptr<TreeNode>> s;
	s.push(root);

	while (!s.empty()) {
		auto p = s.top();
		s.pop();

		int v = p->val[0];
		int child_num = static_cast<int>(p->children.size());

		flag[v].store(child_num, memory_order_relaxed);
		cleaned[v].store(false, memory_order_relaxed);

		for (auto& c : p->children) s.push(c);
	}

	counter.store(0, memory_order_relaxed);
	totals.store(0, memory_order_relaxed);

	pool.enqueue([this]() {
		this->processNode(root);

		if (root->children.empty()) {
			this->try_cleanup_upwards( root, flag, node_mutex, totals );
		}
		});
}

void Graph1::showLabel() {
	cout << "Label list:" << endl;
	for (auto e : node_arr) {
		e->showNode(index[e->val[0]]);
		cout << endl;
	}
}

void Graph1::outputLabel(string datasetname) {
	ofstream ofs(datasetname);
	ofs << node_arr.size() << endl;
	for (int i = 0; i < node_arr.size(); i++) {
		shared_ptr<TreeNode> e = node_arr[node_arr.size() - 1 - i];
		ofs << e->val.size() << "\t";
		for (auto f : e->val) {
			ofs << f << " ";
		}
		ofs << endl;
		if (e->parent != NULL) ofs << e->parent->val[0] << endl;
		ofs << index[e->val[0]].size() << "\t";
		for (auto f : index[e->val[0]]) {
			ofs << f.first << " ";
			ofs << f.second.size() << "\t";
			for (auto k : f.second) {
				ofs << k.weight << " ";
				ofs << k.labels.size() << " ";
				for (int j = 0; j < k.labels.size(); ++j) ofs << k.labels[j];
				ofs << " ";
				ofs << "\t";
			}
		}
		ofs << endl << endl;
	}
	ofs.close();
}

//Output information such as tree height and width
void Graph1::get_h_and_w() {
	queue<shared_ptr<TreeNode>> q; q.push(root);
	height = 0; width = 0;
	int valnums = 0, totalvalnums = 0;
	double rou_max=0, rou = 0;
	while (!q.empty()) {
		width = max(width,(int)q.size());

		for (int i = 0; i < q.size(); ++i) {
			shared_ptr<TreeNode> p = q.front();
			valnums = max(valnums, (int)p->val.size());
			totalvalnums += p->val.size();
			q.pop();
			height = max(height, (int)p->valanc.size());
			for (auto c : p->children) {
				q.push(c);
			}
		}
	}
	int n = 0;
	for (auto un_map : index) {
		for (auto list_ : un_map) {
			rou_max = max(rou_max, (double)list_.second.size());
			rou += (double)list_.second.size();
			n++;
		}
	}
	exist_s.resize(height+1, INT_MAX);
	exist_t.resize(height+1, INT_MAX);
	totalvalnums /= ptsNum;
	cout << "tree height：" << height << "  tree width ：" << width << "  Maximum val count for a single node:" << valnums << "  Average number of node vals:" << totalvalnums << endl;
	cout << "rou: " << rou / n << " rou_max: " << rou_max << endl;
}

void Graph1::saveTree(string name) {
	ofstream out(name); int n;
	n = node_arr.size(); out << n << " ";//Number of nodes
	for (auto e : node_arr) {
		n = e->val.size(); out << n << " ";//The quantity of val
		for (auto i : e->val)	out << i << " ";

		n = index[e->val[0]].size(); out << n << " ";//The number of indices
		for (auto i : index[e->val[0]]) {
			out << i.first << " ";
			n = i.second.size(); out << n << " ";//The number of paths
			for (auto e : i.second) {
				out << e.weight << " ";// out << e.sep << " ";
				for (int j = e.labels.size() - 1; j >= 0; --j) out << e.labels[j];
				out << " ";
			}

		}
	}
	n = node_index.size(); out << n << " ";//The quantity of node_index
	for (auto x : node_index) out << x << " ";
	out.close();
}

void Graph1::saveTree_bin(string name) {
	ofstream out(name, ios::binary); int n;
	n = node_arr.size(); out.write(reinterpret_cast<char*>(&n), sizeof(int));//Number of nodes
	for (auto e : node_arr) {
		n = e->val.size(); out.write(reinterpret_cast<char*>(&n), sizeof(int));//The quantity of val
		for (auto i : e->val)out.write(reinterpret_cast<char*>(&i), sizeof(int));

		n = index[e->val[0]].size(); out.write(reinterpret_cast<char*>(&n), sizeof(int));//The number of indices
		for (const auto& i : index[e->val[0]]) {
			out.write(reinterpret_cast<const char*>(&i.first), sizeof(int));
			n = i.second.size(); out.write(reinterpret_cast<char*>(&n), sizeof(int));//The number of paths
			for (auto e : i.second) {
				out.write(reinterpret_cast<const char*>(&e.weight), sizeof(int)); 
				out.write(reinterpret_cast<const char*>(&e.labels), sizeof(e.labels));
			}
		}
	}
	n = node_index.size(); out.write(reinterpret_cast<char*>(&n), sizeof(int));//The quantity of node_index
	for (auto x : node_index) out.write(reinterpret_cast<char*>(&x), sizeof(int));
	out.close();
}



void Graph1::saveLLSD(string name,int step) {
	ofstream out(name, ios::binary); int n, value; char data;
	n = indexanc_LLSD.size(); out.write(reinterpret_cast<char*>(&n), sizeof(int));
	vector<bool> flags(1500);
	uint16_t mask = 0;
	int p = 0;
	for (auto& x : indexanc_LLSD) {
		
		n = x.size(); out.write(reinterpret_cast<char*>(&n), sizeof(int));
		if (p == 0) { p++; continue; }
		
		shared_ptr<TreeNode> P = node_arr[node_index[p]],TMP = node_arr[node_index[p]];
		int N = min(step, (int)P->valanc.size()-1);
		fill(flags.begin(), flags.end(), 0);
		for (int i = 0; i <= N; ++i) {
			
			for (auto& v_ : TMP->val) {
				flags[node_height[v_]] = 1;
			}
			TMP = TMP->parent;
		}
		p++;
		int i = 1;
		for (auto& y : x) {
			if (flags[i++] == 0)continue;
			mask = 0;
			for (int i = 0; i < L; ++i) {
				if (y[i] != INT_MAX)
					mask |= (1 << i); // Set Bitmask
			}

			out.write(reinterpret_cast<char*>(&mask), sizeof(uint16_t));

			for (int i = 0; i < L; ++i) {
				if (mask & (1 << i)) {
					out.write(reinterpret_cast<const char*>(&y[i]), sizeof(int));
				}
			}
		}
		
	}
	out.close();
}


void Graph1::readTree(string name) {
	ifstream in(name); int nodenum, valnum, indexnum, pathnum, element; string label; int weight; vector<int> sep;
	in >> nodenum;
	for (int i = 0; i < nodenum; ++i) {
		shared_ptr<TreeNode> p(new TreeNode());
		in >> valnum;
		for (int j = 0; j < valnum; ++j) {
			in >> element;
			p->val.emplace_back(element);
		}
		in >> indexnum;
		for (int j = 0; j < indexnum; ++j) {
			in >> element >> pathnum;
			list<Path> paths;
			for (int k = 0; k < pathnum; ++k) {
				in >> weight;/* in >> sep;*/
				bitset<L> labels;
				in >> labels;
				Path path = { weight,labels };
				paths.emplace_back(path);
			}
			index[p->val[0]].insert(make_pair(element, paths));
		}
		node_arr.emplace_back(p);
	}

	in >> nodenum;
	for (int i = 0; i < nodenum; ++i) {
		in >> element;
		node_index[i] = element;
	}
	in.close();
	treeFormation();
	addAnc();
	cout << "readTree-over" << endl;
}

void Graph1::readLLSD(string name) {
	ifstream in(name, ios::binary);
	int nodenum, valnum;
	int a = 0, b = 0;
	
	
	in.read(reinterpret_cast<char*>(&nodenum), sizeof(int));
	indexanc_LLSD.resize(nodenum);
	for (int i = 0; i < nodenum; ++i) {
		++a; ++b;
		if (a == 1000) {
			cout << b << endl;
			a = 0;
		}

		in.read(reinterpret_cast<char*>(&valnum), sizeof(int));
		indexanc_LLSD[i].resize(valnum);


		for (int j = 0; j < valnum; ++j) {

			uint16_t mask;
			in.read(reinterpret_cast<char*>(&mask), sizeof(uint16_t));
			indexanc_LLSD[i][j].fill(INT_MAX);

			for (int k = 0; k < L; ++k) {
				if (mask & (1 << k)) {
					int val;
					in.read(reinterpret_cast<char*>(&val), sizeof(int));
					indexanc_LLSD[i][j][k] = val;
				}
			}
		}

	}
	
	in.close();
	cout << "readLLSD-over" << endl;
}

