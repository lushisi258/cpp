# 定义编译器
CXX = g++

# 定义编译参数
CXXFLAGS = -Wall -O2

# 定义目标文件
TARGETS = spn.exe gen.exe

# 默认目标
all: $(TARGETS)

# 编译 main.cpp 生成 spn.exe
spn.exe: main.cpp
	$(CXX) $(CXXFLAGS) -o spn.exe main.cpp

# 编译 spn_gen_cipher.cpp 生成 gen.exe
gen.exe: spn_gen_cipher.cpp
	$(CXX) $(CXXFLAGS) -o gen.exe spn_gen_cipher.cpp

# 清理生成的文件
clean:
	rm -f $(TARGETS)