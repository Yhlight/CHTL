#include "../src/parser/BasicParser.h"
#include "../src/lexer/BasicLexer.h"
#include <iostream>
#include <cassert>

namespace chtl {
namespace test {

/**
 * BasicParser测试类
 * 测试语法分析器的各种功能
 */
class BasicParserTest {
public:
    void runAllTests() {
        std::cout << "=== BasicParser 测试开始 ===" << std::endl;
        
        testBasicParsing();
        testElementParsing();
        testAttributeParsing();
        testStyleParsing();
        testTextParsing();
        testCommentParsing();
        testErrorHandling();
        testComplexDocument();
        
        std::cout << "=== BasicParser 测试完成 ===" << std::endl;
        std::cout << "总计: " << totalTests_ << " 个测试, " 
                  << passedTests_ << " 个通过, " 
                  << (totalTests_ - passedTests_) << " 个失败" << std::endl;
    }

private:
    int totalTests_ = 0;
    int passedTests_ = 0;
    
    void testBasicParsing() {
        std::cout << "\n--- 测试基础解析功能 ---" << std::endl;
        
        // 测试空文档
        {
            std::vector<Token> tokens = {
                {TokenType::EOF_TOKEN, "", Position()}
            };
            
            auto parser = ParserFactory::createBasicParser(tokens);
            auto ast = parser->parse();
            
            assertTest(ast != nullptr, "空文档应该返回有效AST");
            assertTest(ast->getType() == NodeType::DOCUMENT, "根节点应该是DOCUMENT类型");
            assertTest(ast->getChildCount() == 0, "空文档应该没有子节点");
            assertTest(!parser->hasErrors(), "空文档解析不应该有错误");
        }
        
        // 测试基础Token序列
        {
            std::vector<Token> tokens = {
                {TokenType::HTML_TAG, "div", Position(1, 1)},
                {TokenType::LEFT_BRACE, "{", Position(1, 5)},
                {TokenType::RIGHT_BRACE, "}", Position(2, 1)},
                {TokenType::EOF_TOKEN, "", Position(3, 1)}
            };
            
            auto parser = ParserFactory::createBasicParser(tokens);
            auto ast = parser->parse();
            
            assertTest(ast != nullptr, "基础元素应该解析成功");
            assertTest(ast->getChildCount() == 1, "应该有一个子元素");
            
            auto element = std::dynamic_pointer_cast<ElementNode>(ast->getChild(0));
            assertTest(element != nullptr, "子节点应该是ElementNode");
            assertTest(element->getTagName() == "div", "标签名应该是div");
        }
        
        std::cout << "基础解析功能测试完成" << std::endl;
    }
    
    void testElementParsing() {
        std::cout << "\n--- 测试元素解析 ---" << std::endl;
        
        // 测试简单元素
        {
            std::string chtlCode = "html { }";
            auto tokens = tokenizeString(chtlCode);
            auto parser = ParserFactory::createBasicParser(tokens);
            auto ast = parser->parse();
            
            assertTest(ast->getChildCount() == 1, "应该有一个HTML元素");
            auto element = std::dynamic_pointer_cast<ElementNode>(ast->getChild(0));
            assertTest(element != nullptr, "应该是ElementNode");
            assertTest(element->getTagName() == "html", "标签名应该是html");
        }
        
        // 测试嵌套元素
        {
            std::vector<Token> tokens = {
                {TokenType::HTML_TAG, "div", Position(1, 1)},
                {TokenType::LEFT_BRACE, "{", Position(1, 5)},
                {TokenType::HTML_TAG, "span", Position(2, 3)},
                {TokenType::LEFT_BRACE, "{", Position(2, 8)},
                {TokenType::RIGHT_BRACE, "}", Position(2, 9)},
                {TokenType::RIGHT_BRACE, "}", Position(3, 1)},
                {TokenType::EOF_TOKEN, "", Position(4, 1)}
            };
            
            auto parser = ParserFactory::createBasicParser(tokens);
            auto ast = parser->parse();
            
            assertTest(ast->getChildCount() == 1, "应该有一个根元素");
            auto div = std::dynamic_pointer_cast<ElementNode>(ast->getChild(0));
            assertTest(div != nullptr, "根元素应该是ElementNode");
            assertTest(div->getTagName() == "div", "根元素应该是div");
            assertTest(div->getChildCount() == 1, "div应该有一个子元素");
            
            auto span = std::dynamic_pointer_cast<ElementNode>(div->getChild(0));
            assertTest(span != nullptr, "子元素应该是ElementNode");
            assertTest(span->getTagName() == "span", "子元素应该是span");
        }
        
        std::cout << "元素解析测试完成" << std::endl;
    }
    
