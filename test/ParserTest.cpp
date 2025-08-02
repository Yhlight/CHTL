#include "../src/parser/BasicParser.h"
#include "../src/lexer/BasicLexer.h"
#include "../src/loader/ChtlLoader.h"
#include <iostream>
#include <fstream>
#include <memory>

using namespace chtl;

class ParserTest {
private:
    int testsPassed = 0;
    int testsFailed = 0;
    std::string testDir = "/tmp/chtl_parser_test";
    
    void test(const std::string& name, bool condition) {
        if (condition) {
            std::cout << "[PASS] " << name << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] " << name << std::endl;
            testsFailed++;
        }
    }
    
    void createTestFile(const std::string& path, const std::string& content) {
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash != std::string::npos) {
            std::string dir = path.substr(0, lastSlash);
            system(("mkdir -p " + dir).c_str());
        }
        
        std::ofstream file(path);
        file << content;
        file.close();
    }
    
    void setupTestFiles() {
        // 创建测试目录
        system(("rm -rf " + testDir).c_str());
        system(("mkdir -p " + testDir + "/styles").c_str());
        system(("mkdir -p " + testDir + "/components").c_str());
        
        // 主文件
        createTestFile(testDir + "/main.chtl",
            "[Import] @Style from \"styles/base.css\"\n"
            "[Import] @Chtl from components.button\n"
            "[Import] [Custom] @Element [Box] from \"components/box.chtl\" as MyBox\n"
            "\n"
            "html {\n"
            "    head {\n"
            "        title {\n"
            "            text { \"Test Page\" }\n"
            "        }\n"
            "    }\n"
            "    body {\n"
            "        div class=\"container\" {\n"
            "            MyBox {\n"
            "                text { \"Hello World\" }\n"
            "            }\n"
            "        }\n"
            "    }\n"
            "}");
        
        // CSS文件
        createTestFile(testDir + "/styles/base.css",
            "body { margin: 0; padding: 0; }\n"
            ".container { max-width: 1200px; margin: 0 auto; }");
        
        // 组件文件
        createTestFile(testDir + "/components/button.chtl",
            "button class=\"btn\" {\n"
            "    style {\n"
            "        padding: 10px 20px;\n"
            "        background: blue;\n"
            "    }\n"
            "}");
        
        createTestFile(testDir + "/components/box.chtl",
            "[Custom] @Element Box {\n"
            "    div class=\"box\" {\n"
            "        style {\n"
            "            border: 1px solid #ccc;\n"
            "            padding: 20px;\n"
            "        }\n"
            "    }\n"
            "}");
        
        // 循环依赖测试
        createTestFile(testDir + "/circular1.chtl",
            "[Import] @Chtl from \"circular2.chtl\"\n"
            "div { }");
            
        createTestFile(testDir + "/circular2.chtl",
            "[Import] @Chtl from \"circular1.chtl\"\n"
            "span { }");
        
        // 通配符导入测试
        createTestFile(testDir + "/wildcard_test.chtl",
            "[Import] from \"components/*\"\n"
            "div { }");
    }
    
    void cleanupTestFiles() {
        system(("rm -rf " + testDir).c_str());
    }
    
    NodePtr parseString(const std::string& content) {
        BasicLexer lexer;
        std::vector<Token> tokens = lexer.tokenize(content);
        
        BasicParser parser;
        return parser.parse(tokens);
    }
    
