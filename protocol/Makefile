# 编译器和编译选项
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g

# 文件路径
SRC_DIR = src
CLIENT_SRC_DIR = $(SRC_DIR)/client
SERVER_SRC_DIR = $(SRC_DIR)/server
COMMON_SRC_DIR = $(SRC_DIR)/common
BUILD_DIR = build

# 源文件
CLIENT_SRC = $(CLIENT_SRC_DIR)/client.cpp $(CLIENT_SRC_DIR)/file_sender.cpp $(COMMON_SRC_DIR)/network.cpp $(COMMON_SRC_DIR)/packet.cpp
SERVER_SRC = $(SERVER_SRC_DIR)/server.cpp $(SERVER_SRC_DIR)/file_receiver.cpp $(COMMON_SRC_DIR)/network.cpp $(COMMON_SRC_DIR)/packet.cpp

# 目标文件
CLIENT_OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CLIENT_SRC))
SERVER_OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SERVER_SRC))

# 可执行文件
CLIENT_EXEC = $(BUILD_DIR)/client
SERVER_EXEC = $(BUILD_DIR)/server

# 默认目标
all: $(CLIENT_EXEC) $(SERVER_EXEC)

# 客户端目标
$(CLIENT_EXEC): $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lws2_32

# 服务器目标
$(SERVER_EXEC): $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lws2_32

# 中间文件规则
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# 清理目标文件
clean:
	rm -rf $(BUILD_DIR)

# 依赖文件
$(CLIENT_OBJ): $(CLIENT_SRC_DIR)/file_sender.h $(COMMON_SRC_DIR)/network.h $(COMMON_SRC_DIR)/packet.h
$(SERVER_OBJ): $(SERVER_SRC_DIR)/file_receiver.h $(COMMON_SRC_DIR)/network.h $(COMMON_SRC_DIR)/packet.h