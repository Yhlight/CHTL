#include "ChtlLoader.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <ctime>
#include <sys/stat.h>

namespace chtl {

// ChtlLoader实现
ChtlLoader::ChtlLoader() 
    : modulePath_("module"), workingDirectory_("."), cacheEnabled_(true), fileWatchingEnabled_(false) {
}

void ChtlLoader::setModulePath(const std::string& modulePath) {
    modulePath_ = normalizePath(modulePath);
}

std::string ChtlLoader::getModulePath() const {
    return modulePath_;
}

void ChtlLoader::setWorkingDirectory(const std::string& workingDir) {
    workingDirectory_ = normalizePath(workingDir);
}

std::string ChtlLoader::getWorkingDirectory() const {
    return workingDirectory_;
}

ChtlLoader::LoadResult ChtlLoader::loadFile(const std::string& fileName) {
    // 首先尝试加载预设模块
    if (isModuleAvailable(fileName)) {
        return loadModuleFile(fileName);
    }
    
    // 然后尝试在当前目录加载用户文件
    std::string userFilePath = joinPath(workingDirectory_, fileName);
    if (fileExists(userFilePath)) {
        return loadUserFile(userFilePath);
    }
    
    // 如果文件名包含扩展名，尝试加载
    if (getFileExtension(fileName) == ".chtl") {
        return loadUserFile(fileName);
    }
    
    // 尝试添加.chtl扩展名
    std::string chtlFileName = fileName + ".chtl";
    userFilePath = joinPath(workingDirectory_, chtlFileName);
    if (fileExists(userFilePath)) {
        return loadUserFile(userFilePath);
    }
    
    // 失败情况
    LoadResult result;
    result.success = false;
    result.errors.push_back("File not found: " + fileName);
    addError("File not found: " + fileName);
    return result;
}

ChtlLoader::LoadResult ChtlLoader::loadUserFile(const std::string& filePath) {
    std::string fullPath = isAbsolutePath(filePath) ? filePath : joinPath(workingDirectory_, filePath);
    return loadFileFromPath(fullPath, FileType::USER_FILE);
}

ChtlLoader::LoadResult ChtlLoader::loadModuleFile(const std::string& moduleName) {
    std::string modulePath = joinPath(modulePath_, moduleName + ".chtl");
    return loadFileFromPath(modulePath, FileType::MODULE_FILE);
}

ChtlLoader::LoadResult ChtlLoader::loadFileFromPath(const std::string& fullPath, FileType fileType) {
    std::string normalizedPath = normalizePath(fullPath);
    
    // 检查缓存
    if (cacheEnabled_ && fileCache_.find(normalizedPath) != fileCache_.end()) {
        auto& cached = fileCache_[normalizedPath];
        if (!isModifiedSinceLastLoad(normalizedPath)) {
            return cached;
        }
    }
    
    // 加载文件
    LoadResult result = loadFileInternal(normalizedPath, fileType);
    
    // 更新缓存
    if (cacheEnabled_ && result.success) {
        fileCache_[normalizedPath] = result;
    }
    
    // 更新文件信息
    if (result.success) {
        updateFileInfo(normalizedPath, fileType);
        loadedFiles_.insert(normalizedPath);
        
        // 检测依赖
        detectDependencies(normalizedPath, result.content);
    }
    
    return result;
}

std::vector<std::string> ChtlLoader::getAvailableModules() const {
    std::vector<std::string> modules;
    
    // 扫描模块目录
    DIR* dir = opendir(modulePath_.c_str());
    if (dir != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_REG) { // 常规文件
                std::string fileName = entry->d_name;
                if (getFileExtension(fileName) == ".chtl") {
                    std::string moduleName = fileName;
                    // 移除.chtl扩展名
                    if (moduleName.length() > 5) {
                        moduleName = moduleName.substr(0, moduleName.length() - 5);
                    }
                    modules.push_back(moduleName);
                }
            }
        }
        closedir(dir);
    }
    
    return modules;
}

