#include <cassert>
#include <iostream>
#include <string>
#include <vector>

// 大整数存储于内部数组中，构造时输入字符串格式的数字，重载运算符加减乘
class BigIntArray {
  public:
    std::vector<int> num;
    int length;
    bool isNegative;

    BigIntArray() : length(0), isNegative(false) {}
    BigIntArray(std::string num_str) {
        // 逆序存储大数
        if (num_str[0] == '-') {
            isNegative = true;
            length = num_str.size() - 1;
            for (int i = num_str.size() - 1; i > 0; i--) { // 从后往前
                num.push_back(num_str[i] - '0');
            }
        } else {
            isNegative = false;
            length = num_str.size();
            for (int i = num_str.size() - 1; i >= 0; i--) {
                num.push_back(num_str[i] - '0');
            }
        }
    }

    // 加法重载
    BigIntArray operator+(const BigIntArray &right) {
        BigIntArray result;

        // 判断绝对值大小
        bool this_larger = true;
        if (length > right.length) {
        } else if (right.length > length) {
            this_larger = false;
        } else {
            for (int i = length - 1; i >= 0; i--) {
                if (num[i] != right.num[i]) {
                    this_larger = num[i] > right.num[i] ? true : false;
                }
            }
        }
        // 同号相加
        if (isNegative == right.isNegative) {
            int carry = 0;
            if (this_larger) {
                // 先加两个数共有的位
                for (int i = 0; i < right.length; i++) {
                    int sum = num[i] + right.num[i] + carry;
                    result.num.push_back(sum % 10);
                    carry = sum / 10;
                }
                // 再加大数独有的位
                for (int i = right.length; i < length; i++) {
                    int sum = num[i] + carry;
                    result.num.push_back(sum % 10);
                    carry = sum / 10;
                }
                // 最后检查进位
                if (carry != 0) {
                    result.num.push_back(carry);
                    carry = 0;
                }
            } else {
                for (int i = 0; i < length; i++) {
                    int sum = num[i] + right.num[i] + carry;
                    result.num.push_back(sum % 10);
                    carry = sum / 10;
                }
                for (int i = length; i < right.length; i++) {
                    int sum = num[i] + carry;
                    result.num.push_back(sum % 10);
                    carry = sum / 10;
                }
                if (carry != 0) {
                    result.num.push_back(carry);
                    carry = 0;
                }
            }
            result.isNegative = isNegative; // 结果符号与操作数相同
        }
        // 异号相加：转换为减法
        else {
            int borrow = 0;
            if (this_larger) {
                // |this| >= |right|，用this减right
                for (int i = 0; i < right.length; i++) {
                    int diff = num[i] - right.num[i] - borrow;
                    if (diff < 0) {
                        diff += 10;
                        borrow = 1;
                    } else {
                        borrow = 0;
                    }
                    result.num.push_back(diff);
                }
                // 处理被减数剩余的位
                for (int i = right.length; i < length; i++) {
                    int diff = num[i] - borrow;
                    if (diff < 0) {
                        diff += 10;
                        borrow = 1;
                    } else {
                        borrow = 0;
                    }
                    result.num.push_back(diff);
                }
                // 确定符号
                result.isNegative = isNegative;
            } else {
                // |right| > |this|，用right减this
                for (int i = 0; i < length; i++) {
                    int diff = right.num[i] - num[i] - borrow;
                    if (diff < 0) {
                        diff += 10;
                        borrow = 1;
                    } else {
                        borrow = 0;
                    }
                    result.num.push_back(diff);
                }
                for (int i = length; i < right.length; i++) {
                    int diff = right.num[i] - borrow;
                    if (diff < 0) {
                        diff += 10;
                        borrow = 1;
                    } else {
                        borrow = 0;
                    }
                    result.num.push_back(diff);
                }
                result.isNegative = right.isNegative;
            }
        }
        // 移除结果尾部多余的0
        while (result.num.back() == 0) {
            result.num.pop_back();
        }
        result.length = result.num.size();

        return result;
    }

    // 减法重载（本质还是加法）
    BigIntArray operator-(const BigIntArray &right) {
        BigIntArray new_right = right;
        new_right.isNegative = !new_right.isNegative;
        return *this + new_right;
    }

    // 乘法重载
    BigIntArray operator*(const BigIntArray &right) {
        BigIntArray result;
        // 确定符号
        result.isNegative = this->isNegative == right.isNegative ? false : true;
    }

