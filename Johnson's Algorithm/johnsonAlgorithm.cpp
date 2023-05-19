#include "infinity.h"
#include "graph.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <tuple>
#include <string>
using namespace std;

// Constructor/Destructor for Graph.
Graph::Graph() {
	bllmanJohnsonPath.clear();
	johnsonData.clear();
	matrixBackup.clear();
	edgeAdjacencyMatrix.clear();
	verticesList.clear();
	edgeList.clear();
	shortestPath.clear();
}
Graph::~Graph() {
	bllmanJohnsonPath.clear();
	johnsonData.clear();
	matrixBackup.clear();
	edgeAdjacencyMatrix.clear();
	verticesList.clear();
	edgeList.clear();
	shortestPath.clear();
}

// Figure out why the program isn't reading the file in g++.

// Loads the graph.
void Graph::LoadGraph(char* graphFileName) {
	bllmanJohnsonPath.clear();
	shortestPath.clear();
	string str(graphFileName);
	ifstream readingFile(str);
	string currentLine;
	if (readingFile.is_open()) {
		// Default choice.
		bool isItVertices = true;

		// Skip the first line due to garbage data.
		getline(readingFile, currentLine);
		while (getline(readingFile, currentLine)) {
			/*getline(readingFile, currentLine);*/

			while (currentLine.find_first_not_of("\t\n\r") == string::npos) { getline(readingFile, currentLine); }

			if (currentLine.find("[Vertices]") != string::npos) {
				getline(readingFile, currentLine);
				isItVertices = true;
			}
			if (currentLine.find("[Edges]") != string::npos) {
				isItVertices = false;
				getline(readingFile, currentLine);
			}

			if (isItVertices) {
				// Uses a lambda function that checks each character in the string to make sure it's applicable.
				//cout << "It's a vertex! Now checking if it's exclusively numbers and alphabetical!\n";
				if (isalnum(currentLine.at(0))) {//(all_of(currentLine.begin(), currentLine.end(), //[](unsigned char c) {return isalnum(c);})) { // Broken code because the professor hates advanced c++.
					//cout << "Awesome! Now let's check if the line is not whitespace!\n";
					if (currentLine.length() > 0) {
						//cout << "It isn't! It's good to go!\n";
						verticesList.push_back(currentLine.at(0));
						resizeMatrix(verticesList.size());
					}
				}
			}
			else {
				string srcString, dstString, weightString;
				stringstream ss(currentLine);

				// Check if the first letter of the name is either a number or an alphabetical letter.
				if (currentLine.length() > 0 && (isalpha(currentLine.at(0)) || isdigit(currentLine.at(0)))) {
					// Source vertice of the edge.
					getline(ss, srcString, ' ');
					if (srcString.length() > 0 && all_of(srcString.begin(), srcString.end(), [](unsigned char c) {return isalnum(c);}))
						srcString = srcString.at(0);
					// Destination vertice of the edge.
					getline(ss, dstString, ' ');
					if (dstString.length() > 0 && all_of(dstString.begin(), dstString.end(), [](unsigned char c) {return isalnum(c);}))
						dstString = dstString.at(0);
					// The weight of the edge.
					ss >> weightString;
					//getline(ss, weightString, ' ');
					if (weightString.length() > 0 && ((weightString.at(0) == '-' && isAdigit(weightString)) || isAdigit(weightString))) {// && (stoi(weightString) >= 0)) { // Opting out of this portion of the code to include bellman ford.
						int weight = stoi(weightString);
						// If the vertices actually exist, create the edge.
						if ((SearchNode(verticesList, srcString.at(0)) != verticesList.end()) && (SearchNode(verticesList, dstString.at(0)) != verticesList.end())) {
							//cout << "Process completed!\n";
							string edgeName = srcString + " " + dstString;
							edgeList.push_back(edgeName);
							connectEdgesMatrix(srcString.at(0), dstString.at(0), weight);
						}
						else {
							srcString = "";
							weightString + "";
							dstString = "";
							weight = 0;
						}
					}
				}
			}
		}
		// Shortest path has to be initialized here as there may be a chance the RunDijkstra function won't always run to initialize it.
		for (auto i : verticesList) shortestPath.push_back(tuple<char, int, char, int>(i, GetMAX(), ' ', 0));
		
		copy(edgeAdjacencyMatrix.begin(), edgeAdjacencyMatrix.end(), back_inserter(matrixBackup));
		
		/*shortestPath[0] = make_tuple(get<0>(shortestPath[0]), 0, get<2>(shortestPath[0]), get<3>(shortestPath[0]));*/
		readingFile.close();
	}
}

