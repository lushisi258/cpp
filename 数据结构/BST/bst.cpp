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
    TreeNode *insert(TreeNode *root, vector<int> nums) {
        // 排序数组
        sort(nums.begin(), nums.end());

        // 调整插入序列
        vector<int> balancedOrder;
        getMinHeightOrder(nums, 0, nums.size() - 1, balancedOrder);
        // 输出插入序列
        for (int val : balancedOrder) {
            cout << val << ' ';
        }
        cout << endl;

        // 执行插入操作
        for (int val : balancedOrder) {
            root = insert(root, val); // 调用单值插入函数
        }

        return root;
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

    // 二分提取调整次序
    void getMinHeightOrder(const vector<int> &nums, int left, int right,
                           vector<int> &order) {
        if (left > right)
            return;
        int mid = left + (right - left) / 2;
        order.push_back(nums[mid]);
        getMinHeightOrder(nums, left, mid - 1, order);
        getMinHeightOrder(nums, mid + 1, right, order);
    }
};

// 输出树
// 获取树的高度
int getHeight(TreeNode *root) {
    if (!root)
        return 0;
    return max(getHeight(root->left), getHeight(root->right)) + 1;
}
// 将节点填充到二维数组中
void fillArray(TreeNode *root, vector<string> &res, int row, int col,
               int height) {
    if (!root)
        return;

    // 放入当前节点的值
    string s = to_string(root->val);
    int res_col = col - s.length() / 2; // 居中对齐
    for (int i = 0; i < s.length() && res_col + i < res[row].size(); i++) {
        res[row][res_col + i] = s[i];
    }

    if (height == 1)
        return;

    // 计算下一层分支的跨度
    int gap = pow(2, height - 2);

    // 处理左子树
    if (root->left) {
        res[row + 1][col - gap / 2 - 1] = '/'; // 绘制连线
        fillArray(root->left, res, row + 2, col - gap, height - 1);
    }

    // 处理右子树
    if (root->right) {
        res[row + 1][col + gap / 2 + 1] = '\\'; // 绘制连线
        fillArray(root->right, res, row + 2, col + gap, height - 1);
    }
}
// 打印树
void printTree(TreeNode *root) {
    int h = getHeight(root);
    if (h == 0)
        return;

    int rows = h * 2;         // 节点行 + 连线行
    int cols = pow(2, h + 1); // 宽度随高度指数增长
    vector<string> res(rows, string(cols, ' '));

    fillArray(root, res, 0, cols / 2, h);

    for (const string &line : res) {
        // 剪掉行尾空格
        string trimmed = line;
        trimmed.erase(trimmed.find_last_not_of(' ') + 1);
        if (!trimmed.empty())
            cout << line << endl;
    }
}

int main() {
    Solution sol;
    vector<int> preorder = {8, 5, 1, 7, 12, 15};
    vector<int> num = {16, 17, 18};

    TreeNode *root = sol.bst(preorder);

    printTree(root);
    cout << endl;

    sol.insert(root, num);
    printTree(root);

    return 0;
}