#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<queue>

using namespace std;

class NKProblem {
public:
	int dataSize = 1;
	vector<int> nodesStatus;
	vector<vector<int>> graph;

	// 构造函数
	NKProblem(vector<vector<int>> matrix) {
		dataSize = matrix.size(); // 初始化dataSize
		nodesStatus.resize(dataSize); // 初始化nodesStatus
		nodesStatus[0] = 2;
		// faultyNodes 和 nodesStatus 默认初始化为空vector
		graph = matrix; // 初始化graph为传入的matrix
	}
	void solution() {
		queue<int> q;
		q.push(0); // 将0号节点入队
		while (!q.empty()) {
			// 取出队首元素
			int f = q.front();
			q.pop();
			// 遍历f节点的所有邻接节点
			for (int i = 0; i < dataSize; ++i) {
				if (graph[f][i] == 0) {
					continue;
				}
				// 如果f和i之间有连接，判断i的状态
				// 如果i的状态为0，说明i未被访问过
				else if(nodesStatus[i] == 0) {
					// 如果连接为-1的话，那么i的颜色和f的颜色相反
					if (graph[f][i] == -1) {
						nodesStatus[i] = 0 - nodesStatus[f];
					}
					// 如果连接为1的话，那么i的颜色和f的颜色相同
					else if (graph[f][i] == 1) {
						nodesStatus[i] = nodesStatus[f];
					}
					q.push(i); // 将i入队
				}
			}
		}
	};
	vector<int> getResult() {
		vector<int> indices;
		for (int i = 0; i < nodesStatus.size(); ++i) {
			if (nodesStatus[i] == -2) {
				indices.push_back(i);
			}
		}
		return indices;
	};
	~NKProblem() {};
};

int main() {
	ifstream file("matrix.csv");
	string line;
	vector<vector<int>> matrix;
	vector<int> firstRow; // 用于存储第一行的数据
	bool firstLine = true; // 用于判断是否是第一行

	while (getline(file, line)) {
		if (firstLine) { // 如果是第一行
			stringstream ss(line);
			string num;
			getline(ss, num, ','); // 首先读取并丢弃第一个空数据
			while (getline(ss, num, ',')) {
				firstRow.push_back(stoi(num)); // 将第一行的数据存储到firstRow数组中
			}
			firstLine = false; // 更新标志，表示第一行已经处理
			continue;
		}

		vector<int> row;
		stringstream ss(line);
		string num;
		int columnIndex = 0; // 用于跳过每行的第一个数据

		while (getline(ss, num, ',')) {
			if (columnIndex != 0) { // 如果不是每行的第一个数据
				row.push_back(std::stoi(num));
			}
			columnIndex++; // 更新列索引
		}
		matrix.push_back(row);
	}
	NKProblem problem(matrix);
	problem.solution();
	vector<int> result = problem.getResult();
	for(auto i: result) {
		cout << firstRow[i] << " ";
	}
}