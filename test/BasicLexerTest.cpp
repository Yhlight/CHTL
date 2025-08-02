#include "../src/lexer/BasicLexer.h"
#include <iostream>
#include <cassert>
#include <vector>

namespace chtl {
namespace test {

/**
 * 基础词法分析器测试类
 * 测试CHTL基础语法的Token识别
 */
class BasicLexerTest {
public:
    void runAllTests() {
        std::cout << "=== BasicLexer 测试开始 ===" << std::endl;
        
        testBasicTokens();
        testHtmlTags();
        testCssSelectors();
        testComments();
        testStrings();
        testKeywords();
        testNumbers();
        testContextSwitching();
        testErrorHandling();
        
        std::cout << "=== BasicLexer 测试完成 ===" << std::endl;
        std::cout << "总计: " << totalTests_ << " 个测试, " 
                  << passedTests_ << " 个通过, " 
                  << (totalTests_ - passedTests_) << " 个失败" << std::endl;
    }

private:
    int totalTests_ = 0;
    int passedTests_ = 0;
    
    void testBasicTokens() {
        std::cout << "\n--- 测试基础Token ---" << std::endl;
        
        // 测试分隔符
        testTokenSequence(";", {TokenType::SEMICOLON});
        testTokenSequence(":", {TokenType::COLON});
        testTokenSequence("=", {TokenType::EQUAL});
        testTokenSequence("{}", {TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE});
        testTokenSequence("[]", {TokenType::LEFT_BRACKET, TokenType::RIGHT_BRACKET});
        testTokenSequence("()", {TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN});
        testTokenSequence(",", {TokenType::COMMA});
        
        std::cout << "基础Token测试完成" << std::endl;
    }
    
    void testHtmlTags() {
        std::cout << "\n--- 测试HTML标签 ---" << std::endl;
        
        // 测试标准HTML标签
        testTokenSequence("div", {TokenType::HTML_TAG});
        testTokenSequence("span", {TokenType::HTML_TAG});
        testTokenSequence("html", {TokenType::HTML_TAG});
        testTokenSequence("body", {TokenType::HTML_TAG});
        testTokenSequence("head", {TokenType::HTML_TAG});
        
        // 测试HTML结构
        testTokenSequence("html { body { div { } } }", {
            TokenType::HTML_TAG, TokenType::LEFT_BRACE,
            TokenType::HTML_TAG, TokenType::LEFT_BRACE,
            TokenType::HTML_TAG, TokenType::LEFT_BRACE,
            TokenType::RIGHT_BRACE, TokenType::RIGHT_BRACE, TokenType::RIGHT_BRACE
        });
        
        std::cout << "HTML标签测试完成" << std::endl;
    }
    
    void testCssSelectors() {
        std::cout << "\n--- 测试CSS选择器 ---" << std::endl;
        
        // 测试类选择器
        testTokenSequence(".class", {TokenType::CLASS_SELECTOR});
        testTokenSequence(".my-class", {TokenType::CLASS_SELECTOR});
        testTokenSequence(".className123", {TokenType::CLASS_SELECTOR});
        
        // 测试ID选择器
        testTokenSequence("#id", {TokenType::ID_SELECTOR});
        testTokenSequence("#my-id", {TokenType::ID_SELECTOR});
        testTokenSequence("#idName123", {TokenType::ID_SELECTOR});
        
        // 测试伪类和伪元素
        testTokenSequence(":hover", {TokenType::PSEUDO_CLASS});
        testTokenSequence("::before", {TokenType::PSEUDO_ELEMENT});
        testTokenSequence("&:hover", {TokenType::PSEUDO_CLASS});
        testTokenSequence("&::after", {TokenType::PSEUDO_ELEMENT});
        
        std::cout << "CSS选择器测试完成" << std::endl;
    }
    
    void testComments() {
        std::cout << "\n--- 测试注释 ---" << std::endl;
        
        // 测试单行注释
        testTokenSequence("// 这是单行注释", {TokenType::COMMENT_SINGLE});
        
        // 测试多行注释
        testTokenSequence("/* 这是多行注释 */", {TokenType::COMMENT_MULTI});
        
        // 测试生成器注释
        testTokenSequence("-- 这是生成器注释", {TokenType::COMMENT_GENERATOR});
        
        std::cout << "注释测试完成" << std::endl;
    }
    
    void testStrings() {
        std::cout << "\n--- 测试字符串 ---" << std::endl;
        
        // 测试双引号字符串
        testTokenSequence("\"hello world\"", {TokenType::STRING_DOUBLE_QUOTE});
        
        // 测试单引号字符串
        testTokenSequence("'hello world'", {TokenType::STRING_SINGLE_QUOTE});
        
        // 测试无引号字符串
        testTokenSequence("hello", {TokenType::IDENTIFIER});
        
        std::cout << "字符串测试完成" << std::endl;
    }
    
