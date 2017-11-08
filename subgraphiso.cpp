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
	string atom;
	Atom(string atom) {
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
		return a == b;
	}
};

class BondComparator: public AttrComparator {
public:
	virtual bool compatible(void* a, void* b) {
		Bond *a1 = (Bond*) a;
		Bond *b1 = (Bond*) b;
		return a == b;
	}
};

bool subgraphiso(ARGraph<Atom, Bond> *g1, ARGraph<Atom, Bond> *g2) {
	VF2SubState s0(g1, g2);

	node_id ni1[5], ni2[5];
	int n;
	return match(&s0, &n, ni1, ni2);
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

		if (!gaston) {
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
				ed.InsertNode(new Atom(to_string(val)));
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

void check_significance(vector<DBGraph*>& subgraphs, vector<DBGraph*>& min_class_graphs, double alpha) {
	int out_trial = 0;
	for(int i = 14; i < subgraphs.size(); i++) {
		cout << "Out_trial: " << out_trial << "\n";
		int in_trial = 0;
		int min_count = 0;
		for(int j = 0; j < min_class_graphs.size(); j++) {
			cout << (min_class_graphs[j]->g)->NodeCount() << "\n";
			cout << (subgraphs[i]->g)->NodeCount() << "\n";
			int is_subgraph = subgraphiso(subgraphs[i]->g, min_class_graphs[j]->g);
			cout << is_subgraph << "\n";
			if(is_subgraph)
				min_count++;
			cout << "In_trial: " << in_trial << "\n";
			in_trial++;
		}
		double ratio = ((double) subgraphs[i]->support)/((double) subgraphs[i]->support + min_count);
		if(ratio > alpha) {
			subgraphs[i]->feature = true;
			cout << "Min_count: " << min_count << "\n";
		}
		out_trial++;
	}
}

void generate_features(map<int, DBGraph*>& train, vector<DBGraph*>& gaston_pos, vector<DBGraph*>& gaston_neg, vector<DBGraph*>& test) {
	ofstream ftrain("train.txt");
	for(int j = 0; j < train.size(); j++) {
		ftrain << train[j]->type;
		for(int i = 0; i < gaston_pos.size(); i++) {
			if(!(gaston_pos[i]->feature))
				continue;
			if(subgraphiso(gaston_pos[i]->g, train[j]->g))
				ftrain << gaston_pos[i]->id << ":1 ";
			else
				ftrain << gaston_pos[i]->id << ":0 ";
		}
		for(int i = 0; i < gaston_neg.size(); i++) {
			if(!(gaston_neg[i]->feature))
				continue;
			if(subgraphiso(gaston_neg[i]->g, train[j]->g))
				ftrain << gaston_neg[i]->id << ":1 ";
			else
				ftrain << gaston_neg[i]->id << ":0 ";
		}
		ftrain << "\n";
	}
	ftrain.close();
	ofstream ftest("test.txt");
	for(int j = 0; j < train.size(); j++) {
		for(int i = 0; i < gaston_pos.size(); i++) {
			if(!(gaston_pos[i]->feature))
				continue;
			if(subgraphiso(gaston_pos[i]->g, train[j]->g))
				cout << gaston_pos[i]->id << ":1 ";
			else
				cout << gaston_pos[i]->id << ":0 ";
		}
		for(int i = 0; i < gaston_neg.size(); i++) {
			if(!(gaston_neg[i]->feature))
				continue;
			if(subgraphiso(gaston_neg[i]->g, train[j]->g))
				ftrain << gaston_neg[i]->id << ":1 ";
			else
				ftrain << gaston_neg[i]->id << ":0 ";
		}
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
	check_significance(gaston_neg, train_pos, 0.8);
	return 0;
}