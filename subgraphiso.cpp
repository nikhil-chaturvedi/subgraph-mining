#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

// VFLib includes
#include <argraph.h>
#include <argedit.h>
#include <vf2_sub_state.h>
#include <match.h>

using namespace std;

class Atom {
public:
	int atom;
	Atom(int atom) {
		this->atom = atom;
	}
};

class Bond {
public:
	int bond_type;
	Bond(int bond_type) {
		this->bond_type = bond_type;
	}
};

class AtomDestroyer: public AttrDestroyer {
public:
	virtual void destroy(void* atom) {
		delete (Atom*) atom;
	}
};

class BondDestroyer: public AttrDestroyer {
public:
	virtual void destroy(void* bond) {
		delete (Bond*) bond;
	}
};

class AtomComparator: public AttrComparator {
public:
	virtual bool compatible(void* a, void* b) {
		Atom *a1 = (Atom*) a;
		Atom *b1 = (Atom*) b;
		return (a1->atom == b1->atom);
	}
};

class BondComparator: public AttrComparator {
public:
	virtual bool compatible(void* a, void* b) {
		Bond *a1 = (Bond*) a;
		Bond *b1 = (Bond*) b;
		return (a1->bond_type == b1->bond_type);
	}
};

bool subgraphiso(ARGraph<Atom, Bond> *g1, ARGraph<Atom, Bond> *g2) {
	VF2SubState s0(g1, g2);

	node_id ni1[100], ni2[100];
	int n;
	bool x = match(&s0, &n, ni1, ni2);
	// for(int i = 0; i < 100; i++)
	// 	cout << ni2[i] << " ";
	// cout << "\n";
	return x;
}

class DBGraph {

public:

	ARGraph<Atom, Bond>* g;

	int id;
	int type;
	int support;

	bool feature;

	DBGraph (ARGEdit *ed, int id, int type, int support, bool gaston) {
		this->g = new ARGraph<Atom, Bond>(ed);

		this->id = id;
		this->type = type;
		this->feature = false;
		this->support = (gaston)? support : 0;

		this->g->SetNodeDestroyer(new AtomDestroyer());
		this->g->SetEdgeDestroyer(new BondDestroyer());

		if (gaston) {
			this->g->SetNodeComparator(new AtomComparator());
			this->g->SetEdgeComparator(new BondComparator());
		}
	}
};

void readFile(string filename, bool gaston, int type, vector<DBGraph*>& db, bool train, map<int, DBGraph*>& ordered_train) {
	ifstream f(filename);

	char temp;
	f >> temp;
	while (1) {
		int id, support = 0, gcount;

		if (gaston) {
			f >> support;
			f >> temp;
		}
		if (!gaston)
			f >> temp;
		f >> id;
		if (train) {
			f >> gcount;
		}

		f >> temp;
		ARGEdit ed;
		while (temp == 'e' || temp == 'v') {
			if (temp == 'v') {
				int val;
				f >> val;
				f >> val;
				ed.InsertNode(new Atom(val));
			} else {
				int node1, node2, bond;
				f >> node1;
				f >> node2;
				f >> bond;

				ed.InsertEdge(node1, node2, new Bond(bond));
				ed.InsertEdge(node2, node1, new Bond(bond));
			}
			temp = (char)0;
			f >> temp;

			if ((int)temp == 0)
				break;
		}

		db.push_back(new DBGraph(&ed, id, type, support, gaston));
		if(train) {
			ordered_train.insert(pair<int, DBGraph*>(gcount, db.back()));
		}

		if ((int)temp == 0)
			break;
	}
	f.close();
}

void check_significance(vector<DBGraph*>& subgraphs, vector<DBGraph*>& min_class_graphs, int max_class_size, double alpha) {
	int significant = 0;
	for(int i = 0; i < subgraphs.size(); i++) {
		int min_count = 0;
		for(int j = 0; j < min_class_graphs.size(); j++) {
			int is_subgraph = subgraphiso(subgraphs[i]->g, min_class_graphs[j]->g);
			// if(!is_subgraph)
			// 	cout << "Found: " << is_subgraph << "\n";
			if(is_subgraph)
				min_count++;
		}

		double min_ratio = ((double) min_count) / ((double) min_class_graphs.size());
		double max_ratio = ((double) subgraphs[i]->support) / ((double) max_class_size);

		double ratio = max_ratio / (max_ratio + min_ratio);
		if(ratio > alpha) {
			subgraphs[i]->feature = true;
			significant++;
		}
	}
	cout << "Significant: " << significant << "\n";
}

