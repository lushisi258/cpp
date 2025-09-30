# 编译器
CC = g++

# 编译选项
CFLAGE = -Wall -g

# 目标文件
TARGET = main

# 源文件
SRC = main.cpp

# 对象文件
OBJ = $(SRC:.cpp=.o)

# 默认目标
all: $(TARGET)

# 生成目标文件
$(TARGET): $(OBJ)
	$(CC) $(CFLAGE) -o $(TARGET) $(OBJ)

# 编译源文件
%.o: %.cpp
	$(CC) $(CFLAGE) -c $(SRC)

# 清理
clean:
	rm -f $(TARGET) $(OBJ)

# 伪目标
.PHONY: all clean