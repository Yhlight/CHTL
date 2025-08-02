#ifndef CHTL_ORIGIN_H
#define CHTL_ORIGIN_H

#include "Node.h"
#include <string>

namespace chtl {

// 原始嵌入节点基类
class OriginNode : public Node {
protected:
    std::string content;
    
public:
    OriginNode(NodeType type, const std::string& c, int line = 0, int col = 0)
        : Node(type, line, col), content(c) {}
    
    const std::string& getContent() const { return content; }
    void setContent(const std::string& c) { content = c; }
};

// 原始HTML节点 - [Origin] @Html
class OriginHtmlNode : public OriginNode {
public:
    OriginHtmlNode(const std::string& content, int line = 0, int col = 0)
        : OriginNode(NodeType::ORIGIN_HTML_NODE, content, line, col) {}
    
    std::string toString() const override {
        return "OriginHtmlNode";
    }
};

// 原始Style节点 - [Origin] @Style
class OriginStyleNode : public OriginNode {
public:
    OriginStyleNode(const std::string& content, int line = 0, int col = 0)
        : OriginNode(NodeType::ORIGIN_STYLE_NODE, content, line, col) {}
    
    std::string toString() const override {
        return "OriginStyleNode";
    }
};

// 原始JavaScript节点 - [Origin] @JavaScript
class OriginJsNode : public OriginNode {
public:
    OriginJsNode(const std::string& content, int line = 0, int col = 0)
        : OriginNode(NodeType::ORIGIN_JS_NODE, content, line, col) {}
    
    std::string toString() const override {
        return "OriginJsNode";
    }
};

} // namespace chtl

#endif // CHTL_ORIGIN_H