// Useful for Dijktra's Algorithm but more importantly the Johnson's algorithm.
int Graph::returnDistance(char srcVrtx) {
	return get<1>(shortestPath[(SearchNode(verticesList, srcVrtx) - verticesList.begin())]);
}

bool Graph::runJohnson() {
	if (edgeList.size() < 1 || verticesList.size() < 2) return 0;
	isJohnson = true;
	verticesList.push_back('s');

	bllmanJohnsonPath.clear();

	// Initialize special vertex "s"
	shortestPath.push_back(make_tuple('s', 0, ' ', 0));
	resizeMatrix(matrixBackup.size() + 1);
	for (auto i : verticesList) connectEdgesMatrix(verticesList.back(), i, 0);

	if (RunBellmanFord('s') == 0) return false;

	// Reweigh to positive values via (original weight) + (weight of head traversal) - (weight of tail traversal).
	for (int i = 0; i < edgeAdjacencyMatrix.size(); i++) {
		for (int j = 0; j < edgeAdjacencyMatrix[i].size(); j++) {
			if (edgeAdjacencyMatrix[i][j] != GetINFINITY()) {
				//cout << "johnsonAdjacencyMatrix[i][j] = " << edgeAdjacencyMatrix[i][j] << " + " << get<1>(shortestPath[i]) << " - " << get<1>(shortestPath[j]) << " = " << edgeAdjacencyMatrix[i][j] + get<1>(shortestPath[i]) - get<1>(shortestPath[j]) << endl;
				//cout << edgeAdjacencyMatrix[i][j] << " = " << edgeAdjacencyMatrix[i][j] << " + " << get<1>(shortestPath[i]) << " - " << get<1>(shortestPath[j]) << endl;
				edgeAdjacencyMatrix[i][j] = edgeAdjacencyMatrix[i][j] + get<1>(shortestPath[i]) - get<1>(shortestPath[j]);
			}
		}
	}

	// Sever special vertex "s" from the matrix.
	for (int i = 0; i < edgeAdjacencyMatrix.size(); i++) {
		edgeAdjacencyMatrix[i].pop_back();
		johnsonData[i].pop_back();
	}

	// Remove special vertex "s".
	shortestPath.pop_back();
	verticesList.pop_back();
	edgeAdjacencyMatrix.pop_back();
	johnsonData.pop_back();

	// Prepare for Dijkstra's Algorithm.
	bllmanJohnsonPath = shortestPath;
	for (auto i : shortestPath) i = make_tuple(get<0>(i), get<1>(i), get<2>(i), ' ');

	printMatrix(edgeAdjacencyMatrix);

	// Finally, run Dijkstra's Algorithm on every node in the graph.
	for (int i = 0; i < verticesList.size(); i++) {
		cout << "\nStarting vertex: " << verticesList[i] << endl;
		RunDijkstra(verticesList[i]);
		printTuples();
		for (int j = 0; j < edgeAdjacencyMatrix[i].size(); j++) {
			//if (edgeAdjacencyMatrix[i][j] == GetINFINITY()) edgeAdjacencyMatrix[i][j] = 0; // This is the root of all our problems. Dijkstra cannot resolve it's adjacent nodes if they're not infinity!
			if (i != j) {
				if (get<1>(shortestPath[j]) != GetMAX() && get<1>(shortestPath[i]) != GetMAX()) {
					cout << "edgeAdjacencyMatrix[" << i << "][" << j << "] = " << "get<1>(shortestPath[j]) (" << get<1>(shortestPath[j]) << ") + get<1>(bllmanJohnsonPath[j]) (" << get<1>(bllmanJohnsonPath[j]) << ") - get<1>(bllmanJohnsonPath[i]) (" << get<1>(bllmanJohnsonPath[i]) << ")\n";
					johnsonData[i][j] = get<1>(shortestPath[j]) + get<1>(bllmanJohnsonPath[j]) - get<1>(bllmanJohnsonPath[i]);
					cout << " = " << johnsonData[i][j] << endl;
				}
			}
		}
	}
	isJohnson = false;
	return true;
}

