#ifndef CHTL_IMPORT_H
#define CHTL_IMPORT_H

#include "Node.h"
#include <string>

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
    std::string targetName;     // 要导入的具体元素/样式/变量名
    std::string fromPath;       // 文件路径
    std::string asName;         // 重命名（可选）
    bool isWildcard;            // 是否是通配符导入 /*
    
public:
    ImportNode(ImportType type, const std::string& path, int line = 0, int col = 0)
        : Node(NodeType::IMPORT_NODE, line, col), 
          importType(type), fromPath(path), isWildcard(false) {}
    
    ImportType getImportType() const { return importType; }
    const std::string& getTargetName() const { return targetName; }
    const std::string& getFromPath() const { return fromPath; }
    const std::string& getAsName() const { return asName; }
    bool getIsWildcard() const { return isWildcard; }
    
    void setImportType(ImportType type) { importType = type; }
    void setTargetName(const std::string& name) { targetName = name; }
    void setFromPath(const std::string& path) { fromPath = path; }
    void setAsName(const std::string& name) { asName = name; }
    void setIsWildcard(bool value) { isWildcard = value; }
    
    std::string toString() const override {
        std::string result = "ImportNode: ";
        
        // 添加导入类型
        switch (importType) {
            case IMPORT_HTML: result += "@Html "; break;
            case IMPORT_STYLE: result += "@Style "; break;
            case IMPORT_JAVASCRIPT: result += "@JavaScript "; break;
            case IMPORT_CHTL: result += "@Chtl "; break;
            case IMPORT_CUSTOM_ELEMENT: result += "[Custom] @Element "; break;
            case IMPORT_CUSTOM_STYLE: result += "[Custom] @Style "; break;
            case IMPORT_CUSTOM_VAR: result += "[Custom] @Var "; break;
            case IMPORT_TEMPLATE_ELEMENT: result += "[Template] @Element "; break;
            case IMPORT_TEMPLATE_STYLE: result += "[Template] @Style "; break;
            case IMPORT_TEMPLATE_VAR: result += "[Template] @Var "; break;
        }
        
        // 添加目标名称
        if (!targetName.empty()) {
            result += targetName + " ";
        }
        
        // 添加路径
        result += "from " + fromPath;
        
        // 添加重命名
        if (!asName.empty()) {
            result += " as " + asName;
        }
        
        // 标记通配符
        if (isWildcard) {
            result += " (wildcard)";
        }
        
        return result;
    }
};

} // namespace chtl

#endif // CHTL_IMPORT_H