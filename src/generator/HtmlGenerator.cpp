#include "HtmlGenerator.h"
#include "../node/Custom.h"
#include "../node/Operator.h"
#include <regex>
#include <algorithm>

namespace chtl {

HtmlGenerator::HtmlGenerator(const GeneratorConfig& cfg) : Generator(cfg) {}

std::string HtmlGenerator::generate(const NodePtr& ast) {
    // 重置状态
    context = GeneratorContext();
    globalStyles.str("");
    globalScripts.str("");
    styleRules.clear();
    autoClassCounter = 0;
    
    // 第一遍：收集所有样式和脚本
    collectStyles(ast);
    collectScripts(ast);
    
    // 第二遍：生成HTML结构
    generateNode(ast);
    
    // 如果需要样式提升，合并和优化样式
    if (config.hoistStyles) {
        mergeStyleRules();
        optimizeStyles();
    }
    
    return context.output.str();
}

std::string HtmlGenerator::generateCompleteHtml(const NodePtr& ast) {
    std::string html = generate(ast);
    
    // 如果有收集到的样式，插入到<head>中
    if (globalStyles.str().length() > 0 || context.styles.str().length() > 0) {
        std::string styleContent = "<style>\n" + globalStyles.str() + context.styles.str() + "</style>\n";
        
        // 查找</head>标签的位置
        size_t headEnd = html.find("</head>");
        if (headEnd != std::string::npos) {
            html.insert(headEnd, styleContent);
        } else {
            // 如果没有head标签，在开始处插入
            html = styleContent + html;
        }
    }
    
    // 如果有收集到的脚本，插入到<body>末尾
    if (globalScripts.str().length() > 0 || context.scripts.str().length() > 0) {
        std::string scriptContent = "<script>\n" + globalScripts.str() + context.scripts.str() + "</script>\n";
        
        // 查找</body>标签的位置
        size_t bodyEnd = html.find("</body>");
        if (bodyEnd != std::string::npos) {
            html.insert(bodyEnd, scriptContent);
        } else {
            // 如果没有body标签，在末尾插入
            html += scriptContent;
        }
    }
    
    return html;
}

void HtmlGenerator::generateElementNode(const ElementNode* node) {
    // 处理CHTL特有的样式语法
    ElementNode* mutableNode = const_cast<ElementNode*>(node);
    
    // 检查是否有style块需要处理
    StyleNode* styleNode = nullptr;
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::STYLE_NODE) {
            styleNode = static_cast<StyleNode*>(child.get());
            break;
        }
    }
    
    if (styleNode) {
        processStyleSelectors(styleNode, mutableNode);
    }
    
    // 调用基类方法生成HTML
    Generator::generateElementNode(node);
}

void HtmlGenerator::generateStyleNode(const StyleNode* node) {
    // 处理样式节点中的规则
    for (const auto& rule : node->getRules()) {
        // 如果是增强选择器（包含&），需要特殊处理
        if (rule->getSelector().find('&') != std::string::npos) {
            // 这些规则应该已经在processStyleSelectors中处理了
            continue;
        }
        
        generateStyleRule(rule.get());
    }
    
    // 处理样式组引用
    for (const auto& group : node->getStyleGroups()) {
        // 展开样式组
        const std::string& groupName = group->getName();
        if (groupName.substr(0, 7) == "@Style ") {
            std::string styleName = groupName.substr(7);
            expandCustomStyle(styleName, const_cast<StyleNode*>(node));
        }
    }
}

void HtmlGenerator::generateStyleRule(const StyleRuleNode* node) {
    // 收集样式规则以便后续合并
    const std::string& selector = node->getSelector();
    
    if (styleRules.find(selector) == styleRules.end()) {
        styleRules[selector] = std::vector<std::shared_ptr<StylePropertyNode>>();
    }
    
    for (const auto& prop : node->getProperties()) {
        styleRules[selector].push_back(prop);
    }
}

