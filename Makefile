# CHTL编译器 Makefile
# 使用C++14标准，高质量编译选项

CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -O2 -g
INCLUDES = -Isrc
SRCDIR = src
TESTDIR = test
OBJDIR = build
BINDIR = bin

# 源文件
COMMON_SRCS = $(wildcard $(SRCDIR)/common/*.cpp)
LEXER_SRCS = $(wildcard $(SRCDIR)/lexer/*.cpp)
NODE_SRCS = $(wildcard $(SRCDIR)/node/*.cpp)
LOADER_SRCS = $(wildcard $(SRCDIR)/loader/*.cpp)
MODULE_SRCS = $(wildcard $(SRCDIR)/module/*.cpp)
GENERATOR_SRCS = $(wildcard $(SRCDIR)/generator/*.cpp)
PREDEFINE_SRCS = $(wildcard $(SRCDIR)/predefine/*.cpp)
PARSER_SRCS = $(wildcard $(SRCDIR)/parser/*.cpp)

ALL_SRCS = $(COMMON_SRCS) $(LEXER_SRCS) $(NODE_SRCS) $(LOADER_SRCS) \
           $(MODULE_SRCS) $(GENERATOR_SRCS) $(PREDEFINE_SRCS) $(PARSER_SRCS)

# 对象文件
OBJS = $(ALL_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# 测试文件
TEST_SRCS = $(wildcard $(TESTDIR)/*.cpp)
TEST_BINS = $(TEST_SRCS:$(TESTDIR)/%.cpp=$(BINDIR)/%)

# 主要目标
.PHONY: all clean test directories

all: directories $(BINDIR)/chtl_compiler $(TEST_BINS)

# 创建目录
directories:
	@mkdir -p $(OBJDIR)/common
	@mkdir -p $(OBJDIR)/lexer
	@mkdir -p $(OBJDIR)/node
	@mkdir -p $(OBJDIR)/loader
	@mkdir -p $(OBJDIR)/module
	@mkdir -p $(OBJDIR)/generator
	@mkdir -p $(OBJDIR)/predefine
	@mkdir -p $(OBJDIR)/parser
	@mkdir -p $(BINDIR)
	@mkdir -p test/sample_chtl_files

# 主编译器目标（暂时创建一个空的main函数）
$(BINDIR)/chtl_compiler: $(OBJS) main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ main.cpp $(OBJS)

# 编译对象文件
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# 编译测试文件
$(BINDIR)/%: $(TESTDIR)/%.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(OBJS)

# 运行测试
test: $(TEST_BINS)
	@echo "=== 运行CHTL编译器测试套件 ==="
	@for test in $(TEST_BINS); do \
		echo "运行测试: $$test"; \
		$$test; \
		echo ""; \
	done

# 运行特定测试
test-lexer: $(BINDIR)/BasicLexerTest
	@echo "=== 运行词法分析器测试 ==="
	$(BINDIR)/BasicLexerTest

# 清理
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# 显示项目信息
info:
	@echo "CHTL编译器构建信息:"
	@echo "编译器: $(CXX)"
	@echo "编译选项: $(CXXFLAGS)"
	@echo "源文件数量: $(words $(ALL_SRCS))"
	@echo "测试文件数量: $(words $(TEST_SRCS))"

# 检查代码风格（如果有clang-format）
format:
	@if command -v clang-format >/dev/null 2>&1; then \
		find $(SRCDIR) $(TESTDIR) -name "*.cpp" -o -name "*.h" | xargs clang-format -i; \
		echo "代码格式化完成"; \
	else \
		echo "clang-format未安装，跳过格式化"; \
	fi

# 生成依赖关系
deps: $(ALL_SRCS)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MM $(ALL_SRCS) > deps.mk

# 包含依赖关系文件（如果存在）
-include deps.mk

# 安装目标
install: all
	@echo "CHTL编译器安装功能待实现"

# 文档生成
docs:
	@echo "文档生成功能待实现"

# 代码分析
analyze:
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --std=c++14 $(SRCDIR); \
	else \
		echo "cppcheck未安装，跳过静态分析"; \
	fi