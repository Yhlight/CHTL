#include "../src/lexer/BasicLexer.h"
#include "../src/lexer/ConfigLexer.h"
#include <iostream>
#include <vector>
#include <sstream>

using namespace chtl;

class LexerTest {
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
    
    std::string tokensToString(const std::vector<Token>& tokens) {
        std::stringstream ss;
        for (const auto& token : tokens) {
            if (token.type != TokenType::END_OF_FILE) {
                ss << tokenTypeToStr(token.type) << "(" << token.value << ") ";
            }
        }
        return ss.str();
    }
    
public:
    void runAllTests() {
        std::cout << "=== Lexer Tests ===" << std::endl;
        
        testBasicTokenization();
        testComments();
        testStrings();
        testKeywords();
        testStyleBlock();
        testConfigurationBlock();
        testComplexChtl();
        
        std::cout << "\nSummary: " << testsPassed << " passed, " 
                  << testsFailed << " failed" << std::endl;
    }
    
    void testBasicTokenization() {
        std::cout << "\n-- Basic Tokenization Tests --" << std::endl;
        
        BasicLexer lexer;
        
        // Test simple element
        auto tokens = lexer.tokenize("div { }");
        test("Simple element tokenization", 
             tokens.size() == 4 && // div, {, }, EOF
             tokens[0].type == TokenType::IDENTIFIER &&
             tokens[0].value == "div" &&
             tokens[1].type == TokenType::LEFT_BRACE &&
             tokens[2].type == TokenType::RIGHT_BRACE);
        
        // Test attribute
        tokens = lexer.tokenize("id: \"myId\";");
        test("Attribute tokenization",
             tokens.size() == 5 && // id, :, "myId", ;, EOF
             tokens[0].type == TokenType::IDENTIFIER &&
             tokens[1].type == TokenType::COLON &&
             tokens[2].type == TokenType::STRING_LITERAL &&
             tokens[2].value == "myId" &&
             tokens[3].type == TokenType::SEMICOLON);
        
        // Test numbers
        tokens = lexer.tokenize("width: 100px;");
        test("Number with unit tokenization",
             tokens.size() == 5 &&
             tokens[0].type == TokenType::IDENTIFIER &&
             tokens[2].type == TokenType::NUMBER &&
             tokens[2].value == "100px");
    }
    
    void testComments() {
        std::cout << "\n-- Comment Tests --" << std::endl;
        
        BasicLexer lexer;
        
        // Single line comment
        auto tokens = lexer.tokenize("// This is a comment\ndiv { }");
        test("Single line comment",
             tokens[0].type == TokenType::COMMENT_SINGLE &&
             tokens[0].value == " This is a comment");
        
        // Multi line comment
        tokens = lexer.tokenize("/* Multi\nline\ncomment */\nspan { }");
        test("Multi line comment",
             tokens[0].type == TokenType::COMMENT_MULTI &&
             tokens[0].value == " Multi\nline\ncomment ");
        
        // Generator comment
        tokens = lexer.tokenize("-- Generator comment\ndiv { }");
        test("Generator comment",
             tokens[0].type == TokenType::COMMENT_GENERATOR &&
             tokens[0].value == " Generator comment");
    }
    
    void testStrings() {
        std::cout << "\n-- String Tests --" << std::endl;
        
        BasicLexer lexer;
        
        // Double quoted string
        auto tokens = lexer.tokenize("\"Hello World\"");
        test("Double quoted string",
             tokens[0].type == TokenType::STRING_LITERAL &&
             tokens[0].value == "Hello World");
        
        // Single quoted string
        tokens = lexer.tokenize("'Single quoted'");
        test("Single quoted string",
             tokens[0].type == TokenType::STRING_LITERAL_SINGLE &&
             tokens[0].value == "Single quoted");
        
        // Unquoted string in attribute value
        tokens = lexer.tokenize("color: red;");
        test("Unquoted string detection",
             tokens.size() == 5 &&
             tokens[0].type == TokenType::IDENTIFIER &&
             tokens[1].type == TokenType::COLON &&
             tokens[2].type == TokenType::IDENTIFIER &&
             tokens[2].value == "red");
    }
    
    void testKeywords() {
        std::cout << "\n-- Keyword Tests --" << std::endl;
        
        BasicLexer lexer;
        
        // Test basic keywords
        auto tokens = lexer.tokenize("text { \"content\" }");
        test("'text' keyword",
             tokens[0].type == TokenType::KEYWORD_TEXT);
        
        tokens = lexer.tokenize("style { }");
        test("'style' keyword",
             tokens[0].type == TokenType::KEYWORD_STYLE);
        
        tokens = lexer.tokenize("add div { }");
        test("'add' keyword",
             tokens[0].type == TokenType::KEYWORD_ADD);
        
        // Test bracket keywords
        tokens = lexer.tokenize("[Custom] @Style MyStyle { }");
        test("'[Custom]' keyword",
             tokens[0].type == TokenType::KEYWORD_CUSTOM);
        
        // Test @ keywords
        tokens = lexer.tokenize("@Style Text { }");
        test("'@Style' keyword",
             tokens[0].type == TokenType::AT_STYLE);
    }
    
