#pragma once
#include "Node.h"
#include <tuple>
#include <vector>
#include <unordered_map>

namespace chtl {

/**
 * 自定义样式节点
 * 表示[Custom] @Style定义
 */
class CustomStyleNode : public Node {
public:
    explicit CustomStyleNode(const std::string& name = "",
                            const NodePosition& position = NodePosition());
    virtual ~CustomStyleNode() = default;
    
    // 样式组名称
    const std::string& getStyleName() const;
    void setStyleName(const std::string& name);
    
    // 属性管理
    void addProperty(const std::string& property, const std::string& value);
    void removeProperty(const std::string& property);
    std::string getPropertyValue(const std::string& property) const;
    std::unordered_map<std::string, std::string> getAllProperties() const;
    
    // 继承管理
    void addInheritedStyle(const std::string& styleName);
    void removeInheritedStyle(const std::string& styleName);
    std::vector<std::string> getInheritedStyles() const;
    
    // 特例化管理
    void addSpecialization(const std::string& inheritedStyle,
                          const std::string& property,
                          const std::string& value);
    std::vector<std::tuple<std::string, std::string, std::string>> getSpecializations() const;
    
    // 操作管理（add, delete属性）
    void addOperation(const std::string& operation,
                     const std::string& target,
                     const std::string& property = "",
                     const std::string& value = "");
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> getOperations() const;
    
    // 验证
    bool validate() const override;
    
    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;
    
    // 克隆
    std::shared_ptr<Node> clone() const override;
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override;
    
private:
    std::unordered_map<std::string, std::string> properties_;
    std::vector<std::string> inheritedStyles_;
    std::vector<std::tuple<std::string, std::string, std::string>> specializations_; // inheritedStyle, property, value
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> operations_; // operation, target, property, value
};

/**
 * 自定义元素节点
 * 表示[Custom] @Element定义
 */
class CustomElementNode : public Node {
public:
    explicit CustomElementNode(const std::string& name = "",
                              const NodePosition& position = NodePosition());
    virtual ~CustomElementNode() = default;
    
    // 元素名称
    const std::string& getElementName() const;
    void setElementName(const std::string& name);
    
    // 根元素管理
    void setRootElement(const std::string& tagName);
    std::string getRootElement() const;
    void addRootAttribute(const std::string& name, const std::string& value);
    std::unordered_map<std::string, std::string> getRootAttributes() const;
    
    // 继承管理
    void addInheritedElement(const std::string& elementName);
    void removeInheritedElement(const std::string& elementName);
    std::vector<std::string> getInheritedElements() const;
    
    // 索引操作管理
    struct IndexedSpecialization {
        std::string operation;  // add, delete, modify
        std::string target;     // 目标元素或索引
        std::string property;   // 属性名（可选）
        std::string value;      // 值（可选）
        size_t index;          // 索引（可选）
    };
    
    void addIndexedOperation(const std::string& operation, const std::string& target, size_t index = 0);
    void addPropertyOperation(const std::string& operation, const std::string& target, 
                             const std::string& property, const std::string& value);
    std::vector<IndexedSpecialization> getIndexedSpecializations() const;
    
    // 验证
    bool validate() const override;
    
    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;
    
    // 克隆
    std::shared_ptr<Node> clone() const override;
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override;
    
private:
    std::string rootTagName_;
    std::unordered_map<std::string, std::string> rootAttributes_;
    std::vector<std::string> inheritedElements_;
    std::vector<IndexedSpecialization> specializations_;
};

/**
 * 自定义变量节点
 * 表示[Custom] @Var定义
 */
class CustomVarNode : public Node {
public:
    explicit CustomVarNode(const std::string& name = "",
                          const NodePosition& position = NodePosition());
    virtual ~CustomVarNode() = default;
    
    // 变量组名称
    const std::string& getVarName() const;
    void setVarName(const std::string& name);
    
    // 变量管理
    void addVariable(const std::string& name, const std::string& value);
    void removeVariable(const std::string& name);
    std::string getVariableValue(const std::string& name) const;
    std::unordered_map<std::string, std::string> getAllVariables() const;
    
    // 继承管理
    void addInheritedVar(const std::string& varName);
    void removeInheritedVar(const std::string& varName);
    std::vector<std::string> getInheritedVars() const;
    
    // 特例化管理
    void addVarSpecialization(const std::string& inheritedVar,
                             const std::string& variable,
                             const std::string& value);
    std::vector<std::tuple<std::string, std::string, std::string>> getVarSpecializations() const;
    
    // 覆盖检查
    bool hasVariableOverride(const std::string& varName) const;
    std::string getResolvedVariableValue(const std::string& varName) const;
    
    // 验证
    bool validate() const override;
    
    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;
    
    // 克隆
    std::shared_ptr<Node> clone() const override;
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override;
    
private:
    std::string varName_;
    std::unordered_map<std::string, std::string> variables_;
    std::vector<std::string> inheritedVars_;
    std::vector<std::tuple<std::string, std::string, std::string>> specializations_; // inheritedVar, variable, value
};

/**
 * 自定义变量调用节点
 * 表示自定义变量组调用，如ThemeColor(primary)
 */
class CustomVariableCallNode : public Node {
public:
        explicit CustomVariableCallNode(const std::string& varGroupName = "",
                                    const NodePosition& position = NodePosition());
    virtual ~CustomVariableCallNode() = default;
    
    // 变量组名称
    const std::string& getVarGroupName() const;
    void setVarGroupName(const std::string& name);
    
    // 变量名称
    const std::string& getVariableName() const;
    void setVariableName(const std::string& name);
    
    // 特例化参数
    void addSpecialization(const std::string& name, const std::string& value);
    void removeSpecialization(const std::string& name);
    std::unordered_map<std::string, std::string> getSpecializations() const;
    bool hasSpecialization(const std::string& name) const;
    std::string getSpecializationValue(const std::string& name) const;
    
    // 验证
    bool validate() const override;
    
    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;
    
    // 克隆
    std::shared_ptr<Node> clone() const override;
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override;
    
private:
    std::string varGroupName_;
    std::string variableName_;
    std::unordered_map<std::string, std::string> specializations_;
};

} // namespace chtl