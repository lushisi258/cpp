#include <algorithm>
#include <iostream>
#include <list>
#include <set>
#include <vector>

class HashTable {
  private:
    // 使用vector存储桶，每个桶是一个list
    // 存储pair<int, int>，即<key, value>
    std::vector<std::list<std::pair<int, int>>> table;
    int bucketCount;
    int size;

    // 简单的哈希函数
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

class IncreasingSubsequences {
  public:
    std::vector<std::vector<int>> findSubsequences(std::vector<int> &nums) {
        std::vector<std::vector<int>> res;
        std::vector<int> path;
        dfs(nums, 0, path, res);
        return res;
    }

  private:
    void dfs(const std::vector<int> &nums, int startIndex,
             std::vector<int> &path, std::vector<std::vector<int>> &res) {
        if (path.size() >= 2) {
            res.push_back(path);
        }

        // 使用 set 对本层元素进行去重，防止生成重复组合
        std::set<int> usedInThisLayer;

        for (int i = startIndex; i < nums.size(); i++) {
            // 不大于path最后一个元素或者在当前递归层级中nums[i]被用过，则抛弃
            if ((!path.empty() && nums[i] <= path.back()) ||
                (usedInThisLayer.find(nums[i]) != usedInThisLayer.end())) {
                continue;
            }

            usedInThisLayer.insert(nums[i]);
            path.push_back(nums[i]);
            dfs(nums, i + 1, path, res); // 递归
            path.pop_back();             // 回溯
        }
    }
};

int main() {
    std::vector<int> nums = {1, 2, 6, 6, 7};
    IncreasingSubsequences solver;
    auto result = solver.findSubsequences(nums);

    for (const auto &seq : result) {
        for (int x : seq)
            std::cout << x << " ";
        std::cout << std::endl;
        ;
    }
    return 0;
}