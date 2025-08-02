# CHTL Compiler Makefile
CXX = g++
CXXFLAGS = -std=c++14 -Wall -O2
INCLUDES = -Isrc

# 目标可执行文件
TARGET = chtlc

# 源文件目录
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# 源文件
COMMON_SRC = $(wildcard $(SRC_DIR)/common/*.cpp)
LEXER_SRC = $(wildcard $(SRC_DIR)/lexer/*.cpp)
PARSER_SRC = $(wildcard $(SRC_DIR)/parser/*.cpp)
LOADER_SRC = $(wildcard $(SRC_DIR)/loader/*.cpp)
GENERATOR_SRC = $(wildcard $(SRC_DIR)/generator/*.cpp)
PREDEFINE_SRC = $(wildcard $(SRC_DIR)/predefine/*.cpp)
MAIN_SRC = $(SRC_DIR)/main.cpp

# 所有源文件
ALL_SRC = $(COMMON_SRC) $(LEXER_SRC) $(PARSER_SRC) $(LOADER_SRC) \
          $(GENERATOR_SRC) $(PREDEFINE_SRC) $(MAIN_SRC)

# 对象文件
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(ALL_SRC))

# 创建目录
$(shell mkdir -p $(OBJ_DIR)/common $(OBJ_DIR)/lexer $(OBJ_DIR)/parser \
                 $(OBJ_DIR)/loader $(OBJ_DIR)/generator $(OBJ_DIR)/predefine $(BIN_DIR))

# 默认目标
all: $(BIN_DIR)/$(TARGET)

# 链接
$(BIN_DIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "Build complete: $@"

# 编译规则
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# 清理
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# 运行测试
test:
	cd test && make test

# 安装
install: $(BIN_DIR)/$(TARGET)
	cp $(BIN_DIR)/$(TARGET) /usr/local/bin/

# 卸载
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# 显示帮助
help:
	@echo "CHTL Compiler Makefile"
	@echo "Usage:"
	@echo "  make          - Build the compiler"
	@echo "  make clean    - Clean build files"
	@echo "  make test     - Run tests"
	@echo "  make install  - Install to /usr/local/bin"
	@echo "  make help     - Show this help"

.PHONY: all clean test install uninstall help