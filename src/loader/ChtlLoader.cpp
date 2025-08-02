#include "ChtlLoader.h"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>

namespace chtl {

// DependencyGraph 实现

void DependencyGraph::addDependency(const std::string& from, const std::string& to) {
    dependencies[from].insert(to);
    // 确保 to 也在图中
    if (dependencies.find(to) == dependencies.end()) {
        dependencies[to] = std::set<std::string>();
    }
}

bool DependencyGraph::hasCycleDFS(const std::string& node,
                                 std::set<std::string>& visited,
                                 std::set<std::string>& recursionStack,
                                 std::vector<std::string>& cycle) const {
    visited.insert(node);
    recursionStack.insert(node);
    
    auto it = dependencies.find(node);
    if (it != dependencies.end()) {
        for (const auto& neighbor : it->second) {
            if (recursionStack.find(neighbor) != recursionStack.end()) {
                // 找到循环，构建循环路径
                cycle.clear();
                cycle.push_back(neighbor);
                cycle.push_back(node);
                return true;
            }
            
            if (visited.find(neighbor) == visited.end()) {
                if (hasCycleDFS(neighbor, visited, recursionStack, cycle)) {
                    // 如果找到循环，继续构建路径
                    if (cycle.front() != cycle.back()) {
                        cycle.push_back(node);
                    }
                    return true;
                }
            }
        }
    }
    
    recursionStack.erase(node);
    return false;
}

bool DependencyGraph::hasCycle(std::vector<std::string>& cycle) const {
    std::set<std::string> visited;
    std::set<std::string> recursionStack;
    
    for (const auto& pair : dependencies) {
        if (visited.find(pair.first) == visited.end()) {
            if (hasCycleDFS(pair.first, visited, recursionStack, cycle)) {
                // 反转路径以获得正确的循环顺序
                std::reverse(cycle.begin(), cycle.end());
                return true;
            }
        }
    }
    
    return false;
}

std::vector<std::string> DependencyGraph::getTopologicalOrder() const {
    std::vector<std::string> result;
    std::map<std::string, int> inDegree;
    
    // 计算入度
    for (const auto& pair : dependencies) {
        if (inDegree.find(pair.first) == inDegree.end()) {
            inDegree[pair.first] = 0;
        }
        for (const auto& dep : pair.second) {
            inDegree[dep]++;
        }
    }
    
    // 找出所有入度为0的节点
    std::vector<std::string> queue;
    for (const auto& pair : inDegree) {
        if (pair.second == 0) {
            queue.push_back(pair.first);
        }
    }
    
    // 拓扑排序
    while (!queue.empty()) {
        std::string node = queue.back();
        queue.pop_back();
        result.push_back(node);
        
        auto it = dependencies.find(node);
        if (it != dependencies.end()) {
            for (const auto& neighbor : it->second) {
                inDegree[neighbor]--;
                if (inDegree[neighbor] == 0) {
                    queue.push_back(neighbor);
                }
            }
        }
    }
    
    // 如果结果数量不等于节点数量，说明有循环
    if (result.size() != dependencies.size()) {
        result.clear();
    }
    
    return result;
}

void DependencyGraph::clear() {
    dependencies.clear();
}

// ChtlLoader 实现

ChtlLoader::ChtlLoader() : workingDirectory(".") {
}

ChtlLoader::ChtlLoader(const std::string& workDir) : workingDirectory(workDir) {
    workingDirectory = normalizePath(workingDirectory);
}

void ChtlLoader::setWorkingDirectory(const std::string& dir) {
    workingDirectory = normalizePath(dir);
}

std::string ChtlLoader::normalizePath(const std::string& path) const {
    std::string result = path;
    
    // 将反斜杠替换为正斜杠
    std::replace(result.begin(), result.end(), '\\', '/');
    
    // 处理 . 和 ..
    std::vector<std::string> parts;
    std::stringstream ss(result);
    std::string part;
    
    // 特殊处理开头的斜杠
    bool startsWithSlash = !result.empty() && result[0] == '/';
    
    while (std::getline(ss, part, '/')) {
        if (part.empty()) {
            // 跳过空部分（连续的斜杠）
            continue;
        } else if (part == ".") {
            // 跳过当前目录引用
            continue;
        } else if (part == "..") {
            if (!parts.empty() && parts.back() != "..") {
                parts.pop_back();
            } else if (!startsWithSlash) {
                // 只有相对路径才保留..
                parts.push_back(part);
            }
        } else {
            parts.push_back(part);
        }
    }
    
    // 重建路径
    result.clear();
    
    if (startsWithSlash) {
        result = "/";
    }
    
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) {
            result += "/";
        } else if (startsWithSlash && !parts.empty()) {
            // 已经添加了开头的斜杠
        }
        result += parts[i];
    }
    
    return result.empty() ? "." : result;
}

