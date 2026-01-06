#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <vector>

class HashTable {
  private:
    // 使用vector存储桶，每个桶是一个list双向链表
    // 存储pair<int, int>，即<key, value>
    std::vector<std::list<std::pair<int, int>>> table;
    int bucketCount;
    int size;

    // 哈希函数
    int hashFunction(int key) { return std::abs(key) % bucketCount; }

  public:
    // 构造函数，初始化桶的数量
    HashTable(int capacity = 10) : bucketCount(capacity), size(0) {
        table.resize(bucketCount);
    }

    // 添加元素
    void put(int key, int value) {
        int index = hashFunction(key);
        for (auto &pair : table[index]) {
            if (pair.first == key) {
                pair.second = value; // 如果键存在，更新值
                return;
            }
        }
        // 如果键不存在，插入新节点
        table[index].push_back({key, value});
        size++;
    }

    // 删除元素
    bool remove(int key) {
        int index = hashFunction(key);
        auto &bucket = table[index];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->first == key) {
                bucket.erase(it);
                size--;
                return true; // 删除成功
            }
        }
        return false; // 未找到键
    }

    // 查询元素
    bool get(int key, int &value) {
        int index = hashFunction(key);
        for (const auto &pair : table[index]) {
            if (pair.first == key) {
                value = pair.second;
                return true; // 找到并返回结果
            }
        }
        return false; // 未找到
    }

    // 获取当前元素数量
    int getSize() const { return size; }
};

class UniqueSubsequences {
  public:
    void solve(std::vector<int> &nums) {
        std::map<std::vector<int>, int> counts; // 记录每个子序列出现的次数
        std::vector<int> path;

        // dfs找出所有递增子序列
        dfs(nums, 0, path, counts);

        // 遍历结果，只打印计数为1的序列
        for (auto const &[seq, count] : counts) {
            if (count == 1) {
                for (int x : seq)
                    std::cout << x << " ";
                std::cout << std::endl;
            }
        }
    }

  private:
    void dfs(const std::vector<int> &nums, int index, std::vector<int> &path,
             std::map<std::vector<int>, int> &counts) {
        if (path.size() >= 2) {
            counts[path]++; // 记录该序列出现的次数
        }

        for (int i = index; i < nums.size(); ++i) {
            // 如果path为空或者当前值大于path末尾的值（即符合单调递增），将当前值加入到path末尾
            if (path.empty() || nums[i] > path.back()) {
                path.push_back(nums[i]);
                dfs(nums, i + 1, path, counts);
                path.pop_back(); // 回溯
            }
        }
    }
};

int main() {
    std::vector<int> nums = {3, 5, 0, 2, 3, 4};
    UniqueSubsequences solver;
    solver.solve(nums);

    return 0;
}