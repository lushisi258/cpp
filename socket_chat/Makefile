# 编译器
CXX = g++

# 编译器选项
CXXFLAGS = -std=c++11 -Wall -Iinclude

# 链接选项
LDFLAGS = -lws2_32

# 目标文件
TARGETS = server client

# 源文件
SERVER_SRC = src/server.cpp
CLIENT_SRC = src/client.cpp

# 头文件
HEADERS = include/server.h include/client.h

# 生成的可执行文件
SERVER_OBJ = server
CLIENT_OBJ = client

# 默认目标
all: $(TARGETS)

# 编译 server
server: $(SERVER_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(SERVER_OBJ) $(SERVER_SRC) $(LDFLAGS) -mconsole

# 编译 client
client: $(CLIENT_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(CLIENT_OBJ) $(CLIENT_SRC) $(LDFLAGS) -mconsole

# 清理生成的文件
clean:
	rm -f $(TARGETS) $(SERVER_OBJ) $(CLIENT_OBJ)

.PHONY: all clean