    void testAttributeParsing() {
        std::cout << "\n--- 测试属性解析 ---" << std::endl;
        
        // 测试简单属性
        {
            std::vector<Token> tokens = {
                {TokenType::HTML_TAG, "div", Position(1, 1)},
                {TokenType::LEFT_BRACE, "{", Position(1, 5)},
                {TokenType::IDENTIFIER, "id", Position(2, 3)},
                {TokenType::COLON, ":", Position(2, 5)},
                {TokenType::STRING_DOUBLE_QUOTE, "container", Position(2, 7)},
                {TokenType::SEMICOLON, ";", Position(2, 18)},
                {TokenType::RIGHT_BRACE, "}", Position(3, 1)},
                {TokenType::EOF_TOKEN, "", Position(4, 1)}
            };
            
            auto parser = ParserFactory::createBasicParser(tokens);
            auto ast = parser->parse();
            
            auto element = std::dynamic_pointer_cast<ElementNode>(ast->getChild(0));
            assertTest(element != nullptr, "应该解析出元素");
            assertTest(element->hasAttribute("id"), "元素应该有id属性");
            assertTest(element->getAttributeValue("id") == "container", "id属性值应该正确");
        }
        
        // 测试多个属性
        {
            std::vector<Token> tokens = {
                {TokenType::HTML_TAG, "div", Position(1, 1)},
                {TokenType::LEFT_BRACE, "{", Position(1, 5)},
                {TokenType::IDENTIFIER, "id", Position(2, 3)},
                {TokenType::COLON, ":", Position(2, 5)},
                {TokenType::STRING_DOUBLE_QUOTE, "container", Position(2, 7)},
                {TokenType::SEMICOLON, ";", Position(2, 18)},
                {TokenType::IDENTIFIER, "class", Position(3, 3)},
                {TokenType::COLON, ":", Position(3, 8)},
                {TokenType::STRING_SINGLE_QUOTE, "main", Position(3, 10)},
                {TokenType::SEMICOLON, ";", Position(3, 16)},
                {TokenType::RIGHT_BRACE, "}", Position(4, 1)},
                {TokenType::EOF_TOKEN, "", Position(5, 1)}
            };
            
            auto parser = ParserFactory::createBasicParser(tokens);
            auto ast = parser->parse();
            
            auto element = std::dynamic_pointer_cast<ElementNode>(ast->getChild(0));
            assertTest(element->hasAttribute("id"), "应该有id属性");
            assertTest(element->hasAttribute("class"), "应该有class属性");
            assertTest(element->getAttributeValue("id") == "container", "id值应该正确");
            assertTest(element->getAttributeValue("class") == "main", "class值应该正确");
        }
        
        std::cout << "属性解析测试完成" << std::endl;
    }
    
