#include <iostream>
#include <vector>
#include <ctime>
#include <climits>
#include <random> // 균등 난수 참고: https://modoocode.com/304
#include <tuple> // 튜플: https://jjeongil.tistory.com/148
#include <map>
#include <set>
#include <string>
using namespace std;

// 현재 시간 표기
string currentDateTime();

// 오류 코드
enum class _error : int { shut_down, ValueErrorInt, ValueErrorChar, UnknownError };

// C++ 에러 메시지 참고 : https://learn.microsoft.com/ko-kr/cpp/error-messages/compiler-errors-1/c-cpp-build-errors?view=msvc-170
// 오류 코드와 메시지 출력 후 프로그램 종료
void error(_error code, string message = "");

struct Edge {
	unsigned from; // 시작점
	unsigned to; // 종점
	int w; // 가중치
};

class Graph {
private:
	unsigned v; // 정점 수
	vector<Edge> edges; // 그래프가 갖는 간선들

public:
	// 생성자
	Graph() { this->v = 0; }
	Graph(unsigned v) { this->v = v; }

	// 함수 정의에 쓰인 const : 이 함수 안에서 쓰는 값들을 변경할 수 없다
	// 그래프가 갖는 정점의 수를 반환
	unsigned size() const { return v; }
	// 그래프가 갖는 간선들을 반환
	vector<Edge> edges_from() const { return edges; }
	// 특정 정점에 연결된 간선들만 반환
	vector<Edge> edges_from(unsigned i) const;

	// 방향 간선 추가
	void add(Edge&& e);
	// 무방향 간선 추가
	void add_undir(Edge&& e);

	// 그래프 출력
	void print();
};

class GA {
	/*
	* 해결해야 할 문제
	* 주어진 그래프를 두 부류로 나누고, 각 부류를 연결하는 간선의 합이 최대가 되게 하라.
	*/
private:
	mt19937 gen; // 난수 생성기
	clock_t start_timestamp; // 프로그램 시작 시간
	Graph graph; // 문제 그래프
	/* 유전자 풀: 가중치에 따른 선택을 위해 카운팅 배열 방식으로 저장 */
	map<int, vector<string>> pool; // 가중치, 해
	vector<tuple<int, string>> temp_pool; // 임시 자식 풀: cost, 유전자
	int thresh; // 부모 쌍 cost 차이 제한
	tuple<int, string> sol; // 반환할 해

private:
	// thresh 설정
	void set_thresh(int thr) { thresh = thr; };
	// 시간 초과 확인
	bool is_timeout(int deadline, bool is_print = false);
	// 현재 pool에서 가장 좋은 해 반환
	tuple<int, string> get_current_best() {
		this->sol = make_tuple((--pool.end())->first, (--pool.end())->second[0]);
		return sol;
	}
	// 해 유효성 확인 및 cost 계산
	int validate(string chromosome);
	// 해 생성
	string generate();
	// 부모 쌍 선택: 토너먼트 이용
	tuple<string, int, string, int> selection();
	// 교배
	string crossover(string female, string male);
	// 돌연변이
	string mutation(string chromosome);
	// 세대 교체
	bool replacement(string chromosome, int cost);

public:
	GA() {
		graph = Graph();
		random_device rd;
		mt19937 g(rd());
		this->gen = g;
		start_timestamp = clock();
	}
	GA(Graph graph) {
		this->graph = graph;
		random_device rd;
		mt19937 g(rd());
		this->gen = g;
		start_timestamp = clock();
	}
	GA(Graph graph, mt19937 gen) {
		this->graph = graph;
		this->gen = gen;
		start_timestamp = clock();
	}
	GA(Graph graph, clock_t start) {
		this->graph = graph;
		random_device rd;
		mt19937 g(rd());
		this->gen = g;
		start_timestamp = start;
	}
	GA(Graph graph, mt19937 gen, clock_t start) {
		this->graph = graph;
		this->gen = gen;
		start_timestamp = start;
	}

	// 유전 알고리즘 실행
	tuple<int, string> execute(int due = 30);
	// 해 문자열과 가중치 반환
	tuple<int, string> get_solution();
	// 정답 반환
	string to_string_solution();
};

