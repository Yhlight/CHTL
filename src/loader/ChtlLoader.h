#pragma once
#include "../common/Token.h"
#include "../node/Node.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <dirent.h>
#include <sys/stat.h>
#include <ctime>

namespace chtl {

/**
 * CHTL文件加载器
 * 负责加载和管理CHTL文件，包括预设模块和用户文件
 */
class ChtlLoader {
public:
    // 文件类型
    enum class FileType {
        USER_FILE,      // 用户文件
        MODULE_FILE,    // 预设模块文件
        IMPORTED_FILE   // 导入的文件
    };

    // 加载结果
    struct LoadResult {
        bool success;
        std::string content;
        std::string filePath;
        FileType fileType;
        std::vector<std::string> errors;
        std::unordered_set<std::string> dependencies;
        
        LoadResult() : success(false), fileType(FileType::USER_FILE) {}
    };

    // 文件信息
    struct FileInfo {
        std::string fullPath;
        std::string relativePath;
        std::string fileName;
        std::string directory;
        FileType fileType;
        size_t fileSize;
        time_t lastModified;
        bool isLoaded;
        
        FileInfo() : fileType(FileType::USER_FILE), fileSize(0), lastModified(0), isLoaded(false) {}
    };

    explicit ChtlLoader();
    ~ChtlLoader() = default;

    // 设置模块路径
    void setModulePath(const std::string& modulePath);
    std::string getModulePath() const;

    // 设置工作目录
    void setWorkingDirectory(const std::string& workingDir);
    std::string getWorkingDirectory() const;

    // 文件加载
    LoadResult loadFile(const std::string& fileName);
    LoadResult loadUserFile(const std::string& filePath);
    LoadResult loadModuleFile(const std::string& moduleName);
    LoadResult loadFileFromPath(const std::string& fullPath, FileType fileType);

    // 模块管理
    std::vector<std::string> getAvailableModules() const;
    bool isModuleAvailable(const std::string& moduleName) const;
    std::vector<FileInfo> getModuleInfo() const;

    // 文件搜索
    std::vector<std::string> searchFiles(const std::string& pattern) const;
    std::string resolveFilePath(const std::string& fileName) const;
    bool fileExists(const std::string& filePath) const;

    // 依赖管理
    void addDependency(const std::string& fromFile, const std::string& toFile);
    void removeDependency(const std::string& fromFile, const std::string& toFile);
    std::vector<std::string> getDependencies(const std::string& filePath) const;
    std::vector<std::string> getAllDependencies(const std::string& filePath) const;
    bool hasDependency(const std::string& fromFile, const std::string& toFile) const;
    bool hasCircularDependency(const std::string& filePath) const;

    // 缓存管理
    void enableCache(bool enable);
    bool isCacheEnabled() const;
    void clearCache();
    void invalidateCache(const std::string& filePath);
    size_t getCacheSize() const;

    // 文件监视
    void enableFileWatching(bool enable);
    bool isFileWatchingEnabled() const;
    std::vector<std::string> getModifiedFiles() const;
    void refreshModifiedFiles();

    // 错误处理
    std::vector<std::string> getErrors() const;
    std::string getLastError() const;
    void clearErrors();
    bool hasErrors() const;

    // 统计信息
    size_t getTotalFilesLoaded() const;
    size_t getModuleFilesLoaded() const;
    size_t getUserFilesLoaded() const;
    std::vector<FileInfo> getAllLoadedFiles() const;

    // 路径工具
    static std::string normalizePath(const std::string& path);
    static std::string getFileName(const std::string& path);
    static std::string getDirectory(const std::string& path);
    static std::string getFileExtension(const std::string& path);
    static bool isAbsolutePath(const std::string& path);
    static std::string joinPath(const std::string& dir, const std::string& file);

private:
    // 配置
    std::string modulePath_;
    std::string workingDirectory_;
    bool cacheEnabled_;
    bool fileWatchingEnabled_;

    // 缓存
    std::unordered_map<std::string, LoadResult> fileCache_;
    std::unordered_map<std::string, FileInfo> fileInfoCache_;

    // 依赖图
    std::unordered_map<std::string, std::unordered_set<std::string>> dependencies_;

    // 加载状态
    std::unordered_set<std::string> loadedFiles_;
    std::vector<std::string> errors_;
    std::unordered_map<std::string, time_t> lastModifiedTimes_;

