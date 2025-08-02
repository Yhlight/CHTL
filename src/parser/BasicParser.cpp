#include "BasicParser.h"
#include "../lexer/BasicLexer.h"
#include "../lexer/ConfigLexer.h"
#include <sstream>

namespace chtl {

BasicParser::BasicParser() 
    : Parser(), loader(std::make_shared<ChtlLoader>()) {
}

BasicParser::BasicParser(std::shared_ptr<ChtlLoader> fileLoader)
    : Parser(), loader(fileLoader ? fileLoader : std::make_shared<ChtlLoader>()) {
}

NodePtr BasicParser::parse(const std::vector<Token>& tokenList) {
    tokens = tokenList;
    current = 0;
    
    auto root = std::make_shared<RootNode>();
    
    while (!isAtEnd()) {
        try {
            auto stmt = parseStatement();
            if (stmt) {
                root->addChild(stmt);
            }
        } catch (const ParseError& e) {
            // 错误恢复
            synchronize();
        }
    }
    
    return root;
}

NodePtr BasicParser::parseFile(const std::string& filePath) {
    try {
        // 使用loader加载文件
        FileInfo fileInfo = loader->loadFile(filePath, currentFilePath);
        
        // 保存当前文件路径
        std::string previousPath = currentFilePath;
        currentFilePath = fileInfo.path;
        
        // 根据文件扩展名选择合适的处理方式
        std::string ext = loader->getFileExtension(fileInfo.path);
        
        NodePtr result;
        
        if (ext == "chtl") {
            // 解析CHTL文件
            BasicLexer lexer;
            std::vector<Token> tokens = lexer.tokenize(fileInfo.content);
            result = parse(tokens);
        } else if (ext == "html") {
            // HTML文件作为原始内容
            auto node = std::make_shared<OriginHtmlNode>(fileInfo.content);
            result = node;
        } else if (ext == "css") {
            // CSS文件作为样式内容
            auto node = std::make_shared<OriginStyleNode>(fileInfo.content);
            result = node;
        } else if (ext == "js") {
            // JavaScript文件作为脚本内容
            auto node = std::make_shared<OriginJsNode>(fileInfo.content);
            result = node;
        } else {
            throw ParseError("Unsupported file type: " + ext, 0, 0);
        }
        
        // 恢复文件路径
        currentFilePath = previousPath;
        
        return result;
        
    } catch (const LoaderError& e) {
        throw ParseError(std::string("Failed to load file: ") + e.what(), 
                        current < tokens.size() ? tokens[current].line : 0,
                        current < tokens.size() ? tokens[current].column : 0);
    }
}

NodePtr BasicParser::parseStatement() {
    // 跳过注释
    if (match(TokenType::COMMENT)) {
        return parseComment();
    }
    
    // [Import]
    if (match(TokenType::KEYWORD_IMPORT)) {
        return parseImport();
    }
    
    // [Configuration]
    if (match(TokenType::KEYWORD_CONFIGURATION)) {
        return parseConfiguration();
    }
    
    // [Custom]
    if (match(TokenType::KEYWORD_CUSTOM)) {
        return parseCustomDefinition();
    }
    
    // [Template]
    if (match(TokenType::KEYWORD_TEMPLATE)) {
        return parseTemplateDefinition();
    }
    
    // [Namespace]
    if (match(TokenType::KEYWORD_NAMESPACE)) {
        return parseNamespace();
    }
    
    // [Origin]
    if (match(TokenType::KEYWORD_ORIGIN)) {
        return parseOrigin();
    }
    
    // expect / not expect
    if (match(TokenType::KEYWORD_EXPECT)) {
        return parseExpect();
    }
    
    if (match(TokenType::KEYWORD_NOT)) {
        if (match(TokenType::KEYWORD_EXPECT)) {
            return parseExpect();
        }
        error("Expected 'expect' after 'not'");
    }
    
    // text { }
    if (match(TokenType::KEYWORD_TEXT)) {
        return parseTextNode();
    }
    
    // style { }
    if (match(TokenType::KEYWORD_STYLE)) {
        return parseStyleNode();
    }
    
    // add / delete / inherit
    if (match(TokenType::KEYWORD_ADD)) {
        return parseAddOperation();
    }
    
    if (match(TokenType::KEYWORD_DELETE)) {
        return parseDeleteOperation();
    }
    
    if (match(TokenType::KEYWORD_INHERIT)) {
        return parseInheritOperation();
    }
    
    // HTML元素或自定义元素
    if (check(TokenType::IDENTIFIER)) {
        return parseElement();
    }
    
    // 未知语句
    error("Unexpected token");
    advance();
    return nullptr;
}

NodePtr BasicParser::parseImport() {
    Token importToken = previous();
    
    // 解析导入类型
    ImportNode::ImportType importType = parseImportType();
    
    // 创建导入节点
    auto importNode = std::make_shared<ImportNode>(
        importType, "", importToken.line, importToken.column);
    
    // 解析目标名称（如果需要）
    if (importNode->requiresTargetName()) {
        consume(TokenType::LEFT_BRACKET, "Expected '[' before import target name");
        
        if (!check(TokenType::IDENTIFIER)) {
            error("Expected import target name");
        }
        
        importNode->setTargetName(advance().value);
        
        consume(TokenType::RIGHT_BRACKET, "Expected ']' after import target name");
    }
    
    // 解析 from
    consume(TokenType::KEYWORD_FROM, "Expected 'from' in import statement");
    
    // 解析路径
    std::string importPath = parseImportPath();
    importNode->setFromPath(importPath);
    
    // 检查是否是通配符导入
    if (!importPath.empty() && importPath.back() == '*') {
        importNode->setIsWildcard(true);
    }
    
    // 解析 as（可选）
    if (match(TokenType::KEYWORD_AS)) {
        if (!check(TokenType::IDENTIFIER)) {
            error("Expected identifier after 'as'");
        }
        importNode->setAsName(advance().value);
    }
    
    // 使用ChtlLoader处理导入
    processImport(importNode.get());
    
    return importNode;
}

ImportNode::ImportType BasicParser::parseImportType() {
    // [Custom] 或 [Template]
    bool isCustom = false;
    bool isTemplate = false;
    
    if (match(TokenType::KEYWORD_CUSTOM)) {
        isCustom = true;
    } else if (match(TokenType::KEYWORD_TEMPLATE)) {
        isTemplate = true;
    }
    
    // @Html, @Style, @JavaScript, @Element, @Var, @Chtl
    if (match(TokenType::AT_HTML)) {
        if (isCustom || isTemplate) {
            error("Cannot use [Custom] or [Template] with @Html");
        }
        return ImportNode::IMPORT_HTML;
    }
    
    if (match(TokenType::AT_STYLE)) {
        if (isCustom) return ImportNode::IMPORT_CUSTOM_STYLE;
        if (isTemplate) return ImportNode::IMPORT_TEMPLATE_STYLE;
        return ImportNode::IMPORT_STYLE;
    }
    
    if (match(TokenType::AT_JAVASCRIPT)) {
        if (isCustom || isTemplate) {
            error("Cannot use [Custom] or [Template] with @JavaScript");
        }
        return ImportNode::IMPORT_JAVASCRIPT;
    }
    
    if (match(TokenType::AT_ELEMENT)) {
        if (isCustom) return ImportNode::IMPORT_CUSTOM_ELEMENT;
        if (isTemplate) return ImportNode::IMPORT_TEMPLATE_ELEMENT;
        error("@Element must be used with [Custom] or [Template]");
    }
    
    if (match(TokenType::AT_VAR)) {
        if (isCustom) return ImportNode::IMPORT_CUSTOM_VAR;
        if (isTemplate) return ImportNode::IMPORT_TEMPLATE_VAR;
        error("@Var must be used with [Custom] or [Template]");
    }
    
    if (match(TokenType::AT_CHTL)) {
        if (isCustom || isTemplate) {
            error("Cannot use [Custom] or [Template] with @Chtl");
        }
        return ImportNode::IMPORT_CHTL;
    }
    
    // 如果没有指定类型，根据文件扩展名推断
    if (!isCustom && !isTemplate) {
        return ImportNode::IMPORT_CHTL;  // 默认
    }
    
    error("Expected import type (@Html, @Style, etc.)");
    return ImportNode::IMPORT_CHTL;  // 默认返回值
}

std::string BasicParser::parseImportPath() {
    if (check(TokenType::STRING_LITERAL) || check(TokenType::STRING_LITERAL_SINGLE)) {
        return parseStringLiteral();
    }
    
    // 支持无引号的路径
    if (check(TokenType::IDENTIFIER)) {
        return parseUnquotedString();
    }
    
    error("Expected import path");
    return "";  // 默认返回值
}

void BasicParser::processImport(ImportNode* importNode) {
    if (!loader) {
        return;  // 如果没有加载器，跳过处理
    }
    
    std::string importPath = importNode->getFromPath();
    
    // 处理通配符导入
    if (importNode->getIsWildcard()) {
        std::vector<std::string> expandedPaths = 
            loader->resolveWildcardImport(importPath, currentFilePath);
        
        for (const auto& path : expandedPaths) {
            importNode->addExpandedPath(path);
            
            // 添加依赖关系
            if (!currentFilePath.empty()) {
                loader->addImportDependency(currentFilePath, path);
            }
        }
        
        // 设置解析后的路径（对于通配符，使用原始模式）
        importNode->setResolvedPath(importPath);
        
    } else {
        // 解析单个文件路径
        std::string resolvedPath = 
            loader->resolveImportPath(importPath, currentFilePath);
        
        importNode->setResolvedPath(resolvedPath);
        
        // 添加依赖关系
        if (!currentFilePath.empty()) {
            loader->addImportDependency(currentFilePath, resolvedPath);
        }
        
        // 检查循环依赖
        std::vector<std::string> cycle;
        if (loader->hasCircularDependency(cycle)) {
            std::stringstream ss;
            ss << "Circular dependency detected: ";
            for (size_t i = 0; i < cycle.size(); ++i) {
                if (i > 0) ss << " -> ";
                ss << cycle[i];
            }
            throw ParseError(ss.str(), 
                           importNode->getLine(), 
                           importNode->getColumn());
        }
    }
    
    // 标记为已加载（实际加载将在代码生成阶段进行）
    importNode->setIsLoaded(true);
}

// 解析元素节点
NodePtr BasicParser::parseElement() {
    Token nameToken = advance();
    auto element = std::make_shared<ElementNode>(
        nameToken.value, nameToken.line, nameToken.column);
    
    // 解析属性（如果有）
    if (!check(TokenType::LEFT_BRACE) && !isAtEnd()) {
        parseAttributes(element.get());
    }
    
    // 解析元素内容
    if (match(TokenType::LEFT_BRACE)) {
        parseElementContent(element.get());
        consume(TokenType::RIGHT_BRACE, "Expected '}' after element content");
    }
    
    return element;
}

void BasicParser::parseElementContent(ElementNode* element) {
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto child = parseStatement();
        if (child) {
            element->addChild(child);
        }
    }
}