int main()
{
	// 빠른 입출력
	ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

	// 코드 실행 시간 측정 : https://scarlettb.tistory.com/5
	clock_t clock_start, clock_finish;
	double clock_duration;

	cout << "main.cpp 실행 시각 : " << currentDateTime() << "\n\n";

	clock_start = clock();

	// 프로그램 실행 시작
	int v, e; // 정점 수 v, 간선 수 e
	unsigned from, to; // 출발점, 도착점
	int w; // 가중치
	Graph graph;

	cin >> v >> e; // 그래프 정보 입력

	graph = Graph(v); // 그래프 생성

	// 그래프 노드 입력
	for (int i = 0; i < e; i++) {
		cin >> from >> to >> w;
		graph.add_undir(Edge{ from, to, w });
	}

	// 그래프 출력
	//graph.print();

	// 유전 알고리즘 실행자 생성
	GA agent = GA(graph, clock_start);

	// 유전 알고리즘 실행 후 결과 출력
	// cout << "\n\nGA::execute() 테스트\n";
	tuple<int, string> sol = agent.execute(10);
	sol = agent.get_solution();
	cout << "\nsolution: " << get<1>(sol) << "(" << get<0>(sol) << ")\n";
	cout << "\nanswer: " << agent.to_string_solution() << "\n";

	// 종료 시간 측정
	clock_finish = clock();

	clock_duration = (double)(clock_finish - clock_start) / CLOCKS_PER_SEC; // 초 단위로 환산
	// clock_duration = (double)(clock_finish - clock_start); // ms 단위로 계산
	cout << "\n프로그램 실행 시간 : " << clock_duration << "s\n";

	return 0;
}

// 현재 시간 표기
string currentDateTime() {
	time_t t = time(nullptr);
	tm now;
	errno_t is_error = localtime_s(&now, &t);

	if (is_error == 0) {
		char buffer[256];
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", &now);
		return buffer;
	}
	else {
		return "현재 시간을 얻을 수 없음";
	}
}

// 오류 코드와 메시지 출력 후 프로그램 종료
void error(_error code, string message) {
	if (!message.empty())
		cout << "error: " << message << "\n";

	switch (code) {
	case _error::shut_down:
		cout << "프로그램 비정상 종료\n";
		break;
	case _error::ValueErrorInt: // 잘못된 입력 - int
		cout << "ValueErrorInt: int 값이 입력되어야 합니다.\n";
		break;
	case _error::ValueErrorChar: // 잘못된 입력 - char
		cout << "ValueErrorChar: char 값이 입력되어야 합니다.\n";
		break;
	default:
		cout << "UnknownError: 알 수 없는 오류\n";
	}

	exit(1); // 프로그램 비정상 종료
}

// 특정 정점에 연결된 간선들만 반환
vector<Edge> Graph::edges_from(unsigned i) const {
	vector<Edge> edge_from_i;
	for (auto& e : edges) {
		if (e.from == i)
			edge_from_i.push_back(e);
	}
	/* // 이쪽 코드도 똑같은 기능을 함
	for (int idx = 0; idx < this->edges.size(); idx++) {
		if (this->edges[idx].from == i)
			edge_from_i.push_back(edges[idx]);
	}*/
	return edge_from_i;
}

// 방향 간선 추가
void Graph::add(Edge&& e) {
	if (e.from > 0 && e.from <= this->v && e.to > 0 && e.to <= this->v)
		this->edges.push_back(e);
	else
		error(_error::shut_down, "정점 범위 초과");

	return;
}

// 무방향 간선 추가
void Graph::add_undir(Edge&& e) {
	if (e.from > 0 && e.from <= this->v && e.to > 0 && e.to <= this->v) {
		this->edges.push_back(e);
		this->edges.push_back(Edge{e.to, e.from, e.w});
	}
	else
		error(_error::shut_down, "정점 범위 초과");

	return;
}

// 그래프 출력
void Graph::print() {
	for (int i = 1; i <= v; i++) {
		cout << "# " << i << ": "; // 정점 번호
		vector<Edge> edge = this->edges_from(i); // 정점에 연결된 간선 가져오기
		for (auto& e : edge)
			cout << "(" << e.to << ", " << e.w << ")  "; // 정점에 연결된 간선 출력
		cout << "\n";
	}
	return;
}

// 제한 시간 초과 확인
bool GA::is_timeout(int deadline, bool is_print) {
	clock_t running_time = clock();
	double time_len = (double)(running_time - start_timestamp) / CLOCKS_PER_SEC;
	if (is_print)
		cout << "current time: " << time_len << "\n";
	if (time_len > deadline) {
		if (is_print)
			cout << "time over\n";
		return true;
	}
	return false;
}

