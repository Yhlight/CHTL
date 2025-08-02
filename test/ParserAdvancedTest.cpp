#include "../src/parser/BasicParser.h"
#include "../src/lexer/BasicLexer.h"
#include "../src/loader/ChtlLoader.h"
#include <iostream>
#include <memory>

using namespace chtl;

class ParserAdvancedTest {
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
    
    NodePtr parseString(const std::string& content) {
        BasicLexer lexer;
        std::vector<Token> tokens = lexer.tokenize(content);
        
        BasicParser parser;
        return parser.parse(tokens);
    }
    
public:
    void runAllTests() {
        std::cout << "=== Advanced Parser Tests ===" << std::endl;
        
        testConfiguration();
        testCustomDefinitions();
        testTemplateDefinitions();
        testNamespace();
        testOriginEmbedding();
        testOperators();
        testExpect();
        
        std::cout << "\nSummary: " << testsPassed << " passed, " 
                  << testsFailed << " failed" << std::endl;
    }
    
    void testConfiguration() {
        std::cout << "\n-- Configuration Tests --" << std::endl;
        
        // 测试基本配置
        auto root = parseString(
            "[Configuration] {\n"
            "    INDEX_INITIAL_COUNT = 0;\n"
            "    DEBUG_MODE = true;\n"
            "    CUSTOM_STYLE = @Style;\n"
            "}");
        
        test("Parse configuration block", root && root->getChildCount() == 1);
        
        if (root && root->getChildCount() > 0) {
            auto config = std::dynamic_pointer_cast<ConfigurationNode>(root->getChild(0));
            test("Configuration node type", config != nullptr);
            test("Configuration item INDEX_INITIAL_COUNT", 
                 config && config->getConfigItem("INDEX_INITIAL_COUNT") == "0");
            test("Configuration item DEBUG_MODE", 
                 config && config->getConfigItem("DEBUG_MODE") == "true");
            test("Configuration item CUSTOM_STYLE", 
                 config && config->getConfigItem("CUSTOM_STYLE") == "@Style");
        }
        
        // 测试组选项
        root = parseString(
            "[Configuration] {\n"
            "    KEYWORD_STYLE = [style, Style, STYLE];\n"
            "}");
        
        if (root && root->getChildCount() > 0) {
            auto config = std::dynamic_pointer_cast<ConfigurationNode>(root->getChild(0));
            test("Configuration with options", 
                 config && config->getConfigItem("KEYWORD_STYLE") == "style");
        }
    }
    
    void testCustomDefinitions() {
        std::cout << "\n-- Custom Definition Tests --" << std::endl;
        
        // 测试自定义样式
        auto root = parseString(
            "[Custom] @Style ButtonStyle {\n"
            "    padding: 10px 20px;\n"
            "    background: blue;\n"
            "}");
        
        test("Parse custom style", root && root->getChildCount() == 1);
        
        if (root && root->getChildCount() > 0) {
            auto style = std::dynamic_pointer_cast<CustomStyleNode>(root->getChild(0));
            test("Custom style node type", style != nullptr);
            test("Custom style name", style && style->getName() == "ButtonStyle");
            test("Custom style has properties", style && style->getChildCount() > 0);
        }
        
        // 测试自定义元素
        root = parseString(
            "[Custom] @Element Box {\n"
            "    div class=\"box\" {\n"
            "        text { \"Content\" }\n"
            "    }\n"
            "}");
        
        if (root && root->getChildCount() > 0) {
            auto element = std::dynamic_pointer_cast<CustomElementNode>(root->getChild(0));
            test("Custom element node type", element != nullptr);
            test("Custom element name", element && element->getName() == "Box");
            test("Custom element has content", element && element->getChildCount() > 0);
        }
        
        // 测试自定义变量组
        root = parseString(
            "[Custom] @Var ThemeColors {\n"
            "    primary: \"#007bff\";\n"
            "    secondary: \"#6c757d\";\n"
            "}");
        
        if (root && root->getChildCount() > 0) {
            auto varGroup = std::dynamic_pointer_cast<CustomVarNode>(root->getChild(0));
            test("Custom var group node type", varGroup != nullptr);
            test("Custom var group name", varGroup && varGroup->getName() == "ThemeColors");
            test("Custom var primary", 
                 varGroup && varGroup->getVariable("primary") == "#007bff");
            test("Custom var secondary", 
                 varGroup && varGroup->getVariable("secondary") == "#6c757d");
        }
        
        // 测试继承
        root = parseString(
            "[Custom] @Element Box2 {\n"
            "    inherit @Element Box;\n"
            "    div { }\n"
            "}");
        
        if (root && root->getChildCount() > 0) {
            auto element = std::dynamic_pointer_cast<CustomElementNode>(root->getChild(0));
            test("Custom element with inheritance", 
                 element && element->getInheritedElements().size() > 0);
        }
    }
    
