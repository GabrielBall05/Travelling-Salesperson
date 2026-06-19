//Name: Gabriel Ball
//Date: 06/19/26
//Desc: Travelling Salesperson Solver

#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <random>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <chrono>

using namespace std;

//Struct for City data (id and position)
struct City {
    int id;
    double x;
    double y;
};

//Struct for nodes in the state-space tree
struct Node {
    vector<int> curPath;
    vector<int> unvisitedCities;
    double curCost;
    double bound;

    //Makes priority_queue act as a min-heap instead of the default max-heap
    bool operator<(const Node& other) const {
        return this->bound > other.bound;
    }
};

//Struct for the result of the TSP algorithm
struct TSPResult {
    double minLength;
    vector<int> bestPath;
    chrono::microseconds duration;
};

TSPResult solveTSPBruteForce(const vector<vector<double>>& adjMat); //For verifying B&B result

//Generate n cities at random distances from each other 
vector<City> generateCities(int n) {
    vector<City> cities;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 100.0);

    //Add n cities with random x and y coordinates
    for (int i = 0; i < n; i++)
        cities.push_back({ i, dis(gen), dis(gen) });

    return cities;
}

//Returns distance between 2 cities
double getCityDistance(const City& a, const City& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

//Returns 2D Adjacency Matrix for the given list of cities
vector<vector<double>> generateAdjacencyMatrix(const vector<City>& cities) {
    int n = cities.size();
    //Initialize 2D graph (adjacency matrix)
    vector<vector<double>> graph(n, vector<double>(n, 0.0));

    //Calculate distance from each city to all other cities
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) { //Infinite distance if the city is itself
                graph[i][j] = numeric_limits<double>::infinity();
            }
            else { //Actual Euclidean distance
                graph[i][j] = getCityDistance(cities[i], cities[j]);
            }
        }
    }

    return graph;
}

//Returns the bound for the given node
double computeBound(const vector<vector<double>>& adjMat, const Node& curNode) {
    double estimatedCost = curNode.curCost; //Initialize to the cost so far

    const vector<int>& unvisitedCities = curNode.unvisitedCities;
    int curCity = curNode.curPath.back();

    double minEdge = numeric_limits<double>::infinity();
    if (unvisitedCities.size() == 0) { //If all cities are visited, go back to city 0
        minEdge = adjMat[curCity][0];
    }
    else {
        //Find the minimum outgoing edge from curCity to the unvisited cities
        for (int i = 0; i < unvisitedCities.size(); i++) {
            double distance = adjMat[curCity][unvisitedCities[i]];
            if (distance < minEdge)
                minEdge = distance;
        }
    }
    //Add curCity's minimum outgoing edge to bound
    estimatedCost += minEdge;

    //For every unvisited city, add its minimum edge to other unvisited cities to the estimated cost
    for (int cityId : unvisitedCities) {
        //Find minimum outgoing edge from this unvisited city to the other unvisited cities
        double minEdge = numeric_limits<double>::infinity();
        for (int i = 0; i < unvisitedCities.size(); i++) {
            //If source == destination, skip
            if (cityId == unvisitedCities[i]) continue;

            double distance = adjMat[cityId][unvisitedCities[i]];
            if (distance < minEdge)
                minEdge = distance;
        }
        //Also check distance from this unvisited city to the starting city (0)
        if (adjMat[cityId][0] < minEdge) minEdge = adjMat[cityId][0];

        //Add this unvisited city's minimum outgoing cost to the total estimated cost (bound)
        estimatedCost += minEdge;
    }

    //Return the final lower bound for this node
    return estimatedCost;
}

//TSP Solver Main Algorithm
TSPResult solveTSP(const vector<vector<double>>& adjMat) {
    int n = adjMat.size();
    double minLength = numeric_limits<double>::infinity();
    vector<int> bestPath;
    priority_queue<Node> pq;
    
    //Set up root node with current path: {0}, unvisitiedCities: {1...n-1}, current cost: 0, bound: computeBound() return value
    Node root{};
    root.curPath = { 0 };
    root.unvisitedCities.reserve(n - 1);
    for (int i = 1; i < n; i++) root.unvisitedCities.push_back(i);
    root.curCost = 0.0;
    root.bound = computeBound(adjMat, root);
    
    //Push root into pq
    pq.push(root);

    //Branch-and-Bound loop - stops only when priority queue is empty
    while (!pq.empty()) {
        //Get the node with the lowest bound
        Node curNode = pq.top();
        pq.pop();

        //Skip if this node's bound is >= minLength
        if (curNode.bound >= minLength) continue;

        //If it's a leaf node and its bound is < minLength, update current best solution and continue
        if (curNode.unvisitedCities.empty() && curNode.bound < minLength) {
            minLength = curNode.bound;
            bestPath = curNode.curPath;
            continue;
        }

        //Expand curNode by creating child nodes for each unvisited reachable city from curNode
        for (int i = 0; i < curNode.unvisitedCities.size(); i++) {
            int nextCity = curNode.unvisitedCities[i];

            //Create a child node for curNode
            Node child{};
            
            //Set child's current path to be the parent's path + this destination
            child.curPath = curNode.curPath;
            child.curPath.push_back(nextCity);

            //Set child's unvisited cities list to be the parent's list - this destination
            child.unvisitedCities = curNode.unvisitedCities;
            auto it = find(child.unvisitedCities.begin(), child.unvisitedCities.end(), nextCity);
            if (it != child.unvisitedCities.end())
                child.unvisitedCities.erase(it);

            //Set child's current cost to parent's cost + cost to get here (from adjacency matrix)
            child.curCost = curNode.curCost + adjMat[curNode.curPath.back()][nextCity];

            //Set child's bound
            child.bound = computeBound(adjMat, child);

            //Only add this child to pq if its bound is < minLength
            if (child.bound < minLength)
                pq.push(child);
        }
    }

    return TSPResult{ minLength, bestPath };
}