// 해 유효성 검사 및 가중치 계산
int GA::validate(string chromosome) {
	/*
	* 유효한 해의 조건
	* 두 부류는 최소한 1개 이상의 노드를 가져야 한다. 어느 한 부류에 모든 노드가 포함될 수 없다.
	* 두 부류는 최소 1개 이상의 간선으로 서로 연결되어야 한다: 한쪽 부류에 있는 노드가 갖는 모든 간선 중 반대쪽 부류로 이어지는 게 하나라도 있으면 통과
	*/
	set<int> a, b; // 두 부류의 노드 모음
	set<int>& key_pool = a; // 가중치 계산할 때 키가 되는 노드
	int cost = 0; // 계산된 가중치
	bool flag = false; // 두 부류가 최소 하나 이상의 간선으로 연결되었는지
	bool is_ok = false; // 두 부류 사이의 간선이 맞는지

	// 해의 길이는 그래프 노드 수와 같아야 함
	if (chromosome.length() != graph.size())
		return INT_MIN;

	// 두 부류 분리
	for (int i = 0; i < chromosome.length(); i++) {
		switch (chromosome.at(i)) {
		case 'A':
			a.insert(i + 1);
			break;
		case 'B':
			b.insert(i + 1);
			break;
		default: return INT_MIN; // 여기로 넘어온다면 해가 완전히 잘못 생성된 것
		}
	}

	// 두 부류는 최소한 1개 이상의 노드를 가져야 한다. 어느 한 부류에 모든 노드가 포함될 수 없다.
	if (a.size() == 0 || b.size() == 0)
		return INT_MIN;

	// 두 부류는 최소 1개 이상의 간선으로 서로 연결되어야 한다: 수가 더 적은 쪽의 노드를 전수조사하여 cost도 동시에 계산
	if (a.size() > b.size()) {
		key_pool = b;
	}
	for (const int& node : key_pool) { // 가중치 계산 키 pool에서 노드별 반복
		vector<Edge> edges = graph.edges_from(node); // 노드에 연결된 모든 간선 불러오기
		for (Edge e : edges) { // 각 간선마다
			is_ok = find(key_pool.begin(), key_pool.end(), e.to) == key_pool.end(); // 도착점이 같은 pool 내에 존재하지 않으면 OK
			if (flag && is_ok)
				cost += e.w;
			else if (!flag && is_ok) {
				flag = true; // 정상 해 flag
				cost += e.w;
			}
		}
	}

	if (flag)
		return cost;
	else // 두 부류가 서로 연결되지 않았다면 무효한 해
		return INT_MIN;
}

// 랜덤 해 생성
string GA::generate() {
	int len = graph.size(); // 해의 길이
	string chromosome = ""; // 생성될 해
	uniform_int_distribution<int> dis(0, 1); // 난수 생성 범위 지정

	for (int i = 0; i < len; i++) { // 50% 확률로 두 문자 중 하나 선택하여 추가
		switch (dis(this->gen)) {
		case 0: chromosome.push_back('A'); break;
		case 1: chromosome.push_back('B'); break;
		default: i--;
		}
	}

	return chromosome;
}

