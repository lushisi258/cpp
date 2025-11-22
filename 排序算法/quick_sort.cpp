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

// 快速排序
int partition(vector<int> &nums, int low, int high) {
    int pi = nums[low];

    int left = low + 1;
    int right = high;

    while (left < right) {
        while (nums[left] <= pi)
            left++;
        while (nums[right] >= pi)
            right--;
        while (left < right)
            swap(nums[left], nums[right]);
    }
    swap(nums[left], nums[low]);

    return left;
}

void quickSortFunction(vector<int> &nums, int low, int high) {
    if (low < high) {
        int pivo_index = partition(nums, low, high);
        partition(nums, low, pivo_index - 1);
        partition(nums, pivo_index + 1, high);
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
    quickSortFunction(data );
    auto end = chrono::steady_clock::now();

    cout << "排序耗时: "
         << chrono::duration_cast<chrono::milliseconds>(end - start).count()
         << " 毫秒" << endl;

    bool sorted = isSorted(data);
    cout << "排序结果验证: " << (sorted ? "成功" : "失败") << endl << endl;

    return 0;
}