void generate_features(map<int, DBGraph*>& train, vector<DBGraph*>& gaston_pos, vector<DBGraph*>& gaston_neg, vector<DBGraph*>& test) {
	ofstream ftrain("train.txt");
	for(int j = 0; j < train.size(); j++) {
		int fcount = 0;
		ftrain << train[j]->type << " ";
		for(int i = 0; i < gaston_pos.size(); i++) {
			if(!(gaston_pos[i]->feature))
				continue;
			fcount++;
			if(subgraphiso(gaston_pos[i]->g, train[j]->g))
				ftrain << fcount << ":1 ";
			else
				ftrain << fcount << ":0 ";
		}
		for(int i = 0; i < gaston_neg.size(); i++) {
			if(!(gaston_neg[i]->feature))
				continue;
			fcount++;
			if(subgraphiso(gaston_neg[i]->g, train[j]->g))
				ftrain << fcount << ":1 ";
			else
				ftrain << fcount << ":0 ";
		}
		ftrain << "\n";
	}
	ftrain.close();
	ofstream ftest("test.txt");
	for(int j = 0; j < test.size(); j++) {
		int fcount = 0;
		for(int i = 0; i < gaston_pos.size(); i++) {
			if(!(gaston_pos[i]->feature))
				continue;
			fcount++;
			if(subgraphiso(gaston_pos[i]->g, test[j]->g))
				ftest << fcount << ":1 ";
			else
				ftest << fcount << ":0 ";
		}
		for(int i = 0; i < gaston_neg.size(); i++) {
			if(!(gaston_neg[i]->feature))
				continue;
			fcount++;
			if(subgraphiso(gaston_neg[i]->g, test[j]->g))
				ftest << fcount << ":1 ";
			else
				ftest << fcount << ":0 ";
		}
		ftest << "\n";
	}
	ftest.close();
}

int main() {
	vector<DBGraph*> train_pos, train_neg, gaston_pos, gaston_neg, test;
	map<int, DBGraph*> ordered_train;
	readFile("train_pos.txt", false, 1, train_pos, true, ordered_train);
	readFile("train_neg.txt", false, -1, train_neg, true, ordered_train);
	readFile("gaston_pos.txt", true, 1, gaston_pos, false, ordered_train);
	readFile("gaston_neg.txt", true, -1, gaston_neg, false, ordered_train);
	readFile("test_graphs.txt", false, 0, test, false, ordered_train);
	int pos_size = train_pos.size();
	int neg_size = train_neg.size();
	check_significance(gaston_neg, train_pos, neg_size, 0.8);
	check_significance(gaston_pos, train_neg, pos_size, 0.8);
	generate_features(ordered_train, gaston_pos, gaston_neg, test);


	// ARGEdit ed1, ed2;
	// ed1.InsertNode(new Atom(1));
	// ed1.InsertNode(new Atom(0));
	// ed1.InsertNode(new Atom(0));
	// ed1.InsertNode(new Atom(0));
	// ed1.InsertNode(new Atom(3));
	// ed2.InsertNode(new Atom(0));
	// ed2.InsertNode(new Atom(0));
	// ed2.InsertNode(new Atom(0));
	// ed1.InsertEdge(2, 1, new Bond(1));
	// ed1.InsertEdge(1, 2, new Bond(1));
	// ed1.InsertEdge(2, 3, new Bond(0));
	// ed1.InsertEdge(3, 2, new Bond(0));
	// ed1.InsertEdge(1, 3, new Bond(0));
	// ed1.InsertEdge(3, 1, new Bond(0));
	// ed1.InsertEdge(2, 0, new Bond(2));
	// ed1.InsertEdge(0, 2, new Bond(2));
	// ed1.InsertEdge(1, 4, new Bond(0));
	// ed1.InsertEdge(4, 1, new Bond(0));
	// ed2.InsertEdge(0, 1, new Bond(1));
	// ed2.InsertEdge(1, 0, new Bond(1));
	// ed2.InsertEdge(2, 1, new Bond(0));
	// ed2.InsertEdge(1, 2, new Bond(0));
	// ed2.InsertEdge(0, 2, new Bond(0));
	// ed2.InsertEdge(2, 0, new Bond(0));

	// ARGraph<Atom, Bond> g1(&ed1);
	// ARGraph<Atom, Bond> g2(&ed2);
	// g1.SetNodeDestroyer(new AtomDestroyer());
	// g1.SetEdgeDestroyer(new BondDestroyer());
	// g2.SetNodeDestroyer(new AtomDestroyer());
	// g2.SetEdgeDestroyer(new BondDestroyer());
	// g2.SetNodeComparator(new AtomComparator());
	// g2.SetEdgeComparator(new BondComparator());

	// VF2SubState s0(&g2, &g1);

	// node_id ni1[100], ni2[100];
	// int n;
	// bool x = match(&s0, &n, ni1, ni2);
	// if(x) {
	// 	cout << "Found\n";
	// }

	return 0;
}