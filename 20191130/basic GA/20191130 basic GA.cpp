#include <iostream>
#include <vector>
#include <ctime>
using namespace std;

string currentDateTime(); // 현재 시간 표기

enum class _error : int { shut_down, ValueErrorInt, ValueErrorChar, UnknownError };

// C++ 에러 메시지 참고 : https://learn.microsoft.com/ko-kr/cpp/error-messages/compiler-errors-1/c-cpp-build-errors?view=msvc-170
void error(_error code, string message = ""); // 오류 코드와 메시지 출력 후 프로그램 종료

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
	unsigned size() const { return v; } // 그래프가 갖는 정점의 수를 반환
	vector<Edge> edges_from() const { return edges; } // 그래프가 갖는 간선들을 반환
	vector<Edge> edges_from(unsigned i) const; // 특정 정점에 연결된 간선들만 반환

	void add(Edge&& e); // 방향 간선 추가
	void add_undir(Edge&& e); // 무방향 간선 추가

	void print(); // 그래프 출력
};

class GA {
private:
	Graph graph;
	vector<string> pool;
public:
	GA() { graph = Graph(); }
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

	graph.print();

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