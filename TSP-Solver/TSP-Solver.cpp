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
double computeBound(const vector<vector<double>>& adjMat, int curCity, const vector<City>& unvisitedCities) {

}

//TSP Solver Main Algorithm
void solveTSP(const vector<vector<double>>& adjMat) {
    
}

int main() {
    


}