std::string ChtlLoader::getAbsolutePath(const std::string& path, 
                                        const std::string& basePath) const {
    std::string normalizedPath = normalizePath(path);
    
    // 如果已经是绝对路径，直接返回
    if (!normalizedPath.empty() && normalizedPath[0] == '/') {
        return normalizedPath;
    }
    
    // 确定基础路径
    std::string base = basePath.empty() ? workingDirectory : basePath;
    
    // 如果基础路径是文件，获取其目录
    if (!isDirectory(base)) {
        base = getDirectoryPath(base);
    }
    
    // 组合路径
    return normalizePath(base + "/" + normalizedPath);
}

std::string ChtlLoader::getDirectoryPath(const std::string& filePath) const {
    size_t lastSlash = filePath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        return filePath.substr(0, lastSlash);
    }
    return ".";
}

std::string ChtlLoader::getFileName(const std::string& filePath) const {
    size_t lastSlash = filePath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        return filePath.substr(lastSlash + 1);
    }
    return filePath;
}

std::string ChtlLoader::getFileExtension(const std::string& filePath) const {
    size_t lastDot = filePath.find_last_of('.');
    if (lastDot != std::string::npos && lastDot > 0) {
        return filePath.substr(lastDot + 1);
    }
    return "";
}

std::string ChtlLoader::convertDotToSlash(const std::string& path) const {
    std::string result = path;
    
    // 只有在路径中使用点号作为路径分隔符时才替换
    // 例如：module.submodule.file -> module/submodule/file
    // 但不替换文件扩展名中的点号
    
    size_t lastSlash = result.find_last_of("/\\");
    size_t lastDot = result.find_last_of('.');
    
    // 如果最后一个点号在最后一个斜杠之后，可能是文件扩展名
    if (lastDot != std::string::npos && 
        (lastSlash == std::string::npos || lastDot > lastSlash)) {
        // 检查是否是已知的文件扩展名
        std::string ext = result.substr(lastDot + 1);
        if (ext == "chtl" || ext == "html" || ext == "css" || ext == "js") {
            // 保留扩展名，只替换路径部分的点号
            std::string pathPart = result.substr(0, lastDot);
            std::replace(pathPart.begin(), pathPart.end(), '.', '/');
            result = pathPart + "." + ext;
        } else {
            // 全部替换
            std::replace(result.begin(), result.end(), '.', '/');
        }
    } else {
        // 全部替换
        std::replace(result.begin(), result.end(), '.', '/');
    }
    
    return result;
}

bool ChtlLoader::fileExists(const std::string& path) const {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool ChtlLoader::isDirectory(const std::string& path) const {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0) {
        return S_ISDIR(buffer.st_mode);
    }
    return false;
}

std::vector<std::string> ChtlLoader::listDirectory(const std::string& path) const {
    std::vector<std::string> files;
    
    DIR* dir = opendir(path.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string name = entry->d_name;
            // 跳过 . 和 ..
            if (name != "." && name != "..") {
                files.push_back(path + "/" + name);
            }
        }
        closedir(dir);
    }
    
    return files;
}

std::string ChtlLoader::readFile(const std::string& path) const {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw LoaderError(LoaderError::FILE_NOT_FOUND, path, 
                         "Cannot open file: " + path);
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    
    file.close();
    return content;
}

std::string ChtlLoader::detectEncoding(const std::string& content) const {
    // 简单的UTF-8 BOM检测
    if (content.size() >= 3 &&
        static_cast<unsigned char>(content[0]) == 0xEF &&
        static_cast<unsigned char>(content[1]) == 0xBB &&
        static_cast<unsigned char>(content[2]) == 0xBF) {
        return "UTF-8-BOM";
    }
    
    // 默认假设UTF-8
    return "UTF-8";
}

FileInfo ChtlLoader::loadFile(const std::string& path, const std::string& basePath) {
    // 转换点号为斜杠
    std::string convertedPath = convertDotToSlash(path);
    
    // 获取绝对路径
    std::string absolutePath = getAbsolutePath(convertedPath, basePath);
    absolutePath = normalizePath(absolutePath);
    
    // 检查是否已加载
    auto it = loadedFiles.find(absolutePath);
    if (it != loadedFiles.end()) {
        return it->second;
    }
    
    // 检查循环加载
    if (loadingFiles.find(absolutePath) != loadingFiles.end()) {
        throw LoaderError(LoaderError::CIRCULAR_DEPENDENCY, absolutePath,
                         "Circular dependency detected while loading: " + absolutePath);
    }
    
    // 标记正在加载
    loadingFiles.insert(absolutePath);
    
    try {
        // 检查文件是否存在
        if (!fileExists(absolutePath)) {
            throw LoaderError(LoaderError::FILE_NOT_FOUND, absolutePath,
                             "File not found: " + absolutePath);
        }
        
        // 读取文件内容
        FileInfo fileInfo;
        fileInfo.path = absolutePath;
        fileInfo.content = readFile(absolutePath);
        fileInfo.encoding = detectEncoding(fileInfo.content);
        fileInfo.isLoaded = true;
        
        // 如果是UTF-8 BOM，移除BOM
        if (fileInfo.encoding == "UTF-8-BOM" && fileInfo.content.size() >= 3) {
            fileInfo.content = fileInfo.content.substr(3);
            fileInfo.encoding = "UTF-8";
        }
        
        // 缓存文件信息
        loadedFiles[absolutePath] = fileInfo;
        
        // 移除正在加载标记
        loadingFiles.erase(absolutePath);
        
        return fileInfo;
        
    } catch (...) {
        // 确保移除正在加载标记
        loadingFiles.erase(absolutePath);
        throw;
    }
}

