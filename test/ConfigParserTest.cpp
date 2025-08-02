#include "../src/parser/ConfigParser.h"
#include "../src/lexer/BasicLexer.h"
#include "../src/lexer/ConfigLexer.h"
#include <iostream>
#include <memory>

using namespace chtl;

class ConfigParserTest {
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
    
    NodePtr parseString(const std::string& content, bool useConfigLexer = false) {
        std::vector<Token> tokens;
        
        if (useConfigLexer) {
            ConfigLexer lexer;
            tokens = lexer.tokenize(content);
        } else {
            BasicLexer lexer;
            tokens = lexer.tokenize(content);
        }
        
        ConfigParser parser;
        return parser.parse(tokens);
    }
    
public:
    void runAllTests() {
        std::cout << "=== ConfigParser Tests ===" << std::endl;
        
        testDefaultConfiguration();
        testCustomKeywords();
        testGroupOptions();
        testNameBlock();
        testDynamicParsing();
        testComplexConfiguration();
        
        std::cout << "\nSummary: " << testsPassed << " passed, " 
                  << testsFailed << " failed" << std::endl;
    }
    
    void testDefaultConfiguration() {
        std::cout << "\n-- Default Configuration Tests --" << std::endl;
        
        ConfigParser parser;
        
        // 测试默认配置值
        test("Default INDEX_INITIAL_COUNT", parser.getIndexInitialCount() == 0);
        test("Default DEBUG_MODE", !parser.isDebugMode());
        test("Default KEYWORD_TEXT", parser.getConfig("KEYWORD_TEXT") == "text");
        test("Default CUSTOM_STYLE", parser.getConfig("CUSTOM_STYLE") == "@Style");
    }
    
    void testCustomKeywords() {
        std::cout << "\n-- Custom Keywords Tests --" << std::endl;
        
        // 测试自定义关键字
        auto root = parseString(
            "[Configuration] {\n"
            "    KEYWORD_TEXT = texto;\n"
            "    KEYWORD_STYLE = estilo;\n"
            "}\n"
            "\n"
            "texto { \"Hola Mundo\" }\n"
            "estilo { padding: 10px; }");
        
        test("Parse with custom keywords", root && root->getChildCount() >= 3);
        
        // 找到文本节点
        bool foundText = false;
        bool foundStyle = false;
        
        for (const auto& child : root->getChildren()) {
            if (child->getType() == NodeType::TEXT_NODE) {
                foundText = true;
            }
            if (child->getType() == NodeType::STYLE_NODE) {
                foundStyle = true;
            }
        }
        
        test("Custom text keyword works", foundText);
        test("Custom style keyword works", foundStyle);
    }
    
    void testGroupOptions() {
        std::cout << "\n-- Group Options Tests --" << std::endl;
        
        // 测试组选项
        auto root = parseString(
            "[Configuration] {\n"
            "    CUSTOM_STYLE = [@Style, @style, @CSS];\n"
            "}\n"
            "\n"
            "[Custom] @CSS ButtonStyle {\n"
            "    color: blue;\n"
            "}\n"
            "[Custom] @style LinkStyle {\n"
            "    color: red;\n"
            "}");
        
        test("Parse with group options", root && root->getChildCount() >= 3);
        
        // 统计自定义样式节点
        int customStyleCount = 0;
        for (const auto& child : root->getChildren()) {
            if (child->getType() == NodeType::CUSTOM_STYLE_NODE) {
                customStyleCount++;
            }
        }
        
        test("Group options for custom style", customStyleCount == 2);
    }
    
    void testNameBlock() {
        std::cout << "\n-- Name Block Tests --" << std::endl;
        
        // 测试[Name]块
        auto root = parseString(
            "[Configuration] {\n"
            "    DISABLE_NAME_GROUP = false;\n"
            "    \n"
            "    [Name] {\n"
            "        KEYWORD_ADD = [add, plus, append];\n"
            "        KEYWORD_DELETE = [delete, remove, drop];\n"
            "    }\n"
            "}\n"
            "\n"
            "plus @Style NewStyle;\n"
            "remove @Element OldElement;");
        
        test("Parse with Name block", root && root->getChildCount() >= 3);
        
        // 查找操作节点
        bool foundAdd = false;
        bool foundDelete = false;
        
        for (const auto& child : root->getChildren()) {
            if (child->getType() == NodeType::ADD_NODE) {
                foundAdd = true;
            }
            if (child->getType() == NodeType::DELETE_NODE) {
                foundDelete = true;
            }
        }
        
        test("Name block add keyword", foundAdd);
        test("Name block delete keyword", foundDelete);
    }
    
