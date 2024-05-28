#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

struct Edge {
	int u, v, w;
	Edge(int u, int v, int w) : u(u), v(v), w(w) {}
};

class UnionFind {
private:
    vector<int> parent;
public:
    UnionFind(int n) {
        parent.resize(n);
        for (int i = 0; i < n; i++) {
            parent[i] = i;
        }
    }

    int find(int x) {
        if (x != parent[x]) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    void unionSet(int x, int y) {
        parent[find(x)] = find(y);
    }
};

class Kruskal {
private:
    vector<Edge> edges;         // �洢��
	int result = 0;             // ��С��������Ȩ�صĺ�
    int n;
public:
    Kruskal(int n, const vector<Edge>& edges) : n(n), edges(edges) {}

    void run() {
        // ���ȶԱ߰�Ȩ�ؽ�������
        sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
            return a.w < b.w;
            });

        // ��ʼ�����鼯�ͽ����
        UnionFind uf(n);
        vector<Edge> res;

        // �������бߣ�����������㲻��ͬһ�������У���ϲ��������ϲ����߼�������
        for (const auto& e : edges) {
            if (uf.find(e.u) != uf.find(e.v)) {
                uf.unionSet(e.u, e.v);
				result += e.w;
            }
        }
    }

	int getRes() {
		return result;
	}
};

class Prim {
private:
    vector<Edge> edges;         // �洢��
    int n;
    vector<Edge> res;           // �洢���
	int result = 0;             // ��С��������Ȩ�صĺ�
public:
    Prim(int n, const vector<Edge>& edges) : n(n), edges(edges) {}

    void run() {
        // ���ȹ����ڽӾ���洢ͼ
        vector<vector<int>> matrix(n, vector<int>(n, INT_MAX));
		// ֻ�洢��̵ı�
        for (const auto& e : edges) {
			if (e.w < matrix[e.u][e.v]) {
				matrix[e.u][e.v] = e.w;
				matrix[e.v][e.u] = e.w;
			}
        }

        // ��ʼ��visited���飬visited�������ڱ�Ƕ����Ƿ��Ѿ�������С������
        vector<bool> visited(n, false);
        // minWeight�������ڴ洢��ǰ���㵽��С����������С��
        vector<int> minWeight(n, INT_MAX);
        // parent�������ڴ洢��ǰ����ĸ��ڵ㣬�������жϵ�ǰ�ڵ��Ƿ�������ӵ���С������
        vector<int> parent(n, -1);
        minWeight[0] = 0;

        // ѭ��n�Σ�ÿ���ҵ�һ�����������С������
        for (int i = 0; i < n; i++) {
            int u = -1;
            // �ҵ���ǰδ������С�������Ķ����е���С������������С�Ķ���
            for (int j = 0; j < n; j++) {
                if (!visited[j] && (u == -1 || minWeight[j] < minWeight[u])) {
                    u = j;
                }
            }

            if (u == -1) {
                break;
            }

            // ��ǵ�ǰ�����Ѿ�������С������
            visited[u] = true;
            if (parent[u] != -1) {
				result += matrix[u][parent[u]];
            }
            // ���µ�ǰ���㵽��С����������С��
            for (int v = 0; v < n; ++v) {
                if (!visited[v] && matrix[u][v] != INT_MAX && matrix[u][v] < minWeight[v]) {
                    minWeight[v] = matrix[u][v];
                    parent[v] = u;
                }
            }
        }
    }

    int getRes() {
        return result;
    }
};

int main() {
    int n;      // n����
    int m;	    // m����
    int u, v, w;    // ����u������v��Ȩ��w
    while (scanf("%d %d", &n, &m) != EOF) {
		vector<Edge> edges;
        for (int i = 0; i < m; i++) {
			scanf("%d %d %d", &u, &v, &w);
			edges.push_back(Edge(u, v, w));
		}
		// ϡ��ͼ��kruskal�㷨������ͼ��prim�㷨
        if (n > 5000){
			Kruskal kruskal(n, edges);
			kruskal.run();
            printf("%d\n", kruskal.getRes());
        }
        else {
			Prim prim(n, edges);
			prim.run();
            printf("%d\n", prim.getRes());
		}
	}
	return 0;
}