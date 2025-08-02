#ifndef CHTL_CUSTOM_H
#define CHTL_CUSTOM_H

#include "Node.h"
#include <string>
#include <vector>
#include <map>

namespace chtl {

// 自定义样式组节点 - [Custom] @Style
class CustomStyleNode : public Node {
private:
    std::string name;
    std::vector<std::string> inheritedStyles;  // 继承的样式组
    
public:
    CustomStyleNode(const std::string& n, int line = 0, int col = 0)
        : Node(NodeType::CUSTOM_STYLE_NODE, line, col), name(n) {}
    
    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }
    
    void addInheritedStyle(const std::string& style) {
        inheritedStyles.push_back(style);
    }
    
    const std::vector<std::string>& getInheritedStyles() const {
        return inheritedStyles;
    }
    
    std::string toString() const override {
        return "CustomStyleNode: " + name;
    }
};

// 自定义元素节点 - [Custom] @Element
class CustomElementNode : public Node {
private:
    std::string name;
    std::vector<std::string> inheritedElements;  // 继承的元素
    
public:
    CustomElementNode(const std::string& n, int line = 0, int col = 0)
        : Node(NodeType::CUSTOM_ELEMENT_NODE, line, col), name(n) {}
    
    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }
    
    void addInheritedElement(const std::string& element) {
        inheritedElements.push_back(element);
    }
    
    const std::vector<std::string>& getInheritedElements() const {
        return inheritedElements;
    }
    
    std::string toString() const override {
        return "CustomElementNode: " + name;
    }
};

// 自定义变量组节点 - [Custom] @Var
class CustomVarNode : public Node {
private:
    std::string name;
    std::map<std::string, std::string> variables;  // 变量名->值
    std::vector<std::string> inheritedVarGroups;   // 继承的变量组
    
public:
    CustomVarNode(const std::string& n, int line = 0, int col = 0)
        : Node(NodeType::CUSTOM_VAR_NODE, line, col), name(n) {}
    
    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }
    
    void setVariable(const std::string& varName, const std::string& value) {
        variables[varName] = value;
    }
    
    std::string getVariable(const std::string& varName) const {
        auto it = variables.find(varName);
        if (it != variables.end()) {
            return it->second;
        }
        return "";
    }
    
    const std::map<std::string, std::string>& getVariables() const {
        return variables;
    }
    
    void addInheritedVarGroup(const std::string& group) {
        inheritedVarGroups.push_back(group);
    }
    
    const std::vector<std::string>& getInheritedVarGroups() const {
        return inheritedVarGroups;
    }
    
    std::string toString() const override {
        return "CustomVarNode: " + name;
    }
};

// 变量使用节点 - 用于表示变量组的使用，如 ThemeColor(tableColor)
class VarUsageNode : public Node {
private:
    std::string varGroupName;
    std::string varName;
    std::map<std::string, std::string> overrides;  // 特例化覆盖
    
public:
    VarUsageNode(const std::string& group, const std::string& var, 
                 int line = 0, int col = 0)
        : Node(NodeType::VAR_USAGE_NODE, line, col), 
          varGroupName(group), varName(var) {}
    
    const std::string& getVarGroupName() const { return varGroupName; }
    const std::string& getVarName() const { return varName; }
    
    void addOverride(const std::string& var, const std::string& value) {
        overrides[var] = value;
    }
    
    const std::map<std::string, std::string>& getOverrides() const {
        return overrides;
    }
    
    std::string toString() const override {
        return "VarUsageNode: " + varGroupName + "(" + varName + ")";
    }
};

} // namespace chtl

#endif // CHTL_CUSTOM_H