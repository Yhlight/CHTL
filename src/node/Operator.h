#pragma once
#include "Node.h"
#include <vector>
#include <unordered_map>

namespace chtl {

/**
 * 操作符基类
 * 表示CHTL中的各种操作符（add, delete, from, as, inherit）
 */
class OperatorNode : public Node {
public:
    enum class OperatorType {
        ADD,        // add操作符
        DELETE,     // delete操作符
        FROM,       // from操作符
        AS,         // as操作符
        INHERIT     // inherit操作符
    };
    
    explicit OperatorNode(OperatorType type, const NodePosition& position = NodePosition());
    
protected:
    // 为子类提供的构造函数，允许指定特定的NodeType
    OperatorNode(OperatorType type, NodeType nodeType, const NodePosition& position = NodePosition());
    
public:
    virtual ~OperatorNode() = default;
    
    // 操作符类型
    OperatorType getOperatorType() const;
    void setOperatorType(OperatorType type);
    
    // 操作符名称
    std::string getOperatorName() const;
    
    // 验证
    bool validate() const override;
    
    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;
    
    // 克隆
    std::shared_ptr<Node> clone() const override;
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override;
    
protected:
    OperatorType operatorType_;
    
    std::string operatorTypeToString() const;
};

/**
 * Add操作符节点
 * 表示添加操作，如add @Element Box
 */
class AddOperatorNode : public OperatorNode {
public:
    explicit AddOperatorNode(const NodePosition& position = NodePosition());
    virtual ~AddOperatorNode() = default;
    
    // 目标类型
    enum class TargetType {
        ELEMENT,
        STYLE,
        VAR,
        PROPERTY,
        CHILD
    };
    
    // 目标管理
    void setTarget(const std::string& target);
    std::string getTarget() const;
    
    void setTargetType(TargetType type);
    TargetType getTargetType() const;
    
    // 索引管理（用于数组操作）
    void setIndex(size_t index);
    size_t getIndex() const;
    bool hasIndex() const;
    
    // 值管理
    void setValue(const std::string& value);
    std::string getValue() const;
    bool hasValue() const;
    
    // 属性管理（用于对象添加）
    void addProperty(const std::string& name, const std::string& value);
    void removeProperty(const std::string& name);
    std::unordered_map<std::string, std::string> getProperties() const;
    
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
    std::string target_;
    TargetType targetType_;
    size_t index_;
    bool hasIndex_;
    std::string value_;
    bool hasValue_;
    std::unordered_map<std::string, std::string> properties_;
    
    std::string targetTypeToString() const;
};

/**
 * Delete操作符节点
 * 表示删除操作，如delete div[0]
 */
class DeleteOperatorNode : public OperatorNode {
public:
    explicit DeleteOperatorNode(const NodePosition& position = NodePosition());
    virtual ~DeleteOperatorNode() = default;
    
    // 目标类型
    enum class TargetType {
        ELEMENT,
        STYLE,
        VAR,
        PROPERTY,
        CHILD,
        INHERITANCE
    };
    
    // 目标管理
    void setTarget(const std::string& target);
    std::string getTarget() const;
    
    void setTargetType(TargetType type);
    TargetType getTargetType() const;
    
    // 索引管理（用于数组操作）
    void setIndex(size_t index);
    size_t getIndex() const;
    bool hasIndex() const;
    
    // 选择器管理（用于CSS选择器删除）
    void setSelector(const std::string& selector);
    std::string getSelector() const;
    bool hasSelector() const;
    
    // 属性名管理（用于属性删除）
    void setPropertyName(const std::string& propertyName);
    std::string getPropertyName() const;
    bool hasPropertyName() const;
    
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
    std::string target_;
    TargetType targetType_;
    size_t index_;
    bool hasIndex_;
    std::string selector_;
    bool hasSelector_;
    std::string propertyName_;
    bool hasPropertyName_;
    
    std::string targetTypeToString() const;
};

/**
 * From操作符节点
 * 表示导入来源，如from "file.chtl"
 */
class FromOperatorNode : public OperatorNode {
public:
    explicit FromOperatorNode(const NodePosition& position = NodePosition());
    virtual ~FromOperatorNode() = default;
    
    // 来源类型
    enum class SourceType {
        FILE,           // 文件路径
        MODULE,         // 模块名称
        URL,            // 网络地址
        LIBRARY         // 库名称
    };
    
    // 来源管理
    void setSource(const std::string& source);
    std::string getSource() const;
    
    void setSourceType(SourceType type);
    SourceType getSourceType() const;
    
    // 导入项管理
    void addImportItem(const std::string& item);
    void removeImportItem(const std::string& item);
    std::vector<std::string> getImportItems() const;
    bool hasImportItems() const;
    
    // 通配符导入
    void setWildcardImport(bool wildcard);
    bool isWildcardImport() const;
    
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
    std::string source_;
    SourceType sourceType_;
    std::vector<std::string> importItems_;
    bool wildcardImport_;
    
    std::string sourceTypeToString() const;
};

/**
 * As操作符节点
 * 表示别名操作，如as NewName
 */
class AsOperatorNode : public OperatorNode {
public:
    explicit AsOperatorNode(const NodePosition& position = NodePosition());
    virtual ~AsOperatorNode() = default;
    
    // 原名称
    void setOriginalName(const std::string& originalName);
    std::string getOriginalName() const;
    
    // 别名
    void setAlias(const std::string& alias);
    std::string getAlias() const;
    
    // 作用域（可选）
    void setScope(const std::string& scope);
    std::string getScope() const;
    bool hasScope() const;
    
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
    std::string originalName_;
    std::string alias_;
    std::string scope_;
    bool hasScope_;
};

/**
 * Inherit操作符节点
 * 表示继承操作，如inherit @Element Base
 */
class InheritOperatorNode : public OperatorNode {
public:
    explicit InheritOperatorNode(const NodePosition& position = NodePosition());
    virtual ~InheritOperatorNode() = default;
    
    // 继承类型
    enum class InheritType {
        ELEMENT,
        STYLE,
        VAR,
        TEMPLATE,
        CUSTOM
    };
    
    // 目标管理
    void setTarget(const std::string& target);
    std::string getTarget() const;
    
    void setInheritType(InheritType type);
    InheritType getInheritType() const;
    
    // 继承模式
    enum class InheritMode {
        DIRECT,         // 直接继承
        OVERRIDE,       // 覆盖继承
        MERGE,          // 合并继承
        EXTEND          // 扩展继承
    };
    
    void setInheritMode(InheritMode mode);
    InheritMode getInheritMode() const;
    
    // 继承条件
    void addCondition(const std::string& condition);
    void removeCondition(const std::string& condition);
    std::vector<std::string> getConditions() const;
    
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
    std::string target_;
    InheritType inheritType_;
    InheritMode inheritMode_;
    std::vector<std::string> conditions_;
    
    std::string inheritTypeToString() const;
    std::string inheritModeToString() const;
};

} // namespace chtl