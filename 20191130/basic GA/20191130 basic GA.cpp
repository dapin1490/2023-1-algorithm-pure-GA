#include <iostream>
#include <vector>
#include <ctime>
#include <climits>
#include <random> // 균등 난수 참고: https://modoocode.com/304
#include <tuple> // 튜플: https://jjeongil.tistory.com/148
#include <map>
using namespace std;

/*
* 참고 자료
* 룰렛 휠 알고리즘 구현: https://blog.devkcr.org/entry/%EC%9C%A0%EC%A0%84-%EC%95%8C%EA%B3%A0%EB%A6%AC%EC%A6%98-%EA%B0%80%EC%A7%80%EA%B3%A0-%EB%86%80%EA%B8%B0 , http://www.aistudy.com/biology/genetic/operator_moon.htm
*/

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
	clock_t start_timestamp;
	Graph graph; // 문제 그래프
	/* 유전자 풀: 룰렛 휠 선택을 위해 카운팅 배열 방식으로 저장 */
	map<int, vector<vector<string>>> pool; // female[0], male[1]
	vector<tuple<int, int, string>> temp_pool; // 임시 자식 풀: 성별 번호[0, 1], cost, 유전자
	int thresh; // 부모 쌍 cost 차이 제한
	tuple<int, string> sol; // 반환할 해

private:
	// thresh 설정
	void set_thresh(int thr) { thresh = thr; };
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
	bool replacement(string chromosome, int cost, int gender);

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
	tuple<int, string> execute();
	tuple<int, string> get_solution() { return sol; };
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

	int v, e; // 정점 수 v, 간선 수 e
	unsigned from, to; // 출발점, 도착점
	int w; // 가중치
	Graph graph;

	cin >> v >> e;

	graph = Graph(v);

	for (int i = 0; i < e; i++) {
		cin >> from >> to >> w;
		graph.add_undir(Edge{ from, to, w });
	}

	//graph.print();

	GA agent = GA(graph, clock_start);

	cout << "\n\nGA::execute() 테스트\n";
	tuple<int, string> sol = agent.execute();
	sol = agent.get_solution();
	cout << "\nsolution: " << get<1>(sol) << "(" << get<0>(sol) << ")\n";

	clock_finish = clock();

	// clock_duration = (double)(clock_finish - clock_start) / CLOCKS_PER_SEC; // 초 단위로 환산
	clock_duration = (double)(clock_finish - clock_start);
	cout << "\n프로그램 실행 시간 : " << clock_duration << "ms\n";
	
	return 0;
}

