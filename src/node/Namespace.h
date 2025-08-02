#pragma once
#include "Node.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace chtl {

/**
 * 命名空间节点
 * 表示[Namespace]命名空间定义
 */
class NamespaceNode : public Node {
public:
    explicit NamespaceNode(const std::string& name = "",
                          const NodePosition& position = NodePosition());
    virtual ~NamespaceNode() = default;
    
    // 命名空间名称
    const std::string& getNamespaceName() const;
    void setNamespaceName(const std::string& name);
    
    // 完整路径（如A::B::C）
    std::string getFullPath() const;
    void setFullPath(const std::string& path);
    
    // 父命名空间
    void setParentNamespace(std::shared_ptr<NamespaceNode> parent);
    std::shared_ptr<NamespaceNode> getParentNamespace() const;
    bool hasParentNamespace() const;
    
    // 子命名空间管理
    void addChildNamespace(std::shared_ptr<NamespaceNode> child);
    void removeChildNamespace(const std::string& name);
    std::vector<std::shared_ptr<NamespaceNode>> getChildNamespaces() const;
    std::shared_ptr<NamespaceNode> findChildNamespace(const std::string& name) const;
    
    // 命名空间成员管理
    void addMember(const std::string& name, std::shared_ptr<Node> member);
    void removeMember(const std::string& name);
    std::shared_ptr<Node> findMember(const std::string& name) const;
    std::unordered_map<std::string, std::shared_ptr<Node>> getAllMembers() const;
    bool hasMember(const std::string& name) const;
    
    // 类型别名管理
    void addTypeAlias(const std::string& alias, const std::string& originalType);
    void removeTypeAlias(const std::string& alias);
    std::string resolveTypeAlias(const std::string& alias) const;
    std::unordered_map<std::string, std::string> getTypeAliases() const;
    
    // 导入的命名空间
    void addUsing(const std::string& namespaceName);
    void removeUsing(const std::string& namespaceName);
    std::vector<std::string> getUsings() const;
    bool isUsing(const std::string& namespaceName) const;
    
    // 访问控制
    enum class AccessLevel {
        PUBLIC,     // 公开访问
        PRIVATE,    // 私有访问
        PROTECTED   // 保护访问
    };
    
    void setAccessLevel(AccessLevel level);
    AccessLevel getAccessLevel() const;
    
    // 成员查找（支持作用域解析）
    std::shared_ptr<Node> resolveMember(const std::string& qualifiedName) const;
    std::vector<std::shared_ptr<Node>> findMembersByType(NodeType type) const;
    
    // 验证
    bool validate() const override;
    bool validateHierarchy() const;
    
    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;
    std::string toQualifiedString() const;
    
    // 克隆
    std::shared_ptr<Node> clone() const override;
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override;
    
private:
    std::string namespaceName_;
    std::string fullPath_;
    std::weak_ptr<NamespaceNode> parentNamespace_;
    std::vector<std::shared_ptr<NamespaceNode>> childNamespaces_;
    std::unordered_map<std::string, std::shared_ptr<Node>> members_;
    std::unordered_map<std::string, std::string> typeAliases_;
    std::vector<std::string> usings_;
    AccessLevel accessLevel_;
    
    std::string accessLevelToString() const;
    void updateFullPath();
};

/**
 * 命名空间解析器节点
 * 表示命名空间解析操作，如A::B::member
 */
class NamespaceResolverNode : public Node {
public:
    explicit NamespaceResolverNode(const NodePosition& position = NodePosition());
    virtual ~NamespaceResolverNode() = default;
    
    // 命名空间路径
    void setNamespacePath(const std::string& path);
    std::string getNamespacePath() const;
    void addNamespaceComponent(const std::string& component);
    std::vector<std::string> getNamespaceComponents() const;
    
    // 目标成员
    void setTargetMember(const std::string& member);
    std::string getTargetMember() const;
    
    // 完整的限定名
    std::string getQualifiedName() const;
    
    // 解析类型
    enum class ResolveType {
        MEMBER,     // 成员解析
        TYPE,       // 类型解析
        FUNCTION,   // 函数解析
        VARIABLE    // 变量解析
    };
    
    void setResolveType(ResolveType type);
    ResolveType getResolveType() const;
    
    // 解析结果
    void setResolvedNode(std::shared_ptr<Node> node);
    std::shared_ptr<Node> getResolvedNode() const;
    bool isResolved() const;
    
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
    std::string namespacePath_;
    std::vector<std::string> namespaceComponents_;
    std::string targetMember_;
    ResolveType resolveType_;
    std::shared_ptr<Node> resolvedNode_;
    
