#include <cassert>
#include <iostream>
#include <sstream>
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
        while (result.num.size() > 0 && result.num.back() == 0) {
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
        if (length == 0 || right.length == 0) {
            return result;
        }
        // 确定符号
        result.isNegative = this->isNegative == right.isNegative ? false : true;
        // 初始化结果数组
        result.num = std::vector<int>(length + right.length, 0);
        // this 每位数的索引
        for (int i = 0; i < length; i++) {
            // right 每位数的索引
            for (int j = 0; j < right.length; j++) {
                // 将乘积加到结果对应的位上
                result.num[i + j] += num[i] * right.num[j];
            }
        }
        // 处理进位
        int carry = 0;
        for (int i = 0; i < result.num.size(); i++) {
            result.num[i] += carry;
            carry = result.num[i] / 10;
            result.num[i] = result.num[i] % 10;
        }
        // 如果还有进位，继续添加到后面
        while (carry) {
            result.num.push_back(carry % 10);
            carry /= 10;
        }
        // 处理多余的0
        while (result.num.size() > 0 && result.num.back() == 0) {
            result.num.pop_back();
        }
        result.length = result.num.size();
        // 如果为0，调整符号为正
        if (result.length == 1 && result.num[0] == 0) {
            result.isNegative = false;
        }

        return result;
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

struct BigIntNode {
    BigIntNode *next;
    int value;

    BigIntNode() : value(0), next(nullptr) {}
    BigIntNode(char c) : value(c - '0'), next(nullptr) {}
    BigIntNode(int d) : value(d), next(nullptr) {}
};

class BigIntList {
  private:
    // 指向个位
    BigIntNode *head;
    int length;
    bool isNegative;

    BigIntList() : head(nullptr), length(0), isNegative(false) {}
    BigIntList(std::string num_str) {
        BigIntNode *cur_ptr = head;
        // 逆序存储大数
        if (num_str[0] == '-') {
            isNegative = true;
            length = num_str.size() - 1;
            for (int i = num_str.size() - 1; i > 0; i--) { // 从后往前
                cur_ptr->next = new BigIntNode(num_str[i] - '0');
                cur_ptr = cur_ptr->next;
            }
        } else {
            isNegative = false;
            length = num_str.size();
            for (int i = num_str.size() - 1; i >= 0; i--) {
                cur_ptr->next = new BigIntNode(num_str[i] - '0');
                cur_ptr = cur_ptr->next;
            }
        }
    }

    BigIntList *operator+(const BigIntList *right) {
        // 处理特殊情况
        if (length == 0) {
            return right;
        } else if (right->length == 0) {
            return this;
        }

        int carry = 0;
        BigIntList *result;
        BigIntNode *self_node = head;
        BigIntNode *right_node = right->head;
        BigIntNode *result_node = result->head;

        // 同号
        if (isNegative == right->isNegative) {
            result->isNegative = isNegative;
            while (self_node != nullptr || right_node != nullptr) {
                int curr = self_node->value + right_node->value + carry;
                // 处理进位
                if (curr > 9) {
                    carry = curr / 10;
                    curr = curr % 10;
                } else {
                    carry = 0;
                }
                // 添加进结果里
                result_node->next = *BigIntNode(curr);
                result->length++;

                self_node = self_node->next;
                right_node = right_node->next;
                result_node = result_node->next;
            }
        }
        // 异号
        else {
            // 判断绝对值大小
            bool this_larger = true;
            if (length != right->length) {
                if (length > right->length) {
                    this_larger = true;
                    // 结果符号确定
                    result->isNegative = isNegative;
                } else {
                    result->isNegative = right->isNegative;
                    this_larger = false;
                }
            } else {
                while (self_node != nullptr || right_node != nullptr) {
                    if (self_node->value > right_node->value)
                        this_larger = true;
                    else
                        this_larger = false;
                    self_node = self_node->next;
                    right_node = right_node->next;
                }
            }

            self_node = head;
            right_node = right->head;
            result_node = result->head;

            // this - right
            if (this_larger) {
                while (self_node != nullptr || right_node != nullptr) {
                    int curr = self_node->value - right_node->value + carry;
                    if (curr < 0) {
                        carry = -1;
                        curr += 10;
                    } else {
                        carry = 0;
                    }

                    result_node->next = BigIntNode(curr);
                    result->length++;

                    self_node = self_node->next;
                    right_node = right_node->next;
                    result_node = result_node->next;
                }
            }
            // right - this
            else {
                while (self_node != nullptr || right_node != nullptr) {
                    int curr = right_node->value - self_node->value + carry;
                    if (curr < 0) {
                        carry = -1;
                        curr += 10;
                    } else {
                        carry = 0;
                    }

                    result_node->next = BigIntNode(curr);
                    result->length++;

                    self_node = self_node->next;
                    right_node = right_node->next;
                    result_node = result_node->next;
                }
            }
        }
        return result;
    }

    BigIntList operator-(const BigIntList *right) {}
    BigIntList operator*(const BigIntList *right) {}
};

// 测试函数
void runAutomatedTests() {
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

        // 乘法测试 - 新增部分
        {"2", "*", "3", "6", "基本乘法"},
        {"0", "*", "123", "0", "零乘正数"},
        {"123", "*", "0", "0", "正数乘零"},
        {"-5", "*", "3", "-15", "负数乘正数"},
        {"5", "*", "-3", "-15", "正数乘负数"},
        {"-5", "*", "-3", "15", "负数乘负数"},
        {"123", "*", "456", "56088", "两位数乘法"},
        {"999", "*", "999", "998001", "三位数乘法"},
        {"1000", "*", "1000", "1000000", "带零乘法"},
        {"123456789", "*", "987654321", "121932631112635269", "大数乘法"},
        {"999999999", "*", "999999999", "999999998000000001",
         "大数乘法（进位）"},
        {"1234", "*", "5678", "6906652", "四位数乘法"},
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
        } else if (test.op == "*") { // 新增乘法处理
            result = num1 * num2;
        } else {
            std::cout << "未知操作符: " << test.op << std::endl;
            failed++;
            continue;
        }

        // 使用stringstream捕获show()的输出
        std::stringstream ss;
        // 保存cout的缓冲区，重定向到stringstream
        auto cout_buf = std::cout.rdbuf(ss.rdbuf());
        // 调用show()方法，输出会被捕获到ss中
        result.show();
        // 恢复cout的缓冲区
        std::cout.rdbuf(cout_buf);

        // 获取捕获的字符串
        std::string resultStr = ss.str();

        if (resultStr == test.expected) {
            passed++;
            std::cout << "[√] " << test.description << ": " << test.a << " "
                      << test.op << " " << test.b << " = " << resultStr
                      << std::endl;
        } else {
            failed++;
            std::cout << "[×] " << test.description << ": " << test.a << " "
                      << test.op << " " << test.b << " 预期: " << test.expected
                      << " 实际: " << resultStr << std::endl;
        }
    }

    // 输出测试总结
    std::cout << "\n测试总结: " << std::endl;
    std::cout << "总测试用例: " << testCases.size() << std::endl;
    std::cout << "通过: " << passed << std::endl;
    std::cout << "失败: " << failed << std::endl;
}

int main() {
    runAutomatedTests();

    return 0;
}