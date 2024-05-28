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
    vector<Edge> edges;         // 存储边
	int result = 0;             // 最小生成树的权重的和
    int n;
public:
    Kruskal(int n, const vector<Edge>& edges) : n(n), edges(edges) {}

    void run() {
        // 首先对边按权重进行排序
        sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
            return a.w < b.w;
            });

        // 初始化并查集和结果集
        UnionFind uf(n);
        vector<Edge> res;

        // 遍历所有边，如果两个顶点不在同一个集合中，则合并两个集合并将边加入结果集
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
    vector<Edge> edges;         // 存储边
    int n;
    vector<Edge> res;           // 存储结果
	int result = 0;             // 最小生成树的权重的和
public:
    Prim(int n, const vector<Edge>& edges) : n(n), edges(edges) {}

    void run() {
        // 首先构建邻接矩阵存储图
        vector<vector<int>> matrix(n, vector<int>(n, INT_MAX));
		// 只存储最短的边
        for (const auto& e : edges) {
			if (e.w < matrix[e.u][e.v]) {
				matrix[e.u][e.v] = e.w;
				matrix[e.v][e.u] = e.w;
			}
        }

        // 初始化visited数组，visited数组用于标记顶点是否已经加入最小生成树
        vector<bool> visited(n, false);
        // minWeight数组用于存储当前顶点到最小生成树的最小边
        vector<int> minWeight(n, INT_MAX);
        // parent数组用于存储当前顶点的父节点，即用来判断当前节点是否可以连接到最小生成树
        vector<int> parent(n, -1);
        minWeight[0] = 0;

        // 循环n次，每次找到一个顶点加入最小生成树
        for (int i = 0; i < n; i++) {
            int u = -1;
            // 找到当前未加入最小生成树的顶点中到最小生成树距离最小的顶点
            for (int j = 0; j < n; j++) {
                if (!visited[j] && (u == -1 || minWeight[j] < minWeight[u])) {
                    u = j;
                }
            }

            if (u == -1) {
                break;
            }

            // 标记当前顶点已经加入最小生成树
            visited[u] = true;
            if (parent[u] != -1) {
				result += matrix[u][parent[u]];
            }
            // 更新当前顶点到最小生成树的最小边
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
    int n;      // n个点
    int m;	    // m条边
    int u, v, w;    // 顶点u，顶点v，权重w
    while (scanf("%d %d", &n, &m) != EOF) {
		vector<Edge> edges;
        for (int i = 0; i < m; i++) {
			scanf("%d %d %d", &u, &v, &w);
			edges.push_back(Edge(u, v, w));
		}
		// 稀疏图用kruskal算法，稠密图用prim算法
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