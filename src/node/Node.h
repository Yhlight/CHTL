#pragma once
#include "../common/Token.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace chtl {

/**
 * AST节点类型枚举
 * 严格按照CHTL语法文档定义所有节点类型
 */
enum class NodeType {
    // 基础节点
    UNKNOWN = 0,
    ROOT,                    // 根节点
    
    // 文档结构节点
    DOCUMENT,                // 文档节点
    HTML_ELEMENT,            // HTML元素节点
    TEXT_NODE,               // 文本节点
    COMMENT_NODE,            // 注释节点
    
    // 属性和样式节点
    ATTRIBUTE,               // 属性节点
    STYLE_BLOCK,             // 样式块节点
    CSS_RULE,                // CSS规则节点
    CSS_PROPERTY,            // CSS属性节点
    CSS_SELECTOR,            // CSS选择器节点
    
    // 自定义节点
    CUSTOM_STYLE,            // 自定义样式组节点
    CUSTOM_ELEMENT,          // 自定义元素节点
    CUSTOM_VAR,              // 自定义变量组节点
    
    // 模板节点
    TEMPLATE_STYLE,          // 模板样式组节点
    TEMPLATE_ELEMENT,        // 模板元素节点
    TEMPLATE_VAR,            // 模板变量组节点
    
    // 原始嵌入节点
    ORIGIN_HTML,             // 原始HTML节点
    ORIGIN_CSS,              // 原始CSS节点
    ORIGIN_JAVASCRIPT,       // 原始JavaScript节点
    
    // 配置节点
    CONFIGURATION,           // 配置块节点
    CONFIG_OPTION,           // 配置选项节点
    NAME_BLOCK,              // Name配置块节点
    
    // 模块系统节点
    IMPORT,                  // 导入节点
    NAMESPACE,               // 命名空间节点
    
    // 操作节点
    INHERIT,                 // 继承节点
    SPECIALIZATION,          // 特例化节点
    OPERATION,               // 操作节点 (add, delete)
    INDEX_ACCESS,            // 索引访问节点
    
    // 变量和调用节点
    VARIABLE_CALL,           // 变量调用节点
    VARIABLE_DEFINITION,     // 变量定义节点
    
    // 样式组相关
    STYLE_GROUP,             // 样式组节点
    STYLE_GROUP_CALL,        // 样式组调用节点
    
    // 伪类和伪元素
    PSEUDO_CLASS,            // 伪类节点
    PSEUDO_ELEMENT,          // 伪元素节点
    
    // 期盼语法
    EXPECT,                  // 期盼节点
    NOT_EXPECT              // 否定期盼节点
};

/**
 * 节点位置信息
 * 记录节点在源代码中的位置
 */
struct NodePosition {
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    size_t startOffset;
    size_t endOffset;
    
    NodePosition(const Position& start = Position(), const Position& end = Position())
        : startLine(start.line), startColumn(start.column),
          endLine(end.line), endColumn(end.column),
          startOffset(start.offset), endOffset(end.offset) {}
};

/**
 * 基础AST节点类
 * 所有CHTL语法元素的基类
 */
class Node : public std::enable_shared_from_this<Node> {
public:
    explicit Node(NodeType type, const NodePosition& position = NodePosition());
    virtual ~Node() = default;
    
    // 基础属性
    NodeType getType() const { return type_; }
    const NodePosition& getPosition() const { return position_; }
    
    // 父子关系
    void setParent(std::shared_ptr<Node> parent);
    std::shared_ptr<Node> getParent() const;
    
    void addChild(std::shared_ptr<Node> child);
    void removeChild(std::shared_ptr<Node> child);
    const std::vector<std::shared_ptr<Node>>& getChildren() const;
    std::shared_ptr<Node> getChild(size_t index) const;
    size_t getChildCount() const;
    
    // 兄弟节点导航
    std::shared_ptr<Node> getNextSibling() const;
    std::shared_ptr<Node> getPreviousSibling() const;
    
    // 查找功能
    std::vector<std::shared_ptr<Node>> findChildren(NodeType type) const;
    std::shared_ptr<Node> findFirstChild(NodeType type) const;
    std::shared_ptr<Node> findChildByName(const std::string& name) const;
    
    // 节点属性
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    void removeAttribute(const std::string& key);
    const std::unordered_map<std::string, std::string>& getAttributes() const;
    
    // 节点内容
    void setContent(const std::string& content);
    const std::string& getContent() const;
    
    // 节点名称（用于元素标签名、类名等）
    void setName(const std::string& name);
    const std::string& getName() const;
    
    // 调试和序列化
    virtual std::string toString() const;
    virtual std::string toDebugString(int indent = 0) const;
    
    // 克隆
    virtual std::shared_ptr<Node> clone() const;
    
    // 验证
    virtual bool validate() const;
    
    // 访问者模式支持
    virtual void accept(class NodeVisitor& visitor);
    
    // 类型检查辅助方法
    bool isElementNode() const;
    bool isTextNode() const;
    bool isStyleNode() const;
    bool isCustomNode() const;
    bool isTemplateNode() const;
    bool isConfigNode() const;
    bool isOriginNode() const;
    
protected:
    NodeType type_;
    NodePosition position_;
    std::weak_ptr<Node> parent_;
    std::vector<std::shared_ptr<Node>> children_;
    std::unordered_map<std::string, std::string> attributes_;
    std::string content_;
    std::string name_;
    
public:
    // 辅助方法
    void insertChild(size_t index, std::shared_ptr<Node> child);
    void replaceChild(size_t index, std::shared_ptr<Node> newChild);
    
protected:
    
private:
    void updateParentReferences();
};

