#include <iostream>

// 计算 (x * y) % p
unsigned long long mulmod(unsigned long long x, unsigned long long y,
                          unsigned long long p) {
    unsigned long long result = 0;
    x = x % p;
    while (y > 0) {
        if (y % 2 == 1) {
            result = (result + x) % p;
        }
        x = (x * 2) % p;
        y /= 2;
    }
    return result;
}

// 计算 (x ^ y) % p
unsigned long long power(unsigned long long x, unsigned long long y,
                         unsigned long long p) {
    unsigned long long result = 1;
    x = x % p;
    while (y > 0) {
        if (y % 2 == 1) {
            result = mulmod(result, x, p);
        }
        x = mulmod(x, x, p);
        y /= 2;
    }
    return result;
}

bool miller_rabin(unsigned long long n, int k) {
    // 初步检查
    // 如果 n 小于等于 1，返回 false
    if (n <= 1)
        return false;
    // 如果 n 等于 2 或 3，返回 true
    else if (n == 2 || n == 3)
        return true;

    // 将 n-1 表示为 d * 2^r 的形式
    // 令 d = n - 1，然后不断将 d 除以 2，直到 d 变成奇数。记录除以 2 的次数 r
    unsigned long long d = n - 1;
    int r = 0;
    while (d % 2 == 0) {
        d /= 2;
        r++;
    }

    // 重复 k 次测试
    for (int i = 0; i < k; i++) {
        unsigned long long a = 2 + rand() % (n - 3);
        unsigned long long x = power(a, d, n);
        if (x == 1 || x == n - 1)
            continue;

        bool isComposite = true;
        for (int j = 0; j < r - 1; j++) {
            x = mulmod(x, x, n);
            if (x == n - 1) {
                isComposite = false;
                break;
            }
            if (x == 1)
                return false;
        }

        if (isComposite)
            return false;
    }

    return true;
}

int main() {
    int k = 20;
    unsigned long long n;
    std::cin >> n;
    bool result = miller_rabin(n, k);
    if (result) {
        std::cout << "Yes";
    } else {
        std::cout << "No";
    }
    return 0;
}