# 编译器
CXX = g++

# 编译器选项
CXXFLAGS = -std=c++11 -Wall -Iinclude

# 链接选项
LDFLAGS = -lws2_32

# 目标文件
TARGETS = build/server build/client

# 头文件
HEADERS = include/msg.h

# 源文件
SERVER_SRC = src/server.cpp
CLIENT_SRC = src/client.cpp
MSG_SRC = src/msg.cpp

# 生成的目标文件
OBJ_DIR = build
SERVER_OBJ = $(OBJ_DIR)/server.o
CLIENT_OBJ = $(OBJ_DIR)/client.o
MSG_OBJ = $(OBJ_DIR)/msg.o

# 默认目标
all: $(TARGETS)

# 创建目标文件夹
$(OBJ_DIR):
	mkdir $(OBJ_DIR)

# 编译 server
build/server: $(OBJ_DIR) $(SERVER_SRC) $(MSG_OBJ) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(SERVER_SRC) $(MSG_OBJ) $(LDFLAGS) -mconsole

# 编译 client
build/client: $(OBJ_DIR) $(CLIENT_SRC) $(MSG_OBJ) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(CLIENT_SRC) $(MSG_OBJ) $(LDFLAGS) -mconsole

# 编译 msg.cpp
$(MSG_OBJ): src/msg.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c src/msg.cpp -o $@

# 清理
clean:
	rm -f $(TARGETS) $(OBJ_DIR)/*.o
	rmdir $(OBJ_DIR) 2>/dev/null || true

# 编译运行 server
run_server: build/server
	$(SERVER_OBJ)

# 编译运行 client
run_client: build/client
	$(CLIENT_OBJ)

.PHONY: all clean run_server run_client