std::string ChtlLoader::resolveImportPath(const std::string& importPath,
                                          const std::string& currentFilePath) const {
    // 转换点号为斜杠
    std::string convertedPath = convertDotToSlash(importPath);
    
    // 获取当前文件的目录
    std::string currentDir = getDirectoryPath(currentFilePath);
    
    // 解析为绝对路径
    std::string absolutePath = getAbsolutePath(convertedPath, currentDir);
    
    // 如果没有扩展名，尝试添加默认扩展名
    if (getFileExtension(absolutePath).empty()) {
        // 尝试不同的扩展名
        std::vector<std::string> extensions = {".chtl", ".html", ".css", ".js"};
        for (const auto& ext : extensions) {
            std::string pathWithExt = absolutePath + ext;
            if (fileExists(pathWithExt)) {
                return normalizePath(pathWithExt);
            }
        }
    }
    
    return normalizePath(absolutePath);
}

std::vector<std::string> ChtlLoader::resolveWildcardImport(const std::string& pattern,
                                                           const std::string& currentFilePath) const {
    std::vector<std::string> results;
    
    // 检查是否是通配符模式
    if (pattern.empty() || pattern.back() != '*') {
        return results;
    }
    
    // 获取目录路径
    std::string dirPattern = pattern.substr(0, pattern.length() - 1);
    if (dirPattern.empty() || dirPattern.back() == '/') {
        dirPattern = dirPattern.empty() ? "." : dirPattern.substr(0, dirPattern.length() - 1);
    }
    
    // 转换点号为斜杠
    dirPattern = convertDotToSlash(dirPattern);
    
    // 获取绝对路径
    std::string currentDir = getDirectoryPath(currentFilePath);
    std::string absoluteDir = getAbsolutePath(dirPattern, currentDir);
    
    // 检查目录是否存在
    if (!isDirectory(absoluteDir)) {
        return results;
    }
    
    // 列出目录中的文件
    std::vector<std::string> files = listDirectory(absoluteDir);
    
    // 过滤出支持的文件类型
    for (const auto& file : files) {
        std::string ext = getFileExtension(file);
        if (ext == "chtl" || ext == "html" || ext == "css" || ext == "js") {
            results.push_back(normalizePath(file));
        }
    }
    
    return results;
}

bool ChtlLoader::isFileLoaded(const std::string& path) const {
    std::string absolutePath = normalizePath(getAbsolutePath(path, workingDirectory));
    return loadedFiles.find(absolutePath) != loadedFiles.end();
}

const FileInfo* ChtlLoader::getLoadedFile(const std::string& path) const {
    std::string absolutePath = normalizePath(getAbsolutePath(path, workingDirectory));
    auto it = loadedFiles.find(absolutePath);
    if (it != loadedFiles.end()) {
        return &it->second;
    }
    return nullptr;
}

void ChtlLoader::addImportDependency(const std::string& fromFile, const std::string& toFile) {
    std::string fromPath = normalizePath(getAbsolutePath(fromFile, workingDirectory));
    std::string toPath = normalizePath(getAbsolutePath(toFile, workingDirectory));
    
    dependencyGraph.addDependency(fromPath, toPath);
}

bool ChtlLoader::hasCircularDependency(std::vector<std::string>& cycle) const {
    return dependencyGraph.hasCycle(cycle);
}

std::vector<std::string> ChtlLoader::getLoadOrder() const {
    return dependencyGraph.getTopologicalOrder();
}

void ChtlLoader::clearCache() {
    loadedFiles.clear();
    loadingFiles.clear();
    dependencyGraph.clear();
}

std::vector<std::string> ChtlLoader::getLoadedFilePaths() const {
    std::vector<std::string> paths;
    for (const auto& pair : loadedFiles) {
        paths.push_back(pair.first);
    }
    return paths;
}

} // namespace chtl