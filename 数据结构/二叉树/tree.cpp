#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

// 二叉树节点结构
struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class BinaryTreeReconstructor {
  private:
    unordered_map<int, int> inorder_map; // 存储中序遍历 <值， 索引>

    TreeNode *build(const vector<int> &preorder, int pre_start, int pre_end,
                    int in_start, int in_end) {
        if (pre_start > pre_end || in_start > in_end)
            return nullptr;

        // 先序遍历的第一个元素是根节点
        int root_val = preorder[pre_start];
        TreeNode *root = new TreeNode(root_val);

        // 在中序遍历中找到根节点的位置，划分左右子树
        int in_root_idx = inorder_map[root_val];
        int left_size = in_root_idx - in_start; // 左子树节点个数

        // 对划分出来的左子树和右子树递归操作来构建二叉树
        // 左子树：先序遍历中从 pre_start+1 到 pre_start+left_size；中序遍历中从
        // in_start 到 in_root_idx-1
        root->left = build(preorder, pre_start + 1, pre_start + left_size,
                           in_start, in_root_idx - 1);
        // 右子树：先序遍历中从 pre_start+left_size+1 到 pre_end；中序遍历中从
        // in_root_idx+1 到 in_end
        root->right = build(preorder, pre_start + left_size + 1, pre_end,
                            in_root_idx + 1, in_end);

        return root;
    }

  public:
    TreeNode *buildTree(vector<int> &preorder, vector<int> &inorder, int n) {
        for (int i = 0; i < n; ++i) {
            inorder_map[inorder[i]] = i;
        }
        return build(preorder, 0, preorder.size() - 1, 0, inorder.size() - 1);
    }

    // 层序遍历输出
    void levelOrder(TreeNode *root) {
        if (!root)
            return;
        queue<TreeNode *> q;
        q.push(root);

        cout << "层序遍历序列: ";
        while (!q.empty()) {
            TreeNode *curr = q.front();
            q.pop();
            cout << curr->val << " ";
            if (curr->left)
                q.push(curr->left);
            if (curr->right)
                q.push(curr->right);
        }
        cout << endl;
    }
};

// 递归方法找到最近公共祖先
TreeNode *findAncestor1(TreeNode *root, int p, int q) {
    if (!root || root->val == p || root->val == q)
        return root;

    // 判断左子树和右子树是否有节点p或q
    TreeNode *left = findAncestor1(root->left, p, q);
    TreeNode *right = findAncestor1(root->right, p, q);

    if (left && right)
        return root;            // p, q 分别在左右子树
    return left ? left : right; // 都在左边或都在右边，将该节点层层返回到根节点
}

// 非递归方法找到最近公共祖先
TreeNode *findAncestor2(TreeNode *root, int p_val, int q_val) {
    if (!root)
        return nullptr;

    // 使用map记录每个节点的父节点 <当前节点指针， 父节点指针>
    unordered_map<TreeNode *, TreeNode *> parent;
    queue<TreeNode *> qu;

    // 指向对应节点的指针
    TreeNode *p_node = nullptr;
    TreeNode *q_node = nullptr;

    parent[root] = nullptr;
    qu.push(root);

    // 遍历树，直到找到 p 和 q 对应的指针，并存好所有父节点关系
    while (!qu.empty() && (p_node == nullptr || q_node == nullptr)) {
        TreeNode *curr = qu.front();
        qu.pop();

        if (curr->val == p_val)
            p_node = curr;
        if (curr->val == q_val)
            q_node = curr;

        if (curr->left) {
            parent[curr->left] = curr;
            qu.push(curr->left);
        }
        if (curr->right) {
            parent[curr->right] = curr;
            qu.push(curr->right);
        }
    }

    // 如果没找全，说明输入的某个值不在树里
    if (!p_node || !q_node)
        return nullptr;

    // 将p的祖先存入set
    unordered_set<TreeNode *> ancestors;
    while (p_node != nullptr) {
        ancestors.insert(p_node);
        p_node = parent[p_node]; // 向上遍历p节点的祖先
    }

    // 向上追溯q的祖先，第一个在p祖先的set中出现的就是最小公共祖先
    while (q_node != nullptr) {
        if (ancestors.find(q_node) != ancestors.end()) {
            return q_node;
        }
        q_node = parent[q_node]; // 向上遍历q节点的祖先
    }

    return nullptr;
}

int main() {
    int n = 6;
    vector<int> preorder = {10, 4, 2, 6, 14, 20};
    vector<int> inorder = {2, 4, 6, 10, 14, 20};
    int p = 2;
    int q = 20;

    BinaryTreeReconstructor builder;
    TreeNode *root = builder.buildTree(preorder, inorder, n);

    builder.levelOrder(root);

    TreeNode *ans1 = findAncestor1(root, p, q);
    cout << ans1->val << ' ';

    TreeNode *ans2 = findAncestor2(root, p, q);
    cout << ans2->val << endl;

    return 0;
}