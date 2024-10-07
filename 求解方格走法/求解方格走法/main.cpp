#include<vector>
#include<iostream>

using namespace std;

class CalculateWays {
public:
	CalculateWays() {}
	// 初始化矩阵
	void make_matrix(int n, int m) {
		Result = vector<vector<int>>(n + 1, vector<int>(m + 1, 0));
		Result[1][1] = 1;
	}
	// 计算走法
	int calculate(int n, int m) {
		// 如果已经解决过子问题，直接返回结果
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
	cout << "请输入N：";
	cin >> n;
	cout << "请输入M：";
	cin >> m;
	CalculateWays cw;
	while (n > 0 || m > 0) {
		cout << "结果是：" << cw.get_result(n, m) << endl;
		cout << "请输入N：";
		cin >> n;
		cout << "请输入M：";
		cin >> m;
	}
	return 0;
}