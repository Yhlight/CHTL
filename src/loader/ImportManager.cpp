#include "ImportManager.h"
#include <algorithm>
#include <regex>
#include <sstream>
#include <stack>
#include <sys/stat.h>
#include <unistd.h>
#include <ctime>

namespace chtl {

// ImportPathNormalizer实现
ImportPathNormalizer::ImportPathNormalizer() 
    : modulePath_("module"), workingDirectory_(".") {
}

std::string ImportPathNormalizer::normalizePath(const std::string& path) const {
    if (path.empty()) return path;
    
    // 替换反斜杠为正斜杠
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    
    // 移除多余的斜杠
    std::regex multiSlash("/+");
    normalized = std::regex_replace(normalized, multiSlash, "/");
    
    // 移除末尾的斜杠
    if (normalized.length() > 1 && normalized.back() == '/') {
        normalized.pop_back();
    }
    
    return removeRedundantComponents(normalized);
}

std::string ImportPathNormalizer::normalizeModulePath(const std::string& moduleName) const {
    std::string modulePath = ChtlLoader::joinPath(modulePath_, moduleName + ".chtl");
    return normalizePath(modulePath);
}

std::string ImportPathNormalizer::normalizeRelativePath(const std::string& path, const std::string& basePath) const {
    if (ChtlLoader::isAbsolutePath(path)) {
        return normalizePath(path);
    }
    
    std::string fullPath = ChtlLoader::joinPath(basePath, path);
    return normalizePath(fullPath);
}

bool ImportPathNormalizer::arePathsEquivalent(const std::string& path1, const std::string& path2) const {
    std::string canonical1 = getCanonicalPath(path1);
    std::string canonical2 = getCanonicalPath(path2);
    return canonical1 == canonical2;
}

std::string ImportPathNormalizer::getCanonicalPath(const std::string& path) const {
    std::string normalized = normalizePath(path);
    return resolveSymlinks(normalized);
}

ImportPathNormalizer::PathInfo ImportPathNormalizer::analyzePath(const std::string& path) const {
    PathInfo info;
    info.originalPath = path;
    info.normalizedPath = normalizePath(path);
    info.canonicalPath = getCanonicalPath(path);
    info.fileName = ChtlLoader::getFileName(info.normalizedPath);
    info.extension = ChtlLoader::getFileExtension(info.normalizedPath);
    info.directory = ChtlLoader::getDirectory(info.normalizedPath);
    info.isAbsolute = ChtlLoader::isAbsolutePath(info.normalizedPath);
    
    // 检查是否为模块
    std::string moduleName = info.fileName;
    if (info.extension == ".chtl" && moduleName.length() > 5) {
        moduleName = moduleName.substr(0, moduleName.length() - 5);
    }
    
    std::string modulePathCandidate = normalizeModulePath(moduleName);
    info.isModule = arePathsEquivalent(info.canonicalPath, modulePathCandidate);
    
    // 检查文件是否存在
    struct stat buffer;
    info.exists = (stat(info.canonicalPath.c_str(), &buffer) == 0);
    
    return info;
}

void ImportPathNormalizer::setModulePath(const std::string& modulePath) {
    modulePath_ = normalizePath(modulePath);
}

void ImportPathNormalizer::setWorkingDirectory(const std::string& workingDir) {
    workingDirectory_ = normalizePath(workingDir);
}

std::string ImportPathNormalizer::resolveSymlinks(const std::string& path) const {
    char resolved[PATH_MAX];
    if (realpath(path.c_str(), resolved) != nullptr) {
        return std::string(resolved);
    }
    return path; // 如果无法解析，返回原路径
}

std::string ImportPathNormalizer::removeRedundantComponents(const std::string& path) const {
    auto components = splitPathComponents(path);
    std::vector<std::string> resolved;
    
    for (const auto& component : components) {
        if (component == "." || component.empty()) {
            continue; // 跳过当前目录和空组件
        } else if (component == "..") {
            if (!resolved.empty() && resolved.back() != "..") {
                resolved.pop_back(); // 回到上级目录
            } else if (!ChtlLoader::isAbsolutePath(path)) {
                resolved.push_back(component); // 相对路径中的..
            }
        } else {
            resolved.push_back(component);
        }
    }
    
    return joinPathComponents(resolved);
}

std::vector<std::string> ImportPathNormalizer::splitPathComponents(const std::string& path) const {
    std::vector<std::string> components;
    std::string current;
    bool startsWithSlash = (!path.empty() && path[0] == '/');
    
    for (char c : path) {
        if (c == '/') {
            if (!current.empty()) {
                components.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        components.push_back(current);
    }
    
    // 保持绝对路径的前导斜杠
    if (startsWithSlash && !components.empty()) {
        components[0] = "/" + components[0];
    }
    
    return components;
}

std::string ImportPathNormalizer::joinPathComponents(const std::vector<std::string>& components) const {
    if (components.empty()) return ".";
    
    std::string result = components[0];
    for (size_t i = 1; i < components.size(); ++i) {
        if (!result.empty() && result.back() != '/') {
            result += "/";
        }
        result += components[i];
    }
    
    return result;
}

// CircularDependencyDetector实现
CircularDependencyDetector::CircularDependencyDetector() {
}

void CircularDependencyDetector::addDependency(const std::string& from, const std::string& to) {
    dependencies_[from].insert(to);
    dependents_[to].insert(from);
}

void CircularDependencyDetector::removeDependency(const std::string& from, const std::string& to) {
    auto it = dependencies_.find(from);
    if (it != dependencies_.end()) {
        it->second.erase(to);
        if (it->second.empty()) {
            dependencies_.erase(it);
        }
    }
    
    auto it2 = dependents_.find(to);
    if (it2 != dependents_.end()) {
        it2->second.erase(from);
        if (it2->second.empty()) {
            dependents_.erase(it2);
        }
    }
}

void CircularDependencyDetector::clearDependencies() {
    dependencies_.clear();
    dependents_.clear();
}

bool CircularDependencyDetector::hasCircularDependency() const {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursionStack;
    std::vector<std::string> path;
    
    for (const auto& pair : dependencies_) {
        if (visited.find(pair.first) == visited.end()) {
            if (dfsHasCycle(pair.first, visited, recursionStack, path)) {
                return true;
            }
        }
    }
    
    return false;
}

bool CircularDependencyDetector::hasCircularDependency(const std::string& startNode) const {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursionStack;
    std::vector<std::string> path;
    
    return dfsHasCycle(startNode, visited, recursionStack, path);
}

std::vector<std::string> CircularDependencyDetector::findCircularDependencyChain(const std::string& startNode) const {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> recursionStack;
    std::vector<std::string> path;
    
    if (dfsHasCycle(startNode, visited, recursionStack, path)) {
        return path;
    }
    
    return {};
}

std::vector<std::vector<std::string>> CircularDependencyDetector::findAllCircularDependencies() const {
    std::vector<std::vector<std::string>> cycles;
    std::unordered_set<std::string> globalVisited;
    
    for (const auto& pair : dependencies_) {
        if (globalVisited.find(pair.first) == globalVisited.end()) {
            std::unordered_set<std::string> visited;
            std::unordered_set<std::string> recursionStack;
            std::vector<std::string> path;
            
            if (dfsHasCycle(pair.first, visited, recursionStack, path)) {
                cycles.push_back(path);
                // 将循环中的所有节点标记为已访问
                for (const auto& node : path) {
                    globalVisited.insert(node);
                }
            }
            
            // 将本次DFS访问的节点加入全局已访问
            for (const auto& node : visited) {
                globalVisited.insert(node);
            }
        }
    }
    
    return cycles;
}

std::vector<std::string> CircularDependencyDetector::getTopologicalOrder() const {
    std::unordered_set<std::string> visited;
    std::stack<std::string> stack;
    
    for (const auto& pair : dependencies_) {
        if (visited.find(pair.first) == visited.end()) {
            topologicalSortUtil(pair.first, visited, stack);
        }
    }
    
    std::vector<std::string> result;
    while (!stack.empty()) {
        result.push_back(stack.top());
        stack.pop();
    }
    
    return result;
}

std::vector<std::string> CircularDependencyDetector::getDependencyOrder(const std::vector<std::string>& nodes) const {
    // 创建子图
    CircularDependencyDetector subgraph;
    
    std::unordered_set<std::string> nodeSet(nodes.begin(), nodes.end());
    
    for (const auto& node : nodes) {
        auto it = dependencies_.find(node);
        if (it != dependencies_.end()) {
            for (const auto& dep : it->second) {
                if (nodeSet.find(dep) != nodeSet.end()) {
                    subgraph.addDependency(node, dep);
                }
            }
        }
    }
    
    return subgraph.getTopologicalOrder();
}

std::vector<std::string> CircularDependencyDetector::getDependents(const std::string& node) const {
    auto it = dependents_.find(node);
    if (it != dependents_.end()) {
        return std::vector<std::string>(it->second.begin(), it->second.end());
    }
    return {};
}

std::vector<std::string> CircularDependencyDetector::getDependencies(const std::string& node) const {
    auto it = dependencies_.find(node);
    if (it != dependencies_.end()) {
        return std::vector<std::string>(it->second.begin(), it->second.end());
    }
    return {};
}

std::vector<std::string> CircularDependencyDetector::getAllDependencies(const std::string& node) const {
    std::unordered_set<std::string> visited;
    std::vector<std::string> result;
    
    dfsCollectDependencies(node, visited, result);
    
    return result;
}

int CircularDependencyDetector::getDependencyDepth(const std::string& node) const {
    std::unordered_set<std::string> visited;
    std::queue<std::pair<std::string, int>> queue;
    
    queue.push({node, 0});
    visited.insert(node);
    
    int maxDepth = 0;
    
    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();
        std::string currentNode = current.first;
        int depth = current.second;
        
        maxDepth = std::max(maxDepth, depth);
        
        auto it = dependencies_.find(currentNode);
        if (it != dependencies_.end()) {
            for (const auto& dep : it->second) {
                if (visited.find(dep) == visited.end()) {
                    visited.insert(dep);
                    queue.push({dep, depth + 1});
                }
            }
        }
    }
    
    return maxDepth;
}

size_t CircularDependencyDetector::getNodeCount() const {
    std::unordered_set<std::string> allNodes;
    
    for (const auto& pair : dependencies_) {
        allNodes.insert(pair.first);
        for (const auto& dep : pair.second) {
            allNodes.insert(dep);
        }
    }
    
    return allNodes.size();
}

size_t CircularDependencyDetector::getEdgeCount() const {
    size_t count = 0;
    for (const auto& pair : dependencies_) {
        count += pair.second.size();
    }
    return count;
}

std::vector<std::string> CircularDependencyDetector::getAllNodes() const {
    std::unordered_set<std::string> allNodes;
    
    for (const auto& pair : dependencies_) {
        allNodes.insert(pair.first);
        for (const auto& dep : pair.second) {
            allNodes.insert(dep);
        }
    }
    
    return std::vector<std::string>(allNodes.begin(), allNodes.end());
}

bool CircularDependencyDetector::hasNode(const std::string& node) const {
    if (dependencies_.find(node) != dependencies_.end()) {
        return true;
    }
    
    if (dependents_.find(node) != dependents_.end()) {
        return true;
    }
    
    return false;
}

std::string CircularDependencyDetector::toDotFormat() const {
    std::stringstream ss;
    ss << "digraph Dependencies {\n";
    ss << "  rankdir=TB;\n";
    ss << "  node [shape=box];\n\n";
    
    for (const auto& pair : dependencies_) {
        for (const auto& dep : pair.second) {
            ss << "  \"" << pair.first << "\" -> \"" << dep << "\";\n";
        }
    }
    
    ss << "}\n";
    return ss.str();
}

void CircularDependencyDetector::printDependencyGraph() const {
    std::cout << "依赖关系图:\n";
    std::cout << "节点数: " << getNodeCount() << ", 边数: " << getEdgeCount() << "\n\n";
    
    for (const auto& pair : dependencies_) {
        std::cout << pair.first << " 依赖于:\n";
        for (const auto& dep : pair.second) {
            std::cout << "  -> " << dep << "\n";
        }
        std::cout << "\n";
    }
}

bool CircularDependencyDetector::dfsHasCycle(const std::string& node,
                                            std::unordered_set<std::string>& visited,
                                            std::unordered_set<std::string>& recursionStack,
                                            std::vector<std::string>& path) const {
    visited.insert(node);
    recursionStack.insert(node);
    path.push_back(node);
    
    auto it = dependencies_.find(node);
    if (it != dependencies_.end()) {
        for (const auto& neighbor : it->second) {
            if (recursionStack.find(neighbor) != recursionStack.end()) {
                // 找到循环，构建循环路径
                auto cycleStart = std::find(path.begin(), path.end(), neighbor);
                path.erase(path.begin(), cycleStart);
                path.push_back(neighbor); // 完成循环
                return true;
            }
            
            if (visited.find(neighbor) == visited.end()) {
                if (dfsHasCycle(neighbor, visited, recursionStack, path)) {
                    return true;
                }
            }
        }
    }
    
    recursionStack.erase(node);
    path.pop_back();
    return false;
}

void CircularDependencyDetector::dfsCollectDependencies(const std::string& node,
                                                       std::unordered_set<std::string>& visited,
                                                       std::vector<std::string>& result) const {
    if (visited.find(node) != visited.end()) {
        return;
    }
    
    visited.insert(node);
    
    auto it = dependencies_.find(node);
    if (it != dependencies_.end()) {
        for (const auto& dep : it->second) {
            result.push_back(dep);
            dfsCollectDependencies(dep, visited, result);
        }
    }
}

void CircularDependencyDetector::topologicalSortUtil(const std::string& node,
                                                     std::unordered_set<std::string>& visited,
                                                     std::stack<std::string>& stack) const {
    visited.insert(node);
    
    auto it = dependencies_.find(node);
    if (it != dependencies_.end()) {
        for (const auto& neighbor : it->second) {
            if (visited.find(neighbor) == visited.end()) {
                topologicalSortUtil(neighbor, visited, stack);
            }
        }
    }
    
    stack.push(node);
}

// DuplicateImportManager实现
DuplicateImportManager::DuplicateImportManager() {
}

bool DuplicateImportManager::isAlreadyImported(const std::string& path, const std::string& fromFile) const {
    std::string key = makeKey(path, fromFile);
    return importRecords_.find(key) != importRecords_.end();
}

bool DuplicateImportManager::isAlreadyImported(const std::string& path) const {
    auto it = importsOfFile_.find(path);
    return it != importsOfFile_.end() && !it->second.empty();
}

std::vector<DuplicateImportManager::ImportRecord> DuplicateImportManager::findDuplicateImports(const std::string& path) const {
    std::vector<ImportRecord> duplicates;
    
    auto it = importsOfFile_.find(path);
    if (it != importsOfFile_.end()) {
        for (const auto& key : it->second) {
            auto recordIt = importRecords_.find(key);
            if (recordIt != importRecords_.end()) {
                duplicates.push_back(recordIt->second);
            }
        }
    }
    
    return duplicates;
}

void DuplicateImportManager::recordImport(const std::string& path, const std::string& fromFile,
                                         std::shared_ptr<ImportNode> importNode) {
    std::string key = makeKey(path, fromFile);
    
    ImportRecord record;
    record.normalizedPath = path;
    record.originalPath = path;  // 简化实现，使用path而不是importNode->getImportItem()
    record.importNode = importNode;
    record.fromFile = fromFile;
    record.importTime = std::time(nullptr);
    record.isResolved = false;
    
    importRecords_[key] = record;
    updateIndices(path, fromFile, true);
}

void DuplicateImportManager::markAsResolved(const std::string& path, const std::string& fromFile) {
    std::string key = makeKey(path, fromFile);
    auto it = importRecords_.find(key);
    if (it != importRecords_.end()) {
        it->second.isResolved = true;
    }
}

void DuplicateImportManager::removeImport(const std::string& path, const std::string& fromFile) {
    std::string key = makeKey(path, fromFile);
    importRecords_.erase(key);
    updateIndices(path, fromFile, false);
}

std::vector<DuplicateImportManager::ImportRecord> DuplicateImportManager::getImportsForFile(const std::string& fromFile) const {
    std::vector<ImportRecord> imports;
    
    auto it = importsByFile_.find(fromFile);
    if (it != importsByFile_.end()) {
        for (const auto& key : it->second) {
            auto recordIt = importRecords_.find(key);
            if (recordIt != importRecords_.end()) {
                imports.push_back(recordIt->second);
            }
        }
    }
    
    return imports;
}

std::vector<DuplicateImportManager::ImportRecord> DuplicateImportManager::getImportsOfFile(const std::string& path) const {
    return findDuplicateImports(path);
}

std::vector<DuplicateImportManager::ImportRecord> DuplicateImportManager::getAllImports() const {
    std::vector<ImportRecord> allImports;
    
    for (const auto& pair : importRecords_) {
        allImports.push_back(pair.second);
    }
    
    return allImports;
}

size_t DuplicateImportManager::getImportCount() const {
    return importRecords_.size();
}

std::unordered_map<std::string, int> DuplicateImportManager::getImportFrequency() const {
    std::unordered_map<std::string, int> frequency;
    
    for (const auto& pair : importRecords_) {
        frequency[pair.second.normalizedPath]++;
    }
    
    return frequency;
}

std::vector<std::string> DuplicateImportManager::getMostImportedFiles() const {
    auto frequency = getImportFrequency();
    
    std::vector<std::pair<std::string, int>> sorted(frequency.begin(), frequency.end());
    std::sort(sorted.begin(), sorted.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<std::string> result;
    for (const auto& pair : sorted) {
        result.push_back(pair.first);
    }
    
    return result;
}

std::vector<std::string> DuplicateImportManager::getFileImportOrder() const {
    std::vector<std::pair<time_t, std::string>> timeOrder;
    
    for (const auto& pair : importRecords_) {
        timeOrder.push_back({pair.second.importTime, pair.second.normalizedPath});
    }
    
    std::sort(timeOrder.begin(), timeOrder.end());
    
    std::vector<std::string> result;
    std::unordered_set<std::string> seen;
    
    for (const auto& pair : timeOrder) {
        if (seen.find(pair.second) == seen.end()) {
            result.push_back(pair.second);
            seen.insert(pair.second);
        }
    }
    
    return result;
}

void DuplicateImportManager::clearImports() {
    importRecords_.clear();
    importsByFile_.clear();
    importsOfFile_.clear();
}

void DuplicateImportManager::clearImportsForFile(const std::string& fromFile) {
    auto it = importsByFile_.find(fromFile);
    if (it != importsByFile_.end()) {
        for (const auto& key : it->second) {
            auto recordIt = importRecords_.find(key);
            if (recordIt != importRecords_.end()) {
                updateIndices(recordIt->second.normalizedPath, fromFile, false);
                importRecords_.erase(recordIt);
            }
        }
        importsByFile_.erase(it);
    }
}

void DuplicateImportManager::clearOldImports(time_t beforeTime) {
    auto it = importRecords_.begin();
    while (it != importRecords_.end()) {
        if (it->second.importTime < beforeTime) {
            updateIndices(it->second.normalizedPath, it->second.fromFile, false);
            it = importRecords_.erase(it);
        } else {
            ++it;
        }
    }
}

std::string DuplicateImportManager::makeKey(const std::string& path, const std::string& fromFile) const {
    return path + "|" + fromFile;
}

void DuplicateImportManager::updateIndices(const std::string& path, const std::string& fromFile, bool add) {
    std::string key = makeKey(path, fromFile);
    
    if (add) {
        importsByFile_[fromFile].push_back(key);
        importsOfFile_[path].push_back(key);
    } else {
        // 从索引中移除
        auto& byFile = importsByFile_[fromFile];
        byFile.erase(std::remove(byFile.begin(), byFile.end(), key), byFile.end());
        if (byFile.empty()) {
            importsByFile_.erase(fromFile);
        }
        
        auto& ofFile = importsOfFile_[path];
        ofFile.erase(std::remove(ofFile.begin(), ofFile.end(), key), ofFile.end());
        if (ofFile.empty()) {
            importsOfFile_.erase(path);
        }
    }
}

// EnhancedImportManager实现
EnhancedImportManager::EnhancedImportManager() : cacheEnabled_(true) {
    pathNormalizer_ = std::make_unique<ImportPathNormalizer>();
    circularDetector_ = std::make_unique<CircularDependencyDetector>();
    duplicateManager_ = std::make_unique<DuplicateImportManager>();
}

void EnhancedImportManager::initialize(const std::string& modulePath, const std::string& workingDir) {
    pathNormalizer_->setModulePath(modulePath);
    pathNormalizer_->setWorkingDirectory(workingDir);
    
    if (!loader_) {
        loader_ = std::make_unique<ChtlLoader>();
        loader_->setModulePath(modulePath);
        loader_->setWorkingDirectory(workingDir);
    }
}

void EnhancedImportManager::setLoader(std::unique_ptr<ChtlLoader> loader) {
    loader_ = std::move(loader);
}

EnhancedImportManager::ImportResult EnhancedImportManager::processImport(
    std::shared_ptr<ImportNode> importNode, const std::string& fromFile) {
    
    ImportResult result;
    
    if (!importNode) {
        result.errors.push_back("空的导入节点");
        return result;
    }
    
    // 提取文件路径
    std::string filePath = extractFilePathFromImport(importNode);
    if (filePath.empty()) {
        result.errors.push_back("无法从导入节点提取文件路径");
        return result;
    }
    
    // 路径规范化
    std::string normalizedFromFile = pathNormalizer_->normalizePath(fromFile);
    std::string normalizedPath = pathNormalizer_->normalizePath(filePath);
    
    // 检查文件是否存在
    auto pathInfo = pathNormalizer_->analyzePath(normalizedPath);
    if (!pathInfo.exists) {
        result.errors.push_back("文件不存在: " + normalizedPath);
        return result;
    }
    
    result.normalizedPath = pathInfo.canonicalPath;
    
    // 检查循环依赖
    if (checkForCircularDependency(normalizedFromFile, result.normalizedPath)) {
        auto chain = circularDetector_->findCircularDependencyChain(normalizedFromFile);
        result.circularDependencyChain = chain;
        result.errors.push_back("检测到循环依赖: " + normalizedFromFile + " -> " + result.normalizedPath);
        return result;
    }
    
    // 检查重复导入
    if (duplicateManager_->isAlreadyImported(result.normalizedPath, normalizedFromFile)) {
        result.wasDuplicate = true;
        result.warnings.push_back("重复导入: " + result.normalizedPath + " 已在 " + normalizedFromFile + " 中导入");
        
        // 如果启用缓存，直接返回缓存内容
        if (cacheEnabled_) {
            auto it = contentCache_.find(result.normalizedPath);
            if (it != contentCache_.end()) {
                result.success = true;
                result.content = it->second;
                result.wasCached = true;
                return result;
            }
        }
    }
    
    // 记录导入
    duplicateManager_->recordImport(result.normalizedPath, normalizedFromFile, importNode);
    
    // 加载和处理文件
    result = loadAndProcessFile(result.normalizedPath, normalizedFromFile, importNode);
    
    if (result.success) {
        // 更新依赖图
        updateDependencyGraph(normalizedFromFile, result.normalizedPath);
        
        // 标记为已解析
        duplicateManager_->markAsResolved(result.normalizedPath, normalizedFromFile);
        
        // 缓存内容
        if (cacheEnabled_) {
            contentCache_[result.normalizedPath] = result.content;
        }
    }
    
    return result;
}

std::vector<EnhancedImportManager::ImportResult> EnhancedImportManager::processAllImports(
    const std::vector<std::shared_ptr<ImportNode>>& imports, const std::string& fromFile) {
    
    std::vector<ImportResult> results;
    
    // 获取优化的导入顺序
    auto optimalOrder = getOptimalImportOrder(imports, fromFile);
    
    // 按优化顺序处理导入
    for (const auto& filePath : optimalOrder) {
        // 查找对应的ImportNode
        std::shared_ptr<ImportNode> importNode;
        for (const auto& node : imports) {
            if (extractFilePathFromImport(node) == filePath) {
                importNode = node;
                break;
            }
        }
        
        if (importNode) {
            auto result = processImport(importNode, fromFile);
            results.push_back(result);
        }
    }
    
    return results;
}

bool EnhancedImportManager::validateImportChain(const std::vector<std::shared_ptr<ImportNode>>& imports,
                                               const std::string& fromFile) {
    // 临时添加依赖关系进行验证
    CircularDependencyDetector tempDetector = *circularDetector_;
    
    std::string normalizedFromFile = pathNormalizer_->normalizePath(fromFile);
    
    for (const auto& importNode : imports) {
        std::string filePath = extractFilePathFromImport(importNode);
        std::string normalizedPath = pathNormalizer_->normalizePath(filePath);
        
        tempDetector.addDependency(normalizedFromFile, normalizedPath);
    }
    
    return !tempDetector.hasCircularDependency(normalizedFromFile);
}

std::vector<std::string> EnhancedImportManager::getOptimalImportOrder(
    const std::vector<std::shared_ptr<ImportNode>>& imports, const std::string& fromFile) {
    
    std::vector<std::string> filePaths;
    for (const auto& importNode : imports) {
        std::string filePath = extractFilePathFromImport(importNode);
        std::string normalizedPath = pathNormalizer_->normalizePath(filePath);
        filePaths.push_back(normalizedPath);
    }
    
    return circularDetector_->getDependencyOrder(filePaths);
}

bool EnhancedImportManager::hasCircularDependencies() const {
    return circularDetector_->hasCircularDependency();
}

std::vector<std::vector<std::string>> EnhancedImportManager::getAllCircularDependencies() const {
    return circularDetector_->findAllCircularDependencies();
}

std::vector<DuplicateImportManager::ImportRecord> EnhancedImportManager::getDuplicateImports() const {
    auto frequency = duplicateManager_->getImportFrequency();
    std::vector<DuplicateImportManager::ImportRecord> duplicates;
    
    for (const auto& pair : frequency) {
        if (pair.second > 1) {
            auto records = duplicateManager_->findDuplicateImports(pair.first);
            duplicates.insert(duplicates.end(), records.begin(), records.end());
        }
    }
    
    return duplicates;
}

EnhancedImportManager::ImportStatistics EnhancedImportManager::getStatistics() const {
    ImportStatistics stats;
    
    stats.totalImports = duplicateManager_->getImportCount();
    
    auto frequency = duplicateManager_->getImportFrequency();
    stats.uniqueFiles = frequency.size();
    
    stats.duplicateImports = 0;
    for (const auto& pair : frequency) {
        if (pair.second > 1) {
            stats.duplicateImports += pair.second - 1;
        }
    }
    
    auto cycles = circularDetector_->findAllCircularDependencies();
    stats.circularDependencies = cycles.size();
    
    stats.cachedLoads = contentCache_.size();
    
    // 计算平均依赖深度
    auto allNodes = circularDetector_->getAllNodes();
    if (!allNodes.empty()) {
        int totalDepth = 0;
        for (const auto& node : allNodes) {
            totalDepth += circularDetector_->getDependencyDepth(node);
        }
        stats.averageDependencyDepth = static_cast<double>(totalDepth) / allNodes.size();
    }
    
    return stats;
}

std::vector<std::string> EnhancedImportManager::getErrors() const {
    return errors_;
}

std::vector<std::string> EnhancedImportManager::getWarnings() const {
    return warnings_;
}

void EnhancedImportManager::clearErrors() {
    errors_.clear();
}

void EnhancedImportManager::clearWarnings() {
    warnings_.clear();
}

void EnhancedImportManager::enableCache(bool enable) {
    cacheEnabled_ = enable;
    if (!enable) {
        clearCache();
    }
}

bool EnhancedImportManager::isCacheEnabled() const {
    return cacheEnabled_;
}

void EnhancedImportManager::clearCache() {
    contentCache_.clear();
}

void EnhancedImportManager::invalidateCache(const std::string& path) {
    std::string normalizedPath = pathNormalizer_->normalizePath(path);
    contentCache_.erase(normalizedPath);
}

void EnhancedImportManager::printDependencyGraph() const {
    circularDetector_->printDependencyGraph();
}

void EnhancedImportManager::printImportStatistics() const {
    auto stats = getStatistics();
    
    std::cout << "=== Import统计信息 ===\n";
    std::cout << "总导入数: " << stats.totalImports << "\n";
    std::cout << "唯一文件数: " << stats.uniqueFiles << "\n";
    std::cout << "重复导入数: " << stats.duplicateImports << "\n";
    std::cout << "循环依赖数: " << stats.circularDependencies << "\n";
    std::cout << "缓存文件数: " << stats.cachedLoads << "\n";
    std::cout << "平均依赖深度: " << stats.averageDependencyDepth << "\n";
}

std::string EnhancedImportManager::exportDependencyGraph() const {
    return circularDetector_->toDotFormat();
}

EnhancedImportManager::ImportResult EnhancedImportManager::loadAndProcessFile(
    const std::string& normalizedPath, const std::string& fromFile,
    std::shared_ptr<ImportNode> importNode) {
    
    ImportResult result;
    result.normalizedPath = normalizedPath;
    
    if (!loader_) {
        result.errors.push_back("加载器未初始化");
        return result;
    }
    
    auto loadResult = loader_->loadFileFromPath(normalizedPath, ChtlLoader::FileType::IMPORTED_FILE);
    
    if (!loadResult.success) {
        result.errors = loadResult.errors;
        return result;
    }
    
    result.success = true;
    result.content = loadResult.content;
    
    // 分析文件中的导入语句，用于构建依赖图
    auto imports = analyzeFileForImports(result.content);
    for (const auto& importPath : imports) {
        std::string normalizedImportPath = pathNormalizer_->normalizePath(importPath);
        updateDependencyGraph(normalizedPath, normalizedImportPath);
    }
    
    return result;
}

void EnhancedImportManager::addError(const std::string& error) {
    errors_.push_back(error);
}

void EnhancedImportManager::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

std::string EnhancedImportManager::extractFilePathFromImport(std::shared_ptr<ImportNode> importNode) const {
    // 临时简化实现，避免依赖未完成的ImportNode
    (void)importNode;  // 避免未使用警告
    return ""; // 暂时返回空，实际应用中需要完整的ImportNode实现
}

void EnhancedImportManager::updateDependencyGraph(const std::string& fromFile, const std::string& toFile) {
    circularDetector_->addDependency(fromFile, toFile);
}

bool EnhancedImportManager::checkForCircularDependency(const std::string& fromFile, const std::string& toFile) {
    // 临时添加依赖关系检查循环
    circularDetector_->addDependency(fromFile, toFile);
    bool hasCycle = circularDetector_->hasCircularDependency(fromFile);
    
    if (hasCycle) {
        // 如果发现循环依赖，移除这个依赖关系
        circularDetector_->removeDependency(fromFile, toFile);
    }
    
    return hasCycle;
}

std::vector<std::string> EnhancedImportManager::analyzeFileForImports(const std::string& content) const {
    std::vector<std::string> imports;
    
    // 使用正则表达式查找Import语句
    std::regex importRegex(R"(\[Import\][^;]*from\s+([^\s;]+))");
    std::sregex_iterator iter(content.begin(), content.end(), importRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        if (match.size() >= 2) {
            std::string fileName = match[1].str();
            imports.push_back(fileName);
        }
    }
    
    return imports;
}

} // namespace chtl