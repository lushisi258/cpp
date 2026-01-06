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
    int value;        // 存储当前位上的数字（0-9）
    BigIntNode *next; // 指向下一位（更高位）的指针
    BigIntNode(int v) : value(v), next(nullptr) {}
};

class BigIntList {
  private:
    BigIntNode *head; // 链表头指针，始终指向个位
    bool isNegative;  // 符号位，true表示负数

    // 遍历链表并逐个释放节点内存
    void clear() {
        while (head) {
            BigIntNode *temp = head;
            head = head->next;
            delete temp;
        }
    }

    // 深拷贝另一个链表，用于构造函数和赋值操作
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
    // 默认构造函数，初始化为空
    BigIntList() : head(nullptr), isNegative(false) {}

    // 析构函数
    ~BigIntList() { clear(); }

    // 拷贝构造函数
    BigIntList(const BigIntList &other) {
        isNegative = other.isNegative;
        head = copyList(other.head);
    }

    // 重载赋值运算符
    BigIntList &operator=(const BigIntList &other) {
        if (this != &other) { // 防止自我赋值
            clear();          // 先清空旧内容
            isNegative = other.isNegative;
            head = copyList(other.head);
        }
        return *this;
    }

    // 通过字符串构造大整数
    BigIntList(std::string num_str) : head(nullptr), isNegative(false) {
        if (num_str.empty() || num_str == "0") {
            push_back(0); // 处理空串或零
            return;
        }
        int start = 0;
        // 识别负号
        if (num_str[0] == '-') {
            isNegative = true;
            start = 1;
        }
        // 逆序将字符串中的字符转换为整数存入链表
        for (int i = num_str.size() - 1; i >= start; --i) {
            push_back(num_str[i] - '0');
        }
        // 移除前面多余的零
        cleanZeros();
    }

    // 在链表末尾添加一位数字
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

    // 计算当前数字的位数
    int getLength() const {
        int len = 0;
        BigIntNode *cur = head;
        while (cur) {
            len++;
            cur = cur->next;
        }
        return len;
    }

    // 移除链表末尾（高位）不必要的 0
    void cleanZeros() {
        if (!head)
            return;
        std::vector<int> v;
        BigIntNode *cur = head;
        while (cur) {
            v.push_back(cur->value);
            cur = cur->next;
        }
        // 只要长度大于1且最高位是0，就弹出
        while (v.size() > 1 && v.back() == 0)
            v.pop_back();

        clear();
        for (int val : v)
            push_back(val);
    }

    // 重载加法运算
    BigIntList operator+(const BigIntList &right) const {
        // 如果两个数正负号相同，执行标准的逐位相加并处理进位
        if (this->isNegative == right.isNegative) {
            BigIntList res;
            res.isNegative = this->isNegative;
            BigIntNode dummy(0); // 哨兵节点，方便链表操作
            BigIntNode *cur = &dummy;
            BigIntNode *p1 = this->head, *p2 = right.head;
            int carry = 0; // 进位

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
        // 如果正负号不同，转换为减法：a + (-b) 等价于 a - b
        return *this - (right.negated());
    }

    // 重载减法运算
    BigIntList operator-(const BigIntList &right) const {
        // 如果符号不同，转换为加法：a - (-b) 等价于 a + b
        if (this->isNegative != right.isNegative)
            return *this + right.negated();

        // 比较两者的绝对值大小
        int cmp = compareAbs(right);
        if (cmp == 0)
            return BigIntList("0"); // 相等则结果为0

        // 确定较大的数作为被减数，较小的作为减数
        const BigIntList *max = (cmp > 0) ? this : &right;
        const BigIntList *min = (cmp > 0) ? &right : this;

        BigIntList res;
        // 确定结果的符号，如果左边绝对值大，结果符号同左，否则结果符号同右
        res.isNegative = (cmp > 0) ? this->isNegative : !right.isNegative;

        BigIntNode dummy(0);
        BigIntNode *cur = &dummy;
        BigIntNode *p1 = max->head, *p2 = min->head;
        int borrow = 0; // 借位

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

    // 重载乘法运算
    BigIntList operator*(const BigIntList &right) const {
        // 如果其中一个是0，结果直接为0
        if (isZero() || right.isZero())
            return BigIntList("0");

        BigIntList res("0");
        BigIntNode *p2 = right.head;
        int shift = 0; // 记录当前位是第几位

        while (p2) {
            BigIntList temp;
            // 先补上错位的0
            for (int i = 0; i < shift; ++i)
                temp.push_back(0);

            int carry = 0;
            BigIntNode *p1 = this->head;
            // 用 right 的当前位去乘以 this 的每一位
            while (p1 || carry) {
                int mul = (p1 ? p1->value : 0) * p2->value + carry;
                temp.push_back(mul % 10);
                carry = mul / 10;
                if (p1)
                    p1 = p1->next;
            }
            // 将当前位的乘积累加到最终结果中
            res = res + temp;
            p2 = p2->next;
            shift++;
        }
        // 异号得负，同号得正
        res.isNegative = (this->isNegative != right.isNegative);
        return res;
    }

    // 取负值
    BigIntList negated() const {
        BigIntList copy(*this);
        copy.isNegative = !this->isNegative;
        return copy;
    }

    // 比较两个大整数的绝对值大小
    int compareAbs(const BigIntList &right) const {
        int l1 = getLength(), l2 = right.getLength();
        if (l1 != l2)
            return l1 > l2 ? 1 : -1;

        // 长度相同时，通过转字符串来方便地按位比较
        std::string s1 = toAbsString(), s2 = right.toAbsString();
        if (s1 == s2)
            return 0;
        return s1 > s2 ? 1 : -1;
    }

    // 判断当前数值是否为 0
    bool isZero() const { return !head || (head->value == 0 && !head->next); }

    // 将链表存储的数字还原为用于显示的字符串形式
    std::string toAbsString() const {
        std::string s = "";
        BigIntNode *cur = head;
        while (cur) {
            s += std::to_string(cur->value);
            cur = cur->next;
        }
        // 链表是个位在前，转回字符串需要反转
        std::reverse(s.begin(), s.end());
        return s.empty() ? "0" : s;
    }

    // 打印大整数
    void show() const {
        if (isNegative && !isZero())
            std::cout << "-";
        std::cout << toAbsString();
    }
};

int main() {
    // BigIntArray
    BigIntArray a = BigIntArray("5678");
    BigIntArray b = BigIntArray("0");
    BigIntArray c = a * b;
    c.show();

    std::cout << std::endl;

    // BigIntList
    BigIntList d("5678");
    BigIntList e("-1");
    BigIntList f = d * e;
    f.show();

    return 0;
}