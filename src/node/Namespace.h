#ifndef CHTL_NAMESPACE_H
#define CHTL_NAMESPACE_H

#include "Node.h"
#include <string>
#include <vector>

namespace chtl {

// 命名空间节点 - [Namespace]
class NamespaceNode : public Node {
private:
    std::string name;
    std::vector<std::string> nestedPath;  // 嵌套命名空间路径
    
public:
    NamespaceNode(const std::string& n, int line = 0, int col = 0)
        : Node(NodeType::NAMESPACE_NODE, line, col), name(n) {}
    
    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }
    
    void addNestedNamespace(const std::string& ns) {
        nestedPath.push_back(ns);
    }
    
    const std::vector<std::string>& getNestedPath() const {
        return nestedPath;
    }
    
    // 获取完整的命名空间路径，如 "space.room.room2"
    std::string getFullPath() const {
        std::string path = name;
        for (const auto& ns : nestedPath) {
            path += "." + ns;
        }
        return path;
    }
    
    std::string toString() const override {
        return "NamespaceNode: " + getFullPath();
    }
};

} // namespace chtl

#endif // CHTL_NAMESPACE_H