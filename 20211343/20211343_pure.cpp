#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <set>
#include <numeric>
#include <chrono>
#include <cmath>
using namespace std;

#define POP_SIZE 200  
#define GENERATIONS 1000
#define MUTATION_RATE 0.01
#define TOURNAMENT_SIZE 2
#define CROSSOVER_RATE 0.7

struct Edge {
    int u, v, weight;
};

vector<vector<int>> population;
vector<Edge> edges;
int V, E;

void initialize_population() {
    srand(time(NULL));
    for (int i = 0; i < POP_SIZE; ++i) {
        vector<int> individual;
        for (int j = 0; j < V; ++j) {
            individual.push_back(rand() % 2);
        }
        population.push_back(individual);
    }
}

int fitness(const vector<int>& individual) {
    int fitness = 0;
    for (const auto& edge : edges) {
        if (individual[edge.u] != individual[edge.v]) {
            fitness += edge.weight;
        }
    }
    return fitness;
}

vector<int> tournament_selection() {
    set<int> chosen;
    while (chosen.size() < TOURNAMENT_SIZE) {
        chosen.insert(rand() % POP_SIZE);
    }

    int number = rand() % 10;
    int best = *chosen.begin();
    for (const auto& idx : chosen) {
        if (fitness(population[idx]) > fitness(population[best])) {
            best = idx;
        }
    }

    int worse = *chosen.begin();
    for (const auto& idx : chosen) {
        if (fitness(population[idx]) < fitness(population[worse])) {
            worse = idx;
        }
    }

    if (number < 7) {
        return population[best];
    }
    else {
        return population[worse];
    }
}

void crossover(vector<int>& parent1, vector<int>& parent2) {
    int cross_point = int(V / 2);
    for (int i = 0; i < cross_point; i++) {
        swap(parent1[i], parent2[i]);
    }
}

void mutate(vector<int>& individual) {
    for (int& gene : individual) {
        if (((double)rand() / RAND_MAX) < MUTATION_RATE) {
            gene = 1 - gene;
        }
    }
}

void genetic_algorithm() {
    auto start = chrono::steady_clock::now();
    for (int gen = 0; gen < GENERATIONS; ++gen) {
        vector<vector<int>> new_population;
        for (int i = 0; i < POP_SIZE; ++i) {
            vector<int> parent1 = tournament_selection();
            vector<int> parent2 = tournament_selection();
            crossover(parent1, parent2);
            mutate(parent1);
            new_population.push_back(parent1);
        }
        population = new_population;

        auto end = chrono::steady_clock::now();
        if (chrono::duration_cast<chrono::seconds>(end - start).count() > 180) {
            break;
        }
    }
}

vector<int> get_best() {
    int best_idx = 0;
    for (int i = 1; i < POP_SIZE; ++i) {
        if (fitness(population[i]) > fitness(population[best_idx])) {
            best_idx = i;
        }
    }
    return population[best_idx];
}

vector<double> repeated_runs(int runs) {
    vector<double> fitnesses;
    for (int i = 0; i < runs; ++i) {
        initialize_population();
        genetic_algorithm();
        vector<int> best_individual = get_best();
        fitnesses.push_back(fitness(best_individual));
    }
    return fitnesses;
}

double average_fitness(const vector<double>& fitnesses) {
    return accumulate(fitnesses.begin(), fitnesses.end(), 0.0) / fitnesses.size();
}

double standard_deviation_fitness(const vector<double>& fitnesses, double average) {
    double sq_sum = 0.0;
    for (const auto& f : fitnesses) {
        sq_sum += (f - average) * (f - average);
    }
    return sqrt(sq_sum / fitnesses.size());
}

int main() {
    ifstream infile("graph.txt");
    infile >> V >> E;
    for (int i = 0; i < E; i++) {
        int u, v, w;
        infile >> u >> v >> w;
        edges.push_back({ u - 1, v - 1, w });
    }
    infile.close();

    int runs = 30;
    vector<double> fitnesses = repeated_runs(runs);
    double average = average_fitness(fitnesses);
    double std_dev = standard_deviation_fitness(fitnesses, average);

    cout << "Best Fitness: " << *max_element(fitnesses.begin(), fitnesses.end()) << endl;
    cout << "Average Fitness: " << average << endl;
    cout << "Standard Deviation of Fitness: " << std_dev << endl;

    vector<int> best_individual = get_best();
    ofstream outfile("maxcut.txt");
    for (int i = 0; i < V; i++) {
        if (best_individual[i] == 1) {
            outfile << i + 1 << " ";
        }
    }
    outfile.close();
    return 0;
}