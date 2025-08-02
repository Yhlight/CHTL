#ifndef CHTL_CONFIG_H
#define CHTL_CONFIG_H

#include "Node.h"
#include <string>
#include <vector>

namespace chtl {

// 配置节点
class ConfigurationNode : public Node {
private:
    std::map<std::string, std::string> configItems;
    bool hasNameBlock;
    
public:
    ConfigurationNode(int line = 0, int col = 0)
        : Node(NodeType::CONFIGURATION_NODE, line, col), hasNameBlock(false) {}
    
    void setConfigItem(const std::string& key, const std::string& value) {
        configItems[key] = value;
    }
    
    std::string getConfigItem(const std::string& key) const {
        auto it = configItems.find(key);
        if (it != configItems.end()) {
            return it->second;
        }
        return "";
    }
    
    const std::map<std::string, std::string>& getConfigItems() const {
        return configItems;
    }
    
    void setHasNameBlock(bool value) { hasNameBlock = value; }
    bool getHasNameBlock() const { return hasNameBlock; }
    
    std::string toString() const override {
        return "ConfigurationNode";
    }
};

// 配置项节点
class ConfigItemNode : public Node {
private:
    std::string key;
    std::string value;
    std::vector<std::string> options;  // 组选项，如 [option1, option2]
    
public:
    ConfigItemNode(const std::string& k, const std::string& v, int line = 0, int col = 0)
        : Node(NodeType::CONFIG_ITEM_NODE, line, col), key(k), value(v) {}
    
    const std::string& getKey() const { return key; }
    const std::string& getValue() const { return value; }
    
    void setKey(const std::string& k) { key = k; }
    void setValue(const std::string& v) { value = v; }
    
    void addOption(const std::string& opt) {
        options.push_back(opt);
    }
    
    const std::vector<std::string>& getOptions() const {
        return options;
    }
    
    bool hasOptions() const {
        return !options.empty();
    }
    
    std::string toString() const override {
        return "ConfigItemNode: " + key + " = " + value;
    }
};

// 导入节点
class ImportNode : public Node {
public:
    enum ImportType {
        IMPORT_HTML,
        IMPORT_STYLE,
        IMPORT_JAVASCRIPT,
        IMPORT_CHTL,
        IMPORT_CUSTOM_ELEMENT,
        IMPORT_CUSTOM_STYLE,
        IMPORT_CUSTOM_VAR,
        IMPORT_TEMPLATE_ELEMENT,
        IMPORT_TEMPLATE_STYLE,
        IMPORT_TEMPLATE_VAR
    };
    
private:
    ImportType importType;
    std::string targetName;     // 要导入的具体元素/样式/变量名
    std::string fromPath;       // 文件路径
    std::string asName;         // 重命名（可选）
    bool isWildcard;            // 是否是通配符导入
    
public:
    ImportNode(ImportType type, const std::string& path, int line = 0, int col = 0)
        : Node(NodeType::IMPORT_NODE, line, col), 
          importType(type), fromPath(path), isWildcard(false) {}
    
    ImportType getImportType() const { return importType; }
    const std::string& getTargetName() const { return targetName; }
    const std::string& getFromPath() const { return fromPath; }
    const std::string& getAsName() const { return asName; }
    bool getIsWildcard() const { return isWildcard; }
    
    void setTargetName(const std::string& name) { targetName = name; }
    void setAsName(const std::string& name) { asName = name; }
    void setIsWildcard(bool value) { isWildcard = value; }
    
    std::string toString() const override {
        std::string result = "ImportNode: ";
        if (!targetName.empty()) {
            result += targetName + " ";
        }
        result += "from " + fromPath;
        if (!asName.empty()) {
            result += " as " + asName;
        }
        return result;
    }
};

// 命名空间节点
class NamespaceNode : public Node {
private:
    std::string name;
    std::vector<std::string> nestedPath;  // 嵌套命名空间路径
    
public:
    NamespaceNode(const std::string& n, int line = 0, int col = 0)
        : Node(NodeType::NAMESPACE_NODE, line, col), name(n) {}
    
    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }
    
