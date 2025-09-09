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
bool isSorted(const vector<int> &arr) {
    for (int i = 0; i < int(arr.size()) - 1; ++i) {
        if (arr[i] > arr[i + 1]) {
            return false;
        }
    }
    return true;
}

// 冒泡排序
void bubbleSortFunction(vector<int> &data) {
    int n = data.size();
    for (int i = n - 1; i > 0; i--) {
        for (int j = 0; j < i; j++) {
            if (data[j] > data[j + 1]) {
                swap(data[j], data[j + 1]);
            }
        }
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
    bubbleSortFunction(data);
    auto end = chrono::steady_clock::now();

    cout << "排序耗时: "
         << chrono::duration_cast<chrono::milliseconds>(end - start).count()
         << " 毫秒" << endl;

    bool sorted = isSorted(data);
    cout << "排序结果验证: " << (sorted ? "成功" : "失败") << endl << endl;

    return 0;
}