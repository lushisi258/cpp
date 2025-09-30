#include <iostream>
#include <cmath>

// 使用 __int128 类型来处理大整数
using int128 = __int128;

// 函数用于计算 (base^exp) % mod
int128 mod_exp(int128 base, int128 exp, int128 mod) {
    int128 result = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return result;
}

// 函数用于计算 gcd(a, b) 和扩展欧几里得算法
int128 gcd_extended(int128 a, int128 b, int128 *x, int128 *y) {
    if (a == 0) {
        *x = 0;
        *y = 1;
        return b;
    }
    int128 x1, y1;
    int128 gcd = gcd_extended(b % a, a, &x1, &y1);
    *x = y1 - (b / a) * x1;
    *y = x1;
    return gcd;
}

// 函数用于计算模逆
int128 mod_inverse(int128 e, int128 phi) {
    int128 x, y;
    int128 g = gcd_extended(e, phi, &x, &y);
    if (g != 1) {
        std::cerr << "Inverse doesn't exist";
        return -1;
    } else {
        int128 res = (x % phi + phi) % phi;
        return res;
    }
}

// 用于输出 __int128 类型的函数
void print_int128(int128 n) {
    if (n == 0) {
        std::cout << "0";
        return;
    }
    if (n < 0) {
        std::cout << "-";
        n = -n;
    }
    std::string result;
    while (n > 0) {
        result = char('0' + n % 10) + result;
        n /= 10;
    }
    std::cout << result;
}

int main() {
    long long p, q, e, c;
    std::cin >> p >> q >> e >> c;
    int128 n = int128(p) * int128(q);
    int128 phi = int128(p - 1) * int128(q - 1);

    int128 d = mod_inverse(e, phi);
    if (d == -1) {
        return 1;
    }

    int128 m = mod_exp(c, d, n);

    print_int128(m);

    return 0;
}