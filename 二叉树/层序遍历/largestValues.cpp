#include <iostream>
#include <queue>
#include <vector>
using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

TreeNode *buildTestTree() {
    TreeNode *root = new TreeNode(10);
    root->left = new TreeNode(6);
    root->right = new TreeNode(15);

    root->left->left = new TreeNode(3);
    root->left->right = new TreeNode(19);
    root->left->left->left = new TreeNode(1);
    root->left->right->left = new TreeNode(7);
    root->left->right->right = new TreeNode(9);

    root->right->left = new TreeNode(12);
    root->right->right = new TreeNode(8);
    root->right->right->left = new TreeNode(18);

    return root;
}

int main() {
    TreeNode *root = buildTestTree();
    // 这里你来实现层序遍历逻辑
    int size = 0;
    int current_largest = 0;
    queue<TreeNode *> q;
    vector<int> result;

    // 修改：首先确定root不为空
    if (root)
        q.push(root);
    while (!q.empty()) {
        // 初始化该层规模
        size = q.size();
        // 初始化对第一个节点的处理
        current_largest = q.front()->val;
        if (q.front()->left)
            q.push(q.front()->left);
        if (q.front()->right)
            q.push(q.front()->right);
        q.pop();
        // 从剩余节点中选出最大值，并把子节点加入队列
        for (int i = 1; i < size; ++i) {
            if (q.front()->val > current_largest)
                current_largest = q.front()->val;
            if (q.front()->left)
                q.push(q.front()->left);
            if (q.front()->right)
                q.push(q.front()->right);
            q.pop();
        }
        result.push_back(current_largest);
    }
    for (int val : result) {
        cout << val << " ";
    }
    cout << endl;
    return 0;
}