bool Graph::RunBellmanFord(char srcVrtx) {

	shortestPath.clear(); // Might have to remove this.
	for (auto i : verticesList) shortestPath.push_back(tuple<char, int, char, int>(i, 0, ' ', 0));

	// Reverts to the old matrix that was loaded in first.
	if (!isJohnson) edgeAdjacencyMatrix = matrixBackup;

	int updateInc = 0, srcIndex = SearchNode(verticesList, srcVrtx) - verticesList.begin();

	shortestPath[srcIndex] = make_tuple(get<0>(shortestPath[srcIndex]), 0, ' ', 0);

	// Standard distance check run.
	for (int i = 0; i < edgeAdjacencyMatrix.size(); i++) {
		for (int j = 0; j < edgeAdjacencyMatrix[i].size(); j++) {
			// If the tail has a greater weight traversal than relaxing the edge, choose to relax it instead.
			if (i != j && edgeAdjacencyMatrix[i][j] != GetINFINITY() && (get<1>(shortestPath[j]) > get<1>(shortestPath[i]) + edgeAdjacencyMatrix[i][j])) {
				updateTuple(shortestPath, j, get<0>(shortestPath[j]), get<1>(shortestPath[i]) + edgeAdjacencyMatrix[i][j], verticesList[i], ++updateInc);
			}
		}
	}

	cout << "This is the newly updated version after bellman-ford:\n";
	printTuples();

	// Check for negative weight cycles.
	for (int i = 0; i < edgeAdjacencyMatrix.size(); i++) {
		for (int j = 0; j < edgeAdjacencyMatrix[i].size(); j++) {
			if (edgeAdjacencyMatrix[i][j] != GetINFINITY()) {
				//cout << "Checking if edge: " << verticesList[j] << " > " << edgeAdjacencyMatrix[i][j] << " + " << verticesList[i] << " has no negative cycles!\n";
				//cout << "Where shortest paths are: " << verticesList[j] << " = " << get<1>(shortestPath[j]) << " and " << verticesList[i] << " = " << get<1>(shortestPath[i]) << endl;
			}
			if (isJohnson && edgeAdjacencyMatrix[i][j] != GetINFINITY() && get<2>(shortestPath[j]) != verticesList.back() && get<1>(shortestPath[j]) > get<1>(shortestPath[i]) + edgeAdjacencyMatrix[i][j]) return false;
			else if (!isJohnson && edgeAdjacencyMatrix[i][j] != GetINFINITY() && get<1>(shortestPath[j]) > get<1>(shortestPath[i]) + edgeAdjacencyMatrix[i][j]) return false;
		}
	}
	// If the code successfully ran without negative weight cycles, returning true will confirm so.
	return true;
}

// READ THIS: Dijkstra's algorithm only works for one run supposedly. Need to find a way to store the previous runs that have smaller values.

// Runs Dijkstra's algorithm, using the specified vertex as the starting vertex.
void Graph::RunDijkstra(char startVertex) {

	// Reverts to the old matrix that was loaded in first.
	if (!isJohnson) edgeAdjacencyMatrix = matrixBackup;
	
	shortestPath.clear(); // Might have to remove this.

	dijkstrasQue.clear();

	for (auto i : verticesList) {
		shortestPath.push_back(tuple<char, int, char, int>(i, GetMAX(), ' ', 0));
		dijkstrasQue.push_back(false);
	}

	if (SearchNode(verticesList, startVertex) != verticesList.end()) {
		startingIndx = SearchNode(verticesList, startVertex) - verticesList.begin();
		nxtAdjacent = startingIndx;

		dijkstrasQue[startingIndx] = true;

		int closestNode = startingIndx;

		for (size_t i = 0; i < edgeAdjacencyMatrix.size()-1; i++) {

			// Initialize the first node. This was originally shortestPath[0] = make_tuple(get<0>(shortestPath[0]), 0, ' ', 0);
			// It won't always be this case because the starting verticesList node won't always be the starting node in Dijkstra's algorithm.
			updateTuple(shortestPath, startingIndx, get<0>(shortestPath[startingIndx]), 0, ' ', 0);

			for (size_t j = 0; j < edgeAdjacencyMatrix.size()-1; j++) {
				int updateIncrement = get<3>(shortestPath[j]);

				// For some reason, when running Johnson's algorithm, when b is selected as the src vertex, c = 1000, and a = 1000, therefore this needs to be updated so that dijkstra will select the shortest path even if it's behind the starting vertex.
				if (i != j && edgeAdjacencyMatrix[closestNode][j] != GetINFINITY()) {
					if (get<1>(shortestPath[j]) > (get<1>(shortestPath[closestNode]) + edgeAdjacencyMatrix[closestNode][j])) {
						// Make a fresh new tuple since tuples are immutable and cannot be modified.
						updateTuple(shortestPath, j, get<0>(shortestPath[j]), get<1>(shortestPath[closestNode]) + edgeAdjacencyMatrix[closestNode][j], verticesList[closestNode], ++updateIncrement);
					}
				}
			}
			closestNode = findSmallestDist(nxtAdjacent);
		}
	}
}

