/*
��С���������������ɣ�
10^3���㣬5*10^5���ߣ���Ȩ1~10^3
��ʽΪ��
���� ����
u1 v1 w1
u2 v2 w2
����
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
	int edge_num = 0;				//�Ѿ����ɵı���
	random_device rd;				//���������
	mt19937 gen(rd());				//�����������
	uniform_int_distribution<> dis_e(1, 999);	//�߳�������ֲ�
	uniform_int_distribution<> dis_v(0, n-1);	//����������ֲ�
	vector<edge> edges;		//�ߵļ���
	// ���Ȱ�˳������1000����ıߣ���֤ÿ���㶼�б�����
	for (int i = 0; i < n; i++) {
		int temp_e = dis_e(gen);
		int temp_v = dis_v(gen);
		edges.push_back({ i, temp_v, temp_e });
		edge_num++;
	}
	// ����ʣ��ıߣ�ֱ�������ﵽ500000
	for (; edge_num < 500000;)
	{
		int u = dis_v(gen);
		int v = dis_v(gen);
		int w = dis_e(gen);
		if (u == v) continue;
		edges.push_back({ u, v, w });
		edge_num++;
	}
	// ������ļ�
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