void BasicParser::parseAttributes(ElementNode* element) {
    while (!check(TokenType::LEFT_BRACE) && !isAtEnd()) {
        if (!check(TokenType::IDENTIFIER)) {
            break;
        }
        
        std::string attrName = advance().value;
        std::string attrValue;
        
        if (match(TokenType::EQUALS)) {
            if (check(TokenType::STRING_LITERAL) || check(TokenType::STRING_LITERAL_SINGLE)) {
                attrValue = parseStringLiteral();
            } else if (check(TokenType::IDENTIFIER) || check(TokenType::NUMBER)) {
                attrValue = advance().value;
            } else {
                error("Expected attribute value");
            }
        }
        
        element->setAttribute(attrName, attrValue);
    }
}

// 解析文本节点
NodePtr BasicParser::parseTextNode() {
    Token textToken = previous();
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'text'");
    
    std::string content;
    if (check(TokenType::STRING_LITERAL) || check(TokenType::STRING_LITERAL_SINGLE)) {
        content = parseStringLiteral();
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after text content");
    
    return std::make_shared<TextNode>(content, textToken.line, textToken.column);
}

// 解析样式节点
NodePtr BasicParser::parseStyleNode() {
    Token styleToken = previous();
    
    auto styleNode = std::make_shared<StyleNode>(
        false, styleToken.line, styleToken.column);
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'style'");
    
    parseStyleContent(styleNode.get());
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after style content");
    
    return styleNode;
}

void BasicParser::parseStyleContent(StyleNode* style) {
    // TODO: 实现样式内容解析
    // 这里需要解析CSS规则，选择器等
    // 暂时跳过直到遇到右大括号
    int braceCount = 0;
    while (!isAtEnd()) {
        if (check(TokenType::LEFT_BRACE)) {
            braceCount++;
            advance();
        } else if (check(TokenType::RIGHT_BRACE)) {
            if (braceCount == 0) {
                break;
            }
            braceCount--;
            advance();
        } else {
            advance();
        }
    }
}

// 解析注释
NodePtr BasicParser::parseComment() {
    Token commentToken = previous();
    
    CommentNode::CommentType type;
    if (commentToken.value.length() >= 2 && 
        commentToken.value.substr(0, 2) == "//") {
        type = CommentNode::COMMENT_SINGLE_LINE;
    } else if (commentToken.value.length() >= 2 && 
               commentToken.value.substr(0, 2) == "/*") {
        type = CommentNode::COMMENT_MULTI_LINE;
    } else {
        type = CommentNode::COMMENT_GENERATOR;
    }
    
    // 提取注释内容（去掉注释标记）
    std::string content = commentToken.value;
    if (type == CommentNode::COMMENT_SINGLE_LINE) {
        content = content.substr(2);  // 去掉 //
    } else if (type == CommentNode::COMMENT_MULTI_LINE) {
        content = content.substr(2, content.length() - 4);  // 去掉 /* */
    } else {
        content = content.substr(2);  // 去掉 --
    }
    
    return std::make_shared<CommentNode>(
        type, content, commentToken.line, commentToken.column);
}

// 解析字符串字面量
std::string BasicParser::parseStringLiteral() {
    if (!check(TokenType::STRING_LITERAL) && !check(TokenType::STRING_LITERAL_SINGLE)) {
        error("Expected string literal");
        return "";
    }
    
    std::string value = advance().value;
    
    // 去掉引号
    if (value.length() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        return value.substr(1, value.length() - 2);
    }
    
    return value;
}

// 解析无引号字符串
std::string BasicParser::parseUnquotedString() {
    std::string result;
    
    while (!isAtEnd() && 
           (check(TokenType::IDENTIFIER) || 
            check(TokenType::DOT) || 
            check(TokenType::SLASH) ||
            check(TokenType::ASTERISK))) {
        result += advance().value;
    }
    
    return result;
}

// 临时实现的占位方法
NodePtr BasicParser::parseConfiguration() {
    error("Configuration parsing not yet implemented");
    return nullptr;
}

NodePtr BasicParser::parseCustomDefinition() {
    error("Custom definition parsing not yet implemented");
    return nullptr;
}

NodePtr BasicParser::parseTemplateDefinition() {
    error("Template definition parsing not yet implemented");
    return nullptr;
}

NodePtr BasicParser::parseNamespace() {
    error("Namespace parsing not yet implemented");
    return nullptr;
}

NodePtr BasicParser::parseExpect() {
    error("Expect parsing not yet implemented");
    return nullptr;
}

NodePtr BasicParser::parseOrigin() {
    error("Origin parsing not yet implemented");
    return nullptr;
}

NodePtr BasicParser::parseAddOperation() {
    error("Add operation parsing not yet implemented");
    return nullptr;
}

NodePtr BasicParser::parseDeleteOperation() {
    error("Delete operation parsing not yet implemented");
    return nullptr;
}

NodePtr BasicParser::parseInheritOperation() {
    error("Inherit operation parsing not yet implemented");
    return nullptr;
}

bool BasicParser::isHtmlElement(const std::string& name) {
    return HTML_ELEMENTS.find(name) != HTML_ELEMENTS.end();
}

} // namespace chtl