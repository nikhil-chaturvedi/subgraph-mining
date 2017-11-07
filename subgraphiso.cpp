#include <string>
#include <iostream>

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

int main() {
	ARGEdit ed1, ed2;
	ed1.InsertNode(new Atom("C"));
	ed1.InsertNode(new Atom("C"));
	ed1.InsertNode(new Atom("C"));
	ed1.InsertEdge(0, 1, new Bond(1));
	ed1.InsertEdge(1, 0, new Bond(1));
	ed1.InsertEdge(0, 2, new Bond(1));
	ed1.InsertEdge(2, 0, new Bond(1));
	ed1.InsertEdge(2, 1, new Bond(2));
	ed1.InsertEdge(1, 2, new Bond(2));
	ed2.InsertNode(new Atom("N"));
	ed2.InsertNode(new Atom("C"));
	ed2.InsertNode(new Atom("S"));
	ed2.InsertNode(new Atom("C"));
	ed2.InsertNode(new Atom("C"));
	ed2.InsertEdge(1, 4, new Bond(1));
	ed2.InsertEdge(4, 1, new Bond(1));
	ed2.InsertEdge(1, 2, new Bond(2));
	ed2.InsertEdge(2, 1, new Bond(2));
	ed2.InsertEdge(3, 4, new Bond(1));
	ed2.InsertEdge(4, 3, new Bond(1));
	ed2.InsertEdge(3, 1, new Bond(2));
	ed2.InsertEdge(1, 3, new Bond(2));

	ARGraph<Atom, Bond> g1(&ed1);
	ARGraph<Atom, Bond> g2(&ed2);

	g1.SetNodeDestroyer(new AtomDestroyer());
	g2.SetNodeDestroyer(new AtomDestroyer());
	g1.SetEdgeDestroyer(new BondDestroyer());
	g2.SetEdgeDestroyer(new BondDestroyer());

	g2.SetNodeComparator(new AtomComparator());
	g2.SetEdgeComparator(new BondComparator());
	
	if(subgraphiso(&g1, &g2)) {
		printf("Found a matching\n");
	}

	if(subgraphiso(&g1, &g2)) {
		printf("Found a matching\n");
	}


	return 0;
}