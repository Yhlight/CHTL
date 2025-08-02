#pragma once
#include "Node.h"
#include "StyleNode.h"
#include <vector>
#include <unordered_map>

namespace chtl {

/**
 * 样式组节点
 * 表示一组相关的样式定义
 */
class StyleGroupNode : public Node {
public:
    explicit StyleGroupNode(const std::string& name = "",
                           const NodePosition& position = NodePosition());
    virtual ~StyleGroupNode() = default;
    
    // 样式组名称
    const std::string& getGroupName() const;
    void setGroupName(const std::string& name);
    
    // 是否为值样式组（有具体值）
    bool isValuedGroup() const;
    void setValuedGroup(bool valued);
    
    // 样式管理
    void addStyleProperty(const std::string& property, const std::string& value);
    void removeStyleProperty(const std::string& property);
    std::string getStyleProperty(const std::string& property) const;
    std::unordered_map<std::string, std::string> getAllStyleProperties() const;
    
    // CSS规则管理
    void addCssRule(std::shared_ptr<CssRuleNode> rule);
    void removeCssRule(const std::string& selector);
    std::vector<std::shared_ptr<CssRuleNode>> getCssRules() const;
    std::shared_ptr<CssRuleNode> findCssRule(const std::string& selector) const;
    
    // 嵌套样式组管理
    void addNestedGroup(std::shared_ptr<StyleGroupNode> group);
    void removeNestedGroup(const std::string& groupName);
    std::vector<std::shared_ptr<StyleGroupNode>> getNestedGroups() const;
    std::shared_ptr<StyleGroupNode> findNestedGroup(const std::string& groupName) const;
    
    // 继承管理
    void addInheritedGroup(const std::string& groupName);
    void removeInheritedGroup(const std::string& groupName);
    std::vector<std::string> getInheritedGroups() const;
    
    // 验证
    bool validate() const override;
    
    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;
    std::string toCss() const;
    
    // 克隆
    std::shared_ptr<Node> clone() const override;
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override;
    
private:
    std::string groupName_;
    bool isValuedGroup_;
    std::unordered_map<std::string, std::string> styleProperties_;
    std::vector<std::shared_ptr<CssRuleNode>> cssRules_;
    std::vector<std::shared_ptr<StyleGroupNode>> nestedGroups_;
    std::vector<std::string> inheritedGroups_;
};

/**
 * 样式组调用节点
 * 表示对样式组的调用和使用
 */
class StyleGroupCallNode : public Node {
public:
    explicit StyleGroupCallNode(const std::string& groupName = "",
                               const NodePosition& position = NodePosition());
    virtual ~StyleGroupCallNode() = default;
    
    // 样式组名称
    const std::string& getGroupName() const;
    void setGroupName(const std::string& groupName);
    
    // 调用类型
    enum class CallType {
        DIRECT,        // 直接调用
        INHERITED,     // 继承调用
        SPECIALIZED    // 特例化调用
    };
    
    CallType getCallType() const;
    void setCallType(CallType type);
    
    // 参数管理（用于特例化）
    void addParameter(const std::string& name, const std::string& value);
    void removeParameter(const std::string& name);
    std::unordered_map<std::string, std::string> getParameters() const;
    bool hasParameter(const std::string& name) const;
    std::string getParameterValue(const std::string& name) const;
    
    // 覆盖属性管理
    void addOverride(const std::string& property, const std::string& value);
    void removeOverride(const std::string& property);
    std::unordered_map<std::string, std::string> getOverrides() const;
    
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
    std::string groupName_;
    CallType callType_;
    std::unordered_map<std::string, std::string> parameters_;
    std::unordered_map<std::string, std::string> overrides_;
    
    std::string callTypeToString() const;
};

/**
 * 伪类节点
 * 表示CSS伪类选择器，如:hover, :active, :focus等
 */
class PseudoClassNode : public Node {
public:
    explicit PseudoClassNode(const std::string& pseudoClass = "",
                            const NodePosition& position = NodePosition());
    virtual ~PseudoClassNode() = default;
    
    // 伪类名称
    const std::string& getPseudoClass() const;
    void setPseudoClass(const std::string& pseudoClass);
    
    // 参数管理（如:nth-child(2n+1)）
    void setParameter(const std::string& parameter);
    std::string getParameter() const;
    bool hasParameter() const;
    
    // 目标选择器
    void setTargetSelector(const std::string& selector);
    std::string getTargetSelector() const;
    
    // 验证
    bool validate() const override;
    bool isValidPseudoClass() const;
    
    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;
    std::string toCssSelector() const;
    
    // 克隆
    std::shared_ptr<Node> clone() const override;
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override;
    
private:
    std::string pseudoClass_;
    std::string parameter_;
    std::string targetSelector_;
    
    static std::vector<std::string> getValidPseudoClasses();
};

/**
 * 伪元素节点
 * 表示CSS伪元素选择器，如::before, ::after, ::first-line等
 */
class PseudoElementNode : public Node {
public:
    explicit PseudoElementNode(const std::string& pseudoElement = "",
                              const NodePosition& position = NodePosition());
    virtual ~PseudoElementNode() = default;
    
    // 伪元素名称
    const std::string& getPseudoElement() const;
    void setPseudoElement(const std::string& pseudoElement);
    
    // 目标选择器
    void setTargetSelector(const std::string& selector);
    std::string getTargetSelector() const;
    
    // 内容管理（特别是::before和::after）
    void setContent(const std::string& content);
    std::string getContent() const;
    bool hasContent() const;
    
    // 验证
    bool validate() const override;
    bool isValidPseudoElement() const;
    
    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;
    std::string toCssSelector() const;
    
    // 克隆
    std::shared_ptr<Node> clone() const override;
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override;
    
private:
    std::string pseudoElement_;
    std::string targetSelector_;
    std::string content_;
    
    static std::vector<std::string> getValidPseudoElements();
};

/**
 * 媒体查询节点
 * 表示CSS媒体查询，如@media (max-width: 768px)
 */
class MediaQueryNode : public Node {
public:
    explicit MediaQueryNode(const std::string& query = "",
                           const NodePosition& position = NodePosition());
    virtual ~MediaQueryNode() = default;
    
    // 媒体查询字符串
    const std::string& getQuery() const;
    void setQuery(const std::string& query);
    
    // 媒体类型（screen, print, all等）
    void setMediaType(const std::string& mediaType);
    std::string getMediaType() const;
    
    // 条件管理
    void addCondition(const std::string& property, const std::string& value);
    void removeCondition(const std::string& property);
    std::unordered_map<std::string, std::string> getConditions() const;
    
    // 嵌套样式管理
    void addNestedStyle(std::shared_ptr<Node> styleNode);
    std::vector<std::shared_ptr<Node>> getNestedStyles() const;
    
    // 验证
    bool validate() const override;
    
    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;
    std::string toCss() const;
    
    // 克隆
    std::shared_ptr<Node> clone() const override;
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override;
    
private:
    std::string query_;
    std::string mediaType_;
    std::unordered_map<std::string, std::string> conditions_;
    std::vector<std::shared_ptr<Node>> nestedStyles_;
    
    std::string buildQueryString() const;
};

} // namespace chtl