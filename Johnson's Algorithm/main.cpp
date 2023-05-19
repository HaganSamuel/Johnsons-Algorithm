#include "infinity.h"
#include "graph.h"

int GetINFINITY() {
	return -2;
}

int GetMAX() {
	return 1000;
}

// To do list:
/*
Debug Johnson in the case that:
1. The graph is severed.
2. The graph only has 1 vertex
3. The graph has 0 edges.
4. 
*/

int main(int argv, char** argc) {
	Graph* newGraph = new Graph;

	char textFile[] = "graph.txt";

	newGraph->LoadGraph(textFile);

	// Dijkstra's Test Case:

	//newGraph->RunDijkstra('a');

	//newGraph->printTuples();

	// Bellman Ford Test Case:

	//if (newGraph->RunBellmanFord('a') == 0) cout << "Bellman ford detected an error!\n";

	//newGraph->printTuples();

	// Johnson Test case

	if (newGraph->runJohnson() == 0) cout << "\nThe graph detected a negative weight cycle!\n";
	else cout << "Success!\n";

	newGraph->printMatrix(newGraph->johnsonData);
}

/*
Debugging file for Dijkstra:
[Vertices]
a
b
c
d
e
f
g
[Edges]
a b 3
a f 2
b c 1
b g 6
c d 4
d g 3
g f 2
f d 7
f e 5
e a 4
e c 4
*/

/*
Debugging file for graph.txt in Johnson's run.
[Vertices]
a
b
c
d
e
[Edges]
a b 3
a c 4
b c 3
b d -3
c d 7
d e 2
e a -1
e b 5
*/