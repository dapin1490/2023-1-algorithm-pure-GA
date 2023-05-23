#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <ctime>
#include <limits>
using namespace std;


/***************

<순서>
1. 그래프 입력
2. 유전자 해 생성
3. 가중치 계산
4. 부모풀 생성
5. Selection
6. Crossover
7. 자식풀 형성
8. Replace
9. 풀 가중치 총합, 최적 해, 평균 계산
10. main

*****************/





//그래프 저장을 위한 구조체 선언
struct Edge {
    int node1;
    int node2;
    int weight;
};


//그래프 읽기
vector<Edge> readGraphFromFile(const string& filename) {
    vector<Edge> graph;
    ifstream file(filename);
    if (file.is_open()) {
        int numVertices, numEdges;
        file >> numVertices >> numEdges; //첫출

        for (int i = 0; i < numEdges; i++) {
            Edge edge;
            file >> edge.node1 >> edge.node2 >> edge.weight;
            graph.push_back(edge);
        }

        file.close();
        cout << "읽은 파일: " << filename << "." << endl;
    }
    else {
        cerr << filename <<"파일을 읽을 수 없습니다." << endl;
    }

    return graph;
}


//유전자 해 생성
string generateDNA(const vector<Edge>& graph) {
    string dna;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    for (const auto& edge : graph) {
        if (dis(gen) <= 0.5) {
            dna += "1 ";
        }
        else {
            dna += "0 ";
        }
    }
    return dna;
}


//가중치 계산
int calculateWeight(const vector<Edge>& graph, const string& chromosome) {
    int Weight = 0;
    for (const auto& edge : graph) {
        if (chromosome[edge.node1 - 1] == '1' && chromosome[edge.node2 - 1] == '0') {
            Weight += edge.weight;
        }
        else if (chromosome[edge.node1 - 1] == '0' && chromosome[edge.node2 - 1] == '1') {
            Weight += edge.weight;
        }
    }
    return Weight;
}


 
//부모풀 생성
vector<pair<string, double>> parentPool;

void genParentPool(const vector<Edge>& graph) {
    int population=graph.size()*10;
    
    if (population > 1000) {
        population = 1000;
    }

    for (int i = 1; i <= population; i++) {
        string chromo = generateDNA(graph);
        //가중치 계산해서 Weight에 저장
        int Weight = calculateWeight(graph, chromo);
        parentPool.emplace_back(chromo, Weight); //부모풀에 삽입
    }
}



// 유전자와 가중치를 저장하는 페어 타입 정의
using GeneWeightPair_Parent = pair<string, double>;

// Selection 함수 정의
pair<GeneWeightPair_Parent, GeneWeightPair_Parent> Selection(const vector<GeneWeightPair_Parent>& parentPool) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, parentPool.size() - 1);

    while (true) {
        // 랜덤하게 두 개의 유전자 선택
        int index1 = dist(gen);
        int index2 = dist(gen);

        const GeneWeightPair_Parent& gene1 = parentPool[index1];
        const GeneWeightPair_Parent& gene2 = parentPool[index2];

        // 두 유전자의 가중치 차이 계산
        double weightDiff = abs(gene1.second - gene2.second);
        
        // maxDiff에 부모풀 가중치 최대, 최소값의 차의 50%인 값 저장
        double maxDiff = (max_element(parentPool.begin(), parentPool.end(),
            [](const GeneWeightPair_Parent& a, const GeneWeightPair_Parent& b) {
                return a.second < b.second;
            })->second) * 0.5;

        if (weightDiff <= maxDiff) {
            // 조건을 만족하는 경우 선택한 유전자들 반환
            return make_pair(gene1, gene2);
        }
    }
}



