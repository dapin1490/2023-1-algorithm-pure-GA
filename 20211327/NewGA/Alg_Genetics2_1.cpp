#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <numeric>
#include <algorithm>

using namespace std;
vector <pair<int, int>> adj[10001];

//Make Graph
void set_vertice(int _s, int _a, int w)
{
    adj[_s].push_back(make_pair(_a, w));
    adj[_a].push_back(make_pair(_s, w));
}

//Return weight
int get_weight(int _s, int _a) {
    for (auto& p : adj[_s]) {
        if (p.first == _a) {
            return p.second;  // return the weight of the edge
        }
    }
    return 0;  // return -1 if the edge is not found
}

//Return vector that contain destination nodes
vector<int> get_des(int _s) {
    vector<int> vertices;
    for (auto& edge : adj[_s]) {
        vertices.push_back(edge.first);
    }
    return vertices;
}

//Generate chromosomes randomly
vector <int> randomInt(int N) {
    int p = rand() % N; // V °¹¼ö(Number of Nodes in a Set)
    vector <int> indexes(N);
    vector <int> Chrom(N);

    for (int i = 0; i < N; i++) {
        indexes[i] = i;
        Chrom[i] = 0;
    }
    int j = N - 1;
    int p_2 = p;
    while (p_2 > 0) { //Binary integer °ª 0 -> 1
        int selec = rand() % (j + 1);
        swap(indexes[selec], indexes[j]);
        j = j - 1;
        p_2 = p_2 - 1;
    }
    int k = N - 1;
    while (p > 0)
    {
        Chrom[indexes[k]] = 1;
        p = p - 1;
        k = k - 1;
    }
    return Chrom;
}
//Cutsize
//Function that evaluates whether node n exists in the adjacent list
bool Checkadj(int _s, int n)
{
    //Vector that contain destination nodes from the starting node _s
    vector <int> ans = get_des(_s);
    //Iterate each destination node and compare it with n(Node I want to compare with)
    for (int m = 0; m < ans.size(); m++)
    {
        if (ans[m] == n)
            //If node n exists, return 1
        {
            return true;
        }
    }
    return false;
}
//Cutsize function
int CutSize(vector <int> G)
{
    vector <pair<int, int>> Edge;
    int sum = 0;
    //Pairs of vertices that are in different groups
    for (int i = 0; i < G.size(); i++) {
        for (int j = i + 1; j < G.size(); j++)
        {
            if (G[i] != G[j])
            {
                Edge.push_back(make_pair(i + 1, j + 1));
            }
        }
    }
    //Find out whether the pairs of nodes really exist as edges (using adjacent list)and then delete edges that doesn't exist at all
    int cutsize = 0;
    //Iterate the pairs of vertices and if the pair exists as edges, increase the value of cutsize
    for (int l = 0; l < Edge.size(); l++)
    {
        bool ex = Checkadj(Edge[l].first, Edge[l].second);
        if (ex == true)
        {
            cutsize = cutsize + get_weight(Edge[l].first, Edge[l].second);
        }
    }
    return cutsize;
}
//Fitness probability
vector <int> Fitness(int cw, int cb, int k, vector<int>cs)
{
    vector <int> Fit;
    k = 3;
    for (auto iter = cs.begin(); iter != cs.end(); iter++)
    {
        int f = abs(cw - *iter) + abs((cw - cb) / (k - 1));
        Fit.push_back(f);
    }
    return Fit;
}

//Sort the Fitness vector
bool SortV(const std::pair<int, int>& a, const std::pair<int, int>& b)
{
    return a.second < b.second;
}

//Roulette Wheel Function
int Roulette(std::vector<std::pair<int, int>> sortedFit, int SumOfFitness) {
    int point = rand() % SumOfFitness; // Generate a random point
    int sum = 0;
    for (int i = 0; i < sortedFit.size(); ++i)
    {
        sum += sortedFit[i].second;//Cumulative Fitness
        if (point < sum) {
            return sortedFit[i].first; // Select this Chromosome as a parent 
        }
    }
    cout << "Error: parent not selected" << endl;
    return -1;
}