string currentDateTime() { // 현재 시간 표기
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

void error(_error code, string message) { // 오류 코드와 메시지 출력 후 프로그램 종료
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

vector<Edge> Graph::edges_from(unsigned i) const { // 특정 정점에 연결된 간선들만 반환
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

void Graph::add(Edge&& e) { // 방향 간선 추가
	if (e.from > 0 && e.from <= this->v && e.to > 0 && e.to <= this->v)
		this->edges.push_back(e);
	else
		error(_error::shut_down, "정점 범위 초과");

	return;
}

void Graph::add_undir(Edge&& e) { // 무방향 간선 추가
	if (e.from > 0 && e.from <= this->v && e.to > 0 && e.to <= this->v) {
		this->edges.push_back(e);
		this->edges.push_back(Edge{e.to, e.from, e.w});
	}
	else
		error(_error::shut_down, "정점 범위 초과");

	return;
}

void Graph::print() { // 그래프 출력
	for (int i = 1; i <= v; i++) {
		cout << "# " << i << ": "; // 정점 번호
		vector<Edge> edge = this->edges_from(i); // 정점에 연결된 간선 가져오기
		for (auto& e : edge)
			cout << "(" << e.to << ", " << e.w << ")  "; // 정점에 연결된 간선 출력
		cout << "\n";
	}
	return;
}

int GA::validate(string chromosome) {
	/*
	* 유효한 해의 조건
	* 두 부류는 최소한 1개 이상의 노드를 가져야 한다. 어느 한 부류에 모든 노드가 포함될 수 없다.
	* 두 부류는 최소 1개 이상의 간선으로 서로 연결되어야 한다: 한쪽 부류에 있는 노드가 갖는 모든 간선 중 반대쪽 부류로 이어지는 게 하나라도 있으면 통과
	*/
	vector<int> a, b;
	int cost = 0;
	bool flag = false; // 두 부류가 최소 하나 이상의 간선으로 연결되었는지
	bool is_ok = false; // 두 부류 사이의 간선이 맞는지

	// 해의 길이는 그래프 노드 수와 같아야 함
	if (chromosome.length() != graph.size())
		return INT_MIN;

	// 두 부류 분리
	for (int i = 0; i < chromosome.length(); i++) {
		switch (chromosome.at(i)) {
		case 'A':
			a.push_back(i + 1);
			break;
		case 'B':
			b.push_back(i + 1);
			break;
		default: return INT_MIN; // 여기로 넘어온다면 해가 완전히 잘못 생성된 것
		}
	}

	// 두 부류는 최소한 1개 이상의 노드를 가져야 한다. 어느 한 부류에 모든 노드가 포함될 수 없다.
	if (a.size() == 0 || b.size() == 0)
		return INT_MIN;

	// 두 부류는 최소 1개 이상의 간선으로 서로 연결되어야 한다: 수가 더 적은 쪽의 노드를 전수조사하여 cost도 동시에 계산
	if (a.size() <= b.size()) {
		for (int& node : a) {
			vector<Edge> edges = graph.edges_from(node);
			for (Edge e : edges) {
				is_ok = find(a.begin(), a.end(), e.to) == a.end();
				if (flag && is_ok)
					cost += e.w;
				else if (!flag && is_ok) {
					flag = true;
					cost += e.w;
				}
			}
		}
	}
	else {
		for (int& node : b) {
			vector<Edge> edges = graph.edges_from(node);
			for (Edge e : edges) {
				is_ok = find(b.begin(), b.end(), e.to) == b.end();
				if (flag && is_ok)
					cost += e.w;
				else if (!flag && is_ok) {
					flag = true;
					cost += e.w;
				}
			}
		}
	}

	if (flag)
		return cost;
	else // 두 부류가 서로 연결되지 않았다면 무효한 해
		return INT_MIN;
}

string GA::generate() {
	/*
	* 랜덤 해 생성
	*/
	int len = graph.size(); // 해의 길이
	string chromosome = "";
	uniform_int_distribution<int> dis(0, 1); // 난수 생성 범위 지정

	for (int i = 0; i < len; i++) {
		switch (dis(this->gen)) {
		case 0: chromosome.push_back('A'); break;
		case 1: chromosome.push_back('B'); break;
		default: i--;
		}
	}

	return chromosome;
}

tuple<string, int, string, int> GA::selection() {
	tuple<string, int, string, int> parents; // 선택된 부모: female 먼저 선택 후 male 선택
	int n_candis = pow(2, uniform_int_distribution<int>(3, 5)(this->gen)); // 뽑을 후보의 수
	uniform_int_distribution<int> pick_cost(pool.begin()->first, (--pool.end())->first); // cost 뽑기
	uniform_int_distribution<int> pick_chromo(1, 10); // 둘 중 이긴 유전자 뽑기
	uniform_int_distribution<int> special_love(1, 1000); // cost 차이가 큰 쌍이 생성될 확률 0.1%
	int ca, cb, len;
	vector<int> candidates;
	int break_count = 0;
	bool break_flag = false; // 만약 female의 선택 범위에 male이 존재하지 않는다면 자가복제

	for (int i = 0; i < n_candis; i++) {
		while (true) {
			ca = pick_cost(this->gen);
			if (pool.find(ca) != pool.end() && pool[ca][0].size() != 0)
				break;
		}
		candidates.push_back(ca);
	}

	for (int i = 1; i < n_candis; i *= 2) {
		for (int j = 0; j < n_candis; j += 2 * i) {
			ca = (candidates[j] > candidates[j + i] ? candidates[j] : candidates[j + i]);
			cb = candidates[j] + candidates[j + i] - ca;
			candidates[j] = (pick_chromo(this->gen) >= 6 ? ca : cb);
		}
	}

	len = pool[candidates[0]][0].size();
	get<0>(parents) = pool[candidates[0]][0][uniform_int_distribution<int>(0, len - 1)(this->gen)];
	get<1>(parents) = candidates[0];

	if (special_love(this->gen) != 1) {
		pick_cost = uniform_int_distribution<int>(candidates[0], candidates[0] + thresh);
	}
	
	while (true) {
		cb = pick_cost(this->gen);
		if (pool.find(cb) != pool.end() && pool[cb][1].size() != 0)
			break;
		if (break_count > thresh * 2) {
			break_flag = true;
			break;
		}
		break_count++;
	}
	if (!break_flag) {
		len = pool[cb][1].size();
		get<2>(parents) = pool[cb][1][uniform_int_distribution<int>(0, len - 1)(this->gen)];
		get<3>(parents) = cb;
	}
	else {
		get<2>(parents) = get<0>(parents);
		get<3>(parents) = get<1>(parents);
	}

	return parents;
}

string GA::crossover(string female, string male) {
	string child = "";
	uniform_int_distribution<int> dis(0, 1); // 난수 생성 범위 지정
	for (int i = 0; i < graph.size(); i++) {
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

bool GA::replacement(string chromosome, int cost, int gender) {
	uniform_int_distribution<int> gen_cost(0, thresh + 3); // 자식과 교체 대상의 cost 차이 생성
	int r_cost;
	int break_count = 0;
	int s;

	while (true) { // 교체 대상의 cost 뽑기
		r_cost = max(cost - gen_cost(this->gen), 0);
		if (pool.find(r_cost) != pool.end() || break_count > 20)
			break;
		break_count++;
	}
	if (break_count > 20) // 20번을 뽑아도 대체할 cost가 없으면 대체하지 않고 패스
		return false;

	s = pool[r_cost][gender].size();

	if (s == 0)
		return false; // 교체 대상이 없어서 실패

	s = uniform_int_distribution<int>(0, s - 1)(this->gen); // 교체 대상의 인덱스 뽑기
	pool[r_cost][gender].erase(pool[r_cost][gender].begin() + s); // 교체 대상 삭제

	if (pool.find(cost) == pool.end()) {
		pool.insert({ cost, vector<vector<string>>(2) });
	}
	pool[cost][gender].push_back(chromosome); // 자식 추가
	return true; // 교체 성공
}

tuple<int, string> GA::execute() {
	/*
	* 랜덤 해 생성
	* 부모 선택
	* 교배
	* 돌연변이
	* 세대 교체
	* 일정 조건 후 종료
	*/
	/*
	* 초기 풀 생성 시간은 풀 크기에 따라 달라지고, 진화 시간은 종료 조건에 따라 다름.
	* 마지막 테스트 종료 조건: 대체 실패 비율 50% 이상
	* 파라미터 설정(노드, 간선, 초기 풀 개수): 초기 풀 생성 시간, 총 실행 시간, 실행 결과 cost
	* (10, 14, 500) 약 0.2초, 3초 미만, 8
	* (50, 123, 1000) 2초 미만, 11초 미만, 91
	* (100, 495, 1000) 3초 미만, 43초 미만, 336
	* (500, 5000, 1000) 30초 미만, 시간초과, 3452
	*/
	string res = ""; // 마지막에 반환할 결과
	int min_cost = INT_MAX; // 최소 cost
	int max_cost = INT_MIN; // 최대 cost
	int n_pool = min(1000, int(50 * this->graph.size()));
	int k = int(double(n_pool) * 0.1); // 한 세대 수
	uniform_int_distribution<int> gender(0, 1); // 성별 랜덤 지정
	clock_t running_time; // 실행 도중 경과 시간 기록
	double time_len; // 총 실행 시간 계산
	uniform_int_distribution<int> plz_add_me(1, 100); // 대체 대상이 없는 자식이 pool에 추가될 확률 2%
	bool is_child_added = false; // 자식이 pool에 추가되었는지
	int cut_count = 0;

	// 랜덤 해 생성
	cout << "generate\n";
	/*map<int, vector<vector<string>>> pool; // female[0], male[1]*/
	for (int i = 0; i < n_pool; i++) { // 두 성별 포함해 노드 수의 10배 개수만큼 생성
		for (int j = 0; j < 2; j++) {
			string chromosome = generate();
			int cost = validate(chromosome);
			if (cost != INT_MIN) {
				if (cost > max_cost) {
					max_cost = cost;
					res = chromosome;
				}
				if (cost < min_cost)
					min_cost = cost;
				if (pool.find(cost) == pool.end()) {
					pool.insert({ cost, vector<vector<string>>(2) });
				}
				pool[cost][j].push_back(chromosome);
			}
			else
				j--;
		}
		running_time = clock();
		time_len = (double)(running_time - start_timestamp) / CLOCKS_PER_SEC;
		if (time_len > 30) {
			cout << "time over\n";
			this->sol = make_tuple(max_cost, res);
			return this->sol;
		}
	}

	set_thresh(max(int((max_cost - min_cost) * 0.1), 2)); // 부모 쌍 cost 차이 제한

	// 부모 선택, 교배, 세대 교체
	while (true) { // 조건을 만족할 때까지 진화, 제한 시간 임박하면 종료
		// 현재 경과 시간 확인
		running_time = clock();
		time_len = (double)(running_time - start_timestamp) / CLOCKS_PER_SEC;
		cout << "current time len: " << time_len << "\n";
		if (time_len > 175) {
			cout << "time over\n";
			this->sol = make_tuple(max_cost, res);
			return this->sol;
		}
		// 임시 자식 풀 초기화
		temp_pool.clear();

		// 종료 조건 초기화
		cut_count = 0;

		// 자식 생성
		cout << "generate childs\n";
		for (int i = 0; i < k; i++) {
			// 부모 선택
			tuple<string, int, string, int> parent = selection();
			// 교배 및 유효성 확인
			string child = crossover(get<0>(parent), get<2>(parent));
			int child_cost = validate(child);
			if (child_cost != INT_MIN) {
				temp_pool.push_back(make_tuple(gender(this->gen), child_cost, child));
			}
			// thresh 예외 추가 자식
			if (abs(get<1>(parent) - get<3>(parent)) > thresh) {
				child = crossover(get<0>(parent), get<2>(parent));
				child_cost = validate(child);
				if (child_cost != INT_MIN) {
					temp_pool.push_back(make_tuple(gender(this->gen), child_cost, child));
				}
			}
		}

		// 세대 교체
		cout << "replace\n";
		for (auto& child : temp_pool) {
			is_child_added = replacement(get<2>(child), get<1>(child), get<0>(child));
			if (!is_child_added && plz_add_me(this->gen) <= 2) {
				if (pool.find(get<1>(child)) == pool.end()) {
					pool.insert({ get<1>(child), vector<vector<string>>(2) });
				}
				pool[get<1>(child)][get<0>(child)].push_back(get<2>(child));
				is_child_added = true;
			}
			if (is_child_added && get<1>(child) > max_cost) {
				max_cost = get<1>(child);
				res = get<2>(child);
				this->sol = make_tuple(max_cost, res);
			}
			else if (!is_child_added)
				cut_count++;
		}

		if (cut_count > int(double(k) * 0.5)) { // 생성된 자식의 50% 이상이 대체되지 못했다면 진화 수렴 판단
			cout << "evolution complete\n";
			break;
		}
	}

	cout << "return solution\n";
	this->sol = make_tuple(max_cost, res);
	return this->sol;
}