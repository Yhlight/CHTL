#include "../src/node/Node.h"
#include "../src/node/ElementNode.h"
#include "../src/node/StyleNode.h"
#include "../src/node/CustomNode.h"
#include <iostream>
#include <cassert>

namespace chtl {
namespace test {

/**
 * Node系统测试类
 * 测试AST节点的创建、操作和验证
 */
class NodeTest {
public:
    void runAllTests() {
        std::cout << "=== Node系统 测试开始 ===" << std::endl;
        
        testBasicNodeOperations();
        testNodeHierarchy();
        testNodeAttributes();
        testNodeUtils();
        testNodeValidation();
        
        std::cout << "=== Node系统 测试完成 ===" << std::endl;
        std::cout << "总计: " << totalTests_ << " 个测试, " 
                  << passedTests_ << " 个通过, " 
                  << (totalTests_ - passedTests_) << " 个失败" << std::endl;
    }

private:
    int totalTests_ = 0;
    int passedTests_ = 0;
    
    void testBasicNodeOperations() {
        std::cout << "\n--- 测试基础Node操作 ---" << std::endl;
        
        // 测试Node创建
        auto node = std::make_shared<Node>(NodeType::HTML_ELEMENT);
        assertTest(node->getType() == NodeType::HTML_ELEMENT, "Node类型应该正确设置");
        
        // 测试名称设置
        node->setName("div");
        assertTest(node->getName() == "div", "Node名称应该正确设置");
        
        // 测试内容设置
        node->setContent("test content");
        assertTest(node->getContent() == "test content", "Node内容应该正确设置");
        
        // 测试toString
        std::string nodeStr = node->toString();
        assertTest(!nodeStr.empty(), "toString应该返回非空字符串");
        assertTest(nodeStr.find("HTML_ELEMENT") != std::string::npos, "toString应该包含节点类型");
        
        std::cout << "基础Node操作测试完成" << std::endl;
    }
    
    void testNodeHierarchy() {
        std::cout << "\n--- 测试Node层次结构 ---" << std::endl;
        
        // 创建父子节点
        auto parent = std::make_shared<Node>(NodeType::HTML_ELEMENT);
        auto child1 = std::make_shared<Node>(NodeType::TEXT_NODE);
        auto child2 = std::make_shared<Node>(NodeType::COMMENT_NODE);
        
        parent->setName("div");
        child1->setName("text1");
        child2->setName("comment1");
        
        // 测试添加子节点
        parent->addChild(child1);
        parent->addChild(child2);
        
        assertTest(parent->getChildCount() == 2, "父节点应该有2个子节点");
        assertTest(child1->getParent() == parent, "子节点应该正确设置父节点");
        assertTest(child2->getParent() == parent, "子节点应该正确设置父节点");
        
        // 测试获取子节点
        auto firstChild = parent->getChild(0);
        assertTest(firstChild == child1, "第一个子节点应该是child1");
        
        auto secondChild = parent->getChild(1);
        assertTest(secondChild == child2, "第二个子节点应该是child2");
        
        // 测试查找子节点
        auto foundChild = parent->findChildByName("text1");
        assertTest(foundChild == child1, "应该能通过名称找到子节点");
        
        auto textNodes = parent->findChildren(NodeType::TEXT_NODE);
        assertTest(textNodes.size() == 1, "应该找到1个文本节点");
        assertTest(textNodes[0] == child1, "找到的文本节点应该是child1");
        
        // 测试移除子节点
        parent->removeChild(child1);
        assertTest(parent->getChildCount() == 1, "移除后应该剩1个子节点");
        assertTest(child1->getParent() == nullptr, "被移除的子节点父指针应该为空");
        
        std::cout << "Node层次结构测试完成" << std::endl;
    }
    
