#pragma once
#include "ChtlLoader.h"
#include "../node/Import.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <queue>
#include <stack>
#include <iostream>

namespace chtl {

/**
 * Import路径规范化器
 * 统一处理各种路径表达方式，确保同一文件只有唯一的规范化路径
 */
class ImportPathNormalizer {
public:
    ImportPathNormalizer();
    ~ImportPathNormalizer() = default;

    // 路径规范化
    std::string normalizePath(const std::string& path) const;
    std::string normalizeModulePath(const std::string& moduleName) const;
    std::string normalizeRelativePath(const std::string& path, const std::string& basePath) const;

    // 路径等价性检查
    bool arePathsEquivalent(const std::string& path1, const std::string& path2) const;
    std::string getCanonicalPath(const std::string& path) const;

    // 路径解析
    struct PathInfo {
        std::string originalPath;
        std::string normalizedPath;
        std::string canonicalPath;
        std::string fileName;
        std::string extension;
        std::string directory;
        bool isAbsolute;
        bool isModule;
        bool exists;
        
        PathInfo() : isAbsolute(false), isModule(false), exists(false) {}
    };
    
    PathInfo analyzePath(const std::string& path) const;
    
    // 设置
    void setModulePath(const std::string& modulePath);
    void setWorkingDirectory(const std::string& workingDir);

private:
    std::string modulePath_;
    std::string workingDirectory_;
    
    std::string resolveSymlinks(const std::string& path) const;
    std::string removeRedundantComponents(const std::string& path) const;
    std::vector<std::string> splitPathComponents(const std::string& path) const;
    std::string joinPathComponents(const std::vector<std::string>& components) const;
};

/**
 * 循环依赖检测器
 * 使用深度优先搜索和拓扑排序检测和解决循环依赖
 */
class CircularDependencyDetector {
public:
    CircularDependencyDetector();
    ~CircularDependencyDetector() = default;

    // 依赖关系管理
    void addDependency(const std::string& from, const std::string& to);
    void removeDependency(const std::string& from, const std::string& to);
    void clearDependencies();

    // 循环依赖检测
    bool hasCircularDependency() const;
    bool hasCircularDependency(const std::string& startNode) const;
    std::vector<std::string> findCircularDependencyChain(const std::string& startNode) const;
    std::vector<std::vector<std::string>> findAllCircularDependencies() const;

    // 拓扑排序
    std::vector<std::string> getTopologicalOrder() const;
    std::vector<std::string> getDependencyOrder(const std::vector<std::string>& nodes) const;

    // 依赖分析
    std::vector<std::string> getDependents(const std::string& node) const;
    std::vector<std::string> getDependencies(const std::string& node) const;
    std::vector<std::string> getAllDependencies(const std::string& node) const;
    int getDependencyDepth(const std::string& node) const;

    // 图信息
    size_t getNodeCount() const;
    size_t getEdgeCount() const;
    std::vector<std::string> getAllNodes() const;
    bool hasNode(const std::string& node) const;

    // 调试和可视化
    std::string toDotFormat() const;
    void printDependencyGraph() const;

private:
    std::unordered_map<std::string, std::unordered_set<std::string>> dependencies_;
    std::unordered_map<std::string, std::unordered_set<std::string>> dependents_;

    // DFS辅助方法
    bool dfsHasCycle(const std::string& node, 
                     std::unordered_set<std::string>& visited,
                     std::unordered_set<std::string>& recursionStack,
                     std::vector<std::string>& path) const;
    
    void dfsCollectDependencies(const std::string& node,
                               std::unordered_set<std::string>& visited,
                               std::vector<std::string>& result) const;
    
    // 拓扑排序辅助方法
    void topologicalSortUtil(const std::string& node,
                            std::unordered_set<std::string>& visited,
                            std::stack<std::string>& stack) const;
};

/**
 * 重复导入管理器
 * 检测和管理重复导入，避免重复解析和加载
 */
class DuplicateImportManager {
public:
    DuplicateImportManager();
    ~DuplicateImportManager() = default;

    // 导入记录管理
    struct ImportRecord {
        std::string normalizedPath;
        std::string originalPath;
        std::shared_ptr<ImportNode> importNode;
        std::string fromFile;
        time_t importTime;
        bool isResolved;
        
        ImportRecord() : importTime(0), isResolved(false) {}
    };

    // 重复检测
    bool isAlreadyImported(const std::string& path, const std::string& fromFile) const;
    bool isAlreadyImported(const std::string& path) const;
    std::vector<ImportRecord> findDuplicateImports(const std::string& path) const;

