#ifndef CHTL_NODE_H
#define CHTL_NODE_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "../common/Token.h"

namespace chtl {

// 前向声明
class Node;
using NodePtr = std::shared_ptr<Node>;
using NodeList = std::vector<NodePtr>;

// AST节点类型枚举
enum class NodeType {
    // 根节点
    ROOT,
    
    // 基础节点
    TEXT_NODE,              // text { }
    ELEMENT_NODE,           // HTML元素节点
    
    // 注释节点
    COMMENT_NODE,           // 注释节点
    
    // 属性节点
    ATTRIBUTE_NODE,         // 属性节点
    
    // 样式相关节点
    STYLE_NODE,             // style { }
    STYLE_RULE_NODE,        // CSS规则
    STYLE_PROPERTY_NODE,    // CSS属性
    STYLE_SELECTOR_NODE,    // CSS选择器
    STYLE_GROUP_NODE,       // @Style组
    
    // 自定义相关节点
    CUSTOM_STYLE_NODE,      // [Custom] @Style
    CUSTOM_ELEMENT_NODE,    // [Custom] @Element
    CUSTOM_VAR_NODE,        // [Custom] @Var
    
    // 模板相关节点
    TEMPLATE_STYLE_NODE,    // [Template] @Style
    TEMPLATE_ELEMENT_NODE,  // [Template] @Element
    TEMPLATE_VAR_NODE,      // [Template] @Var
    
    // 配置相关节点
    CONFIGURATION_NODE,     // [Configuration]
    CONFIG_ITEM_NODE,       // 配置项
    CONFIG_NAME_BLOCK_NODE, // [Name]块
    
    // 导入相关节点
    IMPORT_NODE,            // [Import]
    
    // 命名空间相关节点
    NAMESPACE_NODE,         // [Namespace]
    
    // 原始嵌入节点
    ORIGIN_HTML_NODE,       // [Origin] @Html
    ORIGIN_STYLE_NODE,      // [Origin] @Style
    ORIGIN_JS_NODE,         // [Origin] @JavaScript
    
    // 操作节点
    ADD_NODE,               // add操作
    DELETE_NODE,            // delete操作
    INHERIT_NODE,           // inherit操作
    
    // 特殊节点
    INDEX_ACCESS_NODE,      // 索引访问[0]
    VAR_USAGE_NODE,         // 变量使用
    EXPECT_NODE,            // expect约束
    
    // 字面量节点
    STRING_LITERAL_NODE,    // 字符串字面量
    NUMBER_LITERAL_NODE,    // 数字字面量
    IDENTIFIER_NODE,        // 标识符
};

// AST节点基类
class Node {
protected:
    NodeType type;
    NodeList children;
    Token token;  // 关联的token（如果有）
    int line;
    int column;
    
public:
    Node(NodeType t, int l = 0, int c = 0) 
        : type(t), line(l), column(c) {}
    
    virtual ~Node() = default;
    
    // 基本访问器
    NodeType getType() const { return type; }
    const NodeList& getChildren() const { return children; }
    NodeList& getChildren() { return children; }
    int getLine() const { return line; }
    int getColumn() const { return column; }
    
    // 子节点管理
    void addChild(NodePtr child) {
        if (child) {
            children.push_back(child);
        }
    }
    
    void insertChild(size_t index, NodePtr child) {
        if (child && index <= children.size()) {
            children.insert(children.begin() + index, child);
        }
    }
    
    void removeChild(size_t index) {
        if (index < children.size()) {
            children.erase(children.begin() + index);
        }
    }
    
    NodePtr getChild(size_t index) const {
        if (index < children.size()) {
            return children[index];
        }
        return nullptr;
    }
    
    size_t getChildCount() const {
        return children.size();
    }
    
    // 虚函数，子类可以重写
    virtual std::string toString() const {
        return "Node";
    }
    
    virtual bool validate() const {
        return true;
    }
};

// 根节点
class RootNode : public Node {
public:
    RootNode() : Node(NodeType::ROOT) {}
    
    std::string toString() const override {
        return "RootNode";
    }
};

// 文本节点
class TextNode : public Node {
private:
    std::string content;
    
public:
    TextNode(const std::string& text, int line = 0, int col = 0) 
        : Node(NodeType::TEXT_NODE, line, col), content(text) {}
    
    const std::string& getContent() const { return content; }
    void setContent(const std::string& text) { content = text; }
    
    std::string toString() const override {
        return "TextNode: " + content;
    }
};

// 元素节点
class ElementNode : public Node {
private:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    
public:
    ElementNode(const std::string& tag, int line = 0, int col = 0) 
        : Node(NodeType::ELEMENT_NODE, line, col), tagName(tag) {}
    
    const std::string& getTagName() const { return tagName; }
    void setTagName(const std::string& tag) { tagName = tag; }
    
    void setAttribute(const std::string& name, const std::string& value) {
        attributes[name] = value;
    }
    
    std::string getAttribute(const std::string& name) const {
        auto it = attributes.find(name);
        if (it != attributes.end()) {
            return it->second;
        }
        return "";
    }
    
    bool hasAttribute(const std::string& name) const {
        return attributes.find(name) != attributes.end();
    }
    
    const std::map<std::string, std::string>& getAttributes() const {
        return attributes;
    }
    
    std::string toString() const override {
        return "ElementNode: " + tagName;
    }
};

// 属性节点
class AttributeNode : public Node {
private:
    std::string name;
    std::string value;
    
public:
    AttributeNode(const std::string& n, const std::string& v, int line = 0, int col = 0)
        : Node(NodeType::ATTRIBUTE_NODE, line, col), name(n), value(v) {}
    
    const std::string& getName() const { return name; }
    const std::string& getValue() const { return value; }
    
    void setName(const std::string& n) { name = n; }
    void setValue(const std::string& v) { value = v; }
    
    std::string toString() const override {
        return "AttributeNode: " + name + " = " + value;
    }
};

// 标识符节点
class IdentifierNode : public Node {
private:
    std::string name;
    
public:
    IdentifierNode(const std::string& n, int line = 0, int col = 0)
        : Node(NodeType::IDENTIFIER_NODE, line, col), name(n) {}
    
    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }
    
    std::string toString() const override {
        return "IdentifierNode: " + name;
    }
};

// 字符串字面量节点
class StringLiteralNode : public Node {
private:
    std::string value;
    char quoteType; // '"', '\'' 或 '\0'（无引号）
    
public:
    StringLiteralNode(const std::string& v, char quote = '"', int line = 0, int col = 0)
        : Node(NodeType::STRING_LITERAL_NODE, line, col), value(v), quoteType(quote) {}
    
    const std::string& getValue() const { return value; }
    char getQuoteType() const { return quoteType; }
    
    void setValue(const std::string& v) { value = v; }
    void setQuoteType(char q) { quoteType = q; }
    
    std::string toString() const override {
        return "StringLiteralNode: " + value;
    }
};

// 数字字面量节点
class NumberLiteralNode : public Node {
private:
    std::string value;  // 包含单位的完整值
    double numericValue;
    std::string unit;   // 单位（如px, rem等）
    
public:
    NumberLiteralNode(const std::string& v, double num, const std::string& u = "", 
                      int line = 0, int col = 0)
        : Node(NodeType::NUMBER_LITERAL_NODE, line, col), 
          value(v), numericValue(num), unit(u) {}
    
    const std::string& getValue() const { return value; }
    double getNumericValue() const { return numericValue; }
    const std::string& getUnit() const { return unit; }
    
    std::string toString() const override {
        return "NumberLiteralNode: " + value;
    }
};

} // namespace chtl

#endif // CHTL_NODE_H