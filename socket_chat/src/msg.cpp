#include "../include/msg.h"
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

// 控制输出的颜色，0,1,2,3分别代表蓝色、绿色、红色和黄色
void print_color_msg(const std::string &msg, int color) {
#ifdef _WIN32
    // 设置颜色
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    switch (color) {
    case 0:
        color = FOREGROUND_BLUE;
        break;
    case 1:
        color = FOREGROUND_GREEN;
        break;
    case 2:
        color = FOREGROUND_RED;
        break;
    case 3:
        color = FOREGROUND_RED | FOREGROUND_GREEN;
        break;
    default:
        color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
    }
    SetConsoleTextAttribute(hConsole, color);
    std::cout << msg;
    // 重置颜色为白色
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN |
                                          FOREGROUND_BLUE);
#else
    // 设置颜色
    std::string colorCode;
    switch (color) {
    case 0:
        colorCode = "\033[34m";
        break;
    case 1:
        colorCode = "\033[32m";
        break;
    case 2:
        colorCode = "\033[31m";
        break;
    case 3:
        colorCode = "\033[33m";
        break;
    default:
        colorCode = "\033[0m";
        break;
    }
    std::cout << colorCode << msg << "\033[0m";
#endif
}

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
    std::cout << std::left;
    std::cout << std::setw(10);
    print_color_msg(msg.username + ": ", 0);
    std::cout << std::setw(50);
    std::cout << msg.message;
    std::cout << std::setw(20);
    print_color_msg(msg.timestamp + "\n", 3);
}