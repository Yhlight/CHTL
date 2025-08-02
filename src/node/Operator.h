#ifndef CHTL_OPERATOR_H
#define CHTL_OPERATOR_H

#include "Node.h"
#include <string>

namespace chtl {

// 操作节点基类
class OperationNode : public Node {
protected:
    std::string target;  // 操作目标
    
public:
    OperationNode(NodeType t, const std::string& tgt, int line = 0, int col = 0)
        : Node(t, line, col), target(tgt) {}
    
    const std::string& getTarget() const { return target; }
    void setTarget(const std::string& t) { target = t; }
};

// Add操作节点 - add
class AddNode : public OperationNode {
public:
    AddNode(const std::string& target, int line = 0, int col = 0)
        : OperationNode(NodeType::ADD_NODE, target, line, col) {}
    
    std::string toString() const override {
        return "AddNode: " + target;
    }
};

// Delete操作节点 - delete
class DeleteNode : public OperationNode {
public:
    DeleteNode(const std::string& target, int line = 0, int col = 0)
        : OperationNode(NodeType::DELETE_NODE, target, line, col) {}
    
    std::string toString() const override {
        return "DeleteNode: " + target;
    }
};

// Inherit操作节点 - inherit
class InheritNode : public OperationNode {
public:
    InheritNode(const std::string& target, int line = 0, int col = 0)
        : OperationNode(NodeType::INHERIT_NODE, target, line, col) {}
    
    std::string toString() const override {
        return "InheritNode: " + target;
    }
};

// 索引访问节点 - 如 div[0]
class IndexAccessNode : public Node {
private:
    std::string elementName;
    int index;
    
public:
    IndexAccessNode(const std::string& elem, int idx, int line = 0, int col = 0)
        : Node(NodeType::INDEX_ACCESS_NODE, line, col), 
          elementName(elem), index(idx) {}
    
    const std::string& getElementName() const { return elementName; }
    int getIndex() const { return index; }
    
    void setElementName(const std::string& name) { elementName = name; }
    void setIndex(int idx) { index = idx; }
    
    std::string toString() const override {
        return "IndexAccessNode: " + elementName + "[" + std::to_string(index) + "]";
    }
};

} // namespace chtl

#endif // CHTL_OPERATOR_H