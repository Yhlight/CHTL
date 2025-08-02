#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include <string>
#include <memory>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../node/Node.h"

namespace chtl {

// 生成器配置
struct GeneratorConfig {
    bool minify = false;                    // 是否压缩输出
    bool prettyPrint = true;                // 是否美化输出
    int indentSize = 2;                     // 缩进大小
    bool useSpaces = true;                  // 使用空格还是制表符
    bool hoistStyles = true;                // 是否提升样式到<head>
    bool hoistScripts = true;               // 是否提升脚本到<body>末尾
    bool generateComments = true;           // 是否生成注释
    bool generateSourceMap = false;         // 是否生成源映射
    std::string charset = "UTF-8";          // 字符集
    std::string doctype = "<!DOCTYPE html>"; // 文档类型
};

// 生成上下文
struct GeneratorContext {
    int indentLevel = 0;                    // 当前缩进级别
    std::stringstream output;               // 输出流
    std::stringstream styles;               // 样式收集
    std::stringstream scripts;              // 脚本收集
    std::unordered_set<std::string> usedClasses;      // 已使用的类名
    std::unordered_set<std::string> usedIds;          // 已使用的ID
    std::unordered_map<std::string, std::string> varValues; // 变量值映射
    bool inHead = false;                    // 是否在<head>中
    bool inStyle = false;                   // 是否在<style>中
    bool inScript = false;                  // 是否在<script>中
    
    // 获取当前缩进
    std::string getIndent() const;
    
    // 增加/减少缩进
    void increaseIndent() { indentLevel++; }
    void decreaseIndent() { if (indentLevel > 0) indentLevel--; }
};

// 生成器基类
class Generator {
protected:
    GeneratorConfig config;
    GeneratorContext context;
    
    // 工具方法
    void write(const std::string& text);
    void writeLine(const std::string& text = "");
    void writeIndent();
    std::string escapeHtml(const std::string& text);
    std::string escapeAttribute(const std::string& text);
    
    // 节点生成方法（子类需要实现）
    virtual void generateNode(const NodePtr& node);
    virtual void generateRootNode(const RootNode* node);
    virtual void generateTextNode(const TextNode* node);
    virtual void generateElementNode(const ElementNode* node);
    virtual void generateCommentNode(const CommentNode* node);
    virtual void generateStyleNode(const StyleNode* node);
    virtual void generateAttributeNode(const AttributeNode* node);
    
    // 样式相关
    virtual void generateStyleProperty(const StylePropertyNode* node);
    virtual void generateStyleRule(const StyleRuleNode* node);
    virtual void generateStyleGroup(const StyleGroupNode* node);
    
    // 自定义相关
    virtual void generateCustomStyle(const CustomStyleNode* node);
    virtual void generateCustomElement(const CustomElementNode* node);
    virtual void generateCustomVar(const CustomVarNode* node);
    
    // 模板相关
    virtual void generateTemplateStyle(const TemplateStyleNode* node);
    virtual void generateTemplateElement(const TemplateElementNode* node);
    virtual void generateTemplateVar(const TemplateVarNode* node);
    
    // 原始嵌入
    virtual void generateOriginHtml(const OriginHtmlNode* node);
    virtual void generateOriginStyle(const OriginStyleNode* node);
    virtual void generateOriginJs(const OriginJsNode* node);
    
    // 其他
    virtual void generateImport(const ImportNode* node);
    virtual void generateNamespace(const NamespaceNode* node);
    virtual void generateConfiguration(const ConfigurationNode* node);
    
    // 收集样式和脚本
    void collectStyles(const NodePtr& node);
    void collectScripts(const NodePtr& node);
    
    // 处理样式提升
    void hoistStyles();
    void hoistScripts();
    
public:
    explicit Generator(const GeneratorConfig& cfg = GeneratorConfig());
    virtual ~Generator() = default;
    
    // 主要接口
    virtual std::string generate(const NodePtr& ast);
    
    // 配置
    void setConfig(const GeneratorConfig& cfg) { config = cfg; }
    const GeneratorConfig& getConfig() const { return config; }
    
    // 获取生成的样式和脚本
    std::string getCollectedStyles() const { return context.styles.str(); }
    std::string getCollectedScripts() const { return context.scripts.str(); }
};

} // namespace chtl

#endif // CHTL_GENERATOR_H