#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

vector<int> loadDataFromFile(const string &filename) {
    vector<int> data;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return data;
    }

    int num;
    while (file >> num) {
        data.push_back(num);
    }
    file.close();
    return data;
}

// 验证数组是否有序
bool isSorted(const vector<int> &nums) {
    for (int i = 0; i < int(nums.size()) - 1; ++i) {
        if (nums[i] > nums[i + 1]) {
            return false;
        }
    }
    return true;
}

// 插入排序
void insertionSortFunction(vector<int> &nums) {
    int n = nums.size();
    for (int unsorted = 1; unsorted < n; ++unsorted) { // 从第二个元素开始
        int key = nums[unsorted];                      // 取出待插入元素
        int backtrack = unsorted - 1; // 回溯索引，用来寻找合适的插入位置

        // 从后向前扫描，寻找插入位置
        while (backtrack >= 0 && nums[backtrack] > key) {
            nums[backtrack + 1] = nums[backtrack]; // 元素交换，待插入元素前移
            backtrack--;
        }
        nums[backtrack + 1] = key; // 插入到正确位置
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "用法: " << argv[0] << " <数据文件>" << endl;
        return 1;
    }

    string data_file = argv[1];
    cout << "正在测试数据集: " << data_file << endl;

    vector<int> data = loadDataFromFile(data_file);
    if (data.empty()) {
        cerr << "数据加载失败！" << endl;
        return 1;
    }

    auto start = chrono::steady_clock::now();
    insertionSortFunction(data);
    auto end = chrono::steady_clock::now();

    cout << "排序耗时: "
         << chrono::duration_cast<chrono::milliseconds>(end - start).count()
         << " 毫秒" << endl;

    bool sorted = isSorted(data);
    cout << "排序结果验证: " << (sorted ? "成功" : "失败") << endl << endl;

    return 0;
}