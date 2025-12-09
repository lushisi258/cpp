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
// 分区函数：对nums[low..high]区间分区，返回基准元素最终位置
int partition(vector<int>& nums, int low, int high) {
    int pivot = nums[low];  // 选取区间最左侧元素作为基准
    int left = low + 1;     // 左指针从基准后一位开始
    int right = high;       // 右指针从区间末尾开始

    // 双指针向中间靠拢，完成元素划分
    while (left <= right) {
        // 左指针找第一个大于基准的元素，小于等于基准则继续右移
        while (left <= right && nums[left] <= pivot) {
            left++;
        }
        // 右指针找第一个小于基准的元素，大于等于基准则继续左移
        while (left <= right && nums[right] >= pivot) {
            right--;
        }
        // 左指针在右指针左侧时，交换两元素位置
        if (left < right) {
            swap(nums[left], nums[right]);
            left++;
            right--;
        }
    }

    // 基准元素归位到正确位置
    swap(nums[low], nums[right]);
    return right;  // 返回基准元素索引，用于分割子区间
}

void quickSortFunction(vector<int> &nums, int low, int high) {
    if (low < high) {
        // 以区间第一个元素（left）为基准，对区间进行分区
        int pivo_index = partition(nums, low, high);
        quickSortFunction(nums, low, pivo_index - 1);
        quickSortFunction(nums, pivo_index + 1, high);
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
    quickSortFunction(data, 0, data.size()-1);
    auto end = chrono::steady_clock::now();

    cout << "排序耗时: "
         << chrono::duration_cast<chrono::milliseconds>(end - start).count()
         << " 毫秒" << endl;

    bool sorted = isSorted(data);
    cout << "排序结果验证: " << (sorted ? "成功" : "失败") << endl << endl;

    return 0;
}