public:
    void runAllTests() {
        std::cout << "=== Parser Tests ===" << std::endl;
        
        setupTestFiles();
        
        testBasicParsing();
        testImportParsing();
        testImportWithLoader();
        testCircularDependency();
        testWildcardImport();
        testElementParsing();
        testTextNodeParsing();
        
        cleanupTestFiles();
        
        std::cout << "\nSummary: " << testsPassed << " passed, " 
                  << testsFailed << " failed" << std::endl;
    }
    
    void testBasicParsing() {
        std::cout << "\n-- Basic Parsing Tests --" << std::endl;
        
        // 测试空文件
        auto root = parseString("");
        test("Parse empty file", root != nullptr && root->getChildCount() == 0);
        
        // 测试简单元素
        root = parseString("div { }");
        test("Parse simple element", root != nullptr && root->getChildCount() == 1);
        
        if (root && root->getChildCount() > 0) {
            auto child = root->getChild(0);
            test("Element is div", 
                 child->getType() == NodeType::ELEMENT_NODE &&
                 std::dynamic_pointer_cast<ElementNode>(child)->getTagName() == "div");
        }
    }
    
    void testImportParsing() {
        std::cout << "\n-- Import Parsing Tests --" << std::endl;
        
        // 测试HTML导入
        auto root = parseString("[Import] @Html from \"test.html\"");
        test("Parse HTML import", root != nullptr && root->getChildCount() == 1);
        
        if (root && root->getChildCount() > 0) {
            auto node = std::dynamic_pointer_cast<ImportNode>(root->getChild(0));
            test("Import type is HTML", 
                 node && node->getImportType() == ImportNode::IMPORT_HTML);
            test("Import path is correct", 
                 node && node->getFromPath() == "test.html");
        }
        
        // 测试带重命名的导入
        root = parseString("[Import] @Style from \"style.css\" as MainStyle");
        if (root && root->getChildCount() > 0) {
            auto node = std::dynamic_pointer_cast<ImportNode>(root->getChild(0));
            test("Import with 'as' name", 
                 node && node->getAsName() == "MainStyle");
        }
        
        // 测试自定义元素导入
        root = parseString("[Import] [Custom] @Element [Button] from \"button.chtl\"");
        if (root && root->getChildCount() > 0) {
            auto node = std::dynamic_pointer_cast<ImportNode>(root->getChild(0));
            test("Custom element import type", 
                 node && node->getImportType() == ImportNode::IMPORT_CUSTOM_ELEMENT);
            test("Custom element target name", 
                 node && node->getTargetName() == "Button");
        }
        
        // 测试点号路径
        root = parseString("[Import] @Chtl from module.submodule.file");
        if (root && root->getChildCount() > 0) {
            auto node = std::dynamic_pointer_cast<ImportNode>(root->getChild(0));
            test("Import with dot notation", 
                 node && node->getFromPath() == "module.submodule.file");
        }
    }
    
    void testImportWithLoader() {
        std::cout << "\n-- Import with Loader Tests --" << std::endl;
        
        auto loader = std::make_shared<ChtlLoader>(testDir);
        BasicParser parser(loader);
        parser.setCurrentFilePath(testDir + "/test.chtl");
        
        // 测试路径解析
        BasicLexer lexer;
        std::vector<Token> tokens = lexer.tokenize(
            "[Import] @Style from \"styles/base.css\"");
        auto root = parser.parse(tokens);
        
        if (root && root->getChildCount() > 0) {
            auto node = std::dynamic_pointer_cast<ImportNode>(root->getChild(0));
            test("Import path resolved", 
                 node && node->getIsResolved());
            test("Import marked as loaded", 
                 node && node->getIsLoaded());
            
            if (node && node->getIsResolved()) {
                std::cout << "  Resolved path: " << node->getResolvedPath() << std::endl;
            }
        }
    }
    
    void testCircularDependency() {
        std::cout << "\n-- Circular Dependency Tests --" << std::endl;
        
        auto loader = std::make_shared<ChtlLoader>(testDir);
        BasicParser parser(loader);
        parser.setCurrentFilePath(testDir + "/circular1.chtl");
        
        BasicLexer lexer;
        std::vector<Token> tokens = lexer.tokenize(
            "[Import] @Chtl from \"circular2.chtl\"");
        
        // 预先添加反向依赖以创建循环
        loader->addImportDependency(testDir + "/circular2.chtl", 
                                   testDir + "/circular1.chtl");
        
        bool caughtCircularDep = false;
        try {
            parser.parse(tokens);
        } catch (const ParseError& e) {
            std::string msg = e.what();
            caughtCircularDep = msg.find("Circular dependency") != std::string::npos;
            std::cout << "  Caught error: " << e.what() << std::endl;
        }
        
        test("Detect circular dependency", caughtCircularDep);
    }
    
    void testWildcardImport() {
        std::cout << "\n-- Wildcard Import Tests --" << std::endl;
        
        auto loader = std::make_shared<ChtlLoader>(testDir);
        BasicParser parser(loader);
        parser.setCurrentFilePath(testDir + "/test.chtl");
        
        BasicLexer lexer;
        std::vector<Token> tokens = lexer.tokenize(
            "[Import] from \"components/*\"");
        auto root = parser.parse(tokens);
        
        if (root && root->getChildCount() > 0) {
            auto node = std::dynamic_pointer_cast<ImportNode>(root->getChild(0));
            test("Wildcard import detected", 
                 node && node->getIsWildcard());
            
            if (node) {
                size_t expandedCount = node->getExpandedPaths().size();
                test("Wildcard expanded to files", expandedCount > 0);
                
                if (expandedCount > 0) {
                    std::cout << "  Expanded to " << expandedCount << " files:" << std::endl;
                    for (const auto& path : node->getExpandedPaths()) {
                        std::cout << "    - " << path << std::endl;
                    }
                }
            }
        }
    }
    
    void testElementParsing() {
        std::cout << "\n-- Element Parsing Tests --" << std::endl;
        
        // 测试带属性的元素
        auto root = parseString("div class=\"container\" id=\"main\" { }");
        if (root && root->getChildCount() > 0) {
            auto elem = std::dynamic_pointer_cast<ElementNode>(root->getChild(0));
            test("Element with attributes", elem != nullptr);
            test("Class attribute", 
                 elem && elem->getAttribute("class") == "container");
            test("ID attribute", 
                 elem && elem->getAttribute("id") == "main");
        }
        
        // 测试嵌套元素
        root = parseString("div { span { text { \"Hello\" } } }");
        if (root && root->getChildCount() > 0) {
            auto div = std::dynamic_pointer_cast<ElementNode>(root->getChild(0));
            test("Nested elements", 
                 div && div->getChildCount() == 1);
            
            if (div && div->getChildCount() > 0) {
                auto span = std::dynamic_pointer_cast<ElementNode>(div->getChild(0));
                test("Nested span element", 
                     span && span->getTagName() == "span");
            }
        }
    }
    
    void testTextNodeParsing() {
        std::cout << "\n-- Text Node Parsing Tests --" << std::endl;
        
        // 测试文本节点
        auto root = parseString("text { \"Hello World\" }");
        if (root && root->getChildCount() > 0) {
            auto text = std::dynamic_pointer_cast<TextNode>(root->getChild(0));
            test("Text node parsed", text != nullptr);
            test("Text content correct", 
                 text && text->getContent() == "Hello World");
        }
        
        // 测试单引号字符串
        root = parseString("text { 'Single quotes' }");
        if (root && root->getChildCount() > 0) {
            auto text = std::dynamic_pointer_cast<TextNode>(root->getChild(0));
            test("Single quote text", 
                 text && text->getContent() == "Single quotes");
        }
    }
};

int main() {
    ParserTest test;
    test.runAllTests();
    return 0;
}