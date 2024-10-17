# 定义编译器
CXX = g++

# 定义编译器标志
CXXFLAGS = -std=c++11 -Wall

# 定义目标文件
TARGET = SM4

# 定义源文件
SRCS = SM4.cpp

# 定义对象文件
OBJS = $(SRCS:.cpp=.o)

# 默认目标
all: $(TARGET)

# 链接目标文件
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# 编译源文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -f $(TARGET) $(OBJS)

# 运行程序
run: $(TARGET)
	./$(TARGET)