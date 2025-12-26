CXX       := g++
CXXFLAGS  := -std=c++17 -O2 -Wall -Iinclude -g

SRC_DIR   := src
OBJ_DIR   := obj
BIN_DIR   := bin

SENDER_SRCS   := $(SRC_DIR)/sender.cpp $(SRC_DIR)/util.cpp
RECEIVER_SRCS := $(SRC_DIR)/receiver.cpp $(SRC_DIR)/util.cpp

SENDER_OBJS   := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SENDER_SRCS))
RECEIVER_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(RECEIVER_SRCS))

SENDER_BIN    := $(BIN_DIR)/sender
RECEIVER_BIN  := $(BIN_DIR)/receiver

all: $(SENDER_BIN) $(RECEIVER_BIN)

$(SENDER_BIN): $(SENDER_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(RECEIVER_BIN): $(RECEIVER_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(SENDER_BIN) $(RECEIVER_BIN)

.PHONY: all clean
