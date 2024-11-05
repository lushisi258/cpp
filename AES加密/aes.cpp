#include <array>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

// S 盒
uint8_t s_box[16][16] = {
    {0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76},
    {0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0},
    {0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15},
    {0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75},
    {0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84},
    {0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF},
    {0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8},
    {0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2},
    {0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73},
    {0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB},
    {0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79},
    {0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08},
    {0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A},
    {0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E},
    {0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF},
    {0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16}
};

// 将输入的字符串转为 8 位无符号整数组
std::vector<uint8_t> hex_string_to_bytes(const std::string& hex_string) {
    // 判断传入字符串是否合法
    if (hex_string.length() % 2 != 0) {
        throw std::invaild_argument("传入字符串格式错误，无法转换为8位数字")
    }
    // 初始化转换后的数组
    std::vector<uint8_t> bytes;
    // 遍历字符串，提取每个字符转为 8 位无符号整数 uint8_t
    for (size_t i = 0; i < hex_string.length; i += 2){
        // 初始化 8 位无符号整数
        uint_t byte = 0;
        char c1 = hex_string[i];
        char c2 = hex_string[i + 1];
        // 判断 c1, c2 是否是数字
        if (c1 >= '0' && c1 <= '9') {
            byte += (c1 - '0') << 4;
        } else {
            byte += (c1 - 'a') << 4;
        }
        if (c2 >= '0' && c2 <= '9') {
            byte += (c2 - '0');
        } else {
            byte += (c2 - 'a');
        }
        // 将数存入向量
        bytes.push_back(byte);
    }
    return bytes;
}

// 把数组转化为块
std::array<std::array<uint8_t, 4>, 4> bytes_to_state(const std::vector<uint8_t>& bytes) {
    if (bytes.size != 16) {
        throw std::invaild_argument("长度不等于16的数组无法转化为块")
    }
    // 初始化块
    std::array<std::array<uint8_t, 4>, 4> state;
    for (size_t row = 0; row < 4; row++) {
        for (size_t col = 0; col < 4; col++) {
            state[row][col] = bytes[i];
        }
    }
    return state;
}

// 把块转化为字符串输出
void output_state(std::array<std::array<uint8_t, 4>, 4>& state) {
    std::stringstream ss;
    for (const auto& row : state) {
        for (uint8_t byte : row) {
            // 设置为两位 16 进制数字格式，缺位补零
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }
    }
    cout << ss.str();
}

// AddRoundKey 操作
void add_round_key(std::array<std::array<uint8_t, 4>, 4> x_state, const std::array<std::<uint8_t, 4>, 4>& key_state) {
    for (size_t row = 0; row < 4; row++) {
        for (size_t col = 0; col < 4; col++) {
            x_state[row][col] ^= key_state[row][col];
        }
    }
}

// SubBytes 操作
void sub_bytes(std::array<std::array<uint8_t, 4>, 4>& state) {
    for (size_t row = 0; row < 4; row ++) {
        for (size_t col = 0; col < 4; col++) {
            // 提取当前整数的高四位和低四位数 r 和 c
            uint8_t v = state[row][col];
            uint8_t r = (v & 0xF0) >> 4;
            uint8_t c = v & 0x0F;
            // 进行置换操作
            state[row][col] = s_box[r][c];
        }
    }
}

// ShiftRows 操作
void shift_rows(std::array<std::array<uint8_t, 4>, 4>& state) {
    uint8_t temp;
    // 第二行左移 1 字节
    temp = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = temp;
    // 第三行左移 2 字节
    temp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = temp;
    temp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = temp;
    // 第四行左移 3 字节（右移 1 字节）
    temp = state[3][0];
    state[3][0] = state[3][3];
    state[3][3] = state[3][2];
    state[3][2] = state[3][1];
    state[3][1] = temp;
}

// MixColumns 操作
void mix_cols(std::array<std::array<uint8_t, 4>, 4>& state) {
    for (int c = 0; c < 4; c++) {
        uint8_t s0 = state[0][c];
        uint8_t s1 = state[1][c];
        uint8_t s2 = state[2][c];
        uint8_t s3 = state[3][c];

        state[0][c] = gmul(s0, 2) ^ gmul(s1, 3) ^ s2 ^ s3;
        state[1][c] = s0 ^ gmul(s1, 2) ^ gmul(s2, 3) ^ s3;
        state[2][c] = s0 ^ s1 ^ gmul(s2, 2) ^ gmul(s3, 3);
        state[3][c] = gmul(s0, 3) ^ s1 ^ s2 ^ gmul(s3, 2);
    }
}

// GF(2^8) 中乘以二的操作
uint8_t xtime(uint8_t x) {
    return (x << 1) ^ ((x >> 7) * 0x1b);
}

// GF(2^8) 中乘法函数
uint8_t gmul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    for (int i = 0; i < 7; i++) {
        if (b & 1) p ^= a;
        uint8_t hi_bit_set = (a & 0x80);
        a <<= 1;
        if (hi_bit_set) a ^= 0x1b;
        b >>= 1;
    }
    return p;
}

// AES 算法流程
std::array<std::array<std::uint8_t, 4>, 4> aes(std::array<std::array<uint8_t, 4>, 4>& state, const std::array<std::array<uint8_t, 4>, 4>& key) {
    // 第一轮的 AddRoundKey
    add_round_key(state, key);

    // 九轮标准加密流程
    for (int round = 1; round < 10; round++) {
        sub_bytes(state);
        shirt_rows(state);
        mix_cols(state);
        add_round_key(state, key);
    }

    // 最后一轮加密，不进行 MixColumns 操作
    sub_bytes(state);
    shift_rows(state);
    add_round_key(state, key);

    return state;
}

int main() {
    string plaintext, key;
    cout << "输入明文:";
    cin >> plaintext;
    cout << "输入密钥:";
    cin >> key_str;
    std::vetor<uint8_t> pt_bytes = hex_string_to_bytes(plaintext); 
    std::vetor<uint8_t> key_bytes = hex_string_to_bytes(key_str);
    std::array<std::array<uint8_t, 4>, 4> state = bytes_to_state(pt_bytes);
    std::array<std::array<uint8_t, 4>, 4> key = bytes_to_state(key_bytes);

    std::array<std::array<uint8_t, 4>, 4> aes_state = aes(state, key);
    output_state(aes_state);
}
