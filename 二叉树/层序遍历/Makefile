# 编译器设置
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

# 目标文件和源文件
TARGET = largestValues
SRC = largestValues.cpp

# 默认目标
all: $(TARGET)

# 链接
$(TARGET): $(SRC:.cpp=.o)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 编译
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理
clean:
	rm -f $(TARGET) *.o

# 伪目标
.PHONY: all clean
    