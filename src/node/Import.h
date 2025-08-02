#pragma once
#include "Node.h"
#include "Operator.h"
#include <vector>
#include <unordered_map>

namespace chtl {

/**
 * 导入节点
 * 表示[Import]导入语句
 */
class ImportNode : public Node {
public:
    explicit ImportNode(const NodePosition& position = NodePosition());
    virtual ~ImportNode() = default;
    
    // 导入类型
    enum class ImportType {
        HTML,           // @Html
        STYLE,          // @Style
        JAVASCRIPT,     // @JavaScript
        CUSTOM_ELEMENT, // [Custom] @Element
        CUSTOM_STYLE,   // [Custom] @Style
        CUSTOM_VAR,     // [Custom] @Var
        TEMPLATE_ELEMENT, // [Template] @Element
        TEMPLATE_STYLE,   // [Template] @Style
        TEMPLATE_VAR,     // [Template] @Var
        CHTL              // @Chtl (整个文件)
    };
    
    // 导入类型管理
    ImportType getImportType() const;
    void setImportType(ImportType type);
    
    // 导入项名称（可选，用于特定导入）
    const std::string& getImportItem() const;
    void setImportItem(const std::string& item);
    
    // 来源信息
    void setFromOperator(std::shared_ptr<FromOperatorNode> fromOp);
    std::shared_ptr<FromOperatorNode> getFromOperator() const;
    bool hasFromOperator() const;
    
    // 别名信息
    void setAsOperator(std::shared_ptr<AsOperatorNode> asOp);
    std::shared_ptr<AsOperatorNode> getAsOperator() const;
    bool hasAsOperator() const;
    
    // 批量导入项（用于多个导入）
    void addImportItem(const std::string& item);
    void removeImportItem(const std::string& item);
    std::vector<std::string> getImportItems() const;
    bool hasMultipleItems() const;
    
    // 条件导入
    void addCondition(const std::string& condition);
    void removeCondition(const std::string& condition);
    std::vector<std::string> getConditions() const;
    bool hasConditions() const;
    
    // 验证
    bool validate() const override;
    
    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;
    std::string toImportStatement() const;
    
    // 克隆
    std::shared_ptr<Node> clone() const override;
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override;
    
private:
    ImportType importType_;
    std::string importItem_;
    std::shared_ptr<FromOperatorNode> fromOperator_;
    std::shared_ptr<AsOperatorNode> asOperator_;
    std::vector<std::string> importItems_;
    std::vector<std::string> conditions_;
    
    std::string importTypeToString() const;
    std::string importTypeToPrefix() const;
};

/**
 * 导入声明节点
 * 表示完整的导入声明，包含多个导入语句
 */
class ImportDeclarationNode : public Node {
public:
    explicit ImportDeclarationNode(const NodePosition& position = NodePosition());
    virtual ~ImportDeclarationNode() = default;
    
    // 导入语句管理
    void addImport(std::shared_ptr<ImportNode> importNode);
    void removeImport(size_t index);
    void removeImport(const std::string& item);
    std::vector<std::shared_ptr<ImportNode>> getImports() const;
    size_t getImportCount() const;
    
    // 按类型查找导入
    std::vector<std::shared_ptr<ImportNode>> getImportsByType(ImportNode::ImportType type) const;
    std::shared_ptr<ImportNode> findImportByItem(const std::string& item) const;
    
    // 模块信息
    void setModuleName(const std::string& moduleName);
    std::string getModuleName() const;
    bool hasModuleName() const;
    
    // 版本信息
    void setVersion(const std::string& version);
    std::string getVersion() const;
    bool hasVersion() const;
    
    // 依赖管理
    void addDependency(const std::string& dependency);
    void removeDependency(const std::string& dependency);
    std::vector<std::string> getDependencies() const;
    bool hasDependencies() const;
    
    // 验证
    bool validate() const override;
    bool validateDependencies() const;
    
    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;
    std::string toImportBlock() const;
    
    // 克隆
    std::shared_ptr<Node> clone() const override;
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override;
    
private:
    std::vector<std::shared_ptr<ImportNode>> imports_;
    std::string moduleName_;
    std::string version_;
    std::vector<std::string> dependencies_;
};

/**
 * 导入解析器
 * 用于解析和处理导入语句
 */
class ImportResolver {
public:
    explicit ImportResolver();
    ~ImportResolver() = default;
    
    // 解析导入语句
    std::shared_ptr<ImportNode> parseImportStatement(const std::string& statement);
    std::shared_ptr<ImportDeclarationNode> parseImportBlock(const std::string& block);
    
    // 解析文件路径
    std::string resolveFilePath(const std::string& path, const std::string& basePath = "") const;
    bool isValidFilePath(const std::string& path) const;
    bool fileExists(const std::string& path) const;
    
    // 解析模块名称
    std::string resolveModuleName(const std::string& moduleName) const;
    bool isValidModuleName(const std::string& moduleName) const;
    
    // 解析URL
    bool isValidUrl(const std::string& url) const;
    std::string normalizeUrl(const std::string& url) const;
    
    // 循环依赖检测
    bool hasCircularDependency(const std::vector<std::string>& dependencies) const;
    std::vector<std::string> findCircularDependencies(const std::vector<std::string>& dependencies) const;
    
    // 依赖排序
    std::vector<std::string> sortDependencies(const std::vector<std::string>& dependencies) const;
    
    // 错误信息
    std::string getLastError() const;
    bool hasError() const;
    void clearErrors();
    
private:
    std::string lastError_;
    std::vector<std::string> searchPaths_;
    
    void setError(const std::string& error);
    bool isAbsolutePath(const std::string& path) const;
    bool isRelativePath(const std::string& path) const;
    std::string joinPath(const std::string& basePath, const std::string& relativePath) const;
};

/**
 * 导入管理器
 * 全局管理所有导入的资源
 */
class ImportManager {
public:
    static ImportManager& getInstance();
    
    // 注册导入
    bool registerImport(std::shared_ptr<ImportNode> importNode);
    bool registerImportDeclaration(std::shared_ptr<ImportDeclarationNode> declaration);
    
    // 查找导入
    std::shared_ptr<ImportNode> findImport(const std::string& item) const;
    std::vector<std::shared_ptr<ImportNode>> findImportsByType(ImportNode::ImportType type) const;
    std::vector<std::shared_ptr<ImportNode>> findImportsBySource(const std::string& source) const;
    
    // 获取所有导入
    std::vector<std::shared_ptr<ImportNode>> getAllImports() const;
    std::vector<std::shared_ptr<ImportDeclarationNode>> getAllDeclarations() const;
    
    // 解析依赖
    bool resolveDependencies();
    std::vector<std::string> getDependencyOrder() const;
    
    // 验证
    bool validateAll() const;
    std::vector<std::string> getValidationErrors() const;
    
    // 统计信息
    size_t getImportCount() const;
    size_t getDeclarationCount() const;
    std::unordered_map<ImportNode::ImportType, size_t> getImportCountByType() const;
    
    // 清理
    void clear();
    void clearImports();
    void clearDeclarations();
    
    // 调试信息
    std::string getDebugInfo() const;
    
private:
    ImportManager() = default;
    ~ImportManager() = default;
    ImportManager(const ImportManager&) = delete;
    ImportManager& operator=(const ImportManager&) = delete;
    
    std::vector<std::shared_ptr<ImportNode>> imports_;
    std::vector<std::shared_ptr<ImportDeclarationNode>> declarations_;
    std::vector<std::string> dependencyOrder_;
    ImportResolver resolver_;
    
    void updateDependencyOrder();
};

} // namespace chtl