#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
using namespace std;

class TopSort {
private:
    int n;
    // 存储每个节点的边的邻接链表
    vector<vector<int>> edges;
    // 存储节点的度数
    vector<int> indegree;
    // 值更小的优先出队
    priority_queue<int, vector<int>, greater<int>> q;
    // 存储排序结果
    vector<int> result;

public:
    TopSort(int n) : n(n), edges(n), indegree(n, 0) {}
    // 添加边
    void addEdge(int u, int v) {
        edges[u].push_back(v);
        indegree[v]++;
    }

    // 排序
    void sort() {
        // 从头开始遍历，入度为0的节点入队
        for (int i = 0; i < n; i++) {
            if (indegree[i] == 0) {
                q.push(i);
            }
        }

        // 优先处理入度为0的节点，然后更新其邻接节点的入度
        while (!q.empty()) {
            int u = q.top();
            q.pop();
            result.push_back(u);
            for (int v : edges[u]) {
                indegree[v]--;
                if (indegree[v] == 0) {
                    q.push(v);
                }
            }
        }
    }

    // 获取排序结果
    vector<int> getResult() {
        return result;
    }
};

int main() {
    int n, m;
    int a, b;
    while (scanf("%d %d", &n, &m) != EOF) {
        TopSort ts(n);
        for (int i = 0; i < m; i++) {
            scanf("%d %d", &a, &b);
            ts.addEdge(a, b);
        }
        ts.sort();
        for (int i = 0; i < n; i++) {
            printf("%d ", ts.getResult()[i]);
        }
        printf("\n");
    }

    return 0;
}