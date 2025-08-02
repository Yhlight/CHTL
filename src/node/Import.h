#ifndef CHTL_IMPORT_H
#define CHTL_IMPORT_H

#include "Node.h"
#include <string>
#include <vector>

namespace chtl {

// 导入节点 - [Import]
class ImportNode : public Node {
public:
    enum ImportType {
        IMPORT_HTML,              // [Import] @Html
        IMPORT_STYLE,             // [Import] @Style
        IMPORT_JAVASCRIPT,        // [Import] @JavaScript
        IMPORT_CHTL,              // [Import] @Chtl
        IMPORT_CUSTOM_ELEMENT,    // [Import] [Custom] @Element
        IMPORT_CUSTOM_STYLE,      // [Import] [Custom] @Style
        IMPORT_CUSTOM_VAR,        // [Import] [Custom] @Var
        IMPORT_TEMPLATE_ELEMENT,  // [Import] [Template] @Element
        IMPORT_TEMPLATE_STYLE,    // [Import] [Template] @Style
        IMPORT_TEMPLATE_VAR       // [Import] [Template] @Var
    };
    
private:
    ImportType importType;
    std::string targetName;         // 要导入的具体元素/样式/变量名
    std::string fromPath;           // 文件路径（原始路径）
    std::string resolvedPath;       // 解析后的绝对路径
    std::string asName;             // 重命名（可选）
    bool isWildcard;                // 是否是通配符导入 /*
    bool isResolved;                // 路径是否已解析
    bool isLoaded;                  // 文件是否已加载
    std::vector<std::string> expandedPaths;  // 通配符展开后的路径列表
    
public:
    ImportNode(ImportType type, const std::string& path, int line = 0, int col = 0)
        : Node(NodeType::IMPORT_NODE, line, col), 
          importType(type), fromPath(path), isWildcard(false),
          isResolved(false), isLoaded(false) {}
    
    // 基本访问器
    ImportType getImportType() const { return importType; }
    const std::string& getTargetName() const { return targetName; }
    const std::string& getFromPath() const { return fromPath; }
    const std::string& getResolvedPath() const { return resolvedPath; }
    const std::string& getAsName() const { return asName; }
    bool getIsWildcard() const { return isWildcard; }
    bool getIsResolved() const { return isResolved; }
    bool getIsLoaded() const { return isLoaded; }
    
    // 设置器
    void setImportType(ImportType type) { importType = type; }
    void setTargetName(const std::string& name) { targetName = name; }
    void setFromPath(const std::string& path) { fromPath = path; }
    void setAsName(const std::string& name) { asName = name; }
    void setIsWildcard(bool value) { isWildcard = value; }
    void setIsLoaded(bool value) { isLoaded = value; }
    
    // 路径解析
    void setResolvedPath(const std::string& path) { 
        resolvedPath = path;
        isResolved = true;
    }
    
    // 通配符展开
    void addExpandedPath(const std::string& path) {
        expandedPaths.push_back(path);
    }
    
    const std::vector<std::string>& getExpandedPaths() const {
        return expandedPaths;
    }
    
    void clearExpandedPaths() {
        expandedPaths.clear();
    }
    
    // 获取有效的导入名称（考虑重命名）
    std::string getEffectiveName() const {
        if (!asName.empty()) {
            return asName;
        }
        if (!targetName.empty()) {
            return targetName;
        }
        // 从路径中提取文件名作为默认名称
        size_t lastSlash = fromPath.find_last_of("/\\");
        size_t lastDot = fromPath.find_last_of('.');
        
        if (lastSlash != std::string::npos && lastDot != std::string::npos && lastDot > lastSlash) {
            return fromPath.substr(lastSlash + 1, lastDot - lastSlash - 1);
        } else if (lastSlash != std::string::npos) {
            return fromPath.substr(lastSlash + 1);
        }
        
        return fromPath;
    }
    
    // 检查是否需要特定目标名称
    bool requiresTargetName() const {
        switch (importType) {
            case IMPORT_CUSTOM_ELEMENT:
            case IMPORT_CUSTOM_STYLE:
            case IMPORT_CUSTOM_VAR:
            case IMPORT_TEMPLATE_ELEMENT:
            case IMPORT_TEMPLATE_STYLE:
            case IMPORT_TEMPLATE_VAR:
                return true;
            default:
                return false;
        }
    }
    
    // 获取导入类型的字符串表示
    std::string getImportTypeString() const {
        switch (importType) {
            case IMPORT_HTML: return "@Html";
            case IMPORT_STYLE: return "@Style";
            case IMPORT_JAVASCRIPT: return "@JavaScript";
            case IMPORT_CHTL: return "@Chtl";
            case IMPORT_CUSTOM_ELEMENT: return "[Custom] @Element";
            case IMPORT_CUSTOM_STYLE: return "[Custom] @Style";
            case IMPORT_CUSTOM_VAR: return "[Custom] @Var";
            case IMPORT_TEMPLATE_ELEMENT: return "[Template] @Element";
            case IMPORT_TEMPLATE_STYLE: return "[Template] @Style";
            case IMPORT_TEMPLATE_VAR: return "[Template] @Var";
            default: return "Unknown";
        }
    }
    
    std::string toString() const override {
        std::string result = "ImportNode: ";
        
        // 添加导入类型
        result += getImportTypeString() + " ";
        
        // 添加目标名称
        if (!targetName.empty()) {
            result += targetName + " ";
        }
        
        // 添加路径
        result += "from " + fromPath;
        
        // 添加解析后的路径
        if (isResolved && !resolvedPath.empty()) {
            result += " (resolved: " + resolvedPath + ")";
        }
        
        // 添加重命名
        if (!asName.empty()) {
            result += " as " + asName;
        }
        
        // 标记通配符
        if (isWildcard) {
            result += " (wildcard)";
            if (!expandedPaths.empty()) {
                result += " [" + std::to_string(expandedPaths.size()) + " files]";
            }
        }
        
        // 标记加载状态
        if (isLoaded) {
            result += " [loaded]";
        }
        
        return result;
    }
    
    // 验证导入节点的有效性
    bool validate() const override {
        // 检查路径是否为空
        if (fromPath.empty()) {
            return false;
        }
        
        // 检查是否需要目标名称
        if (requiresTargetName() && targetName.empty()) {
            return false;
        }
        
        return true;
    }
};

} // namespace chtl

#endif // CHTL_IMPORT_H