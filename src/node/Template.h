#ifndef CHTL_TEMPLATE_H
#define CHTL_TEMPLATE_H

#include "Node.h"
#include <string>

namespace chtl {

// 模板节点基类
class TemplateNode : public Node {
protected:
    std::string name;
    
public:
    TemplateNode(NodeType type, const std::string& n, int line = 0, int col = 0)
        : Node(type, line, col), name(n) {}
    
    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }
};

// 模板样式节点 - [Template] @Style
class TemplateStyleNode : public TemplateNode {
public:
    TemplateStyleNode(const std::string& name, int line = 0, int col = 0)
        : TemplateNode(NodeType::TEMPLATE_STYLE_NODE, name, line, col) {}
    
    std::string toString() const override {
        return "TemplateStyleNode: " + name;
    }
};

// 模板元素节点 - [Template] @Element
class TemplateElementNode : public TemplateNode {
public:
    TemplateElementNode(const std::string& name, int line = 0, int col = 0)
        : TemplateNode(NodeType::TEMPLATE_ELEMENT_NODE, name, line, col) {}
    
    std::string toString() const override {
        return "TemplateElementNode: " + name;
    }
};

// 模板变量节点 - [Template] @Var
class TemplateVarNode : public TemplateNode {
public:
    TemplateVarNode(const std::string& name, int line = 0, int col = 0)
        : TemplateNode(NodeType::TEMPLATE_VAR_NODE, name, line, col) {}
    
    std::string toString() const override {
        return "TemplateVarNode: " + name;
    }
};

} // namespace chtl

#endif // CHTL_TEMPLATE_H