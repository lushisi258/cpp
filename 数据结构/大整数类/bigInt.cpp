#include <algorithm>
#include <cassert>
#include <iomanip>
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
    int value;
    BigIntNode *next;
    BigIntNode(int v) : value(v), next(nullptr) {}
};

class BigIntList {
  private:
    BigIntNode *head; // 指向个位（链表首部）
    bool isNegative;

    // 清空链表释放内存
    void clear() {
        while (head) {
            BigIntNode *temp = head;
            head = head->next;
            delete temp;
        }
    }

    // 复制链表辅助函数
    BigIntNode *copyList(BigIntNode *otherHead) const {
        if (!otherHead)
            return nullptr;
        BigIntNode *newHead = new BigIntNode(otherHead->value);
        BigIntNode *cur = newHead;
        BigIntNode *src = otherHead->next;
        while (src) {
            cur->next = new BigIntNode(src->value);
            cur = cur->next;
            src = src->next;
        }
        return newHead;
    }

  public:
    BigIntList() : head(nullptr), isNegative(false) {}

    // 析构函数：防止内存泄漏
    ~BigIntList() { clear(); }

    // 拷贝构造函数：实现深拷贝
    BigIntList(const BigIntList &other) {
        isNegative = other.isNegative;
        head = copyList(other.head);
    }

    // 赋值运算符：先释放旧内存，再深拷贝
    BigIntList &operator=(const BigIntList &other) {
        if (this != &other) {
            clear();
            isNegative = other.isNegative;
            head = copyList(other.head);
        }
        return *this;
    }

    BigIntList(std::string num_str) : head(nullptr), isNegative(false) {
        if (num_str.empty() || num_str == "0") {
            push_back(0);
            return;
        }
        int start = 0;
        if (num_str[0] == '-') {
            isNegative = true;
            start = 1;
        }
        // 逆序插入：字符串末尾是个位
        for (int i = num_str.size() - 1; i >= start; --i) {
            push_back(num_str[i] - '0');
        }
        cleanZeros();
    }

    void push_back(int val) {
        if (!head) {
            head = new BigIntNode(val);
        } else {
            BigIntNode *cur = head;
            while (cur->next)
                cur = cur->next;
            cur->next = new BigIntNode(val);
        }
    }

    int getLength() const {
        int len = 0;
        BigIntNode *cur = head;
        while (cur) {
            len++;
            cur = cur->next;
        }
        return len;
    }

    // 移除高位多余的0（链表尾部的0）
    void cleanZeros() {
        if (!head)
            return;
        // 先反转找零，或者递归处理。这里使用简单方法：
        std::vector<int> v;
        BigIntNode *cur = head;
        while (cur) {
            v.push_back(cur->value);
            cur = cur->next;
        }
        while (v.size() > 1 && v.back() == 0)
            v.pop_back();

        clear();
        for (int val : v)
            push_back(val);
    }

    // 核心加法：使用哨兵节点简化操作
    BigIntList operator+(const BigIntList &right) const {
        if (this->isNegative == right.isNegative) {
            BigIntList res;
            res.isNegative = this->isNegative;
            BigIntNode dummy(0);
            BigIntNode *cur = &dummy;
            BigIntNode *p1 = this->head, *p2 = right.head;
            int carry = 0;

            while (p1 || p2 || carry) {
                int val1 = p1 ? p1->value : 0;
                int val2 = p2 ? p2->value : 0;
                int sum = val1 + val2 + carry;
                carry = sum / 10;
                cur->next = new BigIntNode(sum % 10);
                cur = cur->next;
                if (p1)
                    p1 = p1->next;
                if (p2)
                    p2 = p2->next;
            }
            res.head = dummy.next;
            return res;
        }
        return *this - (right.negated());
    }

    BigIntList operator-(const BigIntList &right) const {
        if (this->isNegative != right.isNegative)
            return *this + right.negated();

        int cmp = compareAbs(right);
        if (cmp == 0)
            return BigIntList("0");

        const BigIntList *max = (cmp > 0) ? this : &right;
        const BigIntList *min = (cmp > 0) ? &right : this;

        BigIntList res;
        res.isNegative = (cmp > 0) ? this->isNegative : !right.isNegative;

        BigIntNode dummy(0);
        BigIntNode *cur = &dummy;
        BigIntNode *p1 = max->head, *p2 = min->head;
        int borrow = 0;

        while (p1) {
            int val1 = p1->value;
            int val2 = p2 ? p2->value : 0;
            int sub = val1 - val2 - borrow;
            if (sub < 0) {
                sub += 10;
                borrow = 1;
            } else
                borrow = 0;
            cur->next = new BigIntNode(sub);
            cur = cur->next;
            p1 = p1->next;
            if (p2)
                p2 = p2->next;
        }
        res.head = dummy.next;
        res.cleanZeros();
        return res;
    }

