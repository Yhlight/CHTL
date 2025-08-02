#pragma once
#include "Node.h"
#include <tuple>
#include <vector>
#include <unordered_map>

namespace chtl {

/**
 * 模板样式节点
 * 表示[Template] @Style定义
 */
class TemplateStyleNode : public Node {
public:
    explicit TemplateStyleNode(const std::string& name = "",
                              const NodePosition& position = NodePosition());
    virtual ~TemplateStyleNode() = default;
    
    // 样式组名称
    const std::string& getStyleName() const;
    void setStyleName(const std::string& name);
    
    // 属性管理
    void addProperty(const std::string& property, const std::string& value);
    void removeProperty(const std::string& property);
    std::string getPropertyValue(const std::string& property) const;
    std::unordered_map<std::string, std::string> getAllProperties() const;
    
    // 继承管理（模板只能被继承，不能特例化）
    void addInheritedStyle(const std::string& styleName);
    void removeInheritedStyle(const std::string& styleName);
    std::vector<std::string> getInheritedStyles() const;
    
    // 模板特性：只读访问
    bool isReadOnly() const { return true; }
    
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
    
    // 模板管理
    void addInheritedTemplate(const std::string& templateName);
    void removeInheritedTemplate(const std::string& templateName);
    std::vector<std::string> getInheritedTemplates() const;
    
    // 模板特性：只读访问
    bool isReadOnly() const { return true; }
    
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
    std::vector<std::string> inheritedTemplates_;
};

/**
 * 模板变量节点
 * 表示[Template] @Var定义
 */
class TemplateVarNode : public Node {
public:
    explicit TemplateVarNode(const std::string& name = "",
                            const NodePosition& position = NodePosition());
    virtual ~TemplateVarNode() = default;
    
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
    
    // 模板管理
    void addInheritedCustom(const std::string& customName);
    void removeInheritedCustom(const std::string& customName);
    std::vector<std::string> getInheritedCustoms() const;
    
    // 模板特性：只读访问
    bool isReadOnly() const { return true; }
    
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
    std::vector<std::string> inheritedCustoms_;
};

/**
 * 模板调用节点
 * 表示模板的调用和实例化
 */
class TemplateCallNode : public Node {
public:
    explicit TemplateCallNode(const std::string& templateName = "",
                             const NodePosition& position = NodePosition());
    virtual ~TemplateCallNode() = default;
    
    // 模板名称
    const std::string& getTemplateName() const;
    void setTemplateName(const std::string& name);
    
    // 模板类型
    enum class TemplateType {
        STYLE,
        ELEMENT,
        VAR
    };
    
    TemplateType getTemplateType() const;
    void setTemplateType(TemplateType type);
    
    // 参数管理
    void addParameter(const std::string& name, const std::string& value);
    void removeParameter(const std::string& name);
    std::unordered_map<std::string, std::string> getParameters() const;
    bool hasParameter(const std::string& name) const;
    std::string getParameterValue(const std::string& name) const;
    
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
    TemplateType templateType_;
    std::unordered_map<std::string, std::string> parameters_;
    
    std::string templateTypeToString() const;
};

} // namespace chtl