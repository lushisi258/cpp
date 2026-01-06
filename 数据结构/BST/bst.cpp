#include <algorithm>
#include <climits>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

// 定义树节点结构
struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class Solution {
  public:
    TreeNode *bst(vector<int> &preorder) {
        int index = 0;
        return build(preorder, index, INT_MAX);
    }

    // 插入单值
    TreeNode *insert(TreeNode *root, int val) {
        if (!root)
            return new TreeNode(val);
        if (val < root->val)
            root->left = insert(root->left, val);
        else if (val > root->val)
            root->right = insert(root->right, val);
        return root;
    }

    // 插入数组
    vector<int> insert(TreeNode *root, vector<int> nums) {
        // 排序插入数组
        sort(nums.begin(), nums.end());
        // 存储排序结果
        vector<int> order;
        // 插入
        root = insert(root, nums, 0, nums.size() - 1, order);
        return order;
    }

  private:
    // bound作为当前子树的最大值上限，用来判断当前子树的边界
    TreeNode *build(vector<int> &preorder, int &index, int bound) {
        // 如果数组遍历结束或者当前值超过了上限（即不属于该子树的范围）就返回递归的上一层
        if (index == preorder.size() || preorder[index] > bound) {
            return NULL;
        }

        // 创建当前根节点
        TreeNode *root = new TreeNode(preorder[index++]);

        // 左子树的值上限为根节点的值
        root->left = build(preorder, index, root->val);

        // 右子树的值即为当前子树的边界值
        root->right = build(preorder, index, bound);

        return root;
    }

    TreeNode *insert(TreeNode *root, vector<int> &nums, int start, int end,
                     vector<int> &order) {
        if (start > end)
            return root;
        // 如果当前节点为空，即该数组段已经没有可以处理的子节点了，生成bst返回
        if (!root) {
            return buildBalanced(nums, start, end, order);
        }

        // 寻找第一个大于等于root的值的位置，在该值前的数交给左子树处理，在该值后的数交给右子树处理
        int split = start;
        while (split <= end && nums[split] < root->val) {
            split++;
        }

        // 抛弃等于当前节点的值
        int rightStart =
            (split <= end && nums[split] == root->val) ? split + 1 : split;

        // 左子树去处理前半段
        root->left = insert(root->left, nums, start, split - 1, order);
        // 右子树去处理后半段
        root->right = insert(root->right, nums, rightStart, end, order);

        return root;
    }

    // 将有序数组段转换为平衡二叉树插入
    TreeNode *buildBalanced(vector<int> &nums, int s, int e,
                            vector<int> &order) {
        if (s > e)
            return nullptr;
        int mid = s + (e - s) / 2;
        TreeNode *node = new TreeNode(nums[mid]);
        order.push_back(nums[mid]);
        node->left = buildBalanced(nums, s, mid - 1, order);
        node->right = buildBalanced(nums, mid + 1, e, order);
        return node;
    }
};

// 输出树
struct NodeInfo {
    string text;
    int width, height;
    vector<string> pixels;
};
NodeInfo get_node_info(TreeNode *root) {
    if (!root)
        return {"", 0, 0, {}};

    string val_str = to_string(root->val);
    NodeInfo left = get_node_info(root->left);
    NodeInfo right = get_node_info(root->right);

    int val_w = (int)val_str.length();
    int res_w, res_h, left_pos, right_pos;

    // 计算当前层宽度：左子树宽 + 右子树宽 + 间距
    int gap = 2;
    res_w = max(val_w, (left.width ? left.width + gap : 0) +
                           (right.width ? right.width + gap : 0));
    res_h = max(left.height, right.height) + 2; // 2 = 节点行 + 连线行

    vector<string> res(res_h, string(res_w, ' '));

    // 居中放置节点值
    int val_x = (res_w - val_w) / 2;
    for (int i = 0; i < val_w; ++i)
        res[0][val_x + i] = val_str[i];

    // 绘制连线和合并子树
    if (left.height > 0) {
        int lx = (left.width - 1) / 2;
        res[1][lx + (val_x - lx) / 2] = '/';
        for (int i = 0; i < left.height; i++)
            for (int j = 0; j < left.width; j++)
                res[i + 2][j] = left.pixels[i][j];
    }

    if (right.height > 0) {
        int rx = res_w - (right.width + 1) / 2;
        res[1][val_x + val_w + (rx - (val_x + val_w)) / 2] = '\\';
        for (int i = 0; i < right.height; i++)
            for (int j = 0; j < right.width; j++)
                res[i + 2][res_w - right.width + j] = right.pixels[i][j];
    }

    return {val_str, res_w, res_h, res};
}

void printTree(TreeNode *root) {
    NodeInfo info = get_node_info(root);
    for (const string &s : info.pixels) {
        // 剪掉行尾多余空格
        string line = s;
        line.erase(line.find_last_not_of(' ') + 1);
        cout << line << endl;
    }
}

int main() {
    Solution sol;
    vector<int> preorder = {12, 2, 1, 27, 13, 14};
    vector<int> num = {0, 15, 16, 17, 18, 19, 28, 29, 30, 31, 32};

    TreeNode *root = sol.bst(preorder);

    printTree(root);
    cout << endl;

    vector<int> order = sol.insert(root, num);
    printTree(root);

    for (int i : order) {
        cout << i << ' ';
    }

    return 0;
}