//Print Results
void printResults(TSPResult result, string label) {
    cout << label << "\n";
    cout << "Minimum Cost: " << result.minLength << "\n";
    cout << "Best Path: ";
    for (int city : result.bestPath)
        cout << city << " -> ";
    cout << "0\n";
    cout << "Duration: " << result.duration.count() << " microseconds\n";
}

//Print coordinates for cities in a format friendly to desmos.com/calculator
void printCityCoordinates(const vector<City>& cities) {
    for (const City& city : cities)
        cout << "(" << city.x << "," << city.y << ")\n";
}

//Print Adjacency Matrix
void printAdjacencyMatrix(const vector<vector<double>>& adjMat) {
    int n = adjMat.size();
    const int cellWidth = 5;
    cout << "\nAdjacency Matrix(Rounded to ints) :\n      ";
    for (int j = 0; j < n; j++)
        cout << setw(cellWidth) << j << "  ";
    cout << "\n     +" << string(n * (cellWidth + 2) - 1, '-') << "+\n";
    for (int i = 0; i < n; i++) {
        cout << setw(4) << i << " |";
        for (int j = 0; j < n; j++) {
            int displayDist = (i == j) ? 0 : static_cast<int>(adjMat[i][j] + 0.5);
            cout << setw(cellWidth) << displayDist << " |";
        }
        cout << "\n     +" << string(n * (cellWidth + 2) - 1, '-') << "+\n";
    }
}

int main() {
    //Get n from user
    cout << "Number of vertices/cities: ";
    int n;
    cin >> n;

    //Generate graph and adjacency matrix
    vector<City> cities = generateCities(n);
    vector<vector<double>> adjMat = generateAdjacencyMatrix(cities);
    printCityCoordinates(cities);
    printAdjacencyMatrix(adjMat);

    //BRANCH AND BOUND
    //Start timer
    auto bbStart = chrono::high_resolution_clock::now();

    //Send adjacency matrix to the solver function
    cout << "\nRunning Branch-and-Bound...\n";
    TSPResult bbResult = solveTSP(adjMat);

    //Stop timer
    auto bbEnd = chrono::high_resolution_clock::now();

    //Store duration and print results
    bbResult.duration = chrono::duration_cast<chrono::microseconds>(bbEnd - bbStart);
    printResults(bbResult, "--- Branch and Bound Results ---");


    //BRUTE FORCE to verify correctness
    //Start timer
    auto bfStart = chrono::high_resolution_clock::now();

    //Run Brute Force alg to verify correctness of b&b's solution
    cout << "\nRunning Brute Force...\n";
    TSPResult bfResult = solveTSPBruteForce(adjMat);

    //Stop timer
    auto bfEnd = chrono::high_resolution_clock::now();

    //Store duration and print results
    bfResult.duration = chrono::duration_cast<chrono::microseconds>(bfEnd - bfStart);
    printResults(bfResult, "--- Brute Force Results ---");
}


//Brute Force TSP Solver
TSPResult solveTSPBruteForce(const vector<vector<double>>& adjMat) {
    int n = adjMat.size();
    vector<int> cities;
    for (int i = 1; i < n; i++) {
        cities.push_back(i);
    }
    double minLength = numeric_limits<double>::infinity();
    vector<int> bestPath;
    do {
        double currentCost = 0.0;
        int currentCity = 0;
        for (int i = 0; i < cities.size(); i++) {
            int nextCity = cities[i];
            currentCost += adjMat[currentCity][nextCity];
            currentCity = nextCity;
        }
        currentCost += adjMat[currentCity][0];
        if (currentCost < minLength) {
            minLength = currentCost;
            bestPath.clear();
            bestPath.push_back(0);
            bestPath.insert(bestPath.end(), cities.begin(), cities.end());
        }
    } while (next_permutation(cities.begin(), cities.end()));
    return TSPResult{ minLength, bestPath };
}