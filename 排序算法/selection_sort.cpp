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

// 选择排序
void selectionSortFunction(vector<int> &nums) {
    int n = nums.size();
    // i 为未排序区的开始的索引
    for (int i = 0; i < n - 1; i++) {
        int min_index = i;
        // j 为未排序区最小值的遍历索引
        for (int j = i; j < n; j++) {
            if (nums[j] < nums[min_index]) {
                min_index = j;
            }
        }
        swap(nums[i], nums[min_index]);
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
    selectionSortFunction(data);
    auto end = chrono::steady_clock::now();

    cout << "排序耗时: "
         << chrono::duration_cast<chrono::milliseconds>(end - start).count()
         << " 毫秒" << endl;

    bool sorted = isSorted(data);
    cout << "排序结果验证: " << (sorted ? "成功" : "失败") << endl << endl;

    return 0;
}