CXX = g++
CXXFLAGS = -std=c++14 -Wall -g
INCLUDES = -I../src

# 目标可执行文件
TARGETS = TokenTest LexerTest LoaderTest

# 源文件
COMMON_SRC = ../src/common/Token.cpp
LEXER_SRC = ../src/lexer/Lexer.cpp ../src/lexer/BasicLexer.cpp ../src/lexer/ConfigLexer.cpp
LOADER_SRC = ../src/loader/ChtlLoader.cpp

all: $(TARGETS)

TokenTest: TokenTest.cpp $(COMMON_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

LexerTest: LexerTest.cpp $(COMMON_SRC) $(LEXER_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

LoaderTest: LoaderTest.cpp $(LOADER_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

test: $(TARGETS)
	@echo "Running Token Tests..."
	./TokenTest
	@echo "\nRunning Lexer Tests..."
	./LexerTest
	@echo "\nRunning Loader Tests..."
	./LoaderTest

clean:
	rm -f $(TARGETS) *.o

.PHONY: all test clean