    void testStyleParsing() {
        std::cout << "\n--- 测试样式解析 ---" << std::endl;
        
        // 测试内联样式
        {
            std::vector<Token> tokens = {
                {TokenType::STYLE, "style", Position(1, 1)},
                {TokenType::LEFT_BRACE, "{", Position(1, 7)},
                {TokenType::IDENTIFIER, "width", Position(2, 3)},
                {TokenType::COLON, ":", Position(2, 8)},
                {TokenType::STRING_NO_QUOTE, "100px", Position(2, 10)},
                {TokenType::SEMICOLON, ";", Position(2, 15)},
                {TokenType::RIGHT_BRACE, "}", Position(3, 1)},
                {TokenType::EOF_TOKEN, "", Position(4, 1)}
            };
            
            auto parser = ParserFactory::createBasicParser(tokens);
            auto ast = parser->parse();
            
            auto styleBlock = std::dynamic_pointer_cast<StyleBlockNode>(ast->getChild(0));
            assertTest(styleBlock != nullptr, "应该解析出样式块");
            assertTest(styleBlock->hasInlineStyles(), "应该有内联样式");
            
            auto inlineProps = styleBlock->getInlineProperties();
            assertTest(inlineProps.find("width") != inlineProps.end(), "应该有width属性");
            assertTest(inlineProps.at("width") == "100px", "width值应该正确");
        }
        
        // 测试CSS规则
        {
            std::vector<Token> tokens = {
                {TokenType::STYLE, "style", Position(1, 1)},
                {TokenType::LEFT_BRACE, "{", Position(1, 7)},
                {TokenType::CLASS_SELECTOR, ".container", Position(2, 3)},
                {TokenType::LEFT_BRACE, "{", Position(2, 14)},
                {TokenType::IDENTIFIER, "background", Position(3, 5)},
                {TokenType::COLON, ":", Position(3, 15)},
                {TokenType::STRING_DOUBLE_QUOTE, "red", Position(3, 17)},
                {TokenType::SEMICOLON, ";", Position(3, 22)},
                {TokenType::RIGHT_BRACE, "}", Position(4, 3)},
                {TokenType::RIGHT_BRACE, "}", Position(5, 1)},
                {TokenType::EOF_TOKEN, "", Position(6, 1)}
            };
            
            auto parser = ParserFactory::createBasicParser(tokens);
            auto ast = parser->parse();
            
            auto styleBlock = std::dynamic_pointer_cast<StyleBlockNode>(ast->getChild(0));
            assertTest(styleBlock != nullptr, "应该解析出样式块");
            assertTest(styleBlock->hasCssRules(), "应该有CSS规则");
            
            auto rules = styleBlock->getRules();
            assertTest(rules.size() == 1, "应该有一个CSS规则");
            assertTest(rules[0]->getSelectorString() == ".container", "选择器应该正确");
        }
        
        std::cout << "样式解析测试完成" << std::endl;
    }
    
    void testTextParsing() {
        std::cout << "\n--- 测试文本解析 ---" << std::endl;
        
        {
            std::vector<Token> tokens = {
                {TokenType::TEXT, "text", Position(1, 1)},
                {TokenType::LEFT_BRACE, "{", Position(1, 6)},
                {TokenType::STRING_DOUBLE_QUOTE, "Hello World", Position(2, 3)},
                {TokenType::RIGHT_BRACE, "}", Position(3, 1)},
                {TokenType::EOF_TOKEN, "", Position(4, 1)}
            };
            
            auto parser = ParserFactory::createBasicParser(tokens);
            auto ast = parser->parse();
            
            auto textNode = std::dynamic_pointer_cast<TextNode>(ast->getChild(0));
            assertTest(textNode != nullptr, "应该解析出文本节点");
            assertTest(textNode->getText() == "Hello World", "文本内容应该正确");
        }
        
        std::cout << "文本解析测试完成" << std::endl;
    }
    
    void testCommentParsing() {
        std::cout << "\n--- 测试注释解析 ---" << std::endl;
        
        {
            std::vector<Token> tokens = {
                {TokenType::COMMENT_SINGLE, "// This is a comment", Position(1, 1)},
                {TokenType::COMMENT_MULTI, "/* Block comment */", Position(2, 1)},
                {TokenType::COMMENT_GENERATOR, "-- Generator comment", Position(3, 1)},
                {TokenType::EOF_TOKEN, "", Position(4, 1)}
            };
            
            auto parser = ParserFactory::createBasicParser(tokens);
            auto ast = parser->parse();
            
            assertTest(ast->getChildCount() == 3, "应该有3个注释节点");
            
            auto comment1 = std::dynamic_pointer_cast<CommentNode>(ast->getChild(0));
            auto comment2 = std::dynamic_pointer_cast<CommentNode>(ast->getChild(1));
            auto comment3 = std::dynamic_pointer_cast<CommentNode>(ast->getChild(2));
            
            assertTest(comment1 != nullptr, "第一个应该是注释节点");
            assertTest(comment2 != nullptr, "第二个应该是注释节点");
            assertTest(comment3 != nullptr, "第三个应该是注释节点");
            
            assertTest(comment1->getCommentType() == CommentNode::CommentType::SINGLE_LINE, "应该是单行注释");
            assertTest(comment2->getCommentType() == CommentNode::CommentType::MULTI_LINE, "应该是多行注释");
            assertTest(comment3->getCommentType() == CommentNode::CommentType::GENERATOR, "应该是生成器注释");
        }
        
        std::cout << "注释解析测试完成" << std::endl;
    }
    