    std::string resolveTypeToString() const;
};

/**
 * 全局命名空间管理器
 * 管理所有命名空间的全局注册表
 */
class NamespaceManager {
public:
    static NamespaceManager& getInstance();
    
    // 命名空间注册
    bool registerNamespace(std::shared_ptr<NamespaceNode> namespaceNode);
    bool unregisterNamespace(const std::string& path);
    
    // 命名空间查找
    std::shared_ptr<NamespaceNode> findNamespace(const std::string& path) const;
    std::shared_ptr<NamespaceNode> findNamespaceByName(const std::string& name) const;
    std::vector<std::shared_ptr<NamespaceNode>> findNamespacesByPattern(const std::string& pattern) const;
    
    // 根命名空间
    std::shared_ptr<NamespaceNode> getRootNamespace() const;
    void setRootNamespace(std::shared_ptr<NamespaceNode> root);
    
    // 当前命名空间
    std::shared_ptr<NamespaceNode> getCurrentNamespace() const;
    void setCurrentNamespace(std::shared_ptr<NamespaceNode> current);
    void pushNamespace(std::shared_ptr<NamespaceNode> namespaceNode);
    void popNamespace();
    
    // 成员解析
    std::shared_ptr<Node> resolveMember(const std::string& qualifiedName) const;
    std::shared_ptr<Node> resolveMemberInContext(const std::string& name) const;
    
    // 类型解析
    std::string resolveType(const std::string& typeName) const;
    bool isValidType(const std::string& typeName) const;
    
    // 冲突检测
    bool hasNameConflict(const std::string& name) const;
    std::vector<std::string> findNameConflicts() const;
    
    // 依赖分析
    std::vector<std::string> getDependencies(const std::string& namespacePath) const;
    bool hasCircularDependency() const;
    std::vector<std::string> findCircularDependencies() const;
    
    // 获取所有命名空间
    std::vector<std::shared_ptr<NamespaceNode>> getAllNamespaces() const;
    std::vector<std::string> getAllNamespacePaths() const;
    
    // 统计信息
    size_t getNamespaceCount() const;
    size_t getTotalMemberCount() const;
    std::unordered_map<std::string, size_t> getMemberCountByNamespace() const;
    
    // 验证
    bool validateAll() const;
    std::vector<std::string> getValidationErrors() const;
    
    // 导入导出
    bool exportNamespace(const std::string& path, const std::string& filename) const;
    bool importNamespace(const std::string& filename);
    
    // 清理
    void clear();
    void clearNamespaces();
    
    // 调试信息
    std::string getDebugInfo() const;
    std::string getHierarchyString() const;
    
private:
    NamespaceManager() = default;
    ~NamespaceManager() = default;
    NamespaceManager(const NamespaceManager&) = delete;
    NamespaceManager& operator=(const NamespaceManager&) = delete;
    
    std::unordered_map<std::string, std::shared_ptr<NamespaceNode>> namespaces_;
    std::shared_ptr<NamespaceNode> rootNamespace_;
    std::vector<std::shared_ptr<NamespaceNode>> namespaceStack_;
    
    void initializeRootNamespace();
    std::string normalizeNamespacePath(const std::string& path) const;
    std::vector<std::string> splitNamespacePath(const std::string& path) const;
    bool isValidNamespaceName(const std::string& name) const;
};

/**
 * Using声明节点
 * 表示using namespace声明
 */
class UsingNode : public Node {
public:
    explicit UsingNode(const NodePosition& position = NodePosition());
    virtual ~UsingNode() = default;
    
    // Using类型
    enum class UsingType {
        NAMESPACE,      // using namespace
        DECLARATION,    // using declaration
        ALIAS           // using alias
    };
    
    // Using类型管理
    UsingType getUsingType() const;
    void setUsingType(UsingType type);
    
    // 目标命名空间或成员
    void setTarget(const std::string& target);
    std::string getTarget() const;
    
    // 别名（用于using alias）
    void setAlias(const std::string& alias);
    std::string getAlias() const;
    bool hasAlias() const;
    
    // 作用域限制
    void setScope(const std::string& scope);
    std::string getScope() const;
    bool hasScope() const;
    
    // 验证
    bool validate() const override;
    
    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;
    std::string toUsingStatement() const;
    
    // 克隆
    std::shared_ptr<Node> clone() const override;
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override;
    
private:
    UsingType usingType_;
    std::string target_;
    std::string alias_;
    std::string scope_;
    
    std::string usingTypeToString() const;
};

} // namespace chtl