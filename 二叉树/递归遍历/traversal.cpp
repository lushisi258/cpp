#include <iostream>
#include <vector>
using namespace std;

// 二叉树节点定义
struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

// 前序遍历函数
void preorderTraversal(TreeNode *root, vector<int> &result) {
    // 请在这里补全代码
    // 前序遍历就是 中左右
    if (root) {
        result.push_back(root->val);
        preorderTraversal(root->left, result);
        preorderTraversal(root->right, result);
    } else {
        return;
    }
}

// 中序遍历函数
void inorderTraversal(TreeNode *root, vector<int> &result) {
    // 请在这里补全代码
    // 中序遍历就是 左中右
    if (root) {

        inorderTraversal(root->left, result);
        result.push_back(root->val);
        inorderTraversal(root->right, result);
    } else {
        return;
    }
}

// 后序遍历函数
void postorderTraversal(TreeNode *root, vector<int> &result) {
    // 请在这里补全代码
    // 后序遍历就是 左右中
    if (root) {
        postorderTraversal(root->left, result);
        postorderTraversal(root->right, result);
        result.push_back(root->val);
    } else {
        return;
    }
}

// 辅助函数：创建示例二叉树
TreeNode *createExampleTree() {
    TreeNode *root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);
    return root;
}

// 打印遍历结果
void printTraversal(const string &name, const vector<int> &result) {
    cout << name << ": ";
    for (int val : result) {
        cout << val << " ";
    }
    cout << endl;
}

int main() {
    TreeNode *root = createExampleTree();
    vector<int> result;

    // 测试前序遍历
    preorderTraversal(root, result);
    printTraversal("Preorder", result);
    result.clear();

    // 测试中序遍历
    inorderTraversal(root, result);
    printTraversal("Inorder", result);
    result.clear();

    // 测试后序遍历
    postorderTraversal(root, result);
    printTraversal("Postorder", result);

    return 0;
}

// Tips:
// 只需要判断root节点是否为空即可，因为对于左右子节点都是调用了递归，而递归开始的时候会判断子节点是否为空