#include <iostream>
#include <stdexcept>
#include <vector>

// 队列
template <typename T> class MyQueue {
  private:
    // 队列节点
    struct Node {
        T data;
        Node *next;
        Node(T val) : data(val), next(nullptr) {}
    };
    Node *front; // 指向队首
    Node *rear;  // 指向队尾

  public:
    int count; // 队列大小
    MyQueue() : front(nullptr), rear(nullptr), count(0) {}

    // 析构函数，释放内存
    ~MyQueue() {
        while (!isEmpty()) {
            pop();
        }
    }

    // 入队：在队尾添加元素
    void push(T val) {
        Node *newNode = new Node(val);
        if (isEmpty()) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
        count++;
    }

    // 出队：从队首移除元素
    void pop() {
        if (isEmpty()) {
            throw std::runtime_error("Queue is empty");
        }
        Node *temp = front;
        front = front->next;
        delete temp;
        count--;
        if (front == nullptr)
            rear = nullptr;
    }

    // 获取队首元素
    T top() const {
        if (isEmpty())
            throw std::runtime_error("Queue is empty");
        return front->data;
    }

    // 判断队列是否为空
    bool isEmpty() const { return front == nullptr; }
};

// 栈
template <typename T> class MyStack {
  private:
    MyQueue<T> base_queue;

  public:
    // 入栈：将新元素添加到队列头部
    void push(T val) {
        // 原元素数量
        int size = base_queue.count;
        // 先将新元素添加到队尾
        base_queue.push(val);
        // 将原来的元素依次pop出去再push到队尾
        for (int i = 0; i < size; i++) {
            T temp = base_queue.top();
            base_queue.pop();
            base_queue.push(temp);
        }
    }

    // 出栈：弹出队首
    void pop() { base_queue.pop(); }

    // 获取栈顶元素
    T top() const { return base_queue.top(); }

    bool isEmpty() const { return base_queue.isEmpty(); }
};

// 判断括号匹配
// 存储字符索引
struct IndexNode {
    char c;
    int index;
};

// 判断是否是左括号
bool isLeft(char c) {
    if (c == '(' || c == '[' || c == '{') {
        return true;
    } else {
        return false;
    }
};

// 判断是否是右括号
bool isRight(char c) {
    if (c == ')' || c == ']' || c == '}') {
        return true;
    } else {
        return false;
    }
};

// 判断是否匹配
bool isPair(char l, char r) {
    if ((l == '(' && r == ')') || (l == '[' && r == ']') ||
        (l == '{' && r == '}')) {
        return true;
    } else {
        return false;
    }
};

int isValidParentheses(std::string s) {
    std::vector<bool> flag(s.size(), false); // 标记字符是否有匹配的字符
    MyStack<IndexNode> temp;

    // 寻找匹配字符并标记flag
    for (int i = 0; i < s.size(); i++) {
        char c = s[i];
        // 左括号直接入栈
        if (isLeft(c)) {
            temp.push({c, i});
        } else if (isRight(c)) {
            // 右括号检查是否与栈顶匹配
            if (!temp.isEmpty() && isPair(temp.top().c, c)) {
                flag[i] = true;
                flag[temp.top().index] = true;
                temp.pop();
            }
        }
    }

    // 检查是否全匹配
    bool allMatched = true;
    for (bool isMatched : flag) {
        if (!isMatched) {
            allMatched = false;
            break;
        }
    }
    if (allMatched) {
        std::cout << "Yes" << std::endl << std::endl;
        return 0;
    }

    // 提取连续匹配的字符段
    std::string currentSegment;
    for (int i = 0; i < s.size(); i++) {
        if (flag[i]) {
            currentSegment += s[i]; // 如果当前字符是匹配的，加入片段
        } else {
            if (currentSegment.size() > 1) {
                // 遇到不匹配的，如果当前字符段长度大于1，输出之前的片段
                std::cout << currentSegment << std::endl;
            }
            // 清空字符段
            currentSegment.clear();
        }
    }
    // 处理最后一个片段
    if (!currentSegment.empty()) {
        std::cout << currentSegment << std::endl;
    }

    std::cout << std::endl;
    return 0;
}

int main() {
    std::string s = "{[()]}()";
    isValidParentheses(s);

    s = "{[]}())([])({{}}";
    isValidParentheses(s);

    s = "{[}])([])({{}}";
    isValidParentheses(s);

    return 0;
}