    void testTemplateDefinitions() {
        std::cout << "\n-- Template Definition Tests --" << std::endl;
        
        // 测试模板样式
        auto root = parseString(
            "[Template] @Style CardTemplate {\n"
            "    border: 1px solid;\n"
            "    padding: 20px;\n"
            "}");
        
        test("Parse template style", root && root->getChildCount() == 1);
        
        if (root && root->getChildCount() > 0) {
            auto tmpl = std::dynamic_pointer_cast<TemplateStyleNode>(root->getChild(0));
            test("Template style node type", tmpl != nullptr);
            test("Template style name", tmpl && tmpl->getName() == "CardTemplate");
        }
        
        // 测试模板元素
        root = parseString(
            "[Template] @Element CardElement {\n"
            "    div class=\"card\" {\n"
            "        div class=\"header\" { }\n"
            "        div class=\"body\" { }\n"
            "    }\n"
            "}");
        
        if (root && root->getChildCount() > 0) {
            auto tmpl = std::dynamic_pointer_cast<TemplateElementNode>(root->getChild(0));
            test("Template element node type", tmpl != nullptr);
            test("Template element name", tmpl && tmpl->getName() == "CardElement");
        }
    }
    
    void testNamespace() {
        std::cout << "\n-- Namespace Tests --" << std::endl;
        
        // 测试简单命名空间
        auto root = parseString("[Namespace] myspace");
        
        test("Parse simple namespace", root && root->getChildCount() == 1);
        
        if (root && root->getChildCount() > 0) {
            auto ns = std::dynamic_pointer_cast<NamespaceNode>(root->getChild(0));
            test("Namespace node type", ns != nullptr);
            test("Namespace name", ns && ns->getName() == "myspace");
        }
        
        // 测试带内容的命名空间
        root = parseString(
            "[Namespace] components {\n"
            "    [Custom] @Element Button { }\n"
            "}");
        
        if (root && root->getChildCount() > 0) {
            auto ns = std::dynamic_pointer_cast<NamespaceNode>(root->getChild(0));
            test("Namespace with content", ns && ns->getChildCount() > 0);
        }
        
        // 测试嵌套命名空间
        root = parseString(
            "[Namespace] outer {\n"
            "    [Namespace] inner {\n"
            "        div { }\n"
            "    }\n"
            "}");
        
        if (root && root->getChildCount() > 0) {
            auto ns = std::dynamic_pointer_cast<NamespaceNode>(root->getChild(0));
            test("Nested namespace", 
                 ns && ns->getNestedPath().size() > 0);
        }
    }
    
    void testOriginEmbedding() {
        std::cout << "\n-- Origin Embedding Tests --" << std::endl;
        
        // 测试原始HTML嵌入
        auto root = parseString(
            "[Origin] @Html {\n"
            "    div class=\"raw\" id=\"test\"\n"
            "}");
        
        test("Parse origin HTML", root && root->getChildCount() == 1);
        
        if (root && root->getChildCount() > 0) {
            auto origin = std::dynamic_pointer_cast<OriginHtmlNode>(root->getChild(0));
            test("Origin HTML node type", origin != nullptr);
            test("Origin HTML has content", 
                 origin && !origin->getContent().empty());
        }
        
        // 测试原始CSS嵌入
        root = parseString(
            "[Origin] @Style {\n"
            "    .custom { color: red; }\n"
            "}");
        
        if (root && root->getChildCount() > 0) {
            auto origin = std::dynamic_pointer_cast<OriginStyleNode>(root->getChild(0));
            test("Origin Style node type", origin != nullptr);
        }
        
        // 测试原始JavaScript嵌入
        root = parseString(
            "[Origin] @JavaScript {\n"
            "    console.log('Hello');\n"
            "}");
        
        if (root && root->getChildCount() > 0) {
            auto origin = std::dynamic_pointer_cast<OriginJsNode>(root->getChild(0));
            test("Origin JavaScript node type", origin != nullptr);
        }
    }
    