bool ChtlLoader::isModuleAvailable(const std::string& moduleName) const {
    std::string modulePath = joinPath(modulePath_, moduleName + ".chtl");
    return fileExists(modulePath);
}

std::vector<ChtlLoader::FileInfo> ChtlLoader::getModuleInfo() const {
    std::vector<FileInfo> infos;
    auto modules = getAvailableModules();
    
    for (const auto& module : modules) {
        FileInfo info;
        info.fileName = module + ".chtl";
        info.fullPath = joinPath(modulePath_, info.fileName);
        info.relativePath = info.fileName;
        info.directory = modulePath_;
        info.fileType = FileType::MODULE_FILE;
        
        // 获取文件大小和修改时间
        struct stat fileStat;
        if (stat(info.fullPath.c_str(), &fileStat) == 0) {
            info.fileSize = fileStat.st_size;
            info.lastModified = fileStat.st_mtime;
        }
        
        info.isLoaded = loadedFiles_.find(info.fullPath) != loadedFiles_.end();
        infos.push_back(info);
    }
    
    return infos;
}

std::string ChtlLoader::resolveFilePath(const std::string& fileName) const {
    // 首先检查是否是绝对路径
    if (isAbsolutePath(fileName)) {
        return fileName;
    }
    
    // 检查模块
    if (isModuleAvailable(fileName)) {
        return joinPath(modulePath_, fileName + ".chtl");
    }
    
    // 检查当前目录
    std::string userPath = joinPath(workingDirectory_, fileName);
    if (fileExists(userPath)) {
        return userPath;
    }
    
    // 尝试添加.chtl扩展名
    std::string chtlPath = userPath + ".chtl";
    if (fileExists(chtlPath)) {
        return chtlPath;
    }
    
    return "";
}

bool ChtlLoader::fileExists(const std::string& filePath) const {
    struct stat buffer;
    return (stat(filePath.c_str(), &buffer) == 0);
}

void ChtlLoader::addDependency(const std::string& fromFile, const std::string& toFile) {
    dependencies_[normalizePath(fromFile)].insert(normalizePath(toFile));
}

void ChtlLoader::removeDependency(const std::string& fromFile, const std::string& toFile) {
    auto it = dependencies_.find(normalizePath(fromFile));
    if (it != dependencies_.end()) {
        it->second.erase(normalizePath(toFile));
        if (it->second.empty()) {
            dependencies_.erase(it);
        }
    }
}

std::vector<std::string> ChtlLoader::getDependencies(const std::string& filePath) const {
    auto it = dependencies_.find(normalizePath(filePath));
    if (it != dependencies_.end()) {
        return std::vector<std::string>(it->second.begin(), it->second.end());
    }
    return {};
}

bool ChtlLoader::hasDependency(const std::string& fromFile, const std::string& toFile) const {
    auto it = dependencies_.find(normalizePath(fromFile));
    if (it != dependencies_.end()) {
        return it->second.find(normalizePath(toFile)) != it->second.end();
    }
    return false;
}

bool ChtlLoader::hasCircularDependency(const std::string& filePath) const {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursionStack;
    return const_cast<ChtlLoader*>(this)->checkCircularDependencyRecursive(
        normalizePath(filePath), visited, recursionStack);
}

void ChtlLoader::enableCache(bool enable) {
    cacheEnabled_ = enable;
    if (!enable) {
        clearCache();
    }
}

bool ChtlLoader::isCacheEnabled() const {
    return cacheEnabled_;
}

void ChtlLoader::clearCache() {
    fileCache_.clear();
    fileInfoCache_.clear();
}

void ChtlLoader::invalidateCache(const std::string& filePath) {
    std::string normalizedPath = normalizePath(filePath);
    fileCache_.erase(normalizedPath);
    fileInfoCache_.erase(normalizedPath);
}

size_t ChtlLoader::getCacheSize() const {
    return fileCache_.size();
}