    void show() {
        if (isNegative && !(length == 1 && num[0] == 0)) {
            std::cout << '-';
        }
        for (int i = length - 1; i >= 0; i--) {
            std::cout << num[i];
        }
    }
};

class BigIntList {};

// 自动化测试函数
void runAutomatedTests() {
    std::cout << "=== 开始自动化测试 ===" << std::endl;

    struct TestCase {
        std::string a;
        std::string op;
        std::string b;
        std::string expected;
        std::string description;
    };

    std::vector<TestCase> testCases = {
        // 基本正数测试
        {"123", "+", "456", "579", "正数加法"},
        {"1000", "-", "1", "999", "正数减法（无借位）"},
        {"100", "-", "50", "50", "正数减法"},

        // 负数测试
        {"-123", "+", "-456", "-579", "负数加法"},
        {"-100", "-", "-50", "-50", "负数减法"},
        {"-123", "+", "456", "333", "负正加法"},
        {"123", "+", "-456", "-333", "正负加法"},

        // 边界情况
        {"0", "+", "0", "0", "零加零"},
        {"0", "-", "0", "0", "零减零"},
        {"123", "+", "0", "123", "加零"},
        {"123", "-", "0", "123", "减零"},
        {"0", "+", "456", "456", "零加正数"},
        {"0", "-", "456", "-456", "零减正数"},

        // 进位和借位测试
        {"999", "+", "1", "1000", "进位测试"},
        {"1000", "-", "1", "999", "借位测试"},
        {"555", "+", "555", "1110", "多次进位"},

        // 大数测试
        {"123456789", "+", "987654321", "1111111110", "大数加法"},
        {"1000000000", "-", "1", "999999999", "大数减法"},
    };

    int passed = 0;
    int failed = 0;

    for (const auto &test : testCases) {
        BigIntArray num1(test.a);
        BigIntArray num2(test.b);
        BigIntArray result;

        if (test.op == "+") {
            result = num1 + num2;
        } else if (test.op == "-") {
            result = num1 - num2;
        }

        // 转换为字符串进行比较
        std::string resultStr;
        if (result.isNegative && !(result.length == 1 && result.num[0] == 0)) {
            resultStr += '-';
        }
        for (int i = result.length - 1; i >= 0; i--) {
            resultStr += std::to_string(result.num[i]);
        }

        if (resultStr == test.expected) {
            std::cout << "✓ PASS: " << test.description << std::endl;
            std::cout << "   " << test.a << " " << test.op << " " << test.b
                      << " = " << resultStr << std::endl;
            passed++;
        } else {
            std::cout << "✗ FAIL: " << test.description << std::endl;
            std::cout << "   " << test.a << " " << test.op << " " << test.b
                      << " = " << resultStr << " (期望: " << test.expected
                      << ")" << std::endl;
            failed++;
        }
        std::cout << std::endl;
    }

    std::cout << "=== 测试结果 ===" << std::endl;
    std::cout << "通过: " << passed << " / 失败: " << failed
              << " / 总计: " << (passed + failed) << std::endl;
    std::cout << "成功率: " << (passed * 100.0 / (passed + failed)) << "%"
              << std::endl;
}

// 交互式测试函数
void runInteractiveTest() {
    std::string str1, str2, str3;
    std::cout << "请输入表达式 (例如: 123 + 456 或 -100 - 50): ";
    std::cin >> str1 >> str2 >> str3;

    BigIntArray num1(str1);
    BigIntArray num2(str3);
    BigIntArray num3;

    if (str2 == "+") {
        num3 = num1 + num2;
    } else if (str2 == "-") {
        num3 = num1 - num2;
    } else {
        std::cout << "不支持的操作符: " << str2 << std::endl;
        return;
    }

    std::cout << "计算结果: ";
    num1.show();
    std::cout << ' ' << str2 << ' ';
    num2.show();
    std::cout << " = ";
    num3.show();
    std::cout << std::endl;
}

int main() {
    int choice;
    std::cout << "选择测试模式:" << std::endl;
    std::cout << "1. 自动化测试" << std::endl;
    std::cout << "2. 交互式测试" << std::endl;
    std::cout << "请输入选择 (1 或 2): ";
    std::cin >> choice;

    if (choice == 1) {
        runAutomatedTests();
    } else if (choice == 2) {
        runInteractiveTest();
    } else {
        std::cout << "无效选择!" << std::endl;
    }

    return 0;
}