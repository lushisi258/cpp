#include <bitset>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

vector<vector<int>> pt(8000, vector<int>(16, 0));                      // 明文
vector<vector<int>> ct(8000, vector<int>(16, 0));                      // 密文
int sBox[16] = {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7}; // S盒

// 将二进制数组转换为十进制数
int turnToNum(const vector<int> &binaryArray, int length) {
    int num = 0;
    for (int i = 0; i < length; i++) {
        // 先左移，再通过或把二进制数添加到末位
        num = (num << 1) | binaryArray[i];
    }
    return num;
}

// 将十进制数转换为二进制数组
vector<int> turnToBinaryArray(int num, int length) {
    vector<int> binaryArray(length, 0);
    int index = length - 1;
    while (num > 0) {
        binaryArray[index] = num % 2;
        num /= 2;
        index--;
    }
    return binaryArray;
}

// 逆S盒查找
int inverseSBox(int value) {
    for (int i = 0; i < 16; i++) {
        if (sBox[i] == value) {
            return i;
        }
    }
    return -1; // 如果没有找到，返回-1
}

int main() {
    ifstream file("data.csv");
    if (!file.is_open()) {
        cerr << "Failed to open file." << endl;
        return 1;
    }

    string line;
    // Skip the first line
    getline(file, line);

    int pt[8000][16];
    int ct[8000][16];
    int row = 0;

    while (getline(file, line) && row < 8000) {
        stringstream ss(line);
        string pt_str, ct_str;

        getline(ss, pt_str, ',');
        getline(ss, ct_str, ',');

        for (int i = 0; i < 16; i++) {
            pt[row][i] = pt_str[i] - '0';
            ct[row][i] = ct_str[i] - '0';
        }
        row++;
    }
    file.close();

    // 初始化Count矩阵
    vector<vector<int>> Count(16, vector<int>(16, 0));

    // 遍历子密钥 (L1, L2)
    for (int L1 = 0; L1 < 16; L1++) {
        for (int L2 = 0; L2 < 16; L2++) {
            Count[L1][L2] = 0;

            // 遍历每对明密文 (x, y)
            for (int i = 0; i < 8000; i++) {
                int x = turnToNum(vector<int>(pt[i], pt[i] + 16),
                                  16); // 转换明文为整数
                int y = turnToNum(vector<int>(ct[i], ct[i] + 16),
                                  16); // 转换密文为整数

                // v'2,4, v'4,4
                int v2_4 = L1 ^ (y >> 8 & 0xF); // 提取密文并异或子密钥
                int v4_4 = L2 ^ (y >> 12 & 0xF);

                // 逆S盒得到 u'2,4 和 u'4,4
                int u2_4 = inverseSBox(v2_4);
                int u4_4 = inverseSBox(v4_4);

                // 使用 u'2,4, u'4,4 和明文 x 的部分位计算 z
                int z = (x >> 5 & 1) ^ (x >> 7 & 1) ^ (x >> 8 & 1) ^ u2_4 ^
                        u4_4 ^ (x >> 14 & 1) ^ (x >> 16 & 1);

                // 判断 z 是否为 0
                if (z == 0) {
                    Count[L1][L2]++;
                }
            }
        }
    }

    // 找到最大Count的子密钥 (L1, L2)
    int max = -1;
    pair<int, int> maxkey = {0, 0};

    for (int L1 = 0; L1 < 16; L1++) {
        for (int L2 = 0; L2 < 16; L2++) {
            Count[L1][L2] = abs(Count[L1][L2] - 4000);
            if (Count[L1][L2] > max) {
                max = Count[L1][L2];
                maxkey = {L1, L2};
            }
        }
    }

    // 输出二进制格式的子密钥
    cout << "L1: " << bitset<4>(maxkey.first)
         << ", L2: " << bitset<4>(maxkey.second) << endl;

    return 0;
}
