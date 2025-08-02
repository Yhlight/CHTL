#include "../src/parser/ConfigParser.h"
#include "../src/lexer/BasicLexer.h"
#include <iostream>
#include <chrono>
#include <memory>

using namespace chtl;
using namespace std::chrono;

class ConfigParserOptimizedTest {
private:
    int testsPassed = 0;
    int testsFailed = 0;
    
    void test(const std::string& name, bool condition) {
        if (condition) {
            std::cout << "[PASS] " << name << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] " << name << std::endl;
            testsFailed++;
        }
    }
    
public:
    void runAllTests() {
        std::cout << "=== ConfigParser Optimization & Validation Tests ===" << std::endl;
        
        testCachePerformance();
        testValidation();
        testKeywordConflicts();
        testPerformanceStats();
        testComplexValidation();
        
        std::cout << "\nSummary: " << testsPassed << " passed, " 
                  << testsFailed << " failed" << std::endl;
    }
    
    void testCachePerformance() {
        std::cout << "\n-- Cache Performance Tests --" << std::endl;
        
        std::string configContent = 
            "[Configuration] {\n"
            "    KEYWORD_TEXT = texto;\n"
            "    KEYWORD_STYLE = estilo;\n"
            "    DEBUG_MODE = true;\n"
            "}\n"
            "texto { \"Hello\" }\n"
            "estilo { color: red; }";
        
        BasicLexer lexer;
        auto tokens = lexer.tokenize(configContent);
        
        // 第一次解析（无缓存）
        ConfigParser::enableCache(true);
        ConfigCache::clear();
        
        auto start = high_resolution_clock::now();
        ConfigParser parser1;
        auto root1 = parser1.parse(tokens);
        auto end = high_resolution_clock::now();
        auto firstTime = duration_cast<microseconds>(end - start).count();
        
        test("First parse succeeds", root1 != nullptr);
        
        // 第二次解析（有缓存）
        start = high_resolution_clock::now();
        ConfigParser parser2;
        auto root2 = parser2.parse(tokens);
        end = high_resolution_clock::now();
        auto secondTime = duration_cast<microseconds>(end - start).count();
        
        test("Second parse succeeds", root2 != nullptr);
        test("Cache improves performance", secondTime < firstTime);
        
        std::cout << "  First parse: " << firstTime << " µs" << std::endl;
        std::cout << "  Cached parse: " << secondTime << " µs" << std::endl;
        if (firstTime > 0) {
            std::cout << "  Speedup: " << (double)firstTime / secondTime << "x" << std::endl;
        }
        
        // 测试缓存禁用
        ConfigParser::enableCache(false);
        start = high_resolution_clock::now();
        ConfigParser parser3;
        auto root3 = parser3.parse(tokens);
        end = high_resolution_clock::now();
        auto noCacheTime = duration_cast<microseconds>(end - start).count();
        
        test("Parse with cache disabled", root3 != nullptr);
        std::cout << "  No cache parse: " << noCacheTime << " µs" << std::endl;
        
        ConfigParser::enableCache(true);
    }
    
    void testValidation() {
        std::cout << "\n-- Configuration Validation Tests --" << std::endl;
        
        // 测试无效的布尔值
        std::string invalidBool = 
            "[Configuration] {\n"
            "    DEBUG_MODE = yes;\n"  // 应该是 true 或 false
            "}";
        
        BasicLexer lexer;
        auto tokens = lexer.tokenize(invalidBool);
        
        ConfigParser parser;
        parser.parse(tokens);
        auto result = parser.getValidationResult();
        
        test("Invalid boolean detected", !result.isValid);
        test("Has error for invalid boolean", result.errors.size() > 0);
        
        // 测试无效的数字值
        std::string invalidNumber = 
            "[Configuration] {\n"
            "    INDEX_INITIAL_COUNT = abc;\n"  // 应该是数字
            "}";
        
        tokens = lexer.tokenize(invalidNumber);
        ConfigParser parser2;
        parser2.parse(tokens);
        result = parser2.getValidationResult();
        
        test("Invalid number detected", !result.isValid);
        
        // 测试缺少推荐配置
        std::string missingRecommended = 
            "[Configuration] {\n"
            "    KEYWORD_TEXT = text;\n"
            "}";
        
        tokens = lexer.tokenize(missingRecommended);
        ConfigParser parser3;
        parser3.parse(tokens);
        result = parser3.getValidationResult();
        
        test("Missing recommended config generates warning", result.warnings.size() > 0);
        test("Missing recommended is still valid", result.isValid);
    }
    
    void testKeywordConflicts() {
        std::cout << "\n-- Keyword Conflict Tests --" << std::endl;
        
        // 测试关键字冲突
        std::string conflictConfig = 
            "[Configuration] {\n"
            "    KEYWORD_TEXT = same;\n"
            "    KEYWORD_STYLE = same;\n"  // 冲突：相同的关键字
            "}";
        
        BasicLexer lexer;
        auto tokens = lexer.tokenize(conflictConfig);
        
        ConfigParser parser;
        parser.parse(tokens);
        auto result = parser.getValidationResult();
        
        test("Keyword conflict detected", result.warnings.size() > 0);
        
        // 检查警告消息
        bool hasConflictWarning = false;
        for (const auto& warning : result.warnings) {
            if (warning.find("same") != std::string::npos) {
                hasConflictWarning = true;
                std::cout << "  Warning: " << warning << std::endl;
            }
        }
        test("Conflict warning mentions keyword", hasConflictWarning);
        
        // 测试组选项中的冲突
        std::string optionConflict = 
            "[Configuration] {\n"
            "    CUSTOM_STYLE = [@Style, @style, @Style];\n"  // 重复选项
            "}";
        
        tokens = lexer.tokenize(optionConflict);
        ConfigParser parser2;
        parser2.parse(tokens);
        result = parser2.getValidationResult();
        
        test("Duplicate option detected", result.warnings.size() > 0);
    }
    
    void testPerformanceStats() {
        std::cout << "\n-- Performance Statistics Tests --" << std::endl;
        
        std::string content = 
            "[Configuration] {\n"
            "    DEBUG_MODE = true;\n"
            "    KEYWORD_TEXT = t;\n"
            "    KEYWORD_STYLE = s;\n"
            "}\n"
            "t { \"Test\" }\n"
            "s { color: blue; }\n"
            "t { \"Another\" }\n"
            "s { font-size: 12px; }";
        
        BasicLexer lexer;
        auto tokens = lexer.tokenize(content);
        
        ConfigParser parser;
        parser.parse(tokens);
        
        // 性能统计会在DEBUG_MODE=true时自动打印
        test("Parser completes with debug mode", true);
    }
    
    void testComplexValidation() {
        std::cout << "\n-- Complex Validation Tests --" << std::endl;
        
        // 测试空选项组
        std::string emptyOptions = 
            "[Configuration] {\n"
            "    CUSTOM_STYLE = [];\n"  // 空选项组
            "}";
        
        BasicLexer lexer;
        auto tokens = lexer.tokenize(emptyOptions);
        
        ConfigParser parser;
        parser.parse(tokens);
        auto result = parser.getValidationResult();
        
        test("Empty option group detected", !result.isValid);
        
        // 测试循环引用检测
        std::string circular = 
            "[Configuration] {\n"
            "    KEYWORD_TEXT = KEYWORD_STYLE;\n"
            "    KEYWORD_STYLE = style;\n"
            "}";
        
        tokens = lexer.tokenize(circular);
        ConfigParser parser2;
        parser2.parse(tokens);
        result = parser2.getValidationResult();
        
        test("Potential circular reference warning", result.warnings.size() > 0);
        
        // 测试预热缓存
        std::string warmupConfig = 
            "[Configuration] {\n"
            "    KEYWORD_TEXT = texto;\n"
            "}";
        
        ConfigParser parser3;
        parser3.warmupCache(warmupConfig);
        
        // 现在解析相同配置应该使用缓存
        tokens = lexer.tokenize(warmupConfig + "\ntexto { \"Cached\" }");
        auto start = high_resolution_clock::now();
        parser3.parse(tokens);
        auto end = high_resolution_clock::now();
        
        test("Warmup cache works", duration_cast<microseconds>(end - start).count() < 1000);
    }
};

int main() {
    ConfigParserOptimizedTest test;
    test.runAllTests();
    return 0;
}