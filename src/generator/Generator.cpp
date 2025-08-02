#include "Generator.h"
#include "../node/Comment.h"
#include "../node/Style.h"
#include "../node/Custom.h"
#include "../node/Template.h"
#include "../node/Origin.h"
#include "../node/Import.h"
#include "../node/Namespace.h"
#include "../node/Config.h"
#include <algorithm>
#include <cctype>

namespace chtl {

// GeneratorContext实现
std::string GeneratorContext::getIndent() const {
    // 根据配置生成缩进字符串
    std::string indent;
    for (int i = 0; i < indentLevel; ++i) {
        for (int j = 0; j < 2; ++j) { // 默认2空格
            indent += ' ';
        }
    }
    return indent;
}

// Generator实现
Generator::Generator(const GeneratorConfig& cfg) : config(cfg) {}

void Generator::write(const std::string& text) {
    context.output << text;
}

void Generator::writeLine(const std::string& text) {
    if (!config.minify) {
        writeIndent();
    }
    write(text);
    if (!config.minify) {
        write("\n");
    }
}

void Generator::writeIndent() {
    if (!config.minify && config.prettyPrint) {
        write(context.getIndent());
    }
}

std::string Generator::escapeHtml(const std::string& text) {
    std::string result;
    for (char c : text) {
        switch (c) {
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '&': result += "&amp;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string Generator::escapeAttribute(const std::string& text) {
    return escapeHtml(text);
}

std::string Generator::generate(const NodePtr& ast) {
    // 重置上下文
    context = GeneratorContext();
    
    // 如果需要提升样式，先收集
    if (config.hoistStyles) {
        collectStyles(ast);
    }
    
    // 生成主要内容
    generateNode(ast);
    
    return context.output.str();
}

void Generator::generateNode(const NodePtr& node) {
    if (!node) return;
    
    // 根据节点类型分发到相应的生成方法
    switch (node->getType()) {
        case NodeType::ROOT_NODE:
            generateRootNode(static_cast<RootNode*>(node.get()));
            break;
        case NodeType::TEXT_NODE:
            generateTextNode(static_cast<TextNode*>(node.get()));
            break;
        case NodeType::ELEMENT_NODE:
            generateElementNode(static_cast<ElementNode*>(node.get()));
            break;
        case NodeType::COMMENT_NODE:
            generateCommentNode(static_cast<CommentNode*>(node.get()));
            break;
        case NodeType::STYLE_NODE:
            generateStyleNode(static_cast<StyleNode*>(node.get()));
            break;
        case NodeType::CUSTOM_STYLE:
            generateCustomStyle(static_cast<CustomStyleNode*>(node.get()));
            break;
        case NodeType::CUSTOM_ELEMENT:
            generateCustomElement(static_cast<CustomElementNode*>(node.get()));
            break;
        case NodeType::CUSTOM_VAR:
            generateCustomVar(static_cast<CustomVarNode*>(node.get()));
            break;
        case NodeType::TEMPLATE_STYLE:
            generateTemplateStyle(static_cast<TemplateStyleNode*>(node.get()));
            break;
        case NodeType::TEMPLATE_ELEMENT:
            generateTemplateElement(static_cast<TemplateElementNode*>(node.get()));
            break;
        case NodeType::TEMPLATE_VAR:
            generateTemplateVar(static_cast<TemplateVarNode*>(node.get()));
            break;
        case NodeType::ORIGIN_HTML:
            generateOriginHtml(static_cast<OriginHtmlNode*>(node.get()));
            break;
        case NodeType::ORIGIN_STYLE:
            generateOriginStyle(static_cast<OriginStyleNode*>(node.get()));
            break;
        case NodeType::ORIGIN_JS:
            generateOriginJs(static_cast<OriginJsNode*>(node.get()));
            break;
        case NodeType::IMPORT_NODE:
            generateImport(static_cast<ImportNode*>(node.get()));
            break;
        case NodeType::NAMESPACE_NODE:
            generateNamespace(static_cast<NamespaceNode*>(node.get()));
            break;
        case NodeType::CONFIGURATION_NODE:
            generateConfiguration(static_cast<ConfigurationNode*>(node.get()));
            break;
        default:
            // 处理其他子节点
            for (const auto& child : node->getChildren()) {
                generateNode(child);
            }
            break;
    }
}

void Generator::generateRootNode(const RootNode* node) {
    // 添加文档类型
    if (!config.doctype.empty()) {
        writeLine(config.doctype);
    }
    
    // 生成所有子节点
    for (const auto& child : node->getChildren()) {
        generateNode(child);
    }
}

void Generator::generateTextNode(const TextNode* node) {
    // 如果在压缩模式，去除多余空白
    std::string content = node->getContent();
    if (config.minify) {
        // 简单的空白压缩
        content.erase(std::unique(content.begin(), content.end(), 
            [](char a, char b) { return std::isspace(a) && std::isspace(b); }), 
            content.end());
    }
    
    write(escapeHtml(content));
}

void Generator::generateElementNode(const ElementNode* node) {
    const std::string& tagName = node->getTagName();
    
    // 自闭合标签
    static const std::unordered_set<std::string> voidElements = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    bool isVoid = voidElements.find(tagName) != voidElements.end();
    
    // 开始标签
    writeIndent();
    write("<" + tagName);
    
    // 生成属性
    for (const auto& attr : node->getAttributes()) {
        generateAttributeNode(attr.get());
    }
    
    // 处理类名和ID
    if (!node->getClasses().empty()) {
        write(" class=\"");
        bool first = true;
        for (const auto& cls : node->getClasses()) {
            if (!first) write(" ");
            write(escapeAttribute(cls));
            context.usedClasses.insert(cls);
            first = false;
        }
        write("\"");
    }
    
    if (!node->getId().empty()) {
        write(" id=\"" + escapeAttribute(node->getId()) + "\"");
        context.usedIds.insert(node->getId());
    }
    
    // 处理内联样式
    auto styleNode = node->getStyle();
    if (styleNode && !styleNode->getProperties().empty()) {
        write(" style=\"");
        bool first = true;
        for (const auto& prop : styleNode->getProperties()) {
            if (!first) write(" ");
            write(prop->getName() + ": " + prop->getValue() + ";");
            first = false;
        }
        write("\"");
    }
    
    if (isVoid) {
        write(" />");
        if (!config.minify) write("\n");
    } else {
        write(">");
        
        // 检查是否需要换行
        bool hasBlockContent = false;
        for (const auto& child : node->getChildren()) {
            if (child->getType() != NodeType::TEXT_NODE) {
                hasBlockContent = true;
                break;
            }
        }
        
        if (hasBlockContent && !config.minify) {
            write("\n");
            context.increaseIndent();
        }
        
        // 特殊处理head和script标签
        if (tagName == "head") {
            context.inHead = true;
        } else if (tagName == "script") {
            context.inScript = true;
        }
        
        // 生成子节点
        for (const auto& child : node->getChildren()) {
            generateNode(child);
        }
        
        // 重置标志
        if (tagName == "head") {
            context.inHead = false;
        } else if (tagName == "script") {
            context.inScript = false;
        }
        
        if (hasBlockContent && !config.minify) {
            context.decreaseIndent();
            writeIndent();
        }
        
        // 结束标签
        write("</" + tagName + ">");
        if (!config.minify) write("\n");
    }
}

void Generator::generateCommentNode(const CommentNode* node) {
    if (!config.generateComments) return;
    
    const std::string& content = node->getContent();
    
    // 检查是否是生成器注释（--开头）
    if (content.length() >= 2 && content.substr(0, 2) == "--") {
        writeIndent();
        write("<!--" + content.substr(2) + "-->");
        if (!config.minify) write("\n");
    }
    // 普通注释在生成时被忽略
}

void Generator::generateStyleNode(const StyleNode* node) {
    // 内联样式已在ElementNode中处理
    // 这里处理style块中的规则
    
    if (!node->getRules().empty() || !node->getStyleGroups().empty()) {
        if (context.inHead || config.hoistStyles) {
            // 收集到全局样式中
            for (const auto& rule : node->getRules()) {
                generateStyleRule(rule.get());
            }
            for (const auto& group : node->getStyleGroups()) {
                generateStyleGroup(group.get());
            }
        }
    }
}

void Generator::generateAttributeNode(const AttributeNode* node) {
    write(" " + node->getName());
    if (!node->getValue().empty()) {
        write("=\"" + escapeAttribute(node->getValue()) + "\"");
    }
}

void Generator::generateStyleProperty(const StylePropertyNode* node) {
    write(node->getName() + ": " + node->getValue() + ";");
}

void Generator::generateStyleRule(const StyleRuleNode* node) {
    context.styles << node->getSelector() << " {\n";
    for (const auto& prop : node->getProperties()) {
        context.styles << "  ";
        generateStyleProperty(prop.get());
        context.styles << "\n";
    }
    context.styles << "}\n";
}

void Generator::generateStyleGroup(const StyleGroupNode* node) {
    // 样式组的展开由解析器处理，这里只需要生成展开后的属性
    for (const auto& prop : node->getProperties()) {
        generateStyleProperty(prop.get());
    }
}

void Generator::generateCustomStyle(const CustomStyleNode* node) {
    // 自定义样式不直接生成，由使用它的地方展开
}

void Generator::generateCustomElement(const CustomElementNode* node) {
    // 自定义元素不直接生成，由使用它的地方展开
}

void Generator::generateCustomVar(const CustomVarNode* node) {
    // 自定义变量组不直接生成，由使用它的地方展开
}

void Generator::generateTemplateStyle(const TemplateStyleNode* node) {
    // 模板样式不直接生成
}

void Generator::generateTemplateElement(const TemplateElementNode* node) {
    // 模板元素不直接生成
}

void Generator::generateTemplateVar(const TemplateVarNode* node) {
    // 模板变量不直接生成
}

void Generator::generateOriginHtml(const OriginHtmlNode* node) {
    // 直接输出原始HTML内容
    write(node->getContent());
}

void Generator::generateOriginStyle(const OriginStyleNode* node) {
    // 输出原始样式内容到样式收集器
    context.styles << node->getContent() << "\n";
}

void Generator::generateOriginJs(const OriginJsNode* node) {
    // 输出原始脚本内容到脚本收集器
    context.scripts << node->getContent() << "\n";
}

void Generator::generateImport(const ImportNode* node) {
    // Import节点在解析阶段已处理，生成时不输出
}

void Generator::generateNamespace(const NamespaceNode* node) {
    // Namespace节点在解析阶段已处理，生成时不输出
}

void Generator::generateConfiguration(const ConfigurationNode* node) {
    // Configuration节点在解析阶段已处理，生成时不输出
}

void Generator::collectStyles(const NodePtr& node) {
    // 递归收集所有样式节点
    if (node->getType() == NodeType::STYLE_NODE) {
        generateStyleNode(static_cast<StyleNode*>(node.get()));
    }
    
    for (const auto& child : node->getChildren()) {
        collectStyles(child);
    }
}

void Generator::collectScripts(const NodePtr& node) {
    // 递归收集所有脚本节点
    if (node->getType() == NodeType::ORIGIN_JS) {
        generateOriginJs(static_cast<OriginJsNode*>(node.get()));
    }
    
    for (const auto& child : node->getChildren()) {
        collectScripts(child);
    }
}

} // namespace chtl