// This isn't selecting the smallest, closest adjacent node
// Finds the smallest distance between all adjacent nodes for a proper successor node by returning the index.
int Graph::findSmallestDist(int srcIndx) {
	
	int minVal = GetMAX();
	int minIndex = GetINFINITY();

	adjacentNodes.clear();

	printMatrix(edgeAdjacencyMatrix);

	cout << endl;

	// Identify adjacent nodes.
	for (size_t i = 0; i < edgeAdjacencyMatrix[srcIndx].size(); i++) {
		if (!dijkstrasQue[i] && edgeAdjacencyMatrix[srcIndx][i] != GetINFINITY()) {
			//cout << "Successfully found a connection from " << verticesList[srcIndx] << " to " << verticesList[i] << "(" << edgeAdjacencyMatrix[srcIndx][i] << ")" << endl;
			adjacentNodes.push_back(shortestPath[i]);

			// While checking, identify whether there's shorter paths that weren't updated.
			if (get<1>(shortestPath[i]) == GetMAX() || get<1>(shortestPath[srcIndx]) + edgeAdjacencyMatrix[srcIndx][i] < get<1>(shortestPath[i])) {
				//cout << "It's GetMAX(). Setting the path to: " << "edgeAdjacencyMatrix[srcIndx][i] + get<1>(shortestPath[srcIndx]) (" << edgeAdjacencyMatrix[srcIndx][i] + get<1>(shortestPath[srcIndx]) << ")\n";
				updateTuple(shortestPath, i, get<0>(shortestPath[i]), edgeAdjacencyMatrix[srcIndx][i] + get<1>(shortestPath[srcIndx]), verticesList[srcIndx], get<3>(shortestPath[i]) + 1);
			}
		}
	}

	cout << "Total adjacent nodes: \n";
	for (auto i : adjacentNodes) {
		cout << get<0>(i) << " ";
	}
	cout << endl;

	// Select the smallest distance from the list of adjacent nodes.
	for (size_t i = 0; i < adjacentNodes.size(); i++) {
		int edgeIndx = SearchNode(verticesList, get<0>(adjacentNodes[i])) - verticesList.begin();
		//cout << "Comparing if " << get<0>(adjacentNodes[i]) << " is equal to " << verticesList[edgeIndx] << endl;
		cout << "Checking if " << minVal << " > " << "edgeAdjacencyMatrix[" << srcIndx << "][" << edgeIndx << "] (" << edgeAdjacencyMatrix[srcIndx][edgeIndx] << ")\n";
		if (edgeIndx != srcIndx && minVal > edgeAdjacencyMatrix[srcIndx][edgeIndx]) {
			cout << "Test case passed!\n";
			minVal = edgeAdjacencyMatrix[srcIndx][edgeIndx];
			minIndex = edgeIndx;
		}
	}

	// Safe guard in the case that none of the candidates are applicable. Select the next available node that wasn't touched.
	if (minIndex == srcIndx || minIndex == GetINFINITY()) {
		cout << "\nThere was an issue finding a proper candidate. Regressing to other options\n";
		for (size_t i = 0; i < dijkstrasQue.size(); i++) {
			if (!dijkstrasQue[i] && srcIndx != i) {
				minIndex = i;
			}
		}
	}
	//for (size_t i = 0; i < edgeAdjacencyMatrix.size(); i++) {
	//	if (!dijkstrasQue[i] && get<1>(shortestPath[i]) < minVal /*&& edgeAdjacencyMatrix[srcIndx][i] != GetINFINITY()*/) {
	//		minVal = get<1>(shortestPath[i]);
	//		minIndex = i;
	//	}
	//}
	cout << "Next smallest distance: " << verticesList[minIndex] << endl;
	nxtAdjacent = minIndex;

	// dijkstra's queue will now function as a visited boolean vector.
	dijkstrasQue[minIndex] = true;
	
	return minIndex;
}

// Returns the distance of the vertex with the specified name from the starting vertex. Needs to pass GetInfinity1 and LoadGraph1
int Graph::GetDistance(char vertex) {
	for (auto i : shortestPath) {
		if ((get<0>(i) == vertex) && get<1>(i) != GetMAX()) {
			return get<1>(i);
		}
	}
	return -1;
}

// Returns the number of updates made to the vertex's distance during the last call of RunDijkstra.
int Graph::GetNumberOfUpdates(char vertex) {
	for (auto i : shortestPath) {
		if (get<0>(i) == vertex)
			return get<3>(i);
	}
	return -1;
}

