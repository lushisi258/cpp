#include<vector>
#include<iostream>

using namespace std;

class CalculateWays {
public:
	CalculateWays() {}
	// ��ʼ������
	void make_matrix(int n, int m) {
		Result = vector<vector<int>>(n + 1, vector<int>(m + 1, 0));
		Result[1][1] = 1;
	}
	// �����߷�
	int calculate(int n, int m) {
		// ����Ѿ�����������⣬ֱ�ӷ��ؽ��
		if (Result[n][m] != 0) {
			return Result[n][m];
		}
		else if (n > 0 && m > 0) {
			Result[n][m] = calculate(n - 1, m) + calculate(n, m - 1) + calculate(n - 1, m - 1);
			return Result[n][m];
		}
		else if (n > 0) {
			Result[n][m] = calculate(n - 1, m);
			return Result[n][m];
		}
		else if (m > 0) {
			Result[n][m] = calculate(n, m - 1);
			return Result[n][m];
		}
		else {
			return Result[n][m];
		}
	}
	int get_result(int n, int m) {
		make_matrix(n, m);
		return calculate(n, m);
	}
	~CalculateWays() {}
private:
	vector<vector<int>> Result;
};

int main() {
	int n, m; 
	cout << "������N��";
	cin >> n;
	cout << "������M��";
	cin >> m;
	CalculateWays cw;
	while (n > 0 || m > 0) {
		cout << "����ǣ�" << cw.get_result(n, m) << endl;
		cout << "������N��";
		cin >> n;
		cout << "������M��";
		cin >> m;
	}
	return 0;
}