std::vector<std::string> ChtlLoader::getErrors() const {
    return errors_;
}

std::string ChtlLoader::getLastError() const {
    return errors_.empty() ? "" : errors_.back();
}

void ChtlLoader::clearErrors() {
    errors_.clear();
}

bool ChtlLoader::hasErrors() const {
    return !errors_.empty();
}

size_t ChtlLoader::getTotalFilesLoaded() const {
    return loadedFiles_.size();
}

// 静态路径工具方法
std::string ChtlLoader::normalizePath(const std::string& path) {
    std::string normalized = path;
    
    // 替换反斜杠为正斜杠
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    
    // 移除多余的斜杠
    std::regex multiSlash("/+");
    normalized = std::regex_replace(normalized, multiSlash, "/");
    
    return normalized;
}

std::string ChtlLoader::getFileName(const std::string& path) {
    std::string normalized = normalizePath(path);
    size_t pos = normalized.find_last_of('/');
    return pos == std::string::npos ? normalized : normalized.substr(pos + 1);
}

std::string ChtlLoader::getDirectory(const std::string& path) {
    std::string normalized = normalizePath(path);
    size_t pos = normalized.find_last_of('/');
    return pos == std::string::npos ? "." : normalized.substr(0, pos);
}

std::string ChtlLoader::getFileExtension(const std::string& path) {
    std::string fileName = getFileName(path);
    size_t pos = fileName.find_last_of('.');
    return pos == std::string::npos ? "" : fileName.substr(pos);
}

bool ChtlLoader::isAbsolutePath(const std::string& path) {
    if (path.empty()) return false;
    
    // Unix风格的绝对路径
    if (path[0] == '/') return true;
    
    // Windows风格的绝对路径
    if (path.length() >= 2 && path[1] == ':') return true;
    
    return false;
}

std::string ChtlLoader::joinPath(const std::string& dir, const std::string& file) {
    if (dir.empty()) return file;
    if (file.empty()) return dir;
    
    std::string result = normalizePath(dir);
    if (result.back() != '/') {
        result += '/';
    }
    result += normalizePath(file);
    
    return result;
}

// 私有方法实现
ChtlLoader::LoadResult ChtlLoader::loadFileInternal(const std::string& fullPath, FileType fileType) {
    LoadResult result;
    result.filePath = fullPath;
    result.fileType = fileType;
    
    if (!fileExists(fullPath)) {
        result.success = false;
        result.errors.push_back("File does not exist: " + fullPath);
        addError("File does not exist: " + fullPath);
        return result;
    }
    
    try {
        result.content = readFileContent(fullPath);
        if (validateFileContent(result.content)) {
            result.success = true;
        } else {
            result.success = false;
            result.errors.push_back("Invalid file content: " + fullPath);
            addError("Invalid file content: " + fullPath);
        }
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("Failed to read file: " + std::string(e.what()));
        addError("Failed to read file: " + std::string(e.what()));
    }
    
    return result;
}

std::string ChtlLoader::readFileContent(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filePath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool ChtlLoader::validateFileContent(const std::string& content) {
    // 基础验证：检查是否为空，是否包含基本的CHTL语法
    if (content.empty()) {
        return false;
    }
    
    // 可以添加更多的验证逻辑
    return true;
}

void ChtlLoader::updateFileInfo(const std::string& filePath, FileType fileType) {
    FileInfo info;
    info.fullPath = filePath;
    info.fileName = getFileName(filePath);
    info.directory = getDirectory(filePath);
    info.fileType = fileType;
    info.isLoaded = true;
    
    // 获取文件统计信息
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) == 0) {
        info.fileSize = fileStat.st_size;
        info.lastModified = fileStat.st_mtime;
    }
    
    fileInfoCache_[filePath] = info;
    lastModifiedTimes_[filePath] = info.lastModified;
}