//CrossOver (1 point)
vector<int> Crossover(vector<int>& parent1, vector<int>& parent2) {
    int n = parent1.size();
    vector<int> Offspring(n);
    //CrossOver point
    int p = rand() % n;
    // Divide parent1 into two parts
    vector<int> part1(parent1.begin(), parent1.begin() + p);
    vector<int> part2(parent1.begin() + p, parent1.end());
    // Divide parent2 into two parts
    vector<int> part3(parent2.begin(), parent2.begin() + p);
    vector<int> part4(parent2.begin() + p, parent2.end());
    // Combine the parts to form the child
    copy(part1.begin(), part1.end(), Offspring.begin());
    copy(part4.begin(), part4.end(), Offspring.begin() + p);
    return Offspring;
}

//Mutation (Flip one bit)
vector<int> Mutate(vector<int> offspring, double mutation_rate) {
    int mp = rand() % offspring.size();
    if ((double)rand() / RAND_MAX < mutation_rate) // Mutate under a probabiltiy of mutation_rate
    {
        offspring[mp] = 1 - offspring[mp]; // Mutate by flipping the bit
    }
    return offspring;
}


//LocalOptimum
vector<int> LocalOptimum(vector<int> offspring) {
    int lp = rand() % offspring.size();
    offspring[lp] = 1 - offspring[lp]; // Mutate by flipping the bit
    return offspring;
}



//Replace
//Offspring and costsize of the old generation is inserted in the Replace function => The chromosome needed to be replaced or if the replacement is unnecessary, the function returns -1 instead of index.  
int Replace(vector<int> offspring, vector <int> cs)
{
    int index;
    int parent_min = *min_element(cs.begin(), cs.end());
    //Parent min cost < Offspring cost
    if (parent_min < CutSize(offspring))
    {
        auto it = std::find(cs.begin(), cs.end(), parent_min);
        index = std::distance(cs.begin(), it);
    }
    //Parent min cost > Offspring cost => Not Replaced
    else
    {
        index = -1;
    }
    return index;
}

//Report Best Solution
vector <int> Bsolution(vector <int> Cost, vector<vector<int>> sol)
{
    int maxIndex = 0;
    // Find the iterator pointing the biggest element
    auto maxIt = std::max_element(Cost.begin(), Cost.end());
    // Check if the vector is not empty and the max element was found
    if (maxIt != Cost.end()) {
        // Index of the maximum element
        maxIndex = std::distance(Cost.begin(), maxIt);
    }
    return sol[maxIndex];
}

//S set in Best Solution Chromosome 
vector <int> S(vector<int>Bchromosome)
{
    vector <int> S;
    vector <int> V_S;
    for (int p = 0; p < Bchromosome.size(); p++)
    {
        if (Bchromosome[p] == 0)
        {
            S.push_back(p + 1);
        }
        else
        {
            V_S.push_back(p + 1);
        }
    }
    if (S.size() > V_S.size())
    {
        return V_S;
    }
    else if (S.size() <= V_S.size())
    {
        return S;
    }
}

//CSV File
void printBestSolution(const std::vector<int>& cost, const std::vector<std::vector<int>>& sol, int generationNumber, std::ofstream& csvFile) {
    int bestCost = *std::max_element(cost.begin(), cost.end());
    std::vector<int> bestSolution = sol[std::distance(cost.begin(), std::max_element(cost.begin(), cost.end()))];

    csvFile << generationNumber << ",";
    for (int n = 0; n < bestSolution.size(); n++) {
        csvFile << bestSolution[n];
        if (n < bestSolution.size() - 1) {
            csvFile << " ";
        }
    }
    csvFile << "," << bestCost << std::endl;
}

