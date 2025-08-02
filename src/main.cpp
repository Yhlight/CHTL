#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include "lexer/BasicLexer.h"
#include "parser/BasicParser.h"
#include "parser/ConfigParser.h"
#include "generator/HtmlGenerator.h"
#include "loader/ChtlLoader.h"

using namespace chtl;

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options] input.chtl [output.html]\n"
              << "Options:\n"
              << "  -h, --help         Show this help message\n"
              << "  -m, --minify       Minify output HTML\n"
              << "  -p, --pretty       Pretty print output (default)\n"
              << "  -c, --config       Use configuration-driven parser\n"
              << "  --no-hoist         Don't hoist styles to <head>\n"
              << "  --no-predefined    Don't use predefined styles/elements\n"
              << "  --no-comments      Don't generate comments\n"
              << std::endl;
}

int main(int argc, char* argv[]) {
    // 默认配置
    GeneratorConfig config;
    bool useConfigParser = false;
    bool usePredefined = true;
    std::string inputFile;
    std::string outputFile;
    
    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
        else if (arg == "-m" || arg == "--minify") {
            config.minify = true;
            config.prettyPrint = false;
        }
        else if (arg == "-p" || arg == "--pretty") {
            config.prettyPrint = true;
            config.minify = false;
        }
        else if (arg == "-c" || arg == "--config") {
            useConfigParser = true;
        }
        else if (arg == "--no-hoist") {
            config.hoistStyles = false;
            config.hoistScripts = false;
        }
        else if (arg == "--no-predefined") {
            usePredefined = false;
        }
        else if (arg == "--no-comments") {
            config.generateComments = false;
        }
        else if (arg[0] != '-') {
            if (inputFile.empty()) {
                inputFile = arg;
            } else if (outputFile.empty()) {
                outputFile = arg;
            }
        }
    }
    
    // 检查输入文件
    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified\n" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    // 默认输出文件名
    if (outputFile.empty()) {
        size_t dotPos = inputFile.find_last_of('.');
        if (dotPos != std::string::npos) {
            outputFile = inputFile.substr(0, dotPos) + ".html";
        } else {
            outputFile = inputFile + ".html";
        }
    }
    
    try {
        // 创建文件加载器
        auto loader = std::make_shared<ChtlLoader>();
        
        // 创建词法分析器
        std::unique_ptr<Lexer> lexer;
        if (useConfigParser) {
            lexer = std::make_unique<ConfigLexer>();
        } else {
            lexer = std::make_unique<BasicLexer>();
        }
        
        // 读取输入文件
        std::ifstream inFile(inputFile);
        if (!inFile.is_open()) {
            std::cerr << "Error: Cannot open input file: " << inputFile << std::endl;
            return 1;
        }
        
        std::string chtlCode((std::istreambuf_iterator<char>(inFile)),
                            std::istreambuf_iterator<char>());
        inFile.close();
        
        std::cout << "Compiling " << inputFile << "..." << std::endl;
        
        // 词法分析
        std::cout << "  Lexing..." << std::endl;
        auto tokens = lexer->tokenize(chtlCode);
        
        // 语法分析
        std::cout << "  Parsing..." << std::endl;
        std::unique_ptr<Parser> parser;
        if (useConfigParser) {
            parser = std::make_unique<ConfigParser>(loader);
        } else {
            parser = std::make_unique<BasicParser>(loader);
        }
        
        auto ast = parser->parse(tokens);
        if (!ast) {
            std::cerr << "Error: Failed to parse input file" << std::endl;
            return 1;
        }
        
        // 生成HTML
        std::cout << "  Generating HTML..." << std::endl;
        HtmlGenerator generator(config);
        generator.setUsePredefined(usePredefined);
        
        std::string html = generator.generateCompleteHtml(ast);
        
        // 写入输出文件
        std::ofstream outFile(outputFile);
        if (!outFile.is_open()) {
            std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
            return 1;
        }
        
        outFile << html;
        outFile.close();
        
        std::cout << "Successfully generated: " << outputFile << std::endl;
        
        // 显示统计信息
        std::cout << "\nStatistics:" << std::endl;
        std::cout << "  Tokens: " << tokens.size() << std::endl;
        std::cout << "  Output size: " << html.length() << " bytes" << std::endl;
        
        if (config.minify) {
            std::cout << "  Output minified" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}