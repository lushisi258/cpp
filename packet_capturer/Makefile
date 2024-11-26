# 定义编译器
CXX = g++

# 定义编译器标志
CXXFLAGS = -Wall -Wextra -std=c++11 -I"C:\Program Files\Npcap\Include"

# 定义链接器标志
LDFLAGS = -L"C:\Program Files\Npcap\Lib\x64" -lPacket -lwpcap -lws2_32

# 定义目标文件
TARGET = packet_capturer

# 定义源文件
SRCS = main.cpp

# 定义对象文件
OBJS = $(SRCS:.cpp=.o)

# 默认目标
all: $(TARGET)

# 链接目标文件
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# 编译源文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -f $(TARGET) $(OBJS)

# 伪目标
.PHONY: all clean