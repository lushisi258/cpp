#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono> // 用于时间测量

using namespace std;

// 从文件加载数据到vector
vector<int> loadDataFromFile(const string& filename) {
    vector<int> data;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return data; // 返回空vector
    }

    int num;
    while (file >> num) {
        data.push_back(num);
    }

    file.close();
    return data;
}

// 示例：你的排序算法实现（需要你根据实际情况定义）
void yourSortFunction(vector<int>& data) {
    // 这里放置你的排序逻辑
}

int main() {
    // 加载随机数据
    vector<int> randomData = loadDataFromFile("random.txt");

    // 检查是否加载成功
    if (randomData.empty()) {
        cout << "Failed to load data!" << endl;
        return 1;
    }

    // 输出前10个元素（调试用）
    cout << "Loaded " << randomData.size() << " elements. First 10: ";
    for (int i = 0; i < 10 && i < randomData.size(); ++i) {
        cout << randomData[i] << " ";
    }
    cout << endl;

    // 记录开始时间
    auto start = chrono::steady_clock::now();

    // 调用排序算法
    yourSortFunction(randomData);

    // 记录结束时间
    auto end = chrono::steady_clock::now();

    // 计算并输出排序所花费的时间
    cout << "Sorting took "
         << chrono::duration_cast<chrono::milliseconds>(end - start).count()
         << " ms." << endl;

    return 0;
}