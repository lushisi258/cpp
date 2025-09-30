# 定义编译器
CXX = g++

# 定义编译选项
CXXFLAGS = -Wall -O2 -I"C:\Program Files\Npcap\Include"

# 定义链接选项
LDFLAGS = -L"C:\Program Files\Npcap\Lib\x64" -lwpcap -lPacket -lIphlpapi -lws2_32

# 定义目标文件
TARGET = get_mac

# 定义源文件
SRCS = get_mac.cpp

# 定义目标文件
OBJS = $(SRCS:.cpp=.o)

# 默认目标
all: $(TARGET)

# 链接目标文件
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# 编译源文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -f $(OBJS) $(TARGET)