#include "Tree.h"
#include "Graph.h"


//保存树
void test_save(string graph, string output1, string output2,int layer) {
	Graph1 p;
	auto t1 = chrono::steady_clock::now();
	p.readGraph(graph);
	for (auto& x : p.tolabel) {
		cout << "边序号：" << x.first << "  边标签：" << x.second.second << "  边频次：" << x.second.first << endl;
	}
	p.treeNodeContraction();//生成树节点：寻找最小度点，生成单个树节点，生成邻居边，删除最小度点。
	p.treeFormation();//合成分解树：将树节点按照删除顺序连接。
	p.labelAssignment();//从上至下回溯祖先信息，完善天际线路径;
	//p.readTree(output1);
	p.get_h_and_w();
	//p.outputTree();

	//p.saveTree_bin("CAL-10_index_bin.bin");
	auto t2 = chrono::steady_clock::now();
	double dr_us1 = chrono::duration<float, std::ratio<1>>(t2 - t1).count();
	int step = ((layer) / (double)10) * p.height;
	cout << "***tree time cost:" << dr_us1 << endl;
	//p.readLLSD("NY_LLSD_bitset.bin");
	t1 = chrono::steady_clock::now();
	//p.settingLLSD();
	p.settingLLSD_p(step);
	
	t2 = chrono::steady_clock::now();
	string out_name = "setting_time_result_parr_" + to_string(layer) +"0%_" + graph;
	ofstream out(out_name);
	double dr_us2 = chrono::duration<float, std::ratio<1>>(t2 - t1).count();
	out << graph << ":" << endl;
	out << "***tree time cost:" << dr_us1 << endl;
	out << "***setting time cost:" << dr_us2 << endl;
	out.close();

	//p.queryx("query_NY_random.txt");

	p.saveLLSD(output2, step);

	cout << "over" << endl;
	return;
}

void test_save2hop(string graph, string tree, string output) {
	Graph1 p;
	p.readGraph(graph);
	for (auto& x : p.tolabel) {
		cout << "边序号：" << x.first << "  边标签：" << x.second.second << "  边频次：" << x.second.first << endl;
	}
	p.readTree(tree);
	p.get_h_and_w();
	//p.Build2HOP();
	//p.outputTree();
	//p.save2hopbinary(output);
}

//固定生成1对查询
void test(string graph, string tree, string LLSD) {
	Graph1 p;
	p.readGraph(graph);
	for (auto& x : p.tolabel) {
		cout << "边序号：" << x.first << "  边标签：" << x.second.second << "  边频次：" << x.second.first << endl;
	}
	p.readTree(tree);
	
	//p.treeNodeContraction();//生成树节点：寻找最小度点，生成单个树节点，生成邻居边，删除最小度点。
	//p.treeFormation();//合成分解树：将树节点按照删除顺序连接。
	//p.labelAssignment();//从上至下回溯祖先信息，完善天际线路径;
	//p.outputTree();
	//p.preProcess();
	p.get_h_and_w();

	p.readLLSD(LLSD);
	//p.settingLLSD();
	
	int s = 115457, t = 113935;   
	bitset<L> labels{ "0000000010" };
	p.query(s, t, labels);

	shared_ptr<TreeNode> S = p.node_arr[p.node_index[s]], T = p.node_arr[p.node_index[t]],LCA=p.findLca(s,t);

}