    void testNodeAttributes() {
        std::cout << "\n--- 测试Node属性 ---" << std::endl;
        
        auto node = std::make_shared<Node>(NodeType::HTML_ELEMENT);
        
        // 测试设置和获取属性
        node->setAttribute("class", "container");
        node->setAttribute("id", "main");
        
        assertTest(node->hasAttribute("class"), "应该有class属性");
        assertTest(node->getAttribute("class") == "container", "class属性值应该正确");
        assertTest(node->hasAttribute("id"), "应该有id属性");
        assertTest(node->getAttribute("id") == "main", "id属性值应该正确");
        
        // 测试属性覆盖
        node->setAttribute("class", "new-container");
        assertTest(node->getAttribute("class") == "new-container", "属性值应该被正确覆盖");
        
        // 测试获取所有属性
        auto attributes = node->getAttributes();
        assertTest(attributes.size() == 2, "应该有2个属性");
        assertTest(attributes.find("class") != attributes.end(), "应该包含class属性");
        assertTest(attributes.find("id") != attributes.end(), "应该包含id属性");
        
        // 测试移除属性
        node->removeAttribute("class");
        assertTest(!node->hasAttribute("class"), "class属性应该被移除");
        assertTest(node->hasAttribute("id"), "id属性应该仍然存在");
        
        std::cout << "Node属性测试完成" << std::endl;
    }
    
    void testNodeUtils() {
        std::cout << "\n--- 测试NodeUtils ---" << std::endl;
        
        // 测试nodeTypeToString
        std::string typeStr = NodeUtils::nodeTypeToString(NodeType::HTML_ELEMENT);
        assertTest(typeStr == "HTML_ELEMENT", "nodeTypeToString应该返回正确字符串");
        
        // 测试stringToNodeType
        NodeType type = NodeUtils::stringToNodeType("HTML_ELEMENT");
        assertTest(type == NodeType::HTML_ELEMENT, "stringToNodeType应该返回正确类型");
        
        // 测试未知类型
        NodeType unknownType = NodeUtils::stringToNodeType("INVALID_TYPE");
        assertTest(unknownType == NodeType::UNKNOWN, "未知类型应该返回UNKNOWN");
        
        // 测试节点计数
        auto root = std::make_shared<Node>(NodeType::ROOT);
        auto child1 = std::make_shared<Node>(NodeType::HTML_ELEMENT);
        auto child2 = std::make_shared<Node>(NodeType::TEXT_NODE);
        auto grandchild = std::make_shared<Node>(NodeType::ATTRIBUTE);
        
        root->addChild(child1);
        root->addChild(child2);
        child1->addChild(grandchild);
        
        size_t totalNodes = NodeUtils::countNodes(root);
        assertTest(totalNodes == 4, "总节点数应该是4");
        
        size_t elementNodes = NodeUtils::countNodesByType(root, NodeType::HTML_ELEMENT);
        assertTest(elementNodes == 1, "HTML元素节点数应该是1");
        
        // 测试节点收集
        auto allTextNodes = NodeUtils::collectNodes(root, NodeType::TEXT_NODE);
        assertTest(allTextNodes.size() == 1, "应该收集到1个文本节点");
        assertTest(allTextNodes[0] == child2, "收集到的文本节点应该是child2");
        
        std::cout << "NodeUtils测试完成" << std::endl;
    }
    
    void testNodeValidation() {
        std::cout << "\n--- 测试Node验证 ---" << std::endl;
        
        // 测试有效节点
        auto validNode = std::make_shared<Node>(NodeType::HTML_ELEMENT);
        assertTest(validNode->validate(), "有效节点应该通过验证");
        
        // 测试无效节点
        auto invalidNode = std::make_shared<Node>(NodeType::UNKNOWN);
        assertTest(!invalidNode->validate(), "UNKNOWN类型节点应该验证失败");
        
        // 测试树验证
        auto root = std::make_shared<Node>(NodeType::ROOT);
        auto validChild = std::make_shared<Node>(NodeType::HTML_ELEMENT);
        root->addChild(validChild);
        
        assertTest(NodeUtils::validateTree(root), "有效树应该通过验证");
        
        // 添加无效子节点
        auto invalidChild = std::make_shared<Node>(NodeType::UNKNOWN);
        root->addChild(invalidChild);
        
        assertTest(!NodeUtils::validateTree(root), "包含无效节点的树应该验证失败");
        
        // 测试获取验证错误
        auto errors = NodeUtils::getValidationErrors(root);
        assertTest(!errors.empty(), "应该有验证错误");
        
        std::cout << "Node验证测试完成" << std::endl;
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
    chtl::test::NodeTest test;
    test.runAllTests();
    return 0;
}