#include "src/lexer/BasicLexer.h"
#include "src/common/Token.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace chtl;

/**
 * CHTL编译器主程序
 * 临时实现，主要用于测试和验证功能
 */

void printUsage(const char* programName) {
    std::cout << "CHTL编译器 v0.1 - 超文本语言编译器" << std::endl;
    std::cout << "用法: " << programName << " [选项] <输入文件>" << std::endl;
    std::cout << std::endl;
    std::cout << "选项:" << std::endl;
    std::cout << "  -h, --help       显示此帮助信息" << std::endl;
    std::cout << "  -v, --version    显示版本信息" << std::endl;
    std::cout << "  -t, --tokenize   仅进行词法分析" << std::endl;
    std::cout << "  -d, --debug      启用调试模式" << std::endl;
    std::cout << "  -o <输出文件>     指定输出文件" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  " << programName << " example.chtl" << std::endl;
    std::cout << "  " << programName << " -t example.chtl" << std::endl;
    std::cout << "  " << programName << " -o output.html input.chtl" << std::endl;
}

void printVersion() {
    std::cout << "CHTL编译器 版本 0.1.0" << std::endl;
    std::cout << "构建日期: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "使用C++14标准构建" << std::endl;
    std::cout << "严格按照CHTL语法文档实现" << std::endl;
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + filename);
    }
    
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void tokenizeOnly(const std::string& input, bool debug = false) {
    std::cout << "=== 词法分析结果 ===" << std::endl;
    
    BasicLexer lexer(input);
    auto tokens = lexer.tokenize();
    
    if (lexer.hasError()) {
        std::cerr << "词法分析错误: " << lexer.getLastError() << std::endl;
        return;
    }
    
    std::cout << "发现 " << tokens.size() << " 个Token:" << std::endl;
    std::cout << std::endl;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        std::cout << "[" << i << "] " 
                  << TokenUtils::tokenTypeToString(token.type)
                  << " (行:" << token.position.line 
                  << ", 列:" << token.position.column << ")";
        
        if (!token.value.empty()) {
            std::cout << " = \"" << token.value << "\"";
        }
        
        // 显示上下文信息
        if (debug) {
            std::cout << " [";
            if (token.isInStyleBlock) std::cout << "Style ";
            if (token.isInCustomBlock) std::cout << "Custom ";
            if (token.isInTemplateBlock) std::cout << "Template ";
            if (token.isInConfigBlock) std::cout << "Config ";
            if (token.isInOriginBlock) std::cout << "Origin ";
            std::cout << "]";
        }
        
        std::cout << std::endl;
    }
    
    if (debug) {
        std::cout << std::endl;
        std::cout << "=== 调试信息 ===" << std::endl;
        std::cout << lexer.getDebugInfo() << std::endl;
    }
}

void compileFile(const std::string& inputFile, const std::string& outputFile, bool debug = false) {
    try {
        std::string input = readFile(inputFile);
        
        std::cout << "编译文件: " << inputFile << std::endl;
        std::cout << "输出文件: " << outputFile << std::endl;
        std::cout << "输入大小: " << input.size() << " 字符" << std::endl;
        
        // 目前只实现词法分析
        std::cout << std::endl;
        std::cout << "注意: 目前只实现了词法分析阶段" << std::endl;
        std::cout << "完整的编译功能正在开发中..." << std::endl;
        std::cout << std::endl;
        
        tokenizeOnly(input, debug);
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::string inputFile;
    std::string outputFile;
    bool tokenizeMode = false;
    bool debugMode = false;
    
    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "-t" || arg == "--tokenize") {
            tokenizeMode = true;
        } else if (arg == "-d" || arg == "--debug") {
            debugMode = true;
        } else if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg[0] != '-') {
            inputFile = arg;
        } else {
            std::cerr << "未知选项: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "错误: 未指定输入文件" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    if (outputFile.empty()) {
        // 自动生成输出文件名
        size_t dotPos = inputFile.find_last_of('.');
        if (dotPos != std::string::npos) {
            outputFile = inputFile.substr(0, dotPos) + ".html";
        } else {
            outputFile = inputFile + ".html";
        }
    }
    
    try {
        std::string input = readFile(inputFile);
        
        if (tokenizeMode) {
            tokenizeOnly(input, debugMode);
        } else {
            compileFile(inputFile, outputFile, debugMode);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}