void HtmlGenerator::processStyleSelectors(StyleNode* styleNode, ElementNode* parentElement) {
    // 处理内联样式
    std::vector<std::shared_ptr<StylePropertyNode>> inlineProps;
    
    for (const auto& prop : styleNode->getProperties()) {
        inlineProps.push_back(prop);
    }
    
    // 处理选择器规则
    for (const auto& rule : styleNode->getRules()) {
        const std::string& selector = rule->getSelector();
        
        // 处理类选择器（自动添加类名）
        if (selector[0] == '.') {
            std::string className = selector.substr(1);
            parentElement->addClass(className);
            generateStyleRule(rule.get());
        }
        // 处理ID选择器（自动添加ID）
        else if (selector[0] == '#') {
            std::string id = selector.substr(1);
            parentElement->setId(id);
            generateStyleRule(rule.get());
        }
        // 处理增强选择器（包含&）
        else if (selector.find('&') != std::string::npos) {
            // 根据上下文推导&的含义
            std::string replacement;
            
            // 1. 优先使用已有的类名
            if (!parentElement->getClasses().empty()) {
                replacement = "." + *parentElement->getClasses().begin();
            }
            // 2. 其次使用ID
            else if (!parentElement->getId().empty()) {
                replacement = "#" + parentElement->getId();
            }
            // 3. 如果都没有，创建自动类名
            else {
                std::string autoClass = generateAutoClassName();
                parentElement->addClass(autoClass);
                replacement = "." + autoClass;
            }
            
            // 创建新的规则，替换&
            std::string newSelector = selector;
            size_t pos = 0;
            while ((pos = newSelector.find('&', pos)) != std::string::npos) {
                newSelector.replace(pos, 1, replacement);
                pos += replacement.length();
            }
            
            // 创建新规则并复制属性
            auto newRule = std::make_shared<StyleRuleNode>(newSelector, rule->getLine(), rule->getColumn());
            for (const auto& prop : rule->getProperties()) {
                newRule->addProperty(prop);
            }
            
            generateStyleRule(newRule.get());
        }
        // 其他选择器直接生成
        else {
            generateStyleRule(rule.get());
        }
    }
    
    // 设置内联样式
    if (!inlineProps.empty()) {
        auto inlineStyle = parentElement->getStyle();
        if (!inlineStyle) {
            inlineStyle = std::make_shared<StyleNode>(0, 0);
            parentElement->setStyle(inlineStyle);
        }
        
        for (const auto& prop : inlineProps) {
            inlineStyle->addProperty(prop);
        }
    }
}

void HtmlGenerator::processAmpersandSelector(const std::string& selector, ElementNode* element) {
    // 处理&选择器，根据上下文推导
    std::string replacement;
    
    // 1. 首先检查元素是否已有类名（通过class属性或.xxx选择器设置）
    if (!element->getClasses().empty()) {
        // 使用第一个类名，类名优先
        replacement = "." + *element->getClasses().begin();
    }
    // 2. 其次检查是否有ID
    else if (!element->getId().empty()) {
        replacement = "#" + element->getId();
    }
    // 3. 如果都没有，创建一个自动类名
    else {
        std::string autoClass = generateAutoClassName();
        element->addClass(autoClass);
        replacement = "." + autoClass;
    }
    
    // 替换&为推导出的选择器
    std::string newSelector = selector;
    size_t pos = 0;
    while ((pos = newSelector.find('&', pos)) != std::string::npos) {
        newSelector.replace(pos, 1, replacement);
        pos += replacement.length();
    }
    
    // 更新选择器（这里需要找到对应的规则并更新）
    // 注意：这里简化了实现，实际应该更新原规则的选择器
}

std::string HtmlGenerator::generateAutoClassName() {
    return "chtl-auto-" + std::to_string(++autoClassCounter);
}

void HtmlGenerator::expandCustomStyle(const std::string& styleName, StyleNode* targetStyle) {
    // 查找自定义样式
    auto customStyle = findCustomStyle(styleName);
    if (!customStyle) return;
    
    // 展开样式属性
    for (const auto& prop : customStyle->getProperties()) {
        targetStyle->addProperty(prop);
    }
    
    // 递归展开继承的样式
    for (const auto& inherited : customStyle->getInheritedStyles()) {
        expandCustomStyle(inherited, targetStyle);
    }
}