    void testKeywords() {
        std::cout << "\n--- 测试关键字 ---" << std::endl;
        
        // 测试基础关键字
        testTokenSequence("text", {TokenType::TEXT});
        testTokenSequence("style", {TokenType::STYLE});
        
        // 测试修饰符关键字
        testTokenSequence("[Custom]", {TokenType::CUSTOM});
        testTokenSequence("[Template]", {TokenType::TEMPLATE});
        testTokenSequence("[Origin]", {TokenType::ORIGIN});
        testTokenSequence("[Configuration]", {TokenType::CONFIGURATION});
        
        // 测试操作符关键字
        testTokenSequence("add", {TokenType::ADD});
        testTokenSequence("delete", {TokenType::DELETE});
        testTokenSequence("inherit", {TokenType::INHERIT});
        testTokenSequence("from", {TokenType::FROM});
        testTokenSequence("as", {TokenType::AS});
        
        // 测试@关键字
        testTokenSequence("@Style", {TokenType::CUSTOM_STYLE});
        testTokenSequence("@Element", {TokenType::CUSTOM_ELEMENT});
        testTokenSequence("@Var", {TokenType::CUSTOM_VAR});
        
        std::cout << "关键字测试完成" << std::endl;
    }
    
    void testNumbers() {
        std::cout << "\n--- 测试数字 ---" << std::endl;
        
        testTokenSequence("123", {TokenType::NUMBER});
        testTokenSequence("123.456", {TokenType::NUMBER});
        testTokenSequence("0", {TokenType::NUMBER});
        testTokenSequence("3.14", {TokenType::NUMBER});
        
        std::cout << "数字测试完成" << std::endl;
    }
    
    void testContextSwitching() {
        std::cout << "\n--- 测试上下文切换 ---" << std::endl;
        
        // 测试样式块上下文
        std::string styleCode = R"(
            div {
                style {
                    .my-class {
                        color: red;
                    }
                }
            }
        )";
        
        BasicLexer lexer(styleCode);
        auto tokens = lexer.tokenize();
        
        // 验证上下文信息被正确设置
        bool foundStyleBlock = false;
        for (const auto& token : tokens) {
            if (token.type == TokenType::CLASS_SELECTOR) {
                foundStyleBlock = token.isInStyleBlock;
                break;
            }
        }
        
        assertTest(foundStyleBlock, "样式块上下文应该被正确设置");
        
        std::cout << "上下文切换测试完成" << std::endl;
    }
    
    void testErrorHandling() {
        std::cout << "\n--- 测试错误处理 ---" << std::endl;
        
        // 测试未终止的字符串
        BasicLexer lexer1("\"unterminated string");
        auto tokens1 = lexer1.tokenize();
        assertTest(lexer1.hasError(), "应该检测到未终止的字符串错误");
        
        // 测试未终止的括号
        BasicLexer lexer2("[unterminated bracket");
        auto tokens2 = lexer2.tokenize();
        assertTest(lexer2.hasError(), "应该检测到未终止的括号错误");
        
        std::cout << "错误处理测试完成" << std::endl;
    }
    
    void testTokenSequence(const std::string& input, const std::vector<TokenType>& expectedTypes) {
        totalTests_++;
        
        BasicLexer lexer(input);
        auto tokens = lexer.tokenize();
        
        // 移除EOF token进行比较
        if (!tokens.empty() && tokens.back().type == TokenType::EOF_TOKEN) {
            tokens.pop_back();
        }
        
        bool passed = true;
        std::string errorMsg;
        
        if (tokens.size() != expectedTypes.size()) {
            passed = false;
            errorMsg = "Token数量不匹配. 期望: " + std::to_string(expectedTypes.size()) + 
                      ", 实际: " + std::to_string(tokens.size());
        } else {
            for (size_t i = 0; i < tokens.size(); ++i) {
                if (tokens[i].type != expectedTypes[i]) {
                    passed = false;
                    errorMsg = "第" + std::to_string(i) + "个Token类型不匹配. 期望: " + 
                              TokenUtils::tokenTypeToString(expectedTypes[i]) + 
                              ", 实际: " + TokenUtils::tokenTypeToString(tokens[i].type);
                    break;
                }
            }
        }
        
        if (passed) {
            passedTests_++;
            std::cout << "✓ 测试通过: \"" << input << "\"" << std::endl;
        } else {
            std::cout << "✗ 测试失败: \"" << input << "\" - " << errorMsg << std::endl;
            
            // 输出详细的Token信息
            std::cout << "  实际Token序列:" << std::endl;
            for (size_t i = 0; i < tokens.size(); ++i) {
                std::cout << "    [" << i << "] " << TokenUtils::tokenTypeToString(tokens[i].type) 
                         << " (\"" << tokens[i].value << "\")" << std::endl;
            }
        }
    }
    
    void assertTest(bool condition, const std::string& message) {
        totalTests_++;
        
        if (condition) {
            passedTests_++;
            std::cout << "✓ " << message << std::endl;
        } else {
            std::cout << "✗ " << message << std::endl;
        }
    }
};

} // namespace test
} // namespace chtl

int main() {
    chtl::test::BasicLexerTest test;
    test.runAllTests();
    return 0;
}