// 부모 선택
tuple<string, int, string, int> GA::selection() {
	/*
	* 부모 선택 과정
	* 아래 과정을 2번 반복
		* 전체 pool에서 랜덤한 수(2의 거듭제곱)의 cost 뽑기: 뽑힌 cost에 해당하는 해가 최소 1개 존재해야 함
		* 뽑힌 cost끼리 토너먼트
		* 최종 승자 cost에 해당하는 해 랜덤으로 뽑기 -> parent
	*/
	tuple<string, int, string, int> parents; // 선택된 부모: female 먼저 선택 후 male 선택
	int n_candis = pow(2, uniform_int_distribution<int>(3, 5)(this->gen)); // 뽑을 후보의 수
	uniform_int_distribution<int> pick_cost(pool.begin()->first, (--pool.end())->first); // cost 뽑기
	uniform_int_distribution<int> pick_chromo(1, 10); // 둘 중 이긴 유전자 뽑기
	int ca, cb, len;
	vector<int> candidates; // 토너먼트에 참가할 cost 후보

	// female 후보 뽑기: 2^3 ~ 2^5개 사이, 중복은 고려하지 않음
	for (int i = 0; i < n_candis; i++) {
		while (true) { // 유효한 후보가 나올 때까지 뽑기
			ca = pick_cost(this->gen); // cost 선택: 존재하는 모든 cost 중 뽑기 때문에 무조건 하나는 있음
			if (pool.find(ca) != pool.end() && pool[ca].size() != 0) // 선택한 cost를 갖는 해가 있는지 확인
				break;
		}
		candidates.push_back(ca); // 후보 추가
	}

	// 뽑힌 cost로 토너먼트: 승자를 왼쪽에 저장, 최종 승자는 0번에 저장됨
	for (int i = 1; i < n_candis; i *= 2) {
		for (int j = 0; j < n_candis; j += 2 * i) {
			ca = (candidates[j] > candidates[j + i] ? candidates[j] : candidates[j + i]);
			cb = candidates[j] + candidates[j + i] - ca;
			candidates[j] = (pick_chromo(this->gen) >= 6 ? ca : cb);
		}
	}

	// 최종 승자 cost를 갖는 해 중에서 랜덤하게 female 선택
	len = pool[candidates[0]].size(); // 후보 수: 최소 하나 이상 있는 것만 후보로 넣었기 때문에 무조건 있음
	get<0>(parents) = pool[candidates[0]][uniform_int_distribution<int>(0, len - 1)(this->gen)]; // 뽑기
	get<1>(parents) = candidates[0]; // 뽑힌 female의 가중치

	candidates.clear();

	// male 후보 뽑기: 2^3 ~ 2^5개 사이, 중복은 고려하지 않음
	for (int i = 0; i < n_candis; i++) {
		while (true) { // 유효한 후보가 나올 때까지 뽑기
			cb = pick_cost(this->gen); // cost 선택: 존재하는 모든 cost 중 뽑기 때문에 무조건 하나는 있음
			if (pool.find(cb) != pool.end() && pool[cb].size() != 0) // 선택한 cost를 갖는 해가 있는지 확인
				break;
		}
		candidates.push_back(cb); // 후보 추가
	}

	// 뽑힌 cost로 토너먼트: 승자를 왼쪽에 저장, 최종 승자는 0번에 저장됨
	for (int i = 1; i < n_candis; i *= 2) {
		for (int j = 0; j < n_candis; j += 2 * i) {
			ca = (candidates[j] > candidates[j + i] ? candidates[j] : candidates[j + i]);
			cb = candidates[j] + candidates[j + i] - ca;
			candidates[j] = (pick_chromo(this->gen) >= 6 ? ca : cb);
		}
	}

	// 최종 승자 cost를 갖는 해 중에서 랜덤하게 male 선택
	len = pool[candidates[0]].size(); // 후보 수: 최소 하나 이상 있는 것만 후보로 넣었기 때문에 무조건 있음
	get<2>(parents) = pool[candidates[0]][uniform_int_distribution<int>(0, len - 1)(this->gen)]; // 뽑기
	get<3>(parents) = candidates[0]; // 뽑힌 male의 가중치

	return parents;
}

// 교배
string GA::crossover(string female, string male) {
	string child = ""; // 생성될 자식
	uniform_int_distribution<int> dis(0, 1); // 난수 생성 범위 지정
	for (int i = 0; i < graph.size(); i++) { // 50% 확률로 부모 둘 중 한 쪽의 문자를 선택해 받음
		switch (dis(this->gen)) {
		case 0:
			child.push_back(female.at(i));
			break;
		default:
		case 1:
			child.push_back(male.at(i));
		}
	}
	return child;
}

// 돌연변이
string GA::mutation(string chromosome) {
	uniform_int_distribution<int> is_mutate(1, 200 * this->graph.size()); // 돌연변이 발생 확률 조절
	uniform_int_distribution<int> choose(0, 1); // 돌연변이 발생시 문자 재선택: 돌연변이가 발생해도 원본과 똑같을 수 있음
	for (int i = 0; i < chromosome.length(); i++) {
		switch (is_mutate(this->gen) <= 3) {
		case false:
			continue;
		case true:
			chromosome.replace(i, 1, (choose(this->gen) == 0 ? "A" : "B"));
		}
	}
	return chromosome;
}