    void addNestedNamespace(const std::string& ns) {
        nestedPath.push_back(ns);
    }
    
    const std::vector<std::string>& getNestedPath() const {
        return nestedPath;
    }
    
    std::string getFullPath() const {
        std::string path = name;
        for (const auto& ns : nestedPath) {
            path += "." + ns;
        }
        return path;
    }
    
    std::string toString() const override {
        return "NamespaceNode: " + getFullPath();
    }
};

// 原始嵌入节点基类
class OriginNode : public Node {
protected:
    std::string content;
    
public:
    OriginNode(NodeType type, const std::string& c, int line = 0, int col = 0)
        : Node(type, line, col), content(c) {}
    
    const std::string& getContent() const { return content; }
    void setContent(const std::string& c) { content = c; }
};

// 原始HTML节点
class OriginHtmlNode : public OriginNode {
public:
    OriginHtmlNode(const std::string& content, int line = 0, int col = 0)
        : OriginNode(NodeType::ORIGIN_HTML_NODE, content, line, col) {}
    
    std::string toString() const override {
        return "OriginHtmlNode";
    }
};

// 原始Style节点
class OriginStyleNode : public OriginNode {
public:
    OriginStyleNode(const std::string& content, int line = 0, int col = 0)
        : OriginNode(NodeType::ORIGIN_STYLE_NODE, content, line, col) {}
    
    std::string toString() const override {
        return "OriginStyleNode";
    }
};

// 原始JavaScript节点
class OriginJsNode : public OriginNode {
public:
    OriginJsNode(const std::string& content, int line = 0, int col = 0)
        : OriginNode(NodeType::ORIGIN_JS_NODE, content, line, col) {}
    
    std::string toString() const override {
        return "OriginJsNode";
    }
};

// 期盼节点
class ExpectNode : public Node {
public:
    enum ExpectType {
        EXPECT_PRECISE,     // 精准期盼
        EXPECT_TYPE,        // 类型期盼
        EXPECT_NEGATIVE     // 否定期盼
    };
    
private:
    ExpectType expectType;
    std::vector<std::string> expectedItems;
    bool isNot;  // 是否是 not expect
    
public:
    ExpectNode(ExpectType type, bool notExpect = false, int line = 0, int col = 0)
        : Node(NodeType::EXPECT_NODE, line, col), 
          expectType(type), isNot(notExpect) {}
    
    ExpectType getExpectType() const { return expectType; }
    bool getIsNot() const { return isNot; }
    
    void addExpectedItem(const std::string& item) {
        expectedItems.push_back(item);
    }
    
    const std::vector<std::string>& getExpectedItems() const {
        return expectedItems;
    }
    
    std::string toString() const override {
        return isNot ? "NotExpectNode" : "ExpectNode";
    }
};

// 模板节点基类
class TemplateNode : public Node {
protected:
    std::string name;
    
public:
    TemplateNode(NodeType type, const std::string& n, int line = 0, int col = 0)
        : Node(type, line, col), name(n) {}
    
    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }
};

// 模板样式节点
class TemplateStyleNode : public TemplateNode {
public:
    TemplateStyleNode(const std::string& name, int line = 0, int col = 0)
        : TemplateNode(NodeType::TEMPLATE_STYLE_NODE, name, line, col) {}
    
    std::string toString() const override {
        return "TemplateStyleNode: " + name;
    }
};

// 模板元素节点
class TemplateElementNode : public TemplateNode {
public:
    TemplateElementNode(const std::string& name, int line = 0, int col = 0)
        : TemplateNode(NodeType::TEMPLATE_ELEMENT_NODE, name, line, col) {}
    
    std::string toString() const override {
        return "TemplateElementNode: " + name;
    }
};

// 模板变量节点
class TemplateVarNode : public TemplateNode {
public:
    TemplateVarNode(const std::string& name, int line = 0, int col = 0)
        : TemplateNode(NodeType::TEMPLATE_VAR_NODE, name, line, col) {}
    
    std::string toString() const override {
        return "TemplateVarNode: " + name;
    }
};

} // namespace chtl

#endif // CHTL_CONFIG_H