    // 内部方法
    LoadResult loadFileInternal(const std::string& fullPath, FileType fileType);
    std::string readFileContent(const std::string& filePath);
    bool validateFileContent(const std::string& content);
    void updateFileInfo(const std::string& filePath, FileType fileType);
    void detectDependencies(const std::string& filePath, const std::string& content);
    bool isModifiedSinceLastLoad(const std::string& filePath);
    void addError(const std::string& error);
    std::vector<std::string> resolveDependencyOrder(const std::vector<std::string>& files);
    bool checkCircularDependencyRecursive(const std::string& filePath, 
                                         std::unordered_set<std::string>& visited,
                                         std::unordered_set<std::string>& recursionStack);
};

/**
 * 模块注册表
 * 管理所有可用的预设模块
 */
class ModuleRegistry {
public:
    static ModuleRegistry& getInstance();

    // 模块注册
    void registerModule(const std::string& moduleName, const std::string& filePath);
    void unregisterModule(const std::string& moduleName);
    bool isModuleRegistered(const std::string& moduleName) const;

    // 模块查找
    std::string getModulePath(const std::string& moduleName) const;
    std::vector<std::string> getAllModuleNames() const;
    std::unordered_map<std::string, std::string> getAllModules() const;

    // 模块分类
    void addModuleCategory(const std::string& category, const std::vector<std::string>& modules);
    std::vector<std::string> getModulesByCategory(const std::string& category) const;
    std::vector<std::string> getAllCategories() const;

    // 模块信息
    void setModuleDescription(const std::string& moduleName, const std::string& description);
    std::string getModuleDescription(const std::string& moduleName) const;
    void setModuleVersion(const std::string& moduleName, const std::string& version);
    std::string getModuleVersion(const std::string& moduleName) const;

    // 初始化
    void initializeBuiltinModules(const std::string& modulePath);
    void scanModuleDirectory(const std::string& modulePath);

    // 清理
    void clear();

private:
    ModuleRegistry() = default;
    ~ModuleRegistry() = default;
    ModuleRegistry(const ModuleRegistry&) = delete;
    ModuleRegistry& operator=(const ModuleRegistry&) = delete;

    std::unordered_map<std::string, std::string> modules_;
    std::unordered_map<std::string, std::vector<std::string>> categories_;
    std::unordered_map<std::string, std::string> descriptions_;
    std::unordered_map<std::string, std::string> versions_;
};

/**
 * 文件解析器
 * 解析文件路径和导入语句
 */
class FilePathResolver {
public:
    explicit FilePathResolver(const std::string& basePath = "");
    ~FilePathResolver() = default;

    // 路径解析
    std::string resolvePath(const std::string& filePath) const;
    std::string resolveRelativePath(const std::string& relativePath, const std::string& basePath) const;
    std::string resolveModulePath(const std::string& moduleName) const;

    // 导入语句解析
    struct ImportInfo {
        std::string moduleName;
        std::string fileName;
        std::string alias;
        std::vector<std::string> importedItems;
        bool isWildcardImport;
        
        ImportInfo() : isWildcardImport(false) {}
    };
    
    ImportInfo parseImportStatement(const std::string& importStatement);
    std::vector<ImportInfo> extractImportsFromContent(const std::string& content);

    // 路径验证
    bool isValidPath(const std::string& path) const;
    bool isValidModuleName(const std::string& moduleName) const;
    bool isValidFileName(const std::string& fileName) const;

    // 设置
    void setBasePath(const std::string& basePath);
    std::string getBasePath() const;
    void setModulePath(const std::string& modulePath);
    std::string getModulePath() const;

private:
    std::string basePath_;
    std::string modulePath_;

    std::string normalizePathSeparators(const std::string& path) const;
    std::vector<std::string> splitPath(const std::string& path) const;
    std::string joinPathComponents(const std::vector<std::string>& components) const;
};

/**
 * 加载上下文
 * 管理加载过程中的上下文信息
 */
class LoadContext {
public:
    explicit LoadContext();
    ~LoadContext() = default;

    // 上下文栈
    void pushContext(const std::string& filePath);
    void popContext();
    std::string getCurrentContext() const;
    std::vector<std::string> getContextStack() const;
    bool isInContext(const std::string& filePath) const;

    // 循环检测
    bool wouldCreateCircularDependency(const std::string& filePath) const;
    std::vector<std::string> getCircularDependencyChain(const std::string& filePath) const;

    // 状态管理
    void reset();
    size_t getDepth() const;
    bool isEmpty() const;

    // 调试信息
    std::string getContextTrace() const;

private:
    std::vector<std::string> contextStack_;
};

} // namespace chtl