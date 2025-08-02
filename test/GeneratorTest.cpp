#include "../src/generator/HtmlGenerator.h"
#include "../src/parser/BasicParser.h"
#include "../src/lexer/BasicLexer.h"
#include <iostream>
#include <cassert>

using namespace chtl;

class GeneratorTest {
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
        std::cout << "=== Generator Tests ===" << std::endl;
        
        testBasicGeneration();
        testTextNode();
        testElementWithAttributes();
        testNestedElements();
        testInlineStyles();
        testStyleBlocks();
        testComments();
        testMinification();
        testStyleHoisting();
        testChtlFeatures();
        
        std::cout << "\nSummary: " << testsPassed << " passed, " 
                  << testsFailed << " failed" << std::endl;
    }
    
    void testBasicGeneration() {
        std::cout << "\n-- Basic Generation Tests --" << std::endl;
        
        std::string chtl = "div { }";
        std::string html = generateHtml(chtl);
        
        test("Basic div generation", html.find("<div>") != std::string::npos);
        test("Closing tag", html.find("</div>") != std::string::npos);
        test("DOCTYPE included", html.find("<!DOCTYPE html>") != std::string::npos);
    }
    
    void testTextNode() {
        std::cout << "\n-- Text Node Tests --" << std::endl;
        
        std::string chtl = R"(
            div {
                text { "Hello World" }
            }
        )";
        
        std::string html = generateHtml(chtl);
        test("Text content", html.find("Hello World") != std::string::npos);
        
        // Test HTML escaping
        std::string chtlEscape = R"(
            div {
                text { "Hello <world> & \"friends\"" }
            }
        )";
        
        html = generateHtml(chtlEscape);
        test("HTML escaping", html.find("Hello &lt;world&gt; &amp; &quot;friends&quot;") != std::string::npos);
    }
    
    void testElementWithAttributes() {
        std::cout << "\n-- Element with Attributes Tests --" << std::endl;
        
        std::string chtl = R"(
            div {
                id: "myDiv";
                class: "container main";
                data-value: "123";
            }
        )";
        
        std::string html = generateHtml(chtl);
        test("ID attribute", html.find("id=\"myDiv\"") != std::string::npos);
        test("Class attribute", html.find("class=\"container main\"") != std::string::npos);
        test("Data attribute", html.find("data-value=\"123\"") != std::string::npos);
    }
    
    void testNestedElements() {
        std::cout << "\n-- Nested Elements Tests --" << std::endl;
        
        std::string chtl = R"(
            html {
                head {
                    title { text { "Test Page" } }
                }
                body {
                    div {
                        p { text { "Paragraph" } }
                    }
                }
            }
        )";
        
        std::string html = generateHtml(chtl);
        test("HTML structure", html.find("<html>") != std::string::npos);
        test("Head element", html.find("<head>") != std::string::npos);
        test("Title content", html.find("<title>Test Page</title>") != std::string::npos);
        test("Nested paragraph", html.find("<p>Paragraph</p>") != std::string::npos);
    }
    
    void testInlineStyles() {
        std::cout << "\n-- Inline Styles Tests --" << std::endl;
        
        std::string chtl = R"(
            div {
                style {
                    width: "100px";
                    height: "200px";
                    background-color: "red";
                }
            }
        )";
        
        std::string html = generateHtml(chtl);
        test("Inline style attribute", html.find("style=\"") != std::string::npos);
        test("Width property", html.find("width: 100px") != std::string::npos);
        test("Height property", html.find("height: 200px") != std::string::npos);
        test("Background color", html.find("background-color: red") != std::string::npos);
    }
    
    void testStyleBlocks() {
        std::cout << "\n-- Style Blocks Tests --" << std::endl;
        
        std::string chtl = R"(
            div {
                style {
                    .myClass {
                        color: "blue";
                    }
                    #myId {
                        font-size: "16px";
                    }
                }
            }
        )";
        
        std::string html = generateHtml(chtl);
        test("Auto class assignment", html.find("class=\"myClass\"") != std::string::npos);
        test("Auto ID assignment", html.find("id=\"myId\"") != std::string::npos);
        test("Style tag generated", html.find("<style>") != std::string::npos);
        test("Class rule", html.find(".myClass {") != std::string::npos);
        test("ID rule", html.find("#myId {") != std::string::npos);
    }
    
    void testComments() {
        std::cout << "\n-- Comments Tests --" << std::endl;
        
        std::string chtl = R"(
            // This is a regular comment
            div {
                /* Multi-line
                   comment */
                -- This is a generator comment
            }
        )";
        
        std::string html = generateHtml(chtl);
        test("Regular comment ignored", html.find("This is a regular comment") == std::string::npos);
        test("Multi-line comment ignored", html.find("Multi-line") == std::string::npos);
        test("Generator comment included", html.find("<!-- This is a generator comment-->") != std::string::npos);
    }
    
    void testMinification() {
        std::cout << "\n-- Minification Tests --" << std::endl;
        
        GeneratorConfig config;
        config.minify = true;
        config.prettyPrint = false;
        
        HtmlGenerator generator(config);
        
        std::string chtl = R"(
            div {
                p {
                    text { "Hello" }
                }
            }
        )";
        
        auto ast = parseString(chtl);
        std::string html = generator.generateCompleteHtml(ast);
        
        test("No newlines in minified", html.find("\n  ") == std::string::npos);
        test("Compact output", html.find("<div><p>Hello</p></div>") != std::string::npos);
    }
    
    void testStyleHoisting() {
        std::cout << "\n-- Style Hoisting Tests --" << std::endl;
        
        std::string chtl = R"(
            html {
                head { }
                body {
                    div {
                        style {
                            .test { color: "red"; }
                        }
                    }
                }
            }
        )";
        
        std::string html = generateHtml(chtl);
        
        // Check that style is hoisted to head
        size_t headStart = html.find("<head>");
        size_t headEnd = html.find("</head>");
        size_t stylePos = html.find("<style>");
        
        test("Style hoisted to head", 
             stylePos != std::string::npos && 
             stylePos > headStart && 
             stylePos < headEnd);
    }
    
    void testChtlFeatures() {
        std::cout << "\n-- CHTL Features Tests --" << std::endl;
        
        // Test void elements
        std::string chtl = R"(
            div {
                img { src: "test.jpg"; alt: "Test"; }
                br { }
                input { type: "text"; }
            }
        )";
        
        std::string html = generateHtml(chtl);
        test("Self-closing img", html.find("<img src=\"test.jpg\" alt=\"Test\" />") != std::string::npos);
        test("Self-closing br", html.find("<br />") != std::string::npos);
        test("Self-closing input", html.find("<input type=\"text\" />") != std::string::npos);
        
        // Test Origin embedding
        chtl = R"(
            [Origin] @Html {
                <div>Raw HTML</div>
            }
        )";
        
        html = generateHtml(chtl);
        test("Origin HTML", html.find("<div>Raw HTML</div>") != std::string::npos);
    }
};

int main() {
    GeneratorTest test;
    test.runAllTests();
    return 0;
}