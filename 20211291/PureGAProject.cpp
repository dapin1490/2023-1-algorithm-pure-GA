#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <numeric>
#include <cmath>
using namespace std;

#define POP_SIZE 200 // 한 세대를 이루는 해의 총 개수
#define GENERATIONS 1000 // 총 1000 세대
#define TOURNAMENT_SIZE 0.14 // 토너먼트에 참여할 해의 비율
#define TOURNAMENT_RATE 0.6 // 토너먼트 선택이 발생할 확률
#define CROSSOVER_RATE 0.8 // 교차(교배)가 발생할 확률
#define MUTATION_RATE 0.05 // 변이가 발생할 확률


struct Edge {
    int v1, v2, w; // v1 정점, v2 정점, v1과 v2를 잇는 간선의 가중치 w
};

vector<vector<int>> population; // 한 세대를 이루는 해를 저장하는 컨테이너 생성
vector<Edge> edges; // <v1, v2, w>를 저장하는 컨테이너 생성
int count_V, count_E; // 정점 개수, 간선 개수


void initialize_population() { // 해 생성
    srand(time(NULL));
    for (int i = 0; i < POP_SIZE; ++i) { 
        vector<int> individual;
        for (int j = 0; j < count_V; ++j) { // 0 또는 1로 이루어진 count_V 크기의 하나의 해 생성
            individual.push_back(rand() % 2); // individual: 0과 1로 이루어짐
        }
        population.push_back(individual); // 생성한 해를 population 컨테이너에 추가
    }
}

int fitness (const vector<int>& individual) { // 적합도 계산
    int fitness = 0; // 적합도
    for (const auto &edge : edges) {
        if (individual[edge.v1] != individual[edge.v2]) { // v1과 v2가 서로 연결되었으면
            fitness += edge.w; // 연결된 간선의 가중치 값을 더함
        }
    }
    return fitness; // 적합도 값을 반환
}

vector<int> tournament_selection() { // 부모 선택 - 토너먼트 선택 방식
    set<int> chosen;
    int tournament_num = (int) count_V * TOURNAMENT_SIZE;
    while (chosen.size() < tournament_num) { 
        chosen.insert(rand() % POP_SIZE); // 토너먼트에 참여할 해를 랜덤으로 선택 -> 이 중에서 parent가 나옴
    }
    double tournament_prob = (double) rand() / RAND_MAX;

    if (tournament_prob < TOURNAMENT_RATE){ // 0.6보다 작으면 가장 좋은 해를 선택하여 반환
        int best = *chosen.begin(); // 처음 선택된 자식의 index
        for (const auto &idx : chosen) { // 가장 좋은 해를 찾는 for문 (이걸 좀 더 쉽게 쓸 수 있는 방법은 없나? aoto를 모르겠음)
            if (fitness(population[idx]) > fitness(population[best])) { // 적합도를 비교하여 더 좋은 해가 있으면 그 해의 index로 갱신
                best = idx;
            }
        }
        return population[best]; // 가장 좋은 해(0과 1로 이루어진 individual)를 반환함
    }
    else { // chosen에서 랜덤한 하나의 해를 선택하여 반환 
        int ran = rand() % tournament_num;
        auto ran_idx = chosen.begin();
        advance(ran_idx, ran);

        return population[*ran_idx];
    }
}

void crossover(vector<int>& parent1, vector<int>& parent2) { // 두 부모를 교차(교배)
    vector<int> best_parent; // 더 우수한 parent
    vector<int> worst_parent; // 더 열등한 parent
    if (fitness(parent1) >= fitness(parent2)){
        best_parent = parent1;
        worst_parent = parent2;
    }
    else{
        best_parent = parent2;
        worst_parent = parent1;
    }

    if (((double) rand() / RAND_MAX) < CROSSOVER_RATE) { // 교차가 일어나면
    int cross_point = (int)(count_V * 0.75); // 3/4 지점까지 우수한 parent가 들어감
    for (int i = 0; i < cross_point; ++i){
        swap(best_parent[i], worst_parent[i]); // wort_parent에 우수한 부모의 3/4 개의 gene이 들어가게 됨 -> wort_parent에 더 좋은 해가 많아짐
    }
    parent1 = worst_parent;
    parent2 = best_parent;
    }
}

void mutate(vector<int>& individual) { // 변이
    for (int& gene : individual) { // gene: individual 컨테이너 내 원소에 대한 참조값(reference)
        if (((double) rand() / RAND_MAX) < MUTATION_RATE) { // 변이가 발생하면
            gene = 1 - gene; // 0인 gene은 1로, 1인 gene은 0으로 바뀜
        }
    }
}

/*
void steady_state_replace (vector<vector<int>>& population, vector<vector<int>>& new_population) // steady-state 방식으로 대치
{
    for (int i = 0; i < new_population.size(); ++i){ 
        int worst = 0; // 현재 세대에서 가장 열등한 해의 index
        for (int j = 0; j < population.size(); ++j){ // 현재 세대에서 가장 열등한 해의 index를 찾음
            if (fitness(population[j]) < fitness(population[worst]))
                worst = j;
        }
        if (fitness(new_population[i]) > fitness(population[worst])) 
            population[worst] = new_population[i]; // 현재 세대에서 가장 열등한 해를 후속 세대의 해로 바꿈
    }
}


void steady_state_replace(vector<vector<int>>& new_population) {
    for (const auto& ind : new_population) {
        int worst = 0;
        for (int j = 0; j < population.size(); ++j) {
            if (fitness(population[j]) < fitness(population[worst]))
                worst = j;
        }
        if (fitness(ind) > fitness(population[worst]))
            population[worst] = ind;
    }
}
*/

