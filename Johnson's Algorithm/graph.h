#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <tuple>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

// Defines a graph.
class Graph {
public:

	bool isJohnson;

	int startingIndx = 0;

	int nxtAdjacent = 0;

	int GetNumberOfUpdates(char vertex);

	void printTuples();

	void printMatrix(vector<vector<int>> matrixEdges);

	void printVector(vector<char> vectorVar);

	// Dijkstra's algorithm

	// <The node's name, the distance, the previous node, the number of times it was updated>
	vector<tuple<char, int, char, int>> shortestPath;

	// Small list of adjacent nodes.
	vector<tuple<char, int, char, int>> adjacentNodes;

	// Keeps track of the shortest distances so far.
	vector<tuple<char, int, char, int>> shortestPrv;

	// Copy's the original shortest paths from bellman ford's algorithm.
	vector<tuple<char, int, char, int>> bllmanJohnsonPath;

	vector<bool> dijkstrasQue;

	// Char to keep track of the name, int for the distance.
	vector<char> verticesList; // The order of vertices.

	// String for the edge name, int for the weight.
	vector<string> edgeList; // The order of directed edges. String for the edge name, and bool to determine if it's been visited or not.

	// Adjacency Matrix (Where int = weight).
	vector<vector<int>> edgeAdjacencyMatrix; // The matrix of edges.

	vector<vector<int>> johnsonData;

	vector<vector<int>> matrixBackup; // a copy of the old adjacency matrix but now modified for Johnon's Algorithm.

	// Constructors/Destructor for Graph.
	Graph();
	~Graph();

	// Loads the graph.
	void LoadGraph(char* graphFileName);

	int returnDistance(char srcVrtx);

	bool runJohnson();

	// Run's Bellman Ford's audio
	bool RunBellmanFord(char srcVrtx);

	// Runs Dijkstra's algorithm, using the specified vertex as the starting vertex.
	void RunDijkstra(char startVertex);

	// Returns the distance of the vertex with the specified name from the starting vertex.
	int GetDistance(char vertex);

	void updateTuple(vector<tuple<char, int, char, int>> &tupleUsed, int indxVal, char currChar, int distChar, char prvChar, int update);

	int findSmallestDist(int srcIndx);

	bool isAdigit(const std::string& str);

	void resizeMatrix(size_t newSize);

	vector<char>::iterator SearchNode(vector<char>& vectorUsed, char name);

	void connectEdgesMatrix(char srcString, char dstString, int weightVal);

	vector<string>::iterator SearchEdge(string name1, string name2);

	vector<tuple<char, int, char, int>>::iterator SearchPath(char name);
};

#endif