void temp_rhoavg(string graph, string tree, string LLSD) {
	Graph1 p;

	p.readGraph(graph);
	p.readTree(tree);
	p.readLLSD(LLSD);
	for (auto& x : p.tolabel) {
		cout << "边序号：" << x.first << "  边标签：" << x.second.second << "  边频次：" << x.second.first << endl;
	}
	int a = 0; long long total = 0; int x1 = 0, x2 = 0, max_value = 0;;
	for (int i = 1; i < p.indexanc_LLSD.size(); ++i) {	
		if (a >= 1000) {
			cout << i << "/" << p.ptsNum << endl;
			a = 0;
		}
		a++;
		for (int j = 0; j < p.indexanc_LLSD[i].size(); ++j) {
;
			int n = 0;
			for (int k = 0; k < L; ++k) {
				if (p.indexanc_LLSD[i][j][k] != INT_MAX) {
					++n;
					max_value = max(p.indexanc_LLSD[i][j][k], max_value);
				}
			}
			if (n > 5)x1++;
			else x2++;
			p.rhoavg += n;
			p.rho = max(p.rho, n);
		}
		total += p.indexanc_LLSD[i].size();
	}
	p.rhoavg /= total;
	cout << p.rho << " " << p.rhoavg<<" "<<x1<<" "<<x2<<" "<< max_value << endl;
}

void temp_var(string graph, string tree, string LLSD) {
	Graph1 p;

	p.readGraph(graph);
	p.readTree(tree);
	p.get_h_and_w();
	p.readLLSD(LLSD);
	for (auto& x : p.tolabel) {
		cout << "边序号：" << x.first << "  边标签：" << x.second.second << "  边频次：" << x.second.first << endl;
	}
	int a = 0; long long total = 0; int max_val, min_val = 0;
	unordered_map<int, double> mp;
	unordered_map<int, int> count;
	for (int i = 1; i < p.indexanc_LLSD.size(); ++i) {
		if (a >= 1000) {
			cout << i << "/" << p.ptsNum << endl;
			a = 0;
		}
		a++;
		for (int j = 0; j < p.indexanc_LLSD[i].size(); ++j) {
			max_val = INT_MIN;
			min_val = INT_MAX;

			for (auto x: p.indexanc_LLSD[i][j]) {
				if (x != INT_MAX) {
					if (x <= min_val) min_val = x;
					if (x >= max_val) max_val = x;
				}
			}

			mp[p.node_height[i]-j] += max_val - min_val;
			count[p.node_height[i] - j]++;

			
		}
		
	}
	for (int i = 0; i < p.height;++i) {
		cout << i<<"层 : "<<mp[i] / count[i] << endl;
	}
	
}


void querytest(string graph, string tree, string LLSD, string name) {
	Graph1 p;
	p.readGraph(graph);
	p.readTree(tree);
	for (auto& x : p.tolabel) {
		cout << "边序号：" << x.first << "  边标签：" << x.second.second << "  边频次：" << x.second.first << endl;
	}
	//p.treeNodeContraction();//生成树节点：寻找最小度点，生成单个树节点，生成邻居边，删除最小度点。
	//p.treeFormation();//合成分解树：将树节点按照删除顺序连接。
	//p.labelAssignment();//从上至下回溯祖先信息，完善天际线路径;
	p.get_h_and_w();
	p.readLLSD(LLSD);
	//p.settingLLSD();

	p.queryx(name);

}

void querytest2(string graph, string tree, string LLSD, string name) {
	Graph1 p;
	p.readGraph(graph);
	p.readTree(tree);
	
	for (auto& x : p.tolabel) {
		cout << "边序号：" << x.first << "  边标签：" << x.second.second << "  边频次：" << x.second.first << endl;
	}
	//p.treeNodeContraction();//生成树节点：寻找最小度点，生成单个树节点，生成邻居边，删除最小度点。
	//p.treeFormation();//合成分解树：将树节点按照删除顺序连接。
	//p.labelAssignment();//从上至下回溯祖先信息，完善天际线路径;
	p.get_h_and_w();
	p.readLLSD(LLSD);
	//p.settingLLSD();


	p.queryx2(name);

}

int main(int argc, char* argv[]) {

	test_save("NY2.txt", "NY2_index.txt", "NY2_LLSD_bitset.bin",10);
	
	//querytest("NY2.txt", "NY2_index.txt", "NY2_LLSD_bitset.bin", "query_NY2_min_weight_q_.txt");
	

	return 0;
}