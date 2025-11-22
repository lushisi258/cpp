# 编译器和编译选项
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

# 目标文件夹
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# 源文件
CLIENT_SRCS = $(SRC_DIR)/client.cpp $(SRC_DIR)/message.cpp
SERVER_SRCS = $(SRC_DIR)/server.cpp $(SRC_DIR)/message.cpp

# 目标文件
CLIENT_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CLIENT_SRCS))
SERVER_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SERVER_SRCS))

# 可执行文件
CLIENT_TARGET = $(BIN_DIR)/chat_client
SERVER_TARGET = $(BIN_DIR)/chat_server

# 默认目标
all: $(CLIENT_TARGET) $(SERVER_TARGET)

# 客户端目标
$(CLIENT_TARGET): $(CLIENT_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 服务器目标
$(SERVER_TARGET): $(SERVER_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 编译规则
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean