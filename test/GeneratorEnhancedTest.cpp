#include "../src/generator/HtmlGenerator.h"
#include "../src/parser/BasicParser.h"
#include "../src/lexer/BasicLexer.h"
#include <iostream>
#include <cassert>

using namespace chtl;

class GeneratorEnhancedTest {
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
    
    NodePtr parseString(const std::string& chtl) {
        BasicLexer lexer;
        auto tokens = lexer.tokenize(chtl);
        
        BasicParser parser;
        return parser.parse(tokens);
    }
    
    std::string generateHtml(const std::string& chtl) {
        auto ast = parseString(chtl);
        HtmlGenerator generator;
        return generator.generateCompleteHtml(ast);
    }
    
public:
    void runAllTests() {
        std::cout << "=== Enhanced Selector Tests ===" << std::endl;
        
        testAmpersandWithClass();
        testAmpersandWithId();
        testAmpersandWithoutContext();
        testAmpersandPriority();
        testComplexAmpersand();
        
        std::cout << "\nSummary: " << testsPassed << " passed, " 
                  << testsFailed << " failed" << std::endl;
    }
    
    void testAmpersandWithClass() {
        std::cout << "\n-- Ampersand with Class Context --" << std::endl;
        
        // 测试1: 已有class属性
        std::string chtl = R"(
            div {
                class: "mybox";
                style {
                    &:hover { color: "red"; }
                }
            }
        )";
        
        std::string html = generateHtml(chtl);
        test("Uses existing class", html.find(".mybox:hover") != std::string::npos);
        
        // 测试2: 通过.xxx选择器设置的类
        chtl = R"(
            div {
                style {
                    .box { width: "100px"; }
                    &:hover { color: "blue"; }
                }
            }
        )";
        
        html = generateHtml(chtl);
        test("Uses class from selector", html.find(".box:hover") != std::string::npos);
        test("Auto assigns class", html.find("class=\"box\"") != std::string::npos);
    }
    
    void testAmpersandWithId() {
        std::cout << "\n-- Ampersand with ID Context --" << std::endl;
        
        // 测试1: 已有id属性
        std::string chtl = R"(
            div {
                id: "mydiv";
                style {
                    &:hover { color: "green"; }
                }
            }
        )";
        
        std::string html = generateHtml(chtl);
        test("Uses existing ID", html.find("#mydiv:hover") != std::string::npos);
        
        // 测试2: 通过#xxx选择器设置的ID
        chtl = R"(
            div {
                style {
                    #content { padding: "10px"; }
                    &:focus { outline: "none"; }
                }
            }
        )";
        
        html = generateHtml(chtl);
        test("Uses ID from selector", html.find("#content:focus") != std::string::npos);
    }
    
    void testAmpersandWithoutContext() {
        std::cout << "\n-- Ampersand without Context --" << std::endl;
        
        std::string chtl = R"(
            div {
                style {
                    &:hover { color: "purple"; }
                }
            }
        )";
        
        std::string html = generateHtml(chtl);
        test("Creates auto class", html.find("chtl-auto-") != std::string::npos);
        test("Has hover pseudo-class", html.find(":hover") != std::string::npos);
    }
    
    void testAmpersandPriority() {
        std::cout << "\n-- Ampersand Priority (Class > ID) --" << std::endl;
        
        // 同时有class和id时，class优先
        std::string chtl = R"(
            div {
                class: "myclass";
                id: "myid";
                style {
                    &:hover { color: "orange"; }
                }
            }
        )";
        
        std::string html = generateHtml(chtl);
        test("Class takes priority over ID", html.find(".myclass:hover") != std::string::npos);
        test("ID not used for &", html.find("#myid:hover") == std::string::npos);
    }
    
    void testComplexAmpersand() {
        std::cout << "\n-- Complex Ampersand Usage --" << std::endl;
        
        std::string chtl = R"(
            div {
                style {
                    .card {
                        border: "1px solid #ccc";
                    }
                    & > span {
                        font-weight: "bold";
                    }
                    &:hover {
                        background: "#f0f0f0";
                    }
                    &::before {
                        content: "'*'";
                    }
                }
            }
        )";
        
        std::string html = generateHtml(chtl);
        test("Direct child selector", html.find(".card > span") != std::string::npos);
        test("Hover pseudo-class", html.find(".card:hover") != std::string::npos);
        test("Before pseudo-element", html.find(".card::before") != std::string::npos);
    }
};

int main() {
    GeneratorEnhancedTest test;
    test.runAllTests();
    return 0;
}