    void testDynamicParsing() {
        std::cout << "\n-- Dynamic Parsing Tests --" << std::endl;
        
        // 测试动态解析
        auto root = parseString(
            "[Configuration] {\n"
            "    KEYWORD_CUSTOM = [Customize];\n"
            "    CUSTOM_ELEMENT = @Component;\n"
            "    TEMPLATE_STYLE = @TemplateStyle;\n"
            "}\n"
            "\n"
            "[Customize] @Component MyBox {\n"
            "    div { }\n"
            "}\n"
            "[Template] @TemplateStyle CardStyle {\n"
            "    border: 1px solid;\n"
            "}");
        
        test("Parse with dynamic keywords", root && root->getChildCount() >= 3);
        
        // 验证节点类型
        bool foundCustomElement = false;
        bool foundTemplateStyle = false;
        
        for (const auto& child : root->getChildren()) {
            if (child->getType() == NodeType::CUSTOM_ELEMENT_NODE) {
                foundCustomElement = true;
            }
            if (child->getType() == NodeType::TEMPLATE_STYLE_NODE) {
                foundTemplateStyle = true;
            }
        }
        
        test("Dynamic custom element", foundCustomElement);
        test("Dynamic template style", foundTemplateStyle);
    }
    
    void testComplexConfiguration() {
        std::cout << "\n-- Complex Configuration Tests --" << std::endl;
        
        // 测试复杂配置
        auto root = parseString(
            "[Configuration] {\n"
            "    INDEX_INITIAL_COUNT = 1;\n"
            "    DEBUG_MODE = true;\n"
            "    \n"
            "    // 自定义所有关键字\n"
            "    KEYWORD_TEXT = txt;\n"
            "    KEYWORD_STYLE = css;\n"
            "    KEYWORD_IMPORT = [Import];\n"
            "    KEYWORD_FROM = de;\n"
            "    KEYWORD_AS = como;\n"
            "    \n"
            "    [Name] {\n"
            "        ORIGIN_HTML = [@HTML, @html, @webpage];\n"
            "        ORIGIN_STYLE = [@CSS, @css, @styles];\n"
            "        ORIGIN_JAVASCRIPT = [@JS, @js, @script];\n"
            "    }\n"
            "}\n"
            "\n"
            "[Import] @Style de \"styles.css\" como MainStyles\n"
            "\n"
            "txt { \"Hello\" }\n"
            "\n"
            "[Origin] @webpage {\n"
            "    div class=\"test\"\n"
            "}\n"
            "\n"
            "css {\n"
            "    body { margin: 0; }\n"
            "}");
        
        test("Parse complex configuration", root && root->getChildCount() >= 5);
        
        // 验证各种节点
        bool hasImport = false;
        bool hasText = false;
        bool hasOrigin = false;
        bool hasStyle = false;
        
        for (const auto& child : root->getChildren()) {
            if (child->getType() == NodeType::IMPORT_NODE) {
                hasImport = true;
                auto importNode = std::dynamic_pointer_cast<ImportNode>(child);
                test("Custom 'as' keyword", importNode && importNode->getAsName() == "MainStyles");
            }
            if (child->getType() == NodeType::TEXT_NODE) hasText = true;
            if (child->getType() == NodeType::ORIGIN_HTML_NODE) hasOrigin = true;
            if (child->getType() == NodeType::STYLE_NODE) hasStyle = true;
        }
        
        test("Complex config has import", hasImport);
        test("Complex config has text", hasText);
        test("Complex config has origin", hasOrigin);
        test("Complex config has style", hasStyle);
        
        // 测试配置值
        ConfigParser parser;
        auto tokens = BasicLexer().tokenize(
            "[Configuration] { INDEX_INITIAL_COUNT = 1; DEBUG_MODE = true; }");
        parser.parse(tokens);
        
        test("Config index initial count", parser.getIndexInitialCount() == 1);
        test("Config debug mode", parser.isDebugMode());
    }
};

int main() {
    ConfigParserTest test;
    test.runAllTests();
    return 0;
}