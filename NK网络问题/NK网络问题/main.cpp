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

	// ���캯��
	NKProblem(vector<vector<int>> matrix) {
		dataSize = matrix.size(); // ��ʼ��dataSize
		nodesStatus.resize(dataSize); // ��ʼ��nodesStatus
		nodesStatus[0] = 2;
		// faultyNodes �� nodesStatus Ĭ�ϳ�ʼ��Ϊ��vector
		graph = matrix; // ��ʼ��graphΪ�����matrix
	}
	void solution() {
		queue<int> q;
		q.push(0); // ��0�Žڵ����
		while (!q.empty()) {
			// ȡ������Ԫ��
			int f = q.front();
			q.pop();
			// ����f�ڵ�������ڽӽڵ�
			for (int i = 0; i < dataSize; ++i) {
				if (graph[f][i] == 0) {
					continue;
				}
				// ���f��i֮�������ӣ��ж�i��״̬
				// ���i��״̬Ϊ0��˵��iδ�����ʹ�
				else if(nodesStatus[i] == 0) {
					// �������Ϊ-1�Ļ�����ôi����ɫ��f����ɫ�෴
					if (graph[f][i] == -1) {
						nodesStatus[i] = 0 - nodesStatus[f];
					}
					// �������Ϊ1�Ļ�����ôi����ɫ��f����ɫ��ͬ
					else if (graph[f][i] == 1) {
						nodesStatus[i] = nodesStatus[f];
					}
					q.push(i); // ��i���
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
	vector<int> firstRow; // ���ڴ洢��һ�е�����
	bool firstLine = true; // �����ж��Ƿ��ǵ�һ��

	while (getline(file, line)) {
		if (firstLine) { // ����ǵ�һ��
			stringstream ss(line);
			string num;
			getline(ss, num, ','); // ���ȶ�ȡ��������һ��������
			while (getline(ss, num, ',')) {
				firstRow.push_back(stoi(num)); // ����һ�е����ݴ洢��firstRow������
			}
			firstLine = false; // ���±�־����ʾ��һ���Ѿ�����
			continue;
		}

		vector<int> row;
		stringstream ss(line);
		string num;
		int columnIndex = 0; // ��������ÿ�еĵ�һ������

		while (getline(ss, num, ',')) {
			if (columnIndex != 0) { // �������ÿ�еĵ�һ������
				row.push_back(std::stoi(num));
			}
			columnIndex++; // ����������
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