void genetic_algorithm() {
    auto start = chrono::steady_clock::now(); // start: 현재 시간
    for (int gen = 0; gen < GENERATIONS; ++gen) {
        vector<vector<int>> new_population; // 그 다음 후속 세대가 생성
        for (int i = 0; i < POP_SIZE; ++i) { 
            vector<int> parent1 = tournament_selection(); 
            vector<int> parent2 = tournament_selection();
            if (parent1 == parent2){ // 똑같은 부모가 나오면 다시 부모를 선택함
                vector<int> parent1 = tournament_selection(); 
                vector<int> parent2 = tournament_selection();
            }
            crossover(parent1, parent2); // parent1과 parent2 교차(교배)
            mutate(parent1); // 교배했을 때 더 좋은 해(parent1)의 변이
            //mutate(parent2); // 교배했을 때 더 좋지 않은 해(parent2)의 변이
            new_population.push_back(parent1); // 그 다음 후속 세대에 새로운 변이 해 삽입
            /*
            if (new_population.size() < POP_SIZE) {
                new_population.push_back(parent2);
            }
            */
            
        }
        //steady_state_replace(population, new_population); // steady-state 방식으로 대치
        population = new_population; // generational GA 방식으로 대치

        /*
        // steady-state 방식으로 대치
        population.insert(population.end(), new_population.begin(), new_population.end()); // 현재 세대와 후속 세대 결합

        // fitness 값을 기준으로 결합된 세대를 정렬함
        sort(population.begin(), population.end(),
            [](const vector<int>& a, const vector<int>& b) {
                return fitness(a) > fitness(b);
            });

        // 상위 POP_SIZE(세대에 속하는 해의 개수) 만큼이 새로운 후속 세대가 됨
        population.resize(POP_SIZE);
        */

        auto end = chrono::steady_clock::now(); // end: 끝나는 시간
        if (chrono::duration_cast<chrono::seconds>(end - start).count() > 180) { // 
            break;
        }
    }
}

vector<int> get_best() {
    int best_idx = 0;
    for (int i = 1; i < POP_SIZE; ++i) {
        if (fitness(population[i]) > fitness(population[best_idx])) { // 가장 우수한 해 찾음
            best_idx = i;
        }
    }
    return population[best_idx]; // 가장 우수한 해
}

vector<double> repeated_runs(int runs) { 
    vector<double> fitnesses;
    for (int i = 0; i < runs; ++i) { // 30번 반복
        initialize_population();
        genetic_algorithm();
        vector<int> best_individual = get_best(); 
        fitnesses.push_back(fitness(best_individual)); // 가장 우수한 해 fitnesses 컨테이너에 삽입
    }
    return fitnesses;
}

double average_fitness(const vector<double>& fitnesses) { // fitness들의 평균
    return accumulate(fitnesses.begin(), fitnesses.end(), 0.0) / fitnesses.size();
}

double standard_deviation_fitness(const vector<double>& fitnesses, double average) { // fitness들의 표준편차
    double sq_sum = 0.0;
    for (const auto& f : fitnesses) {
        sq_sum += (f - average) * (f - average);
    }
    return sqrt(sq_sum / fitnesses.size());
}


int main(){

    string inputFile = "maxcut.in"; // 입력 파일명
    string outputFile = "maxcut.out"; // 출력 파일명
    

    ifstream inFile(inputFile);
    inFile >> count_V >> count_E; // 정점 개수, 간선 개수 읽기
    for (int i = 0; i < count_E; ++i){ 
        int vertax1, vertax2, weight; // 정점1, 정점2, 정점1과 정점2를 잇는 가중치
        inFile >> vertax1 >> vertax2 >> weight;
        edges.push_back({vertax1 - 1, vertax2 - 1, weight}); // 0부터 시작하기 떄문에 1씩 빼줌
    }
    inFile.close();

    int runs = 30; // 30번 반복
    vector<double> fitnesses = repeated_runs(runs); // 30번 반복하여 나온 해의 fitness 값을 fitnesses 컨테이너에 삽입
    double average = average_fitness(fitnesses); // 30번 나온 해의 fitness 값의 평균
    double std_dev = standard_deviation_fitness(fitnesses, average); // 30번 나온 해의 fitness 값의 표준편차

    // 가장 우수한 해, 평균, 표준편차 출력
    cout << "Best Fitness: " << *max_element(fitnesses.begin(), fitnesses.end()) << endl;
    cout << "Average Fitness: " << average << endl;
    cout << "Standard Deviation of Fitness: " << std_dev << endl;

    //initialize_population();
    //genetic_algorithm();

    vector<int> best_individual = get_best(); // 가장 우수한 해

    ofstream outFile(outputFile);
    for (int i = 0; i < count_V; ++i) {
        if (best_individual[i] == 1) {
            outFile << i + 1 << " "; // 처음에 inFile의 정점을 읽을 때 1씩 뺐기 때문에 다시 1씩 더함
            //cout << i + 1 << " "; // 출력
        }
    }
    outFile.close();
    


    return 0;    
}