void ChtlLoader::detectDependencies(const std::string& filePath, const std::string& content) {
    // 使用正则表达式检测Import语句
    std::regex importRegex(R"(\[Import\]\s*([^\s]+)\s*from\s*([^\s;]+))");
    std::sregex_iterator iter(content.begin(), content.end(), importRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        if (match.size() >= 3) {
            std::string importedModule = match[1].str();
            std::string fileName = match[2].str();
            
            // 解析文件路径
            std::string resolvedPath = resolveFilePath(fileName);
            if (!resolvedPath.empty()) {
                addDependency(filePath, resolvedPath);
            }
        }
    }
}

bool ChtlLoader::isModifiedSinceLastLoad(const std::string& filePath) {
    auto it = lastModifiedTimes_.find(filePath);
    if (it == lastModifiedTimes_.end()) {
        return true; // 首次加载
    }
    
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) == 0) {
        return fileStat.st_mtime > it->second;
    }
    
    return true; // 如果无法获取状态，假设已修改
}

void ChtlLoader::addError(const std::string& error) {
    errors_.push_back(error);
}

bool ChtlLoader::checkCircularDependencyRecursive(const std::string& filePath,
                                                 std::unordered_set<std::string>& visited,
                                                 std::unordered_set<std::string>& recursionStack) {
    visited.insert(filePath);
    recursionStack.insert(filePath);
    
    auto it = dependencies_.find(filePath);
    if (it != dependencies_.end()) {
        for (const auto& dependency : it->second) {
            if (recursionStack.find(dependency) != recursionStack.end()) {
                return true; // 发现循环依赖
            }
            
            if (visited.find(dependency) == visited.end()) {
                if (checkCircularDependencyRecursive(dependency, visited, recursionStack)) {
                    return true;
                }
            }
        }
    }
    
    recursionStack.erase(filePath);
    return false;
}

// ModuleRegistry实现
ModuleRegistry& ModuleRegistry::getInstance() {
    static ModuleRegistry instance;
    return instance;
}

void ModuleRegistry::registerModule(const std::string& moduleName, const std::string& filePath) {
    modules_[moduleName] = filePath;
}

void ModuleRegistry::unregisterModule(const std::string& moduleName) {
    modules_.erase(moduleName);
}

bool ModuleRegistry::isModuleRegistered(const std::string& moduleName) const {
    return modules_.find(moduleName) != modules_.end();
}

std::string ModuleRegistry::getModulePath(const std::string& moduleName) const {
    auto it = modules_.find(moduleName);
    return it != modules_.end() ? it->second : "";
}

std::vector<std::string> ModuleRegistry::getAllModuleNames() const {
    std::vector<std::string> names;
    for (const auto& pair : modules_) {
        names.push_back(pair.first);
    }
    return names;
}

void ModuleRegistry::initializeBuiltinModules(const std::string& modulePath) {
    // 注册内置模块
    registerModule("layout", ChtlLoader::joinPath(modulePath, "layout.chtl"));
    registerModule("components", ChtlLoader::joinPath(modulePath, "components.chtl"));
    registerModule("utils", ChtlLoader::joinPath(modulePath, "utils.chtl"));
    
    // 设置描述
    setModuleDescription("layout", "Layout utilities and containers");
    setModuleDescription("components", "UI components library");
    setModuleDescription("utils", "Utility classes and variables");
    
    // 设置版本
    setModuleVersion("layout", "1.0.0");
    setModuleVersion("components", "1.0.0");
    setModuleVersion("utils", "1.0.0");
    
    // 添加分类
    addModuleCategory("ui", {"layout", "components"});
    addModuleCategory("utilities", {"utils"});
}

void ModuleRegistry::scanModuleDirectory(const std::string& modulePath) {
    DIR* dir = opendir(modulePath.c_str());
    if (dir != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_REG) { // 常规文件
                std::string fileName = entry->d_name;
                if (ChtlLoader::getFileExtension(fileName) == ".chtl") {
                    std::string moduleName = fileName;
                    // 移除.chtl扩展名
                    if (moduleName.length() > 5) {
                        moduleName = moduleName.substr(0, moduleName.length() - 5);
                    }
                    std::string fullPath = ChtlLoader::joinPath(modulePath, fileName);
                    registerModule(moduleName, fullPath);
                }
            }
        }
        closedir(dir);
    }
}

