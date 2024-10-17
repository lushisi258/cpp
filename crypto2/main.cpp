#include <bitset>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

using namespace std;

string binary_repr[256];
int count_array[256] = {0};
// S盒逆映射
unordered_map<string, string> sBox_map = {
    {"1010", "1001"}, {"0010", "0100"}, {"0111", "1111"}, {"0101", "1100"},
    {"1110", "0000"}, {"1001", "1101"}, {"1100", "1011"}, {"0001", "0011"},
    {"0110", "1010"}, {"1101", "0010"}, {"0011", "1000"}, {"1000", "0111"},
    {"1111", "0101"}, {"0000", "1110"}, {"1011", "0110"}, {"0100", "0001"}};

string line;
string plaintext, ciphertext;

int main() {
    // 逐个生成二进制表示
    for (int i = 0; i < 256; i++) {
        string binary = ""; // 用于存储当前整数的二进制表示
        int num = i;        // 复制当前整数

        // 将当前整数转换为二进制
        for (int j = 0; j < 8; j++) {
            // 检查最低位是否为1
            if (num & 1) {
                binary = "1" + binary; // 如果最低位为1，前面加1
            } else {
                binary = "0" + binary; // 如果最低位为0，前面加0
            }
            num >>= 1; // 右移一位，处理下一个二进制位
        }

        binary_repr[i] = binary; // 存储当前整数的二进制表示
    }

    ifstream file_input("data.txt");
    if (!file_input.is_open()) {
        cerr << "无法打开文件" << endl;
        return 1;
    }
    while (getline(file_input, line)) {
        // 使用字符串流进行分割
        stringstream ss(line);
        getline(ss, plaintext, ',');  // 读取逗号前的部分
        getline(ss, ciphertext, ','); // 读取逗号后的部分

        string mid_bits1 = ciphertext.substr(4, 4);
        string mid_bits2 = ciphertext.substr(12, 4);

        for (int i = 0; i < 256; i++) {
            string xor_result1, xor_result2;
            string mapped1, mapped2;

            string upper_half = binary_repr[i].substr(0, 4);
            string lower_half = binary_repr[i].substr(4, 4);

            // 逐位异或运算
            for (int j = 0; j < 4; j++) {
                char xor_bit1 = (upper_half[j] == mid_bits1[j]) ? '0' : '1';
                xor_result1 += xor_bit1;

                char xor_bit2 = (lower_half[j] == mid_bits2[j]) ? '0' : '1';
                xor_result2 += xor_bit2;
            }

            // 使用 S 盒进行映射
            mapped1 = sBox_map[xor_result1];
            mapped2 = sBox_map[xor_result2];

            char bit5 = plaintext[4];
            char bit7 = plaintext[6];
            char bit8 = plaintext[7];

            char mapped1_bit2 = mapped1[1];
            char mapped1_last = mapped1[3];
            char mapped2_bit2 = mapped2[1];
            char mapped2_last = mapped2[3];

            int xor_result = 0;
            xor_result ^= (bit5 - '0');
            xor_result ^= (bit7 - '0');
            xor_result ^= (bit8 - '0');
            xor_result ^= (mapped1_bit2 - '0');
            xor_result ^= (mapped1_last - '0');
            xor_result ^= (mapped2_bit2 - '0');
            xor_result ^= (mapped2_last - '0');

            // 判断异或结果是否为 0
            if (xor_result == 0) {
                count_array[i] += 1;
            }
        }
    }

    // 寻找频率最高的索引
    int max_value = -1;
    int max_index = -1;

    for (int i = 0; i < 256; i++) {
        count_array[i] = abs(count_array[i] - 0.5 * 8000);
        if (count_array[i] >= max_value) {
            max_value = count_array[i];
            max_index = i;
        }
    }

    if (max_index != -1) {
        cout << "K value:" << binary_repr[max_index] << endl;
    } else {
        cout << "No max value." << endl;
    }

    file_input.close();
    return 0;
}
