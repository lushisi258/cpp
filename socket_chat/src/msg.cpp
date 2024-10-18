#include "../include/msg.h"
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

// 定义颜色格式，控制输出字符颜色
#ifdef _WIN32
void setConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}
#else
const std::string RESET = "\033[0m";
const std::string BLUE = "\033[34m";
const std::string YELLOW = "\033[33m";
#endif

// 解析消息格式 "username$message$timestamp"
Message parse_msg(const std::string &input) {
    Message msg;
    size_t firstComma = input.find('$');
    size_t secondComma = input.find('$', firstComma + 1);

    // 提取用户名、消息和时间戳
    if (firstComma != std::string::npos && secondComma != std::string::npos) {
        msg.username = input.substr(0, firstComma);
        msg.message =
            input.substr(firstComma + 1, secondComma - firstComma - 1);
        msg.timestamp = input.substr(secondComma + 1);
        return msg;
    } else {
        std::cerr << "Invalid format" << std::endl;
        return {};
    }
}

// 格式化输出msg
void print_format_msg(const Message &msg) {
#ifdef _WIN32
    // 设置用户名为蓝色
    setConsoleColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    std::cout << std::left << std::setw(10) << msg.username + '>';

    // 重置颜色并打印消息
    setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << std::setw(40) << msg.message;

    // 设置时间戳为黄色
    setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << std::right << std::setw(20) << msg.timestamp;

    // 重置颜色
    setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << std::endl;
#else
    std::cout << std::left << BLUE << std::setw(10) << msg.username + '>'
              << RESET << std::setw(40) << msg.message << std::right << YELLOW
              << std::setw(20) << msg.timestamp << RESET << std::endl;
#endif
}