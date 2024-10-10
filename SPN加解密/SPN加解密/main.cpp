#include <iostream>
#include <vector>

using namespace std;

const int BLOCK_SIZE = 4, SUB_BLOCK_SIZE = 4, NUM_ROUNDS = 4;
int key[32];
int plaintext[16];
int ciphertext[16];

int sBox[16] = { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 };
int pBox[16] = { 0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15 };

int substitute(int input) {
    return (input >= 0 && input < 16) ? sBox[input] : -1;
}

int permute(int input) {
    return (input >= 1 && input <= 16) ? pBox[input - 1] : -1;
}

int* generateRoundKey(int round) {
    int startPoint = (4 * round - 4) % 32;
    int* roundKey = new int[32];

    for (int i = 0, j = startPoint; i < 32; ++i) {
        roundKey[i] = key[j];
        j = (j + 1) % 32;
    }

    return roundKey;
}

int main() {
    int plaintextIndex = 0;

    while (plaintextIndex < 16) {
        char c = getchar();
        if (c == '0' || c == '1') {
            plaintext[plaintextIndex] = c - '0';
            plaintextIndex++;
        }
    }

    int keyIndex = 0;

    while (keyIndex < 32) {
        char c = getchar();
        if (c == '0' || c == '1') {
            key[keyIndex] = c - '0';
            keyIndex++;
        }
    }

    int round = 1;
    int tempW[16], tempU[16], tempV[16];
    int i = 0;

    while (i < 16) {
        tempW[i] = plaintext[i];
        i++;
    }

    while (round <= NUM_ROUNDS) {
        int* roundKey = generateRoundKey(round);

        for (int j = 0; j < 16; ++j) {
            tempU[j] = tempW[j] ^ roundKey[j];
        }

        for (int j = 0; j < BLOCK_SIZE; ++j) {
            int combinedValue = 8 * tempU[j * SUB_BLOCK_SIZE] + 4 * tempU[j * SUB_BLOCK_SIZE + 1] + 2 * tempU[j * SUB_BLOCK_SIZE + 2] + tempU[j * SUB_BLOCK_SIZE + 3];
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
            int* nextRoundKey = generateRoundKey(round + 1);
            for (int j = 0; j < 16; ++j) {
                tempW[j] = tempV[j] ^ nextRoundKey[j];
            }
            delete[] nextRoundKey;
        }

        delete[] roundKey;
        round++;
    }

    for (int i = 0; i < 16; i++) {
        cout << tempW[i];
    }

    return 0;
}