void ModuleRegistry::setModuleDescription(const std::string& moduleName, const std::string& description) {
    descriptions_[moduleName] = description;
}

std::string ModuleRegistry::getModuleDescription(const std::string& moduleName) const {
    auto it = descriptions_.find(moduleName);
    return it != descriptions_.end() ? it->second : "";
}

void ModuleRegistry::setModuleVersion(const std::string& moduleName, const std::string& version) {
    versions_[moduleName] = version;
}

std::string ModuleRegistry::getModuleVersion(const std::string& moduleName) const {
    auto it = versions_.find(moduleName);
    return it != versions_.end() ? it->second : "";
}

void ModuleRegistry::addModuleCategory(const std::string& category, const std::vector<std::string>& modules) {
    categories_[category] = modules;
}

void ModuleRegistry::clear() {
    modules_.clear();
    categories_.clear();
    descriptions_.clear();
    versions_.clear();
}

// FilePathResolver实现
FilePathResolver::FilePathResolver(const std::string& basePath) : basePath_(basePath), modulePath_("module") {
}

std::string FilePathResolver::resolvePath(const std::string& filePath) const {
    if (ChtlLoader::isAbsolutePath(filePath)) {
        return filePath;
    }
    
    return resolveRelativePath(filePath, basePath_);
}

std::string FilePathResolver::resolveRelativePath(const std::string& relativePath, const std::string& basePath) const {
    return ChtlLoader::joinPath(basePath, relativePath);
}

bool FilePathResolver::isValidPath(const std::string& path) const {
    return !path.empty() && path.find_first_of("<>:\"|?*") == std::string::npos;
}

bool FilePathResolver::isValidModuleName(const std::string& moduleName) const {
    if (moduleName.empty()) return false;
    
    // 模块名只能包含字母、数字、下划线和连字符
    for (char c : moduleName) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool FilePathResolver::isValidFileName(const std::string& fileName) const {
    return !fileName.empty() && fileName.find_first_of("<>:\"|?*") == std::string::npos;
}

void FilePathResolver::setBasePath(const std::string& basePath) {
    basePath_ = basePath;
}

std::string FilePathResolver::getBasePath() const {
    return basePath_;
}

// LoadContext实现
LoadContext::LoadContext() {
}

void LoadContext::pushContext(const std::string& filePath) {
    contextStack_.push_back(ChtlLoader::normalizePath(filePath));
}

void LoadContext::popContext() {
    if (!contextStack_.empty()) {
        contextStack_.pop_back();
    }
}

std::string LoadContext::getCurrentContext() const {
    return contextStack_.empty() ? "" : contextStack_.back();
}

std::vector<std::string> LoadContext::getContextStack() const {
    return contextStack_;
}

bool LoadContext::isInContext(const std::string& filePath) const {
    std::string normalized = ChtlLoader::normalizePath(filePath);
    return std::find(contextStack_.begin(), contextStack_.end(), normalized) != contextStack_.end();
}

bool LoadContext::wouldCreateCircularDependency(const std::string& filePath) const {
    return isInContext(filePath);
}

void LoadContext::reset() {
    contextStack_.clear();
}

size_t LoadContext::getDepth() const {
    return contextStack_.size();
}

bool LoadContext::isEmpty() const {
    return contextStack_.empty();
}

std::string LoadContext::getContextTrace() const {
    std::string trace;
    for (size_t i = 0; i < contextStack_.size(); ++i) {
        if (i > 0) trace += " -> ";
        trace += ChtlLoader::getFileName(contextStack_[i]);
    }
    return trace;
}

} // namespace chtl