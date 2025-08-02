#include "PredefinedParser.h"
#include "../parser/BasicParser.h"
#include "../lexer/BasicLexer.h"
#include "../node/Node.h"
#include "../node/Custom.h"
#include "../node/Template.h"

namespace chtl {

std::shared_ptr<Node> PredefinedParser::parseChtlCode(const std::string& chtlCode) {
    // 使用BasicLexer和BasicParser解析CHTL代码
    BasicLexer lexer;
    auto tokens = lexer.tokenize(chtlCode);
    
    BasicParser parser;
    return parser.parse(tokens);
}

template<typename T>
std::shared_ptr<T> PredefinedParser::extractNode(std::shared_ptr<Node> root, 
                                                 const std::string& name) {
    if (!root) return nullptr;
    
    // 遍历根节点的子节点，查找匹配的节点
    for (const auto& child : root->getChildren()) {
        auto targetNode = std::dynamic_pointer_cast<T>(child);
        if (targetNode && targetNode->getName() == name) {
            return targetNode;
        }
    }
    
    return nullptr;
}

std::shared_ptr<CustomStyleNode> PredefinedParser::parseCustomStyle(const std::string& name, 
                                                                   const std::string& chtlCode) {
    auto root = parseChtlCode(chtlCode);
    return extractNode<CustomStyleNode>(root, name);
}

std::shared_ptr<CustomElementNode> PredefinedParser::parseCustomElement(const std::string& name, 
                                                                       const std::string& chtlCode) {
    auto root = parseChtlCode(chtlCode);
    return extractNode<CustomElementNode>(root, name);
}

std::shared_ptr<CustomVarNode> PredefinedParser::parseCustomVar(const std::string& name, 
                                                               const std::string& chtlCode) {
    auto root = parseChtlCode(chtlCode);
    return extractNode<CustomVarNode>(root, name);
}

std::shared_ptr<TemplateStyleNode> PredefinedParser::parseTemplateStyle(const std::string& name, 
                                                                       const std::string& chtlCode) {
    auto root = parseChtlCode(chtlCode);
    return extractNode<TemplateStyleNode>(root, name);
}

std::shared_ptr<TemplateElementNode> PredefinedParser::parseTemplateElement(const std::string& name, 
                                                                           const std::string& chtlCode) {
    auto root = parseChtlCode(chtlCode);
    return extractNode<TemplateElementNode>(root, name);
}

std::shared_ptr<TemplateVarNode> PredefinedParser::parseTemplateVar(const std::string& name, 
                                                                   const std::string& chtlCode) {
    auto root = parseChtlCode(chtlCode);
    return extractNode<TemplateVarNode>(root, name);
}

// 显式实例化模板函数
template std::shared_ptr<CustomStyleNode> PredefinedParser::extractNode<CustomStyleNode>(
    std::shared_ptr<Node> root, const std::string& name);
template std::shared_ptr<CustomElementNode> PredefinedParser::extractNode<CustomElementNode>(
    std::shared_ptr<Node> root, const std::string& name);
template std::shared_ptr<CustomVarNode> PredefinedParser::extractNode<CustomVarNode>(
    std::shared_ptr<Node> root, const std::string& name);
template std::shared_ptr<TemplateStyleNode> PredefinedParser::extractNode<TemplateStyleNode>(
    std::shared_ptr<Node> root, const std::string& name);
template std::shared_ptr<TemplateElementNode> PredefinedParser::extractNode<TemplateElementNode>(
    std::shared_ptr<Node> root, const std::string& name);
template std::shared_ptr<TemplateVarNode> PredefinedParser::extractNode<TemplateVarNode>(
    std::shared_ptr<Node> root, const std::string& name);

} // namespace chtl