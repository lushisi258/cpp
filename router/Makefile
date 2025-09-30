# 编译器
CXX = g++

# 编译选项
CXXFLAGS = -Wall -Wextra -std=c++11 -I"C:\Program Files\Npcap\Include"

# 链接选项
LDFLAGS = -L"C:\Program Files\Npcap\Lib\x64" -lPacket -lwpcap -lws2_32

# 头文件
HEADERS = include/packet_capture.h

# 源文件
SRCS = $(wildcard src/*.cpp)

# 目标文件
TARGET = build/router

# 生成目标文件
all: $(TARGET)

# 链接目标文件
$(TARGET): $(SRCS:src/%.cpp=build/%.o)
	@mkdir -p build
	$(CXX) $^ -o $@ $(LDFLAGS)

# 编译源文件
build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理
clean:
	rm -f build/*

# 伪目标
.PHONY: clean