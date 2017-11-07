f1 = open('aido99_all.txt', 'r')
f2 = open('ca.txt', 'r')
f3 = open('ci.txt', 'r')

active = {}
inactive = {}

for line in f2:
	active[line.strip()] = []

for line in f3:
	inactive[line.strip()] = []

labels = {}
def nodeLabel(node):
	if (node not in labels):
		labels[node] = len(labels)
	return labels[node]

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

	if (graphID in inactive):
		inactive[graphID].append(nodes)
		inactive[graphID].append(edges)

f1.close()
f2.close()
f3.close()

f4 = open('aids_data.txt', 'w')
f5 = open('aids_class.txt', 'w')

for key in active:
	if active[key] == []:
		continue

	f5.write("1\n")
	f4.write("t # " + key + "\n")

	graph = active[key]

	for i in xrange(len(graph[0])):
		 f4.write("v " + str(i) + " " + str(graph[0][i]) + "\n")

	for i in xrange(len(graph[1])):
		 f4.write("e " + graph[1][i] + "\n")

for key in inactive:
	if inactive[key] == []:
		continue

	f5.write("0\n")
	f4.write("t # " + key + "\n")

	graph = inactive[key]

	for i in xrange(len(graph[0])):
		 f4.write("v " + str(i) + " " + str(graph[0][i]) + "\n")

	for i in xrange(len(graph[1])):
		 f4.write("e " + graph[1][i] + "\n")

f4.close()
f5.close()