#pragma once
#include "Node.h"
#include "StyleNode.h"
#include <vector>
#include <unordered_map>

namespace chtl {



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