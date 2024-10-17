#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

const int BLOCK_SIZE = 4, SUB_BLOCK_SIZE = 4, NUM_ROUNDS = 4;
int plaintext[16];
int ciphertext[16];

int key[32] = {0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0,
               1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1};
int sBox[16] = {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7};
int pBox[16] = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15};

// S盒置换
int substitute(int input) {
    return (input >= 0 && input < 16) ? sBox[input] : -1;
}

// P盒置换
int permute(int input) {
    return (input >= 1 && input <= 16) ? pBox[input - 1] : -1;
}

// 生成轮密钥
int *generateRoundKey(int round) {
    int startPoint = (4 * round - 4) % 32;
    int *roundKey = new int[32];

    for (int i = 0, j = startPoint; i < 32; ++i) {
        roundKey[i] = key[j];
        j = (j + 1) % 32;
    }

    return roundKey;
}

// 加密函数
void encrypt(int plaintext[16], int ciphertext[16]) {
    int round = 1;
    int tempW[16], tempU[16], tempV[16];
    int i = 0;

    while (i < 16) {
        tempW[i] = plaintext[i];
        i++;
    }

    while (round <= NUM_ROUNDS) {
        int *roundKey = generateRoundKey(round);

        for (int j = 0; j < 16; ++j) {
            tempU[j] = tempW[j] ^ roundKey[j];
        }

        for (int j = 0; j < BLOCK_SIZE; ++j) {
            int combinedValue = 8 * tempU[j * SUB_BLOCK_SIZE] +
                                4 * tempU[j * SUB_BLOCK_SIZE + 1] +
                                2 * tempU[j * SUB_BLOCK_SIZE + 2] +
                                tempU[j * SUB_BLOCK_SIZE + 3];
            int substitutedValue = substitute(combinedValue);
            tempV[j * SUB_BLOCK_SIZE] = substitutedValue / 8;
            substitutedValue %= 8;
            tempV[j * SUB_BLOCK_SIZE + 1] = substitutedValue / 4;
            substitutedValue %= 4;
            tempV[j * SUB_BLOCK_SIZE + 2] = substitutedValue / 2;
            substitutedValue %= 2;
            tempV[j * SUB_BLOCK_SIZE + 3] = substitutedValue;
        }

        if (round != NUM_ROUNDS) {
            for (int j = 0; j < 16; ++j) {
                int position = permute(j + 1);
                tempW[j] = tempV[position];
            }
        }

        if (round == NUM_ROUNDS) {
            int *nextRoundKey = generateRoundKey(round + 1);
            for (int j = 0; j < 16; ++j) {
                tempW[j] = tempV[j] ^ nextRoundKey[j];
            }
            delete[] nextRoundKey;
        }

        delete[] roundKey;
        round++;
    }

    for (int i = 0; i < 16; i++) {
        ciphertext[i] = tempW[i];
    }
}

// 生成随机明文
void generateRandomPlaintext(int plaintext[16]) {
    for (int i = 0; i < 16; ++i) {
        plaintext[i] = rand() % 2;
    }
}

// 将明文和密文写入CSV文件
void writeToCSV(const string &filename, const vector<vector<int>> &plaintexts,
                const vector<vector<int>> &ciphertexts) {
    ofstream file(filename);
    file << "Plaintext,Ciphertext\n";
    for (size_t i = 0; i < plaintexts.size(); ++i) {
        for (int bit : plaintexts[i]) {
            file << bit;
        }
        file << ",";
        for (int bit : ciphertexts[i]) {
            file << bit;
        }
        file << "\n";
    }
    file.close();
}

// 从CSV文件读取明文和密文
void readFromCSV(const string &filename, vector<vector<int>> &plaintexts,
                 vector<vector<int>> &ciphertexts) {
    ifstream file(filename);
    string line;
    getline(file, line); // 跳过标题行
    while (getline(file, line)) {
        vector<int> plaintext(16), ciphertext(16);
        size_t commaPos = line.find(',');
        string plaintextStr = line.substr(0, commaPos);
        string ciphertextStr = line.substr(commaPos + 1);

        for (int i = 0; i < 16; ++i) {
            plaintext[i] = plaintextStr[i] - '0';
            ciphertext[i] = ciphertextStr[i] - '0';
        }

        plaintexts.push_back(plaintext);
        ciphertexts.push_back(ciphertext);
    }
    file.close();
}

int main() {
    srand(time(0));

    vector<vector<int>> plaintexts;
    vector<vector<int>> ciphertexts;

    // 生成8000个随机明文并加密
    for (int i = 0; i < 8000; ++i) {
        generateRandomPlaintext(plaintext);
        encrypt(plaintext, ciphertext);

        plaintexts.push_back(vector<int>(plaintext, plaintext + 16));
        ciphertexts.push_back(vector<int>(ciphertext, ciphertext + 16));
    }

    // 将明文和密文写入CSV文件
    writeToCSV("encrypted_data.csv", plaintexts, ciphertexts);

    // 读取CSV文件中的明文和密文
    // vector<vector<int>> readPlaintexts, readCiphertexts;
    // readFromCSV("encrypted_data.csv", readPlaintexts, readCiphertexts);

    // 输出读取的明文和密文的数量
    // cout << "Read " << readPlaintexts.size() << " plaintexts and "
    //      << readCiphertexts.size() << " ciphertexts from CSV file." << endl;

    return 0;
}