//Crossover 해서 child를 반환하는 연산자 
string Crossover(const string& gene1, const string& gene2) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    string child;
    int count = 1;

    for (size_t i = 0; i < gene1.length(); ++i) {
        // 가중치가 더 높은 유전자의 60%를 그대로 가져오기
        if (dis(gen) <= 0.6) {
            child += gene1[i];
        }
        else {
            // 나머지 40%는 다른 유전자에서 가져오기
            child += gene2[i];
        }
        // 돌연변이 생성하기
        if (dis(gen) <= (0.0005 * count)) {
            // 랜덤한 자리 지정
            if (child[i] == '0') {
                child[i] = '1';
            }
            else {
                child[i] = '0';
            }
        }

        // 노드 개수가 10을 넘을 때마다 count 증가
        if ((i + 1) % 10 == 0) {
            count++;
        }
    }return child;
    
}



//자식풀 형성 함수
vector<pair<string, double>> childPool;
void genchildPool(const vector<Edge>& graph) {
    //사이즈: 부모풀과 동일
    int parentSize = parentPool.size();
    
    for (int i = 1; i <= parentSize; i++) {
        pair<GeneWeightPair_Parent, GeneWeightPair_Parent> selectedGenes = Selection(parentPool); //selection
        string child = Crossover(selectedGenes.first.first, selectedGenes.second.first); //crossover
        string chromo = child;
        int Weight = calculateWeight(graph, chromo); //가중치 계산
        childPool.emplace_back(chromo, Weight); //자식풀에 pair 삽입
    }
}


// replace: childPool로 parentPool을 대체하고 parentPool은 빈 벡터로 남기는 함수
void replace(vector<int>& parentPool, vector<int>& childPool) {
    // parentPool의 각 염색체에 대해 가중치를 계산하고 합산 및 최대값 업데이트
    
    parentPool = childPool;  // parentPool을 childPool로 대체
    childPool.clear();  // childPool을 빈 벡터로 초기화
    
}


//풀 가중치 총합, 최적 해, 평균 계산
void calculateParentPoolStats(const vector<Edge>& graph, const vector<pair<string, double>>& parentPool) {
    double totalWeight = 0.0; // 가중치 총합
    double maxWeight = numeric_limits<double>::min(); // 최대 가중치
    pair<string, double> maxWeightPair; // 최대 가중치를 가진 해
    double sumSquaredDiff = 0.0; // 제곱합의 합

    for (const auto& pair : parentPool) {
        const string& chromosome = pair.first;
        double weight = calculateWeight(graph, chromosome);
        totalWeight += weight;

        if (weight > maxWeight) {
            maxWeight = weight;
            maxWeightPair = pair;
        }
        double diff = weight - totalWeight / parentPool.size();
        sumSquaredDiff += diff * diff;
    }
    double averageWeight = totalWeight / parentPool.size(); // 가중치 평균
    double variance = sumSquaredDiff / parentPool.size(); // 분산
    double standardDeviation = std::sqrt(variance); // 표준편차


    cout << "최대 가중치: " << maxWeight << endl;
    cout << "부모풀 가중치 총합: " << totalWeight << endl;
    cout << "가중치 평균: " << averageWeight << endl;
    cout << "표준편차: " << standardDeviation << endl;
    cout << "최대 가중치를 가진 해: " << maxWeightPair.first << endl;
}


int main() {

    string inputFilename = "unweighted_50.txt";  // 해당 파일 읽기
    vector<Edge> graph = readGraphFromFile(inputFilename);
    genParentPool(graph);  //초기 부모풀 생성

    //제한 시간 설정
    int seconds = 10;
    chrono::time_point<chrono::system_clock> startTime = chrono::system_clock::now();
    parentPool = parentPool;
    childPool = childPool;


    //제한 시간동안 실행
    while (true) {
        chrono::time_point<chrono::system_clock> currentTime = chrono::system_clock::now();
        chrono::duration<double> elapsedSeconds = currentTime - startTime;

        //제한 시간 도달시 종료
        if (elapsedSeconds.count() >= seconds) {
            break;
        }

        //세대교체 반복 
        while (true) {
            genchildPool(graph); // 선택, 교배, 반복을 통해 자식풀 생성
            //replace(parentPool, childPool); // 세대 교체
            break;
        }   
    }
    
    calculateParentPoolStats(graph, parentPool);
    return 0;
}