// 세대 교체
bool GA::replacement(string chromosome, int cost) {
	uniform_int_distribution<int> gen_cost(0, thresh + 3); // 자식과 교체 대상의 cost 차이 생성
	int r_cost; // 교체 대상의 cost
	int break_count = 0;
	int s; // 교체 대상 해의 수

	while (true) { // 교체 대상의 cost 뽑기: 유효한 cost가 나오거나 포기할 때까지 반복
		r_cost = max(cost - gen_cost(this->gen), 0);
		if ((pool.find(r_cost) != pool.end() && pool[r_cost].size() != 0) || break_count > 20)
			break;
		break_count++;
	}
	if (break_count > 20) // 20번을 뽑아도 대체할 cost가 없으면 대체하지 않고 패스
		return false;

	s = pool[r_cost].size();

	s = uniform_int_distribution<int>(0, s - 1)(this->gen); // 교체 대상의 인덱스 뽑기
	pool[r_cost].erase(pool[r_cost].begin() + s); // 교체 대상 삭제

	if (pool.find(cost) == pool.end()) { // 추가할 자식의 cost가 pool에 없으면 추가
		pool.insert({ cost, vector<string>() });
	}
	pool[cost].push_back(chromosome); // 자식 추가
	return true; // 교체 성공
}

// 유전 알고리즘 실행
tuple<int, string> GA::execute(int due) { // due: 프로그램 실행 마감시간
	/*
	* 랜덤 해 생성
	* 부모 선택
	* 교배
	* 돌연변이
	* 세대 교체
	* 일정 조건 후 종료
	*/
	string res = ""; // 마지막에 반환할 결과
	int n_pool = min(1000, int(50 * this->graph.size())); // 초기 생성 pool 크기
	int k = int(double(n_pool) * 0.1); // 한 세대 수
	uniform_int_distribution<int> plz_add_me(1, 100); // 대체 대상이 없는 자식이 pool에 추가될 확률 2%
	bool is_child_added = false; // 자식이 pool에 추가되었는지
	int cut_count = 0; // 대체 실패한 자식 수

	// 랜덤 해 생성
	// cout << "generate\n";
	for (int i = 0; i < 2 * n_pool; i++) { // 2 * n_pool 만큼 생성
		string chromosome = generate();
		int cost = validate(chromosome);
		if (cost != INT_MIN) { // 유효한 해만 pool에 추가
			if (pool.find(cost) == pool.end()) {
				pool.insert({ cost, vector<string>() });
			}
			pool[cost].push_back(chromosome);
		}
		else
			i--;
		if (is_timeout(due, false)) {
			return get_current_best();
		}
	}

	// cout << "generate complete\n";
	if (is_timeout(due)) {
		return get_current_best();
	}

	set_thresh(max(int(((--pool.end())->first - pool.begin()->first) * 0.1), 2)); // 자식 교체 대상 cost 차이 제한

	// 부모 선택, 교배, 세대 교체
	while (true) { // 조건을 만족할 때까지 진화, 제한 시간 임박하면 종료
		// 시간 제한 확인
		if (is_timeout(due)) {
			return get_current_best();
		}

		// 임시 자식 풀 초기화
		temp_pool.clear();

		// 종료 조건 초기화
		cut_count = 0;

		// 자식 생성
		// cout << "generate children\n";
		for (int i = 0; i < k; i++) {
			// 부모 선택
			tuple<string, int, string, int> parent = selection();
			// 교배 및 유효성 확인
			string child = crossover(get<0>(parent), get<2>(parent));
			int child_cost = validate(child);
			if (child_cost != INT_MIN) {
				temp_pool.push_back(make_tuple(child_cost, child));
			}
		}
		// 시간 제한 확인
		// cout << "children generation complete\n";
		if (is_timeout(due)) {
			return get_current_best();
		}

		// 세대 교체
		// cout << "replace\n";
		for (auto& child : temp_pool) {
			is_child_added = replacement(get<1>(child), get<0>(child));
			if (!is_child_added && plz_add_me(this->gen) <= 2) {
				if (pool.find(get<0>(child)) == pool.end()) {
					pool.insert({ get<0>(child), vector<string>() });
				}
				pool[get<0>(child)].push_back(get<1>(child));
				is_child_added = true;
			}
			else if (!is_child_added)
				cut_count++;
		}
		// 시간 제한 확인
		// cout << "children replace complete\n";
		if (is_timeout(due)) {
			return get_current_best();
		}

		if (cut_count > int(double(k) * 0.5)) { // 생성된 자식의 50% 이상이 대체되지 못했다면 진화 수렴 판단
			// cout << "evolution complete\n";
			break;
		}
	}

	// cout << "return solution\n";
	return get_current_best();
}

tuple<int, string> GA::get_solution() {
	return sol;
};

string GA::to_string_solution() {
	char key = get<1>(sol).at(0);
	string answer = "";

	for (int i = 1; i <= get<1>(sol).length(); i++) {
		if (get<1>(sol).at(i - 1) == key) {
			answer.append(to_string(i));
			answer.push_back(' ');
		}
	}

	return answer;
}