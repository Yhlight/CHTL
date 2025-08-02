#ifndef CHTL_HTML_GENERATOR_H
#define CHTL_HTML_GENERATOR_H

#include "Generator.h"
#include "../predefine/Registry.h"
#include <stack>
#include <set>

namespace chtl {

// HTML生成器 - 生成标准HTML5
class HtmlGenerator : public Generator {
private:
    // 当前处理的元素栈
    std::stack<std::string> elementStack;
    
    // 已处理的自定义/模板定义
    std::set<std::string> processedCustomStyles;
    std::set<std::string> processedCustomElements;
    std::set<std::string> processedCustomVars;
    
    // 样式规则收集器（用于去重和合并）
    std::map<std::string, std::vector<std::shared_ptr<StylePropertyNode>>> styleRules;
    
    // 全局样式内容
    std::stringstream globalStyles;
    std::stringstream globalScripts;
    
    // 自动生成的类名计数器
    int autoClassCounter = 0;
    
    // 辅助方法
    std::string generateAutoClassName();
    void mergeStyleRules();
    void processStyleSelectors(StyleNode* styleNode, ElementNode* parentElement);
    void expandCustomStyle(const std::string& styleName, StyleNode* targetStyle);
    void expandCustomElement(const std::string& elementName, ElementNode* parentElement);
    void expandVarUsage(const std::string& varName, const std::map<std::string, std::string>& params);
    
    // 处理CHTL特有的样式语法
    void processEnhancedSelectors(StyleRuleNode* rule, ElementNode* element);
    void processAmpersandSelector(const std::string& selector, ElementNode* element);
    
    // 样式提升和优化
    void optimizeStyles();
    void hoistGlobalStyles();
    
protected:
    // 重写部分生成方法以支持CHTL特性
    void generateElementNode(const ElementNode* node) override;
    void generateStyleNode(const StyleNode* node) override;
    void generateStyleRule(const StyleRuleNode* node) override;
    
    // 处理自定义和模板
    void generateCustomStyle(const CustomStyleNode* node) override;
    void generateCustomElement(const CustomElementNode* node) override;
    void generateCustomVar(const CustomVarNode* node) override;
    
    // 处理变量使用
    void generateVarUsage(const VarUsageNode* node);
    
public:
    explicit HtmlGenerator(const GeneratorConfig& cfg = GeneratorConfig());
    
    // 重写主生成方法
    std::string generate(const NodePtr& ast) override;
    
    // 获取完整的HTML输出
    std::string generateCompleteHtml(const NodePtr& ast);
    
    // 设置是否使用预定义内容
    void setUsePredefined(bool use) { usePredefined = use; }
    
private:
    bool usePredefined = true;  // 是否使用预定义内容
    
    // 查找并加载预定义内容
    std::shared_ptr<CustomStyleNode> findCustomStyle(const std::string& name);
    std::shared_ptr<CustomElementNode> findCustomElement(const std::string& name);
    std::shared_ptr<CustomVarNode> findCustomVar(const std::string& name);
};

} // namespace chtl

#endif // CHTL_HTML_GENERATOR_H