#include"Tree.h"

void TreeNode::showNode(unordered_map<int, list<Path>> index) {

	if (parent != NULL)
		cout << "node val: " << val[0] << ",parent: " << parent->val[0] << ",val: ";
	else cout << "node val: " << val[0] << ", no parent,val: ";

	for (auto& e : val) {
		cout << e << " ";
	}cout << endl << "anc: ";
	for (auto& a : valanc) {
		cout << a << " ";
	}
	cout << endl << "node label: " << endl;
	for (auto& e : index) {
		cout << "(" << val[0] << "," << e.first << "):";
		showPathSet(e.second);
		cout << endl;
	}
	cout << endl;
}