    BigIntList operator*(const BigIntList &right) const {
        if (isZero() || right.isZero())
            return BigIntList("0");
        BigIntList res("0");
        BigIntNode *p2 = right.head;
        int shift = 0;

        while (p2) {
            BigIntList temp;
            for (int i = 0; i < shift; ++i)
                temp.push_back(0);
            int carry = 0;
            BigIntNode *p1 = this->head;
            while (p1 || carry) {
                int mul = (p1 ? p1->value : 0) * p2->value + carry;
                temp.push_back(mul % 10);
                carry = mul / 10;
                if (p1)
                    p1 = p1->next;
            }
            res = res + temp;
            p2 = p2->next;
            shift++;
        }
        res.isNegative = (this->isNegative != right.isNegative);
        return res;
    }

    BigIntList negated() const {
        BigIntList copy(*this);
        copy.isNegative = !this->isNegative;
        return copy;
    }

    int compareAbs(const BigIntList &right) const {
        int l1 = getLength(), l2 = right.getLength();
        if (l1 != l2)
            return l1 > l2 ? 1 : -1;
        std::string s1 = toAbsString(), s2 = right.toAbsString();
        if (s1 == s2)
            return 0;
        return s1 > s2 ? 1 : -1;
    }

    bool isZero() const { return !head || (head->value == 0 && !head->next); }

    std::string toAbsString() const {
        std::string s = "";
        BigIntNode *cur = head;
        while (cur) {
            s += std::to_string(cur->value);
            cur = cur->next;
        }
        std::reverse(s.begin(), s.end());
        return s.empty() ? "0" : s;
    }

    void show() const {
        if (isNegative && !isZero())
            std::cout << "-";
        std::cout << toAbsString();
    }
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
        {"123", "+", "456", "579", "基础加法"},
        {"1000", "-", "1", "999", "基础减法"},
        {"-123", "+", "456", "333", "异号加法"},
        {"123", "*", "456", "56088", "基础乘法"},
        {"999", "*", "999", "998001", "进位乘法"},
        {"123456789", "*", "987654321", "121932631112635269", "大数乘法"}};

    int arrayPassed = 0, listPassed = 0;

    std::cout << std::left << std::setw(20) << "测试描述" << std::setw(10)
              << "数组版" << std::setw(10) << "链表版" << std::endl;
    std::cout << std::string(45, '-') << std::endl;

    for (const auto &test : testCases) {
        // --- 测试 Array 实现 ---
        BigIntArray aArr(test.a), bArr(test.b), resArr;
        if (test.op == "+")
            resArr = aArr + bArr;
        else if (test.op == "-")
            resArr = aArr - bArr;
        else if (test.op == "*")
            resArr = aArr * bArr;

        std::stringstream ssArr;
        auto old_buf = std::cout.rdbuf(ssArr.rdbuf());
        resArr.show();
        std::cout.rdbuf(old_buf);
        bool arrayOk = (ssArr.str() == test.expected);
        if (arrayOk)
            arrayPassed++;

        // --- 测试 List 实现 ---
        BigIntList aList(test.a), bList(test.b), resList;
        if (test.op == "+")
            resList = aList + bList;
        else if (test.op == "-")
            resList = aList - bList;
        else if (test.op == "*")
            resList = aList * bList;

        std::stringstream ssList;
        old_buf = std::cout.rdbuf(ssList.rdbuf());
        resList.show();
        std::cout.rdbuf(old_buf);
        bool listOk = (ssList.str() == test.expected);
        if (listOk)
            listPassed++;

        // --- 输出对比结果 ---
        std::cout << std::left << std::setw(20) << test.description
                  << std::setw(10) << (arrayOk ? "[√]" : "[×]") << std::setw(10)
                  << (listOk ? "[√]" : "[×]") << std::endl;

        std::cout << "   -> 预期: " << test.expected << std::endl;
        std::cout << "   -> 数组实际: " << ssArr.str() << std::endl;
        std::cout << "   -> 链表实际: " << ssList.str() << std::endl;
    }

    std::cout << "\n================================" << std::endl;
    std::cout << "测试总结:" << std::endl;
    std::cout << "数组版通过: " << arrayPassed << "/" << testCases.size()
              << std::endl;
    std::cout << "链表版通过: " << listPassed << "/" << testCases.size()
              << std::endl;
}
int main() {
    runAutomatedTests();

    return 0;
}