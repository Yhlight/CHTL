#pragma once
#include "Node.h"
#include <vector>

namespace chtl {

/**
 * 样式块节点
 * 表示style { } 语法块
 */
class StyleBlockNode : public Node {
public:
    explicit StyleBlockNode(const NodePosition& position = NodePosition());
    virtual ~StyleBlockNode() = default;
    
    // CSS规则管理
    void addRule(std::shared_ptr<class CssRuleNode> rule);
    void removeRule(std::shared_ptr<class CssRuleNode> rule);
    std::vector<std::shared_ptr<class CssRuleNode>> getRules() const;
    
    // 内联样式管理
    void addInlineProperty(const std::string& property, const std::string& value);
    void removeInlineProperty(const std::string& property);
    std::unordered_map<std::string, std::string> getInlineProperties() const;
    
    // 样式组调用管理
    // StyleGroupCall功能已移至CustomStyleNode
    
    // 样式块类型
    bool hasInlineStyles() const;
    bool hasCssRules() const;
    bool hasStyleGroupCalls() const;
    
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
    std::unordered_map<std::string, std::string> inlineProperties_;
};

/**
 * CSS规则节点
 * 表示CSS选择器和属性的组合
 */
class CssRuleNode : public Node {
public:
    explicit CssRuleNode(const NodePosition& position = NodePosition());
    virtual ~CssRuleNode() = default;
    
    // 选择器管理
    void addSelector(std::shared_ptr<class CssSelectorNode> selector);
    void removeSelector(std::shared_ptr<class CssSelectorNode> selector);
    std::vector<std::shared_ptr<class CssSelectorNode>> getSelectors() const;
    std::string getSelectorString() const;
    
    // 属性管理
    void addProperty(std::shared_ptr<class CssPropertyNode> property);
    void removeProperty(std::shared_ptr<class CssPropertyNode> property);
    std::vector<std::shared_ptr<class CssPropertyNode>> getProperties() const;
    
    // 属性快捷方法
    void addProperty(const std::string& name, const std::string& value);
    void removeProperty(const std::string& name);
    std::string getPropertyValue(const std::string& name) const;
    
    // 嵌套规则
    void addNestedRule(std::shared_ptr<CssRuleNode> rule);
    std::vector<std::shared_ptr<CssRuleNode>> getNestedRules() const;
    
    // 验证
    bool validate() const override;
    
    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;
    
    // 克隆
    std::shared_ptr<Node> clone() const override;
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override;
};

/**
 * CSS选择器节点
 * 表示CSS选择器（类、ID、伪类、伪元素等）
 */
class CssSelectorNode : public Node {
public:
    enum class SelectorType {
        CLASS,          // .className
        ID,             // #idName
        ELEMENT,        // elementName
        PSEUDO_CLASS,   // :hover
        PSEUDO_ELEMENT, // ::before
        ATTRIBUTE,      // [attr=value]
        UNIVERSAL,      // *
        DESCENDANT,     // space
        CHILD,          // >
        SIBLING,        // ~
        ADJACENT        // +
    };
    
    explicit CssSelectorNode(SelectorType type = SelectorType::ELEMENT,
                            const std::string& value = "",
                            const NodePosition& position = NodePosition());
    virtual ~CssSelectorNode() = default;
    
    // 选择器类型和值
    SelectorType getSelectorType() const;
    void setSelectorType(SelectorType type);
    
    const std::string& getSelectorValue() const;
    void setSelectorValue(const std::string& value);
    
    // 特殊处理
    bool isAmpersandSelector() const; // &:hover, &::after
    std::string resolveAmpersand(const std::string& contextSelector) const;
    
    // 选择器组合
    void addCombinator(SelectorType combinator);
    void addCombinedSelector(std::shared_ptr<CssSelectorNode> selector);
    std::vector<std::shared_ptr<CssSelectorNode>> getCombinedSelectors() const;
    
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
    SelectorType selectorType_;
    std::string selectorValue_;
    
    // 辅助方法
    std::string selectorTypeToString() const;
    bool isValidSelectorValue(const std::string& value) const;
};

/**
 * CSS属性节点
 * 表示CSS属性名和值
 */
class CssPropertyNode : public Node {
public:
    explicit CssPropertyNode(const std::string& property = "",
                            const std::string& value = "",
                            const NodePosition& position = NodePosition());
    virtual ~CssPropertyNode() = default;
    
    // 属性名和值
    const std::string& getProperty() const;
    void setProperty(const std::string& property);
    
    const std::string& getValue() const;
    void setValue(const std::string& value);
    
    // 重要性标记
    bool isImportant() const;
    void setImportant(bool important);
    
    // 属性类型检查
    bool isColorProperty() const;
    bool isSizeProperty() const;
    bool isPositionProperty() const;
    bool isCustomProperty() const; // CSS变量
    
    // 值处理
    std::string getNormalizedValue() const;
    std::vector<std::string> getValueTokens() const;
    
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
    std::string property_;
    std::string value_;
    bool important_;
    
    // 验证辅助
    bool isValidProperty(const std::string& property) const;
    bool isValidValue(const std::string& value) const;
    std::string normalizeValue(const std::string& value) const;
    
    // 属性分类
    static const std::unordered_set<std::string> colorProperties_;
    static const std::unordered_set<std::string> sizeProperties_;
    static const std::unordered_set<std::string> positionProperties_;
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