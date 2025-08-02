#include "../src/common/Token.h"
#include <iostream>
#include <cassert>

using namespace chtl;

// 简单的测试框架
class TokenTest {
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
        std::cout << "=== Token System Tests ===" << std::endl;
        
        testTokenCreation();
        testTokenTypeToString();
        testKeywordIdentification();
        testTokenTypeMapping();
        
        std::cout << "\nSummary: " << testsPassed << " passed, " 
                  << testsFailed << " failed" << std::endl;
    }
    
    void testTokenCreation() {
        std::cout << "\n-- Token Creation Tests --" << std::endl;
        
        Token token1(TokenType::IDENTIFIER, "myVar", 1, 5);
        test("Token creation with values", 
             token1.type == TokenType::IDENTIFIER &&
             token1.value == "myVar" &&
             token1.line == 1 &&
             token1.column == 5);
        
        Token token2;
        test("Default token creation",
             token2.type == TokenType::UNKNOWN &&
             token2.value.empty() &&
             token2.line == 0 &&
             token2.column == 0);
    }
    
    void testTokenTypeToString() {
        std::cout << "\n-- Token Type to String Tests --" << std::endl;
        
        test("IDENTIFIER to string",
             tokenTypeToStr(TokenType::IDENTIFIER) == "IDENTIFIER");
        
        test("KEYWORD_TEXT to string",
             tokenTypeToStr(TokenType::KEYWORD_TEXT) == "KEYWORD_TEXT");
        
        test("LEFT_BRACE to string",
             tokenTypeToStr(TokenType::LEFT_BRACE) == "LEFT_BRACE");
        
        test("AT_STYLE to string",
             tokenTypeToStr(TokenType::AT_STYLE) == "AT_STYLE");
        
        test("KEYWORD_CUSTOM to string",
             tokenTypeToStr(TokenType::KEYWORD_CUSTOM) == "KEYWORD_CUSTOM");
    }
    
    void testKeywordIdentification() {
        std::cout << "\n-- Keyword Identification Tests --" << std::endl;
        
        test("'text' is keyword", isKeyword("text"));
        test("'style' is keyword", isKeyword("style"));
        test("'add' is keyword", isKeyword("add"));
        test("'delete' is keyword", isKeyword("delete"));
        test("'[Custom]' is keyword", isKeyword("[Custom]"));
        test("'@Style' is keyword", isKeyword("@Style"));
        
        test("'myVar' is not keyword", !isKeyword("myVar"));
        test("'div' is not keyword", !isKeyword("div"));
    }
    
    void testTokenTypeMapping() {
        std::cout << "\n-- Token Type Mapping Tests --" << std::endl;
        
        test("'text' maps to KEYWORD_TEXT",
             getKeywordType("text") == TokenType::KEYWORD_TEXT);
        
        test("'style' maps to KEYWORD_STYLE",
             getKeywordType("style") == TokenType::KEYWORD_STYLE);
        
        test("'add' maps to KEYWORD_ADD",
             getKeywordType("add") == TokenType::KEYWORD_ADD);
        
        test("'[Custom]' maps to KEYWORD_CUSTOM",
             getKeywordType("[Custom]") == TokenType::KEYWORD_CUSTOM);
        
        test("'@Style' maps to AT_STYLE",
             getKeywordType("@Style") == TokenType::AT_STYLE);
        
        test("'@Element' maps to AT_ELEMENT",
             getKeywordType("@Element") == TokenType::AT_ELEMENT);
        
        test("'unknown' maps to IDENTIFIER",
             getKeywordType("unknown") == TokenType::IDENTIFIER);
    }
};

int main() {
    TokenTest test;
    test.runAllTests();
    return 0;
}