int main()
{
    srand(time(NULL));
    //Make adjacent List
    ifstream inputfile("unweighted_50.txt");
    int N, M;
    inputfile >> N >> M;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < 3; j++) {
            int _s, _a, w;
            inputfile >> _s >> _a >> w;
            set_vertice(_s, _a, w);
        }
    }

    //Generate initial solutions(p solutions)
   //Ex.p=5
    vector<vector<int>> sol;
    for (int k = 0; k < 3 * N; k++)
    {
        vector<int> Chroms = randomInt(N);
        sol.push_back(Chroms);
    }


    //Calculate cutsize(cost)
    vector<int>Cost; //Vector that saves cost of chromosomes sol[0]ÀÇ cost´Â Cost[0]
    for (int i = 0; i < sol.size(); i++) {
        vector<int> a = sol[i];
        Cost.push_back(CutSize(a));
    }
    int cw = *min_element(Cost.begin(), Cost.end());
    int cb = *max_element(Cost.begin(), Cost.end());

    //Calculate Fitness and save it in a Vector
    vector<int> Fit = Fitness(cw, cb, 3, Cost);

    //Roulette Wheel Function and select parents
    //Sum of Fitness
    int Ft_sum = accumulate(Fit.begin(), Fit.end(), 0);

    //In order to implement Roulette function, Fitness need to be sorted in an ascending order
    std::vector<std::pair<int, int>> sortedFit;
    for (int i = 0; i < Fit.size(); i++) {
        sortedFit.push_back(std::make_pair(i, Fit[i]));
    }
    //Unsorted Fitness

    //Sorted Fitness
    std::sort(sortedFit.begin(), sortedFit.end(), SortV);

    int t = 70;
    std::vector<std::pair<int, int>> sortedFit_ = sortedFit;
    int Ft_sum_ = Ft_sum;
    vector<vector<int>> sol_ = sol;
    vector<int>Cost_ = Cost;
    vector<int> Fit_ = Fit;
    while (t > 0)
    {
        int p1, p2;
        p1 = Roulette(sortedFit_, Ft_sum_);
        p2 = Roulette(sortedFit_, Ft_sum_);
        vector<int> Parent1 = sol[p1];
        vector<int> Parent2 = sol[p2];

        //CrossOver 
        vector<int> Offspring(N);
        Offspring = Crossover(Parent1, Parent2);

        //Mutation
        Offspring = Mutate(Offspring, 0.01);

        //LocalOptimum
        vector<int> Offspring2(N);
        Offspring2 = LocalOptimum(Offspring2);
        if (CutSize(Offspring2) > CutSize(Offspring))
        {
            Offspring = Offspring2;
        }

        //Replace
        //New Generation is decided
        int ReplaceRes = Replace(Offspring, Cost_);
        //If the New Generation needs to be formed => the old chromosome is deleted and instead in the same index, offspring is replaced, creating a new generation
        //If Replace Res is a minus, it means the generation can stay
        if (ReplaceRes >= 0)
        {
            //Cost update
            Cost_.erase(Cost_.begin() + ReplaceRes);
            Cost_.insert(Cost_.begin() + ReplaceRes, CutSize(Offspring));
            //New Generation Chromosome
            std::vector <int> ReplChrom = sol_[ReplaceRes];
            sol_[ReplaceRes] = Offspring;
        }
        //Best solution for this generation
        //cout << "Generation number" << 71-t << endl;
        //cout << "This Generation's Best Solution" << endl;
        //int Bcost = *max_element(Cost_.begin(), Cost_.begin());
        //vector<int>Bsol = Bsolution(Cost_, sol_);
        //for (int n = 0; n < Bsol.size(); n++) {
        //    cout << Bsol[n];
        //}
        //cout << endl;
        //cout << "Best Cost: " << Bcost << endl;
        std::ofstream csvFile("best_solutions.csv", std::ios::app);
        bool isFirstRow = true;
        printBestSolution(Cost_, sol_, 71 - t, csvFile);
        csvFile.close();

        //Preparation for Next Generation
        int cw_ = *min_element(Cost_.begin(), Cost_.end());
        int cb_ = *max_element(Cost_.begin(), Cost_.end());

        //Calculate Fitness and save it in a Vector
        Fit_ = Fitness(cw, cb, 3, Cost);


        //Roulette Wheel Function and select parents
        //Sum of Fitness
        Ft_sum_ = accumulate(Fit_.begin(), Fit_.end(), 0);

        //In order to implement Roulette function, Fitness need to be sorted in an ascending order
        for (int i = 0; i < Fit_.size(); i++) {
            sortedFit_.push_back(std::make_pair(i, Fit_[i]));
        }

        t--;
    }

    //Report Best Solution
    vector<int>Bchromosome = Bsolution(Cost_, sol_);
    cout << "Final Best Solution" << endl;
    for (int n = 0; n < Bchromosome.size(); n++) {
        cout << Bchromosome[n];
    }
    cout << "\n";
    cout << "Answer S" << endl;
    vector <int> Set = S(Bchromosome);
    for (int r = 0; r < Set.size(); r++) {
        cout << Set[r] << " ";
    }


}