    void testOperators() {
        std::cout << "\n-- Operator Tests --" << std::endl;
        
        // 测试add操作
        auto root = parseString("add @Style ButtonHover;");
        
        test("Parse add operation", root && root->getChildCount() == 1);
        
        if (root && root->getChildCount() > 0) {
            auto addOp = std::dynamic_pointer_cast<AddNode>(root->getChild(0));
            test("Add node type", addOp != nullptr);
            test("Add target", addOp && addOp->getTarget() == "@Style ButtonHover");
        }
        
        // 测试delete操作
        root = parseString("delete @Element Box;");
        
        if (root && root->getChildCount() > 0) {
            auto deleteOp = std::dynamic_pointer_cast<DeleteNode>(root->getChild(0));
            test("Delete node type", deleteOp != nullptr);
            test("Delete target", deleteOp && deleteOp->getTarget() == "@Element Box");
        }
        
        // 测试inherit操作
        root = parseString("inherit @Style BaseStyle;");
        
        if (root && root->getChildCount() > 0) {
            auto inheritOp = std::dynamic_pointer_cast<InheritNode>(root->getChild(0));
            test("Inherit node type", inheritOp != nullptr);
            test("Inherit target", inheritOp && inheritOp->getTarget() == "@Style BaseStyle");
        }
        
        // 测试带内容的add操作
        root = parseString(
            "add @Element NewBox {\n"
            "    div { }\n"
            "}");
        
        if (root && root->getChildCount() > 0) {
            auto addOp = std::dynamic_pointer_cast<AddNode>(root->getChild(0));
            test("Add with content", addOp && addOp->getChildCount() > 0);
        }
    }
    
    void testExpect() {
        std::cout << "\n-- Expect Tests --" << std::endl;
        
        // 测试精准期盼
        auto root = parseString("expect div, span;");
        
        test("Parse expect statement", root && root->getChildCount() == 1);
        
        if (root && root->getChildCount() > 0) {
            auto expect = std::dynamic_pointer_cast<ExpectNode>(root->getChild(0));
            test("Expect node type", expect != nullptr);
            test("Expect items count", 
                 expect && expect->getExpectedItems().size() == 2);
            test("Expect type is precise", 
                 expect && expect->getExpectType() == ExpectNode::EXPECT_PRECISE);
        }
        
        // 测试类型期盼
        root = parseString("expect [Custom];");
        
        if (root && root->getChildCount() > 0) {
            auto expect = std::dynamic_pointer_cast<ExpectNode>(root->getChild(0));
            test("Type expect", 
                 expect && expect->getExpectType() == ExpectNode::EXPECT_TYPE);
        }
        
        // 测试否定期盼
        root = parseString("not expect [Template];");
        
        if (root && root->getChildCount() > 0) {
            auto expect = std::dynamic_pointer_cast<ExpectNode>(root->getChild(0));
            test("Not expect", expect && expect->getIsNot());
            test("Negative expect type", 
                 expect && expect->getExpectType() == ExpectNode::EXPECT_NEGATIVE);
        }
        
        // 测试复杂期盼
        root = parseString("expect @Element Box, [Custom] @Style MyStyle;");
        
        if (root && root->getChildCount() > 0) {
            auto expect = std::dynamic_pointer_cast<ExpectNode>(root->getChild(0));
            test("Complex expect items", 
                 expect && expect->getExpectedItems().size() >= 2);
        }
    }
};

int main() {
    ParserAdvancedTest test;
    test.runAllTests();
    return 0;
}