void HtmlGenerator::expandCustomElement(const std::string& elementName, ElementNode* parentElement) {
    // 查找自定义元素
    auto customElement = findCustomElement(elementName);
    if (!customElement) return;
    
    // 展开元素结构
    for (const auto& child : customElement->getChildren()) {
        parentElement->addChild(child);
    }
}

void HtmlGenerator::expandVarUsage(const std::string& varName, 
                                  const std::map<std::string, std::string>& params) {
    // 查找变量组
    auto customVar = findCustomVar(varName);
    if (!customVar) return;
    
    // 应用变量值，考虑参数覆盖
    for (const auto& var : customVar->getVariables()) {
        std::string value = var.second;
        
        // 检查是否有参数覆盖
        auto paramIt = params.find(var.first);
        if (paramIt != params.end()) {
            value = paramIt->second;
        }
        
        context.varValues[var.first] = value;
    }
}

void HtmlGenerator::mergeStyleRules() {
    // 合并相同选择器的规则
    for (const auto& pair : styleRules) {
        const std::string& selector = pair.first;
        const auto& properties = pair.second;
        
        if (properties.empty()) continue;
        
        globalStyles << selector << " {\n";
        
        // 去重：后面的属性覆盖前面的
        std::map<std::string, std::string> mergedProps;
        for (const auto& prop : properties) {
            mergedProps[prop->getName()] = prop->getValue();
        }
        
        // 输出合并后的属性
        for (const auto& prop : mergedProps) {
            globalStyles << "  " << prop.first << ": " << prop.second << ";\n";
        }
        
        globalStyles << "}\n\n";
    }
}

void HtmlGenerator::optimizeStyles() {
    // TODO: 实现样式优化
    // - 合并相同的规则
    // - 压缩颜色值
    // - 移除无用的规则
    // - 简化选择器
}

void HtmlGenerator::generateCustomStyle(const CustomStyleNode* node) {
    // 自定义样式定义不生成输出
    processedCustomStyles.insert(node->getName());
}

void HtmlGenerator::generateCustomElement(const CustomElementNode* node) {
    // 自定义元素定义不生成输出
    processedCustomElements.insert(node->getName());
}

void HtmlGenerator::generateCustomVar(const CustomVarNode* node) {
    // 自定义变量组定义不生成输出
    processedCustomVars.insert(node->getName());
}

void HtmlGenerator::generateVarUsage(const VarUsageNode* node) {
    // 处理变量使用
    const std::string& varName = node->getVarName();
    const auto& params = node->getParameters();
    
    expandVarUsage(varName, params);
}

std::shared_ptr<CustomStyleNode> HtmlGenerator::findCustomStyle(const std::string& name) {
    // 先从AST中查找
    // TODO: 实现从当前AST查找自定义样式
    
    // 如果启用了预定义内容，从Registry查找
    if (usePredefined) {
        auto& registry = Registry::getInstance();
        return registry.getCustomStyle(name);
    }
    
    return nullptr;
}

std::shared_ptr<CustomElementNode> HtmlGenerator::findCustomElement(const std::string& name) {
    // 先从AST中查找
    // TODO: 实现从当前AST查找自定义元素
    
    // 如果启用了预定义内容，从Registry查找
    if (usePredefined) {
        auto& registry = Registry::getInstance();
        return registry.getCustomElement(name);
    }
    
    return nullptr;
}

std::shared_ptr<CustomVarNode> HtmlGenerator::findCustomVar(const std::string& name) {
    // 先从AST中查找
    // TODO: 实现从当前AST查找自定义变量组
    
    // 如果启用了预定义内容，从Registry查找
    if (usePredefined) {
        auto& registry = Registry::getInstance();
        return registry.getCustomVar(name);
    }
    
    return nullptr;
}

} // namespace chtl