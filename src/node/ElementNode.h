#pragma once
#include "Node.h"
#include <unordered_set>

namespace chtl {

/**
 * HTML元素节点
 * 表示HTML元素，如div, span, p等
 */
class ElementNode : public Node {
public:
    explicit ElementNode(const std::string& tagName, const NodePosition& position = NodePosition());
    virtual ~ElementNode() = default;
    
    // 标签名相关
    const std::string& getTagName() const;
    void setTagName(const std::string& tagName);
    
    // 属性管理
    void addAttribute(const std::string& name, const std::string& value);
    void removeAttribute(const std::string& name);
    bool hasAttribute(const std::string& name) const;
    std::string getAttributeValue(const std::string& name) const;
    const std::unordered_map<std::string, std::string>& getAllAttributes() const;
    
    // CSS类管理
    void addClass(const std::string& className);
    void removeClass(const std::string& className);
    bool hasClass(const std::string& className) const;
    std::vector<std::string> getClasses() const;
    std::string getClassString() const;
    
    // ID管理
    void setId(const std::string& id);
    std::string getId() const;
    
    // 内联样式管理
    void addInlineStyle(const std::string& property, const std::string& value);
    void removeInlineStyle(const std::string& property);
    std::string getInlineStyle(const std::string& property) const;
    std::string getInlineStyleString() const;
    
    // 子元素查找
    std::vector<std::shared_ptr<ElementNode>> getChildElements() const;
    std::shared_ptr<ElementNode> findChildByTagName(const std::string& tagName) const;
    std::shared_ptr<ElementNode> findChildById(const std::string& id) const;
    std::vector<std::shared_ptr<ElementNode>> findChildrenByClass(const std::string& className) const;
    
    // HTML相关
    bool isSelfClosing() const;
    bool isBlockElement() const;
    bool isInlineElement() const;
    
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
    std::string tagName_;
    std::unordered_map<std::string, std::string> attributes_;
    std::unordered_set<std::string> classes_;
    std::unordered_map<std::string, std::string> inlineStyles_;
    
    // 辅助方法
    void parseClassAttribute();
    void updateClassAttribute();
    void parseStyleAttribute();
    void updateStyleAttribute();
    bool isValidTagName(const std::string& tagName) const;
    bool isValidAttributeName(const std::string& name) const;
};

/**
 * 文本节点
 * 表示text { } 语法块
 */
class TextNode : public Node {
public:
    explicit TextNode(const std::string& content = "", const NodePosition& position = NodePosition());
    virtual ~TextNode() = default;
    
    // 文本内容管理
    const std::string& getText() const;
    void setText(const std::string& text);
    
    // 文本处理
    std::string getProcessedText() const; // 处理转义字符等
    void appendText(const std::string& text);
    void prependText(const std::string& text);
    
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
    std::string text_;
    
    // 文本处理辅助
    std::string processEscapeSequences(const std::string& text) const;
    std::string normalizeWhitespace(const std::string& text) const;
};

/**
 * 注释节点
 * 表示各种注释类型
 */
class CommentNode : public Node {
public:
    enum class CommentType {
        SINGLE_LINE,    // //
        MULTI_LINE,     // /* */
        GENERATOR       // --
    };
    
    explicit CommentNode(const std::string& content = "", 
                        CommentType type = CommentType::SINGLE_LINE,
                        const NodePosition& position = NodePosition());
    virtual ~CommentNode() = default;
    
    // 注释内容
    const std::string& getComment() const;
    void setComment(const std::string& comment);
    
    // 注释类型
    CommentType getCommentType() const;
    void setCommentType(CommentType type);
    
    // 生成器注释特性
    bool isGeneratorComment() const;
    bool shouldOutputToHtml() const;
    
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
    std::string comment_;
    CommentType commentType_;
    
    // 辅助方法
    std::string commentTypeToString() const;
    std::string formatCommentForOutput() const;
};

/**
 * 属性节点
 * 表示元素的属性
 */
class AttributeNode : public Node {
public:
    explicit AttributeNode(const std::string& name = "", 
                          const std::string& value = "",
                          const NodePosition& position = NodePosition());
    virtual ~AttributeNode() = default;
    
    // 属性名和值
    const std::string& getAttributeName() const;
    void setAttributeName(const std::string& name);
    
    const std::string& getAttributeValue() const;
    void setAttributeValue(const std::string& value);
    
    // 属性类型检查
    bool isBooleanAttribute() const;
    bool isUrlAttribute() const;
    bool isEventAttribute() const;
    bool isDataAttribute() const;
    bool isAriaAttribute() const;
    
    // 值类型和验证
    bool hasValue() const;
    bool isValidValue() const;
    
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
    std::string attributeName_;
    std::string attributeValue_;
    
    // 验证辅助
    bool isValidAttributeName(const std::string& name) const;
    bool isValidAttributeValue(const std::string& value) const;
    std::string normalizeAttributeValue(const std::string& value) const;
    
    // 属性类型识别
    static const std::unordered_set<std::string> booleanAttributes_;
    static const std::unordered_set<std::string> urlAttributes_;
    static const std::unordered_set<std::string> eventAttributes_;
};

} // namespace chtl