void Graph::resizeMatrix(size_t newSize) {
	// Create a template of the old matrix.
	vector<vector<int>> oldMatrix = edgeAdjacencyMatrix;
	// Every size in the matrix row will be newSize, and then each vector column will also be new size set to 0.
	edgeAdjacencyMatrix = vector<vector<int>>(newSize, vector<int>(newSize, GetINFINITY()));
	johnsonData = vector<vector<int>>(newSize, vector<int>(newSize, 0));
	// Resizing the matrix.
	for (size_t row = 0; row < min(newSize, oldMatrix.size()); row++) {
		for (size_t column = 0; column < min(newSize, oldMatrix.size()); column++) {
			edgeAdjacencyMatrix[row][column] = oldMatrix[row][column];
		}
	}
}

// Documents the weight of the edges.
void Graph::connectEdgesMatrix(char srcString, char dstString, int weightVal) {
	// Overloading the values to get the index. This is because the iterators (Similiar to pointers but more abstract) are subtracted and result in the distance between the two iterators.

	if (SearchNode(verticesList, srcString) == verticesList.end() || SearchNode(verticesList, dstString) == verticesList.end()) cout << "One of the nodes that were searched doesn't exist!\n";

	int fromNodeIndex = SearchNode(verticesList, srcString) - verticesList.begin();

	int toNodeIndex = SearchNode(verticesList, dstString) - verticesList.begin();

	edgeAdjacencyMatrix[fromNodeIndex][toNodeIndex] = weightVal;
}

void Graph::updateTuple(vector<tuple<char, int, char, int>> &tupleUsed, int indxVal, char currChar, int distChar, char prvChar, int update) {
	auto newTups = make_tuple(currChar, distChar, prvChar, update);
	shortestPath[indxVal] = newTups;
}

void Graph::printTuples() {
	cout << "Tuple stats:\n";
	for (auto i : shortestPath) {
		cout << "The original char is: " << get<0>(i) << ". Its shortest path is: " << get<1>(i) << ". And its previous char is: " << get<2>(i) << endl;
	}
}

void Graph::printMatrix(vector<vector<int>> matrixEdges) {
	cout << "Adjacency Matrix: \n";
	string arrayLine = "";
	for (size_t i = 0; i < matrixEdges.size(); i++) {
		arrayLine += string(1, verticesList[i]) + " [";
		for (auto j : matrixEdges[i]) {
			arrayLine += to_string(j) + "] [";
		}
		arrayLine.erase(arrayLine.begin() + arrayLine.length()-1);
		cout << arrayLine << endl;
		arrayLine.clear();
	}
}

void Graph::printVector(vector<char> vectorVar) {
	for (auto i : vectorVar) cout << i << " ";
	cout << endl;
}

vector<tuple<char, int, char, int>>::iterator Graph::SearchPath(char name) {
	// The lambda function [name](const auto& t) is a function inside the main function that finds the position of the tuple that has the identical name.
	auto targetName = [name](const auto& t) {return get<0>(t) == name;};
	// Good to know that using find_if searches for the "iteration", while "find" searches for the type.
	return find_if(shortestPath.begin(), shortestPath.end(), targetName); // Returns the end of shortestPath if it doesn't exist.
}

// Returns an iterator that represents the position of the node in the vector. If it doesn't exist, it sends the end of the list.
// Powerful when subtracting with the the beginning of the list as it will give an integer version of the index.
vector<char>::iterator Graph::SearchNode(vector<char>& vectorUsed, char name) { // vectorUsed needs to be referenced w/ "&" so that the vector can be modified, aka searched.
	//cout << "Searching for " << name << endl;
	//if (find(vectorUsed.begin(), vectorUsed.end(), name) == vectorUsed.end()) cout << "The node wasn't found.\n";
	return find(vectorUsed.begin(), vectorUsed.end(), name);
}

vector<string>::iterator Graph::SearchEdge(string name1, string name2) {
	return find(edgeList.begin(), edgeList.end(), name1 + " " + name2);
}

// Doesn't function as intended for older versions of c++.
bool Graph::isAdigit(const std::string& str) {
	// If there exists a position in str.find_first_not_of, and it doesn't equal string npos (no position), then it does exist.
	if (str.at(0) == '-') {
		string tempStr = str;
		tempStr.erase(0, 1);
		return tempStr.find_first_not_of("0123456789") == string::npos;
	}
	else return str.find_first_not_of("0123456789") == string::npos;
}