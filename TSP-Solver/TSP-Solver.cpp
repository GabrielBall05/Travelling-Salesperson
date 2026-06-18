//Name: Gabriel Ball
//Date: 06/19/26
//Desc: Travelling Salesperson Solver

#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <cmath>

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
double computeBound(const vector<vector<double>>& adjMat, int curCity, const vector<int>& unvisitedCities, double costSoFar) {
    double estimatedCost = costSoFar; //Initialize to the cost so far

    double minEdge = numeric_limits<double>::infinity();
    if (unvisitedCities.size() == 0) { //If all cities are visited, go back to city 0
        minEdge = adjMat[curCity][0];
    }
    else {
        //Find the minimum outgoing edge from curCity to the unvisited cities
        for (int i = 0; i < unvisitedCities.size(); i++) {
            double distance = adjMat[curCity][unvisitedCities[i]];
            if (distance < minEdge) minEdge = distance;
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
            if (distance < minEdge) minEdge = distance;
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
void solveTSP(const vector<vector<double>>& adjMat) {
    
}

int main() {
    //Get n from user
    cout << "Number of vertices/cities:";
    int n;
    cin >> n;

    //Generate graph and adjacency matrix
    vector<vector<double>> adjMat = generateAdjacencyMatrix(generateCities(n));

    //Send adjacency matrix to the solver function
    solveTSP(adjMat);


}