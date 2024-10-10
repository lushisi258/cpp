/*
最小生成树的数据生成，
10^3个点，5*10^5条边，边权1~10^3
格式为：
点数 边数
u1 v1 w1
u2 v2 w2
……
*/
#include <random>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

struct edge {
	int u, v, w;
};

void data(int n) {
	int edge_num = 0;				//已经生成的边数
	random_device rd;				//随机数种子
	mt19937 gen(rd());				//随机数生成器
	uniform_int_distribution<> dis_e(1, 999);	//边长随机数分布
	uniform_int_distribution<> dis_v(0, n-1);	//点数随机数分布
	vector<edge> edges;		//边的集合
	// 首先按顺序生成1000个点的边，保证每个点都有边相连
	for (int i = 0; i < n; i++) {
		int temp_e = dis_e(gen);
		int temp_v = dis_v(gen);
		edges.push_back({ i, temp_v, temp_e });
		edge_num++;
	}
	// 生成剩余的边，直到边数达到500000
	for (; edge_num < 500000;)
	{
		int u = dis_v(gen);
		int v = dis_v(gen);
		int w = dis_e(gen);
		if (u == v) continue;
		edges.push_back({ u, v, w });
		edge_num++;
	}
	// 输出到文件
	ofstream out("30data.txt", ios::app);
	out << 1000 << " " << 500000 << endl;
	for (auto e : edges) {
		out << e.u << " " << e.v << " " << e.w << endl;
	}
}

int main() {
	for (int i = 0; i < 15; i++) {
		data(1000);
	}
	for (int i = 0; i < 15; i++) {
		data(10000);
	}
}