    void testStyleBlock() {
        std::cout << "\n-- Style Block Tests --" << std::endl;
        
        BasicLexer lexer;
        
        // Test class selector
        auto tokens = lexer.tokenize("style { .myClass { } }");
        bool foundStyleKeyword = false;
        bool foundClassSelector = false;
        
        for (const auto& token : tokens) {
            if (token.type == TokenType::KEYWORD_STYLE) {
                foundStyleKeyword = true;
            }
            if (token.type == TokenType::STYLE_SELECTOR_CLASS &&
                token.value == ".myClass") {
                foundClassSelector = true;
            }
        }
        
        test("Class selector in style block",
             foundStyleKeyword && foundClassSelector);
        
        // Test ID selector
        tokens = lexer.tokenize("style { #myId { } }");
        bool foundIdSelector = false;
        
        for (const auto& token : tokens) {
            if (token.type == TokenType::STYLE_SELECTOR_ID &&
                token.value == "#myId") {
                foundIdSelector = true;
            }
        }
        
        test("ID selector in style block", foundIdSelector);
        
        // Test pseudo-class
        tokens = lexer.tokenize("style { &:hover { } }");
        bool foundPseudoClass = false;
        
        for (const auto& token : tokens) {
            if (token.type == TokenType::STYLE_SELECTOR_PSEUDO &&
                token.value == "&:hover") {
                foundPseudoClass = true;
            }
        }
        
        test("Pseudo-class selector in style block", foundPseudoClass);
    }
    
    void testConfigurationBlock() {
        std::cout << "\n-- Configuration Block Tests --" << std::endl;
        
        ConfigLexer lexer;
        
        std::string configCode = R"(
[Configuration]
{
    INDEX_INITIAL_COUNT = 0;
    CUSTOM_STYLE = @Style;
    KEYWORD_TEXT = mytext;
}

mytext { "Hello" }
)";
        
        auto tokens = lexer.tokenize(configCode);
        
        // 查找自定义的 'mytext' 关键字
        bool foundCustomText = false;
        for (const auto& token : tokens) {
            if (token.type == TokenType::KEYWORD_TEXT && 
                token.value == "mytext") {
                foundCustomText = true;
                break;
            }
        }
        
        test("Configuration-driven keyword replacement", foundCustomText);
    }
    
    void testComplexChtl() {
        std::cout << "\n-- Complex CHTL Tests --" << std::endl;
        
        BasicLexer lexer;
        
        std::string complexCode = R"(
[Custom] @Style DefaultText
{
    color: "black";
    line-height: 1.6;
}

html {
    head {
        style {
            // Global styles
        }
    }
    
    body {
        div {
            id: "main";
            class: "container";
            
            style {
                .container {
                    width: 100%;
                }
                
                &:hover {
                    background: "gray";
                }
            }
            
            text {
                "Welcome to CHTL"
            }
        }
    }
}
)";
        
        auto tokens = lexer.tokenize(complexCode);
        
        // 验证关键token的存在
        bool hasCustom = false;
        bool hasAtStyle = false;
        bool hasText = false;
        bool hasClassSelector = false;
        bool hasPseudoSelector = false;
        
        for (const auto& token : tokens) {
            if (token.type == TokenType::KEYWORD_CUSTOM) hasCustom = true;
            if (token.type == TokenType::AT_STYLE) hasAtStyle = true;
            if (token.type == TokenType::KEYWORD_TEXT) hasText = true;
            if (token.type == TokenType::STYLE_SELECTOR_CLASS) hasClassSelector = true;
            if (token.type == TokenType::STYLE_SELECTOR_PSEUDO) hasPseudoSelector = true;
        }
        
        test("Complex CHTL tokenization",
             hasCustom && hasAtStyle && hasText && 
             hasClassSelector && hasPseudoSelector);
        
        // 验证没有错误token
        bool hasUnknown = false;
        for (const auto& token : tokens) {
            if (token.type == TokenType::UNKNOWN) {
                hasUnknown = true;
                break;
            }
        }
        
        test("No unknown tokens in complex CHTL", !hasUnknown);
    }
};

int main() {
    LexerTest test;
    test.runAllTests();
    return 0;
}