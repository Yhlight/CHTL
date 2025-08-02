#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <fstream>

namespace chtl {

// 文件信息结构
struct FileInfo {
    std::string path;           // 规范化后的绝对路径
    std::string content;        // 文件内容
    std::string encoding;       // 文件编码（默认UTF-8）
    bool isLoaded;              // 是否已加载
    
    FileInfo() : encoding("UTF-8"), isLoaded(false) {}
};

// 导入信息结构
struct ImportInfo {
    std::string fromPath;       // 导入语句中的路径
    std::string resolvedPath;   // 解析后的绝对路径
    std::string asName;         // 重命名
    bool isWildcard;            // 是否是通配符导入
    
    ImportInfo() : isWildcard(false) {}
};

// 模块依赖图
class DependencyGraph {
private:
    // 邻接表表示：文件路径 -> 依赖的文件路径列表
    std::map<std::string, std::set<std::string>> dependencies;
    
    // DFS辅助函数，用于检测循环依赖
    bool hasCycleDFS(const std::string& node, 
                     std::set<std::string>& visited, 
                     std::set<std::string>& recursionStack,
                     std::vector<std::string>& cycle) const;
    
public:
    // 添加依赖关系
    void addDependency(const std::string& from, const std::string& to);
    
    // 检测是否存在循环依赖
    bool hasCycle(std::vector<std::string>& cycle) const;
    
    // 获取拓扑排序（用于确定加载顺序）
    std::vector<std::string> getTopologicalOrder() const;
    
    // 清空依赖图
    void clear();
};

// CHTL文件加载器
class ChtlLoader {
private:
    std::string workingDirectory;                      // 当前工作目录
    std::map<std::string, FileInfo> loadedFiles;      // 已加载的文件缓存
    std::set<std::string> loadingFiles;                // 正在加载的文件（用于检测循环）
    DependencyGraph dependencyGraph;                    // 依赖关系图
    
    // 路径处理
    std::string getAbsolutePath(const std::string& path, const std::string& basePath) const;
    std::string getDirectoryPath(const std::string& filePath) const;
    std::string getFileName(const std::string& filePath) const;
    std::string getFileExtension(const std::string& filePath) const;
    
public:
    // 路径规范化：处理 . 代替 / 的情况
    std::string convertDotToSlash(const std::string& path) const;
    
    // 路径处理（公开给测试使用）
    std::string normalizePath(const std::string& path) const;
    
private:
    
    // 文件系统操作
    bool fileExists(const std::string& path) const;
    bool isDirectory(const std::string& path) const;
    std::vector<std::string> listDirectory(const std::string& path) const;
    
    // 读取文件内容
    std::string readFile(const std::string& path) const;
    
    // 检测文件编码
    std::string detectEncoding(const std::string& content) const;
    
public:
    ChtlLoader();
    explicit ChtlLoader(const std::string& workDir);
    ~ChtlLoader() = default;
    
    // 设置工作目录
    void setWorkingDirectory(const std::string& dir);
    std::string getWorkingDirectory() const { return workingDirectory; }
    
    // 主要接口：加载文件
    FileInfo loadFile(const std::string& path, const std::string& basePath = "");
    
    // 解析导入路径
    std::string resolveImportPath(const std::string& importPath, 
                                  const std::string& currentFilePath) const;
    
    // 处理通配符导入
    std::vector<std::string> resolveWildcardImport(const std::string& pattern,
                                                    const std::string& currentFilePath) const;
    
    // 检查是否已加载（避免重复加载）
    bool isFileLoaded(const std::string& path) const;
    
    // 获取已加载的文件信息
    const FileInfo* getLoadedFile(const std::string& path) const;
    
    // 添加导入依赖
    void addImportDependency(const std::string& fromFile, const std::string& toFile);
    
    // 检测循环依赖
    bool hasCircularDependency(std::vector<std::string>& cycle) const;
    
    // 获取加载顺序
    std::vector<std::string> getLoadOrder() const;
    
    // 清空缓存
    void clearCache();
    
    // 获取所有已加载的文件
    std::vector<std::string> getLoadedFilePaths() const;
};

// 加载器异常类
class LoaderError : public std::runtime_error {
public:
    enum ErrorType {
        FILE_NOT_FOUND,
        CIRCULAR_DEPENDENCY,
        INVALID_PATH,
        READ_ERROR,
        ENCODING_ERROR
    };
    
private:
    ErrorType errorType;
    std::string filePath;
    
public:
    LoaderError(ErrorType type, const std::string& path, const std::string& message)
        : std::runtime_error(message), errorType(type), filePath(path) {}
    
    ErrorType getErrorType() const { return errorType; }
    const std::string& getFilePath() const { return filePath; }
};

} // namespace chtl

#endif // CHTL_LOADER_H