#include <iostream>
#include <tuple>

using namespace std;

// 椭圆曲线参数
const int MOD = 11; // 素数p
const int a = 1;    // 椭圆曲线参数a
const int b = 6;    // 椭圆曲线参数b
const int q = 13;   // 阶数

// 计算模逆，使用扩展欧几里得算法
int modInverse(int k, int mod) {
    int m0 = mod, t, q;
    int x0 = 0, x1 = 1;

    if (mod == 1) return 0;

    while (k > 1) {
        q = k / mod;
        t = mod;
        mod = k % mod, k = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0) x1 += m0;

    return x1;
}

// 计算模运算
int mod(int a, int b) {
    return (a % b + b) % b;
}

// 椭圆曲线点加法
tuple<int, int> pointAddition(int x1, int y1, int x2, int y2) {
    if (x1 == x2 && (y1 + y2) % MOD == 0) {
        return make_tuple(0, 0);
    }

    int lambda;
    if (x1 == x2 && y1 == y2) {
        int numerator = mod(3 * x1 * x1 + a, MOD);
        int denominator = modInverse(mod(2 * y1, MOD), MOD);
        if (denominator == -1) throw invalid_argument("Modular inverse does not exist");
        lambda = mod(numerator * denominator, MOD);
    } else {
        int numerator = mod(y2 - y1, MOD);
        int denominator = modInverse(mod(x2 - x1, MOD), MOD);
        if (denominator == -1) throw invalid_argument("Modular inverse does not exist");
        lambda = mod(numerator * denominator, MOD);
    }

    int x3 = mod(lambda * lambda - x1 - x2, MOD);
    int y3 = mod(lambda * (x1 - x3) - y1, MOD);
    return make_tuple(x3, y3);
}

// 椭圆曲线点乘法
tuple<int, int> pointMultiplication(int k, int x, int y) {
    int xR = x, yR = y;
    k--;
    while (k > 0) {
        tie(xR, yR) = pointAddition(xR, yR, x, y);
        k--;
    }
    return make_tuple(xR, yR);
}

// 哈希函数 2^x mod q
int hashFunction(int x) {
    return (1 << x) % q;
}

int main() {
    // 输入参数
    int x0, y0, m, x, k;
    cin >> x0 >> y0;
    cin >> m >> x >> k;

    // 计算 kA
    int u, v;
    tie(u, v) = pointMultiplication(k, x0, y0);

    // 计算 r
    int r = u % q;

    // 计算哈希值
    int hashX = hashFunction(x);

    // 计算 s
    int kInv = modInverse(k, q);
    int s = kInv * (hashX + m * r) % q;

    // 输出签名
    cout << r << " " << s;

    return 0;
}