import sys

f1 = open(sys.argv[1], 'r')
f2 = open(sys.argv[2], 'r')
f3 = open(sys.argv[3], 'r')

active = {}
inactive = {}
test = {}

for line in f2:
	active[line.strip()] = []

for line in f3:
	inactive[line.strip()] = []

labels = {}
def nodeLabel(node):
	if (node not in labels):
		labels[node] = len(labels)
	return labels[node]

gcount = 0
while 1:
	line = f1.readline().strip()
	if line == '':
		break

	graphID = line[1:]

	nodes = []
	numNodes = int(f1.readline().strip())
	for i in xrange(numNodes):
		nodes.append(nodeLabel(f1.readline().strip()))

	edges = []
	numEdges = int(f1.readline().strip())
	for i in xrange(numEdges):
		edges.append(f1.readline().strip())

	if (graphID in active):
		active[graphID].append(nodes)
		active[graphID].append(edges)
		active[graphID].append(gcount)

	if (graphID in inactive):
		inactive[graphID].append(nodes)
		inactive[graphID].append(edges)
		inactive[graphID].append(gcount)

	gcount += 1

f1.close()
f2.close()
f3.close()

ftest = open(sys.argv[4], 'r')

while 1:
	line = ftest.readline().strip()
	if line == '':
		break

	graphID = line[1:]

	nodes = []
	numNodes = int(ftest.readline().strip())
	for i in xrange(numNodes):
		nodes.append(nodeLabel(ftest.readline().strip()))

	edges = []
	numEdges = int(ftest.readline().strip())
	for i in xrange(numEdges):
		edges.append(ftest.readline().strip())

	test[graphID] = []
	test[graphID].append(nodes)
	test[graphID].append(edges)

ftest.close()

f4 = open('train_pos.txt', 'w')
f5 = open('train_neg.txt', 'w')
f6 = open('test_graphs.txt', 'w')

for key in active:
	if active[key] == []:
		continue

	graph = active[key]

	f4.write("t # " + key + " " + str(graph[2]) + "\n")

	for i in xrange(len(graph[0])):
		f4.write("v " + str(i) + " " + str(graph[0][i]) + "\n")

	for i in xrange(len(graph[1])):
		f4.write("e " + graph[1][i] + "\n")

for key in inactive:
	if inactive[key] == []:
		continue

	f5.write("t # " + key + " " + str(graph[2]) + "\n")

	graph = inactive[key]

	for i in xrange(len(graph[0])):
		f5.write("v " + str(i) + " " + str(graph[0][i]) + "\n")

	for i in xrange(len(graph[1])):
		f5.write("e " + graph[1][i] + "\n")


for key in test:
	if test[key] == []:
		continue

	f6.write("t # " + key + "\n")

	graph = test[key]

	for i in xrange(len(graph[0])):
		f6.write("v " + str(i) + " " + str(graph[0][i]) + "\n")

	for i in xrange(len(graph[1])):
		f6.write("e " + graph[1][i] + "\n")

f4.close()
f5.close()
f6.close()