    void testErrorHandling() {
        std::cout << "\n--- 测试错误处理 ---" << std::endl;
        
        // 测试未匹配的大括号
        {
            std::vector<Token> tokens = {
                {TokenType::HTML_TAG, "div", Position(1, 1)},
                {TokenType::LEFT_BRACE, "{", Position(1, 5)},
                {TokenType::EOF_TOKEN, "", Position(2, 1)}
            };
            
            auto parser = ParserFactory::createBasicParser(tokens);
            auto ast = parser->parse();
            
            assertTest(parser->hasErrors(), "应该有解析错误");
            
            const auto& errors = parser->getErrors();
            assertTest(!errors.empty(), "错误列表不应该为空");
        }
        
        // 测试无效属性语法
        {
            std::vector<Token> tokens = {
                {TokenType::HTML_TAG, "div", Position(1, 1)},
                {TokenType::LEFT_BRACE, "{", Position(1, 5)},
                {TokenType::IDENTIFIER, "id", Position(2, 3)},
                {TokenType::COLON, ":", Position(2, 5)},
                // 缺少属性值
                {TokenType::SEMICOLON, ";", Position(2, 6)},
                {TokenType::RIGHT_BRACE, "}", Position(3, 1)},
                {TokenType::EOF_TOKEN, "", Position(4, 1)}
            };
            
            auto parser = ParserFactory::createBasicParser(tokens);
            auto ast = parser->parse();
            
            assertTest(parser->hasErrors(), "应该检测到属性语法错误");
        }
        
        std::cout << "错误处理测试完成" << std::endl;
    }
    
    void testComplexDocument() {
        std::cout << "\n--- 测试复杂文档解析 ---" << std::endl;
        
        // 使用词法分析器生成Token序列
        std::string chtlCode = R"(
html
{
    head
    {
        style
        {
            .container
            {
                width: 100%;
                margin: auto;
            }
        }
    }
    
    body
    {
        div
        {
            id: "main";
            class: "container";
            
            text
            {
                "Welcome to CHTL"
            }
            
            span
            {
                class: "highlight";
                text
                {
                    "Powered by CHTL Compiler"
                }
            }
        }
    }
}
)";
        
        auto tokens = tokenizeString(chtlCode);
        auto parser = ParserFactory::createBasicParser(tokens);
        auto ast = parser->parse();
        
        assertTest(ast != nullptr, "复杂文档应该解析成功");
        assertTest(ast->getType() == NodeType::DOCUMENT, "根节点应该是DOCUMENT");
        assertTest(ast->getChildCount() == 1, "应该有一个html元素");
        
        auto html = std::dynamic_pointer_cast<ElementNode>(ast->getChild(0));
        assertTest(html != nullptr, "应该是ElementNode");
        assertTest(html->getTagName() == "html", "应该是html标签");
        assertTest(html->getChildCount() == 2, "html应该有2个子元素（head和body）");
        
        // 验证结构
        auto head = std::dynamic_pointer_cast<ElementNode>(html->getChild(0));
        auto body = std::dynamic_pointer_cast<ElementNode>(html->getChild(1));
        assertTest(head != nullptr && head->getTagName() == "head", "第一个子元素应该是head");
        assertTest(body != nullptr && body->getTagName() == "body", "第二个子元素应该是body");
        
        std::cout << "复杂文档解析测试完成" << std::endl;
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
    
    // 辅助方法：使用BasicLexer对字符串进行词法分析
    std::vector<Token> tokenizeString(const std::string& input) {
        BasicLexer lexer(input);
        return lexer.tokenize();
    }
};

} // namespace test
} // namespace chtl

int main() {
    chtl::test::BasicParserTest test;
    test.runAllTests();
    return 0;
}