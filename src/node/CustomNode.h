#pragma once
#include "Node.h"
#include <vector>
#include <tuple>

namespace chtl {

/**
 * 自定义样式组节点
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
    std::string styleName_;
    std::unordered_map<std::string, std::string> properties_;
    std::vector<std::string> inheritedStyles_;
    std::vector<std::tuple<std::string, std::string, std::string>> specializations_; // style, property, value
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
    
    // 子元素管理
    void addChildElement(std::shared_ptr<Node> element);
    void removeChildElement(std::shared_ptr<Node> element);
    std::vector<std::shared_ptr<Node>> getChildElements() const;
    
    // 继承管理
    void addInheritedElement(const std::string& elementName);
    void removeInheritedElement(const std::string& elementName);
    std::vector<std::string> getInheritedElements() const;
    
    // 索引访问支持
    std::shared_ptr<Node> getChildByIndex(const std::string& tagName, size_t index) const;
    size_t getChildCountByTag(const std::string& tagName) const;
    
    // 特例化管理
    void addSpecialization(const std::string& target,
                          const std::string& property,
                          const std::string& value);
    void addIndexedSpecialization(const std::string& tagName,
                                 size_t index,
                                 const std::string& property,
                                 const std::string& value);
    
    // 操作管理（add, delete元素）
    void addElementOperation(const std::string& operation,
                           const std::string& tagName,
                           size_t index = 0,
                           std::shared_ptr<Node> element = nullptr);
    
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
    std::string elementName_;
    std::vector<std::string> inheritedElements_;
    
    struct ElementOperation {
        std::string operation;  // add, delete
        std::string tagName;
        size_t index;
        std::shared_ptr<Node> element;
    };
    std::vector<ElementOperation> operations_;
    
    struct IndexedSpecialization {
        std::string tagName;
        size_t index;
        std::string property;
        std::string value;
    };
    std::vector<IndexedSpecialization> specializations_;
};

/**
 * 自定义变量组节点
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
 * 变量调用节点
 * 表示变量组的使用，如ThemeColor(tableColor)
 */
class VariableCallNode : public Node {
public:
    explicit VariableCallNode(const std::string& varGroupName = "",
                             const NodePosition& position = NodePosition());
    virtual ~VariableCallNode() = default;
    
    // 变量组名称
    const std::string& getVarGroupName() const;
    void setVarGroupName(const std::string& name);
    
    // 变量名称
    const std::string& getVariableName() const;
    void setVariableName(const std::string& name);
    
    // 特例化参数
    void addParameter(const std::string& varName, const std::string& value);
    void removeParameter(const std::string& varName);
    std::unordered_map<std::string, std::string> getParameters() const;
    
    // 调用形式检查
    bool isSimpleCall() const; // VarGroup(varName)
    bool isParameterizedCall() const; // VarGroup(param1: value1, param2: value2)
    
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
    std::unordered_map<std::string, std::string> parameters_;
};

/**
 * 模板样式组节点
 * 表示[Template] @Style定义
 */
class TemplateStyleNode : public Node {
public:
    explicit TemplateStyleNode(const std::string& name = "",
                              const NodePosition& position = NodePosition());
    virtual ~TemplateStyleNode() = default;
    
    // 模板名称
    const std::string& getTemplateName() const;
    void setTemplateName(const std::string& name);
    
    // 属性管理（只读，不可修改）
    void addProperty(const std::string& property, const std::string& value);
    std::string getPropertyValue(const std::string& property) const;
    std::unordered_map<std::string, std::string> getAllProperties() const;
    
    // 继承管理
    void addInheritedTemplate(const std::string& templateName);
    void addInheritedCustom(const std::string& customName);
    std::vector<std::string> getInheritedTemplates() const;
    std::vector<std::string> getInheritedCustoms() const;
    
    // 模板特性
    bool isReadOnly() const { return true; }
    bool canBeSpecialized() const { return false; }
    
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
    std::string templateName_;
    std::unordered_map<std::string, std::string> properties_;
    std::vector<std::string> inheritedTemplates_;
    std::vector<std::string> inheritedCustoms_;
};

/**
 * 模板元素节点
 * 表示[Template] @Element定义
 */
class TemplateElementNode : public Node {
public:
    explicit TemplateElementNode(const std::string& name = "",
                                const NodePosition& position = NodePosition());
    virtual ~TemplateElementNode() = default;
    
    // 模板名称
    const std::string& getTemplateName() const;
    void setTemplateName(const std::string& name);
    
    // 子元素管理（只读）
    void addChildElement(std::shared_ptr<Node> element);
    std::vector<std::shared_ptr<Node>> getChildElements() const;
    
    // 继承管理
    void addInheritedTemplate(const std::string& templateName);
    void addInheritedCustom(const std::string& customName);
    std::vector<std::string> getInheritedTemplates() const;
    std::vector<std::string> getInheritedCustoms() const;
    
    // 模板特性
    bool isReadOnly() const { return true; }
    bool canBeSpecialized() const { return false; }
    
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
    std::string templateName_;
    std::vector<std::string> inheritedTemplates_;
    std::vector<std::string> inheritedCustoms_;
};

/**
 * 模板变量组节点
 * 表示[Template] @Var定义
 */
class TemplateVarNode : public Node {
public:
    explicit TemplateVarNode(const std::string& name = "",
                            const NodePosition& position = NodePosition());
    virtual ~TemplateVarNode() = default;
    
    // 模板名称
    const std::string& getTemplateName() const;
    void setTemplateName(const std::string& name);
    
    // 变量管理（只读）
    void addVariable(const std::string& name, const std::string& value);
    std::string getVariableValue(const std::string& name) const;
    std::unordered_map<std::string, std::string> getAllVariables() const;
    
    // 继承管理
    void addInheritedTemplate(const std::string& templateName);
    void addInheritedCustom(const std::string& customName);
    std::vector<std::string> getInheritedTemplates() const;
    std::vector<std::string> getInheritedCustoms() const;
    
    // 模板特性
    bool isReadOnly() const { return true; }
    bool canBeSpecialized() const { return false; }
    
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
    std::string templateName_;
    std::unordered_map<std::string, std::string> variables_;
    std::vector<std::string> inheritedTemplates_;
    std::vector<std::string> inheritedCustoms_;
};

} // namespace chtl