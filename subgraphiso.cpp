#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

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
	ARGraph<Atom, Bond>* g;

	int id;
	int type;
	int support;

	bool feature;

public:
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

void readFile(string filename, bool gaston, int type, vector<DBGraph*>& db) {
	ifstream f(filename);

	char temp;
	f >> temp;
	while (1) {
		int id, support = 0;

		if (gaston) {
			f >> support;
			f >> temp;
		}
		f >> temp;
		f >> id;

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

		if ((int)temp == 0)
			break;
	}
}

int main() {
	vector<DBGraph*> db;
	readFile("aids_data.txt", false, 0, db);
	return 0;
}