/**
 * 节点工厂类
 * 用于创建各种类型的节点
 */
class NodeFactory {
public:
    // 创建基础节点
    static std::shared_ptr<Node> createNode(NodeType type, const NodePosition& position = NodePosition());
    
    // 创建特定类型节点
    static std::shared_ptr<class ElementNode> createElement(const std::string& tagName, const NodePosition& position = NodePosition());
    static std::shared_ptr<class TextNode> createText(const std::string& content, const NodePosition& position = NodePosition());
    static std::shared_ptr<class CommentNode> createComment(const std::string& content, const NodePosition& position = NodePosition());
    static std::shared_ptr<class AttributeNode> createAttribute(const std::string& name, const std::string& value, const NodePosition& position = NodePosition());
    static std::shared_ptr<class StyleBlockNode> createStyleBlock(const NodePosition& position = NodePosition());
    
    // 创建自定义节点
    static std::shared_ptr<class CustomStyleNode> createCustomStyle(const std::string& name, const NodePosition& position = NodePosition());
    static std::shared_ptr<class CustomElementNode> createCustomElement(const std::string& name, const NodePosition& position = NodePosition());
    static std::shared_ptr<class CustomVarNode> createCustomVar(const std::string& name, const NodePosition& position = NodePosition());
    
    // 创建模板节点
    static std::shared_ptr<class TemplateStyleNode> createTemplateStyle(const std::string& name, const NodePosition& position = NodePosition());
    static std::shared_ptr<class TemplateElementNode> createTemplateElement(const std::string& name, const NodePosition& position = NodePosition());
    static std::shared_ptr<class TemplateVarNode> createTemplateVar(const std::string& name, const NodePosition& position = NodePosition());
    
    // 创建其他节点
    static std::shared_ptr<class ConfigurationNode> createConfiguration(const NodePosition& position = NodePosition());
    static std::shared_ptr<class ImportNode> createImport(const std::string& path, const NodePosition& position = NodePosition());
    static std::shared_ptr<class NamespaceNode> createNamespace(const std::string& name, const NodePosition& position = NodePosition());
};

/**
 * 节点访问者接口
 * 实现访问者模式，用于遍历和处理AST
 */
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    
    // 访问基础节点
    virtual void visitNode(Node& node) = 0;
    virtual void visitElementNode(class ElementNode& node) = 0;
    virtual void visitTextNode(class TextNode& node) = 0;
    virtual void visitCommentNode(class CommentNode& node) = 0;
    virtual void visitAttributeNode(class AttributeNode& node) = 0;
    virtual void visitStyleBlockNode(class StyleBlockNode& node) = 0;
    
    // 访问自定义节点
    virtual void visitCustomStyleNode(class CustomStyleNode& node) = 0;
    virtual void visitCustomElementNode(class CustomElementNode& node) = 0;
    virtual void visitCustomVarNode(class CustomVarNode& node) = 0;
    
    // 访问模板节点
    virtual void visitTemplateStyleNode(class TemplateStyleNode& node) = 0;
    virtual void visitTemplateElementNode(class TemplateElementNode& node) = 0;
    virtual void visitTemplateVarNode(class TemplateVarNode& node) = 0;
    
    // 访问其他节点
    virtual void visitConfigurationNode(class ConfigurationNode& node) = 0;
    virtual void visitImportNode(class ImportNode& node) = 0;
    virtual void visitNamespaceNode(class NamespaceNode& node) = 0;
    
    // 遍历控制
    virtual bool shouldVisitChildren(Node& node) { (void)node; return true; }
    virtual void enterNode(Node& node) { (void)node; }
    virtual void exitNode(Node& node) { (void)node; }
};

/**
 * 节点工具类
 * 提供节点操作的便利方法
 */
class NodeUtils {
public:
    // 节点类型转换
    static std::string nodeTypeToString(NodeType type);
    static NodeType stringToNodeType(const std::string& str);
    
    // 树操作
    static std::shared_ptr<Node> findAncestor(std::shared_ptr<Node> node, NodeType type);
    static std::vector<std::shared_ptr<Node>> collectNodes(std::shared_ptr<Node> root, NodeType type);
    static std::shared_ptr<Node> findNodeByPath(std::shared_ptr<Node> root, const std::vector<std::string>& path);
    
    // 验证
    static bool validateTree(std::shared_ptr<Node> root);
    static std::vector<std::string> getValidationErrors(std::shared_ptr<Node> root);
    
    // 序列化
    static std::string treeToString(std::shared_ptr<Node> root, bool includePosition = false);
    static std::string treeToXml(std::shared_ptr<Node> root);
    static std::string treeToJson(std::shared_ptr<Node> root);
    
    // 统计
    static size_t countNodes(std::shared_ptr<Node> root);
    static size_t countNodesByType(std::shared_ptr<Node> root, NodeType type);
    static std::unordered_map<NodeType, size_t> getNodeTypeStatistics(std::shared_ptr<Node> root);
    
    // 复制和移动
    static std::shared_ptr<Node> deepCopy(std::shared_ptr<Node> node);
    static void moveNode(std::shared_ptr<Node> node, std::shared_ptr<Node> newParent);
    static void moveNodeBefore(std::shared_ptr<Node> node, std::shared_ptr<Node> target);
    static void moveNodeAfter(std::shared_ptr<Node> node, std::shared_ptr<Node> target);
};

} // namespace chtl