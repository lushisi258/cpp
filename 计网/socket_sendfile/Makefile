CXX = g++
CXXFLAGS = -Wall -g

TARGET = bin/sender
SRCS = src/sender.cpp
OBJS = obj/sender.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

obj/%.o: src/%.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	rm target_dir/*
