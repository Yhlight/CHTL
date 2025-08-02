CXX = g++
CXXFLAGS = -std=c++14 -Wall -g

# 源文件
SRC_DIR = ../src
COMMON_SRC = $(SRC_DIR)/common/Token.cpp
LEXER_SRC = $(SRC_DIR)/lexer/Lexer.cpp $(SRC_DIR)/lexer/BasicLexer.cpp $(SRC_DIR)/lexer/ConfigLexer.cpp

# 测试目标
all: TokenTest LexerTest

TokenTest: TokenTest.cpp $(COMMON_SRC)
	$(CXX) $(CXXFLAGS) -o TokenTest TokenTest.cpp $(COMMON_SRC)

LexerTest: LexerTest.cpp $(COMMON_SRC) $(LEXER_SRC)
	$(CXX) $(CXXFLAGS) -o LexerTest LexerTest.cpp $(COMMON_SRC) $(LEXER_SRC)

# 运行所有测试
test: TokenTest LexerTest
	@echo "Running Token Tests..."
	./TokenTest
	@echo "\nRunning Lexer Tests..."
	./LexerTest

# 清理
clean:
	rm -f TokenTest LexerTest *.o

.PHONY: all test clean