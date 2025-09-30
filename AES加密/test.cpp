#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

static int S[16][16] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b,
    0xfe, 0xd7, 0xab, 0x76, 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 0xb7, 0xfd, 0x93, 0x26,
    0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2,
    0xeb, 0x27, 0xb2, 0x75, 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, 0x53, 0xd1, 0x00, 0xed,
    0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f,
    0x50, 0x3c, 0x9f, 0xa8, 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 0xcd, 0x0c, 0x13, 0xec,
    0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14,
    0xde, 0x5e, 0x0b, 0xdb, 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 0xe7, 0xc8, 0x37, 0x6d,
    0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f,
    0x4b, 0xbd, 0x8b, 0x8a, 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, 0xe1, 0xf8, 0x98, 0x11,
    0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f,
    0xb0, 0x54, 0xbb, 0x16};

static int rC[10] = {0x01, 0x02, 0x04, 0x08, 0x10,
                     0x20, 0x40, 0x80, 0x1b, 0x36};

// 基本运算
int aesMult(int a, int b) {
    int res = 0;
    int temp;

    // 如果b的最低位为1，将a与res进行异或
    if (b & 1) {
        res = a ^ res;
    }
    // 如果b的第2位为1，将a左移1位后与res进行异或，如果超过256则应用AES的模运算
    if (b & 2) {
        temp = (a << 1) ^ (a << 1 >= 256 ? 0x11b : 0);
        res = temp % 256 ^ res;
    }
    // 如果b的第3位为1，将a左移2位后与res进行异或，如果超过256则应用AES的模运算
    if (b & 4) {
        temp = (a << 2) ^ ((a << 1) >= 256 ? 0x11b : 0) ^
               (a << 1 >= 256 ? 0x11b : 0);
        res = temp % 256 ^ res;
    }
    // 如果b的第4位为1，将a左移3位后与res进行异或，如果超过256则应用AES的模运算
    if (b & 8) {
        temp = (a << 3) ^ ((a << 2) >= 256 ? 0x11b : 0) ^
               ((a << 1) >= 256 ? 0x11b : 0) ^ (a >= 256 ? 0x11b : 0);
        res = temp % 256 ^ res;
    }

    return res;
}

void keyExpansion(int key[4][4], int w[11][4][4]) {
    int a = 0;
    while (a < 4) {
        int b = 0;
        while (b < 4) {
            w[0][a][b] = key[b][a];
            b++;
        }
        a++;
    }
    for (int i = 1; i < 11; ++i) {
        for (int j = 0; j < 4; ++j) {
            int temp[4];
            if (j == 0) {
                temp[0] = w[i - 1][3][1];
                temp[1] = w[i - 1][3][2];
                temp[2] = w[i - 1][3][3];
                temp[3] = w[i - 1][3][0];
                for (int k = 0; k < 4; ++k) {
                    int m = temp[k];
                    int row = m / 16;
                    int col = m % 16;
                    temp[k] = S[row][col];
                    if (k == 0) {
                        temp[k] = temp[k] ^ rC[i - 1];
                    }
                }
            } else {
                temp[0] = w[i][j - 1][0];
                temp[1] = w[i][j - 1][1];
                temp[2] = w[i][j - 1][2];
                temp[3] = w[i][j - 1][3];
            }
            for (int x = 0; x < 4; x++) {
                w[i][j][x] = w[i - 1][j][x] ^ temp[x];
            }
        }
    }
}

void byteSub(int in[4][4], int type) {
    int i, j, temp, row, col;

    i = 0;
    while (i < 4) {
        j = 0;
        while (j < 4) {
            temp = in[i][j];
            row = temp / 16;
            col = temp % 16;

            in[i][j] = S[row][col];

            j++;
        }
        i++;
    }
}

void shiftRow(int in[4][4], int type) {
    int i, j;

    i = 0;
    while (i < 4) {
        j = 0;
        while (j < i) {
            int temp = in[i][0];
            in[i][0] = in[i][1];
            in[i][1] = in[i][2];
            in[i][2] = in[i][3];
            in[i][3] = temp;
            j++;
        }
        i++;
    }
}

void mixCol(int in[4][4], int type) {
    int i, j;

    i = 0;
    while (i < 4) {
        int t0 = in[0][i];
        int t1 = in[1][i];
        int t2 = in[2][i];
        int t3 = in[3][i];

        // 计算新的列值
        in[0][i] = aesMult(t0, 2) ^ aesMult(t1, 3) ^ t2 ^ t3;
        in[1][i] = t0 ^ aesMult(t1, 2) ^ aesMult(t2, 3) ^ t3;
        in[2][i] = t0 ^ t1 ^ aesMult(t2, 2) ^ aesMult(t3, 3);
        in[3][i] = aesMult(t0, 3) ^ t1 ^ t2 ^ aesMult(t3, 2);

        i++;
    }
}

void addRoundKey(int in[4][4], int key[4][4]) {
    int i, j;

    i = 0;
    while (i < 4) {
        j = 0;
        while (j < 4) {
            in[i][j] = in[i][j] ^ key[j][i];
            j++;
        }
        i++;
    }
}

void printPlaintext(int plaintext[4][4]) {
    int i = 0, j;
    while (i < 4) {
        j = 0;
        while (j < 4) {
            if (plaintext[j][i] < 16)
                cout << "0"; // 在输出小于16的数字前加零
            cout << hex << uppercase
                 << plaintext[j][i]; // 输出16进制，并转换为大写
            j++;
        }
        i++;
    }
    cout << endl; // 输出换行符，便于格式化
}

int main() {
    uint8_t byteplaintext[4][4];
    uint8_t bytekey[4][4];

    int plaintext[4][4];
    int key[4][4];

    string plaintextinput = "00112233445566778899aabbccddeeff";
    string keyinput = "000102030405060708090a0b0c0d0e0f";

    // cin >> keyinput;
    // cin >> plaintextinput;

    int i, j;
    int subKey[11][4][4];

    // 处理密钥
    i = 0;
    while (i < 4) {
        j = 0;
        while (j < 4) {
            // 计算要提取的两个字符的起始索引
            int index = (i * 4 + j) * 2;
            // 提取两个字符的子字符串
            string subStr = keyinput.substr(index, 2);
            // 将十六进制字符串转换为整数
            int value = std::stoi(subStr, nullptr, 16);
            // 将整数转换为uint8_t并存储
            bytekey[j][i] = static_cast<uint8_t>(value);
            // 复制值到plaintext数组
            key[j][i] = bytekey[j][i];
            j++;
        }
        i++;
    }

    // 处理文本
    i = 0;
    while (i < 4) {
        j = 0;
        while (j < 4) {
            // 计算要提取的两个字符的起始索引
            int index = (i * 4 + j) * 2;
            // 提取两个字符的子字符串
            string subStr = plaintextinput.substr(index, 2);
            // 将十六进制字符串转换为整数
            int value = std::stoi(subStr, nullptr, 16);
            // 将整数转换为uint8_t并存储
            byteplaintext[j][i] = static_cast<uint8_t>(value);
            // 复制值到plaintext数组
            plaintext[j][i] = byteplaintext[j][i];
            j++;
        }
        i++;
    }

    keyExpansion(key, subKey);

    cout << "current_key" << endl;
    printPlaintext(subKey[0]);
    addRoundKey(plaintext, subKey[0]);
    // DEBUG
    std::cout << std::endl
              << "Round1------------------------------" << std::endl;

    i = 1;
    while (i <= 10) {
        cout << "add_round_key" << endl;
        printPlaintext(plaintext);

        byteSub(plaintext, 1);
        cout << "byte_sub" << endl;
        printPlaintext(plaintext);

        shiftRow(plaintext, 1);
        cout << "shift_row" << endl;
        printPlaintext(plaintext);

        if (i != 10) {
            mixCol(plaintext, 1);
            cout << "mix_col" << endl;
            printPlaintext(plaintext);
        }

        cout << endl
             << "Round" << i + 1 << "------------------------------" << endl;

        cout << "current_key" << endl;
        printPlaintext(subKey[i]);
        addRoundKey(plaintext, subKey[i]);
        cout << "add_round_key" << endl;
        printPlaintext(plaintext);
        i++;
    }

    i = 0;
    while (i < 4) {
        j = 0;
        while (j < 4) {
            if (plaintext[j][i] < 16)
                cout << "0";
            cout << hex << uppercase << plaintext[j][i];
            j++;
        }
        i++;
    }

    return 0;
}