    // 导入记录
    void recordImport(const std::string& path, const std::string& fromFile, 
                     std::shared_ptr<ImportNode> importNode);
    void markAsResolved(const std::string& path, const std::string& fromFile);
    void removeImport(const std::string& path, const std::string& fromFile);

    // 查询
    std::vector<ImportRecord> getImportsForFile(const std::string& fromFile) const;
    std::vector<ImportRecord> getImportsOfFile(const std::string& path) const;
    std::vector<ImportRecord> getAllImports() const;
    size_t getImportCount() const;

    // 统计
    std::unordered_map<std::string, int> getImportFrequency() const;
    std::vector<std::string> getMostImportedFiles() const;
    std::vector<std::string> getFileImportOrder() const;

    // 清理
    void clearImports();
    void clearImportsForFile(const std::string& fromFile);
    void clearOldImports(time_t beforeTime);

private:
    // Key: normalized_path + "|" + from_file
    std::unordered_map<std::string, ImportRecord> importRecords_;
    
    // 索引
    std::unordered_map<std::string, std::vector<std::string>> importsByFile_;
    std::unordered_map<std::string, std::vector<std::string>> importsOfFile_;
    
    std::string makeKey(const std::string& path, const std::string& fromFile) const;
    void updateIndices(const std::string& path, const std::string& fromFile, bool add);
};

/**
 * 增强的Import管理器
 * 整合路径规范化、循环依赖检测、重复导入管理等功能
 */
class EnhancedImportManager {
public:
    EnhancedImportManager();
    ~EnhancedImportManager() = default;

    // 初始化
    void initialize(const std::string& modulePath, const std::string& workingDir);
    void setLoader(std::unique_ptr<ChtlLoader> loader);

    // Import处理
    struct ImportResult {
        bool success;
        std::string normalizedPath;
        std::string content;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        bool wasCached;
        bool wasDuplicate;
        std::vector<std::string> circularDependencyChain;
        
        ImportResult() : success(false), wasCached(false), wasDuplicate(false) {}
    };

    ImportResult processImport(std::shared_ptr<ImportNode> importNode, const std::string& fromFile);
    std::vector<ImportResult> processAllImports(const std::vector<std::shared_ptr<ImportNode>>& imports, 
                                               const std::string& fromFile);

    // 依赖分析
    bool validateImportChain(const std::vector<std::shared_ptr<ImportNode>>& imports, 
                           const std::string& fromFile);
    std::vector<std::string> getOptimalImportOrder(const std::vector<std::shared_ptr<ImportNode>>& imports,
                                                  const std::string& fromFile);
    
    // 状态查询
    bool hasCircularDependencies() const;
    std::vector<std::vector<std::string>> getAllCircularDependencies() const;
    std::vector<DuplicateImportManager::ImportRecord> getDuplicateImports() const;
    
    // 统计信息
    struct ImportStatistics {
        size_t totalImports;
        size_t uniqueFiles;
        size_t duplicateImports;
        size_t circularDependencies;
        size_t cachedLoads;
        double averageDependencyDepth;
    };
    
    ImportStatistics getStatistics() const;
    
    // 错误和警告
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;
    void clearErrors();
    void clearWarnings();

    // 缓存管理
    void enableCache(bool enable);
    bool isCacheEnabled() const;
    void clearCache();
    void invalidateCache(const std::string& path);

    // 调试
    void printDependencyGraph() const;
    void printImportStatistics() const;
    std::string exportDependencyGraph() const;

private:
    std::unique_ptr<ChtlLoader> loader_;
    std::unique_ptr<ImportPathNormalizer> pathNormalizer_;
    std::unique_ptr<CircularDependencyDetector> circularDetector_;
    std::unique_ptr<DuplicateImportManager> duplicateManager_;
    
    bool cacheEnabled_;
    std::unordered_map<std::string, std::string> contentCache_;
    
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 内部方法
    ImportResult loadAndProcessFile(const std::string& normalizedPath, 
                                   const std::string& fromFile,
                                   std::shared_ptr<ImportNode> importNode);
    
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    
    std::string extractFilePathFromImport(std::shared_ptr<ImportNode> importNode) const;
    void updateDependencyGraph(const std::string& fromFile, const std::string& toFile);
    
    bool checkForCircularDependency(const std::string& fromFile, const std::string& toFile);
    std::vector<std::string> analyzeFileForImports(const std::string& content) const;
};

} // namespace chtl