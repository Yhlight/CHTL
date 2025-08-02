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
            // 将NOT标记传递给parseExpect
            current--; // 退回到NOT token
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

// 解析配置节点
NodePtr BasicParser::parseConfiguration() {
    Token configToken = previous();
    
    auto configNode = std::make_shared<ConfigurationNode>(
        configToken.line, configToken.column);
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after [Configuration]");
    
    // 解析配置项
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (check(TokenType::IDENTIFIER)) {
            std::string key = advance().value;
            
            consume(TokenType::EQUALS, "Expected '=' after configuration key");
            
            std::string value;
            std::vector<std::string> options;
            
            // 检查是否是组选项
            if (match(TokenType::LEFT_BRACKET)) {
                // 解析组选项 [option1, option2]
                while (!check(TokenType::RIGHT_BRACKET) && !isAtEnd()) {
                    if (check(TokenType::IDENTIFIER) || 
                        check(TokenType::STRING_LITERAL) ||
                        check(TokenType::NUMBER)) {
                        options.push_back(advance().value);
                    }
                    
                    if (!match(TokenType::COMMA)) {
                        break;
                    }
                }
                consume(TokenType::RIGHT_BRACKET, "Expected ']' after options");
                
                // 使用第一个选项作为默认值
                if (!options.empty()) {
                    value = options[0];
                }
            } else {
                // 解析单个值
                if (check(TokenType::STRING_LITERAL) || 
                    check(TokenType::STRING_LITERAL_SINGLE)) {
                    value = parseStringLiteral();
                } else if (check(TokenType::NUMBER)) {
                    value = advance().value;
                } else if (check(TokenType::IDENTIFIER)) {
                    value = advance().value;
                } else if (check(TokenType::AT_STYLE) || 
                          check(TokenType::AT_ELEMENT) ||
                          check(TokenType::AT_VAR) ||
                          check(TokenType::AT_HTML) ||
                          check(TokenType::AT_JAVASCRIPT)) {
                    value = advance().value;
                } else if (check(TokenType::KEYWORD_CUSTOM) ||
                          check(TokenType::KEYWORD_TEMPLATE) ||
                          check(TokenType::KEYWORD_ORIGIN) ||
                          check(TokenType::KEYWORD_IMPORT) ||
                          check(TokenType::KEYWORD_NAMESPACE)) {
                    value = advance().value;
                } else if (check(TokenType::KEYWORD_TRUE) || 
                          check(TokenType::KEYWORD_FALSE)) {
                    value = advance().value;
                } else {
                    error("Expected configuration value");
                }
            }
            
            // 设置配置项
            configNode->setConfigItem(key, value);
            
            // 如果有组选项，创建一个ConfigItemNode作为子节点
            if (!options.empty()) {
                auto itemNode = std::make_shared<ConfigItemNode>(
                    key, value, configNode->getLine(), configNode->getColumn());
                for (const auto& opt : options) {
                    itemNode->addOption(opt);
                }
                configNode->addChild(itemNode);
            }
            
            consume(TokenType::SEMICOLON, "Expected ';' after configuration item");
        }
        
        // 检查 [Name] 块
        if (match(TokenType::LEFT_BRACKET)) {
            if (check(TokenType::IDENTIFIER) && peek().value == "Name") {
                advance(); // consume "Name"
                consume(TokenType::RIGHT_BRACKET, "Expected ']' after Name");
                
                configNode->setHasNameBlock(true);
                
                consume(TokenType::LEFT_BRACE, "Expected '{' after [Name]");
                
                // 解析Name块内容
                auto nameBlock = std::make_shared<ConfigNameBlockNode>(
                    current < tokens.size() ? tokens[current].line : 0,
                    current < tokens.size() ? tokens[current].column : 0);
                
                while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                    // 解析Name块中的配置项
                    if (check(TokenType::IDENTIFIER)) {
                        auto itemNode = parseConfigurationItem();
                        if (itemNode) {
                            nameBlock->addChild(itemNode);
                        }
                    }
                }
                
                consume(TokenType::RIGHT_BRACE, "Expected '}' after [Name] block");
                configNode->addChild(nameBlock);
            }
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after configuration items");
    
    return configNode;
}

// 辅助函数：解析配置项
NodePtr BasicParser::parseConfigurationItem() {
    if (!check(TokenType::IDENTIFIER)) {
        return nullptr;
    }
    
    std::string key = advance().value;
    consume(TokenType::EQUALS, "Expected '=' in configuration item");
    
    std::string value;
    if (check(TokenType::STRING_LITERAL) || check(TokenType::STRING_LITERAL_SINGLE)) {
        value = parseStringLiteral();
    } else if (check(TokenType::IDENTIFIER) || check(TokenType::NUMBER)) {
        value = advance().value;
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after configuration item");
    
    return std::make_shared<ConfigItemNode>(key, value, 
        previous().line, previous().column);
}

NodePtr BasicParser::parseCustomDefinition() {
    Token customToken = previous();
    
    // 解析 @Style, @Element, 或 @Var
    if (match(TokenType::AT_STYLE)) {
        return parseCustomStyle();
    } else if (match(TokenType::AT_ELEMENT)) {
        return parseCustomElement();
    } else if (match(TokenType::AT_VAR)) {
        return parseCustomVar();
    } else {
        error("Expected @Style, @Element, or @Var after [Custom]");
        return nullptr;
    }
}

// 解析自定义样式
NodePtr BasicParser::parseCustomStyle() {
    // 获取样式组名称
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected style group name");
        return nullptr;
    }
    
    std::string name = advance().value;
    auto styleNode = std::make_shared<CustomStyleNode>(
        name, previous().line, previous().column);
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after custom style name");
    
    // 解析样式组内容
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // 检查继承
        if (match(TokenType::KEYWORD_INHERIT)) {
            if (match(TokenType::AT_STYLE)) {
                if (check(TokenType::IDENTIFIER)) {
                    styleNode->addInheritedStyle(advance().value);
                }
            }
            consume(TokenType::SEMICOLON, "Expected ';' after inherit");
        }
        // 检查样式规则
        else if (check(TokenType::IDENTIFIER)) {
            // 解析CSS属性
            std::string property = advance().value;
            consume(TokenType::COLON, "Expected ':' after property name");
            
            std::string value;
            // 收集属性值直到分号
            while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
                value += advance().value;
                if (!check(TokenType::SEMICOLON)) {
                    value += " ";
                }
            }
            
            consume(TokenType::SEMICOLON, "Expected ';' after property value");
            
            // 创建属性节点
            auto propNode = std::make_shared<StylePropertyNode>(
                property, value, previous().line, previous().column);
            styleNode->addChild(propNode);
        }
        else {
            advance(); // 跳过未知token
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after custom style content");
    
    return styleNode;
}

// 解析自定义元素
NodePtr BasicParser::parseCustomElement() {
    // 获取元素名称
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected element name");
        return nullptr;
    }
    
    std::string name = advance().value;
    auto elementNode = std::make_shared<CustomElementNode>(
        name, previous().line, previous().column);
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after custom element name");
    
    // 解析元素内容
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // 检查继承
        if (match(TokenType::KEYWORD_INHERIT)) {
            if (match(TokenType::AT_ELEMENT)) {
                if (check(TokenType::IDENTIFIER)) {
                    elementNode->addInheritedElement(advance().value);
                }
            }
            consume(TokenType::SEMICOLON, "Expected ';' after inherit");
        }
        // 检查 @Element 引用
        else if (match(TokenType::AT_ELEMENT)) {
            if (check(TokenType::IDENTIFIER)) {
                std::string refName = advance().value;
                
                // 检查是否有特化操作
                if (match(TokenType::LEFT_BRACE)) {
                    // 解析特化内容
                    auto refElement = std::make_shared<ElementNode>(
                        "@Element", previous().line, previous().column);
                    refElement->setAttribute("ref", refName);
                    
                    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                        auto child = parseStatement();
                        if (child) {
                            refElement->addChild(child);
                        }
                    }
                    
                    consume(TokenType::RIGHT_BRACE, "Expected '}' after element specialization");
                    elementNode->addChild(refElement);
                } else {
                    // 简单引用
                    consume(TokenType::SEMICOLON, "Expected ';' after element reference");
                    elementNode->addInheritedElement(refName);
                }
            }
        }
        // 其他语句
        else {
            auto stmt = parseStatement();
            if (stmt) {
                elementNode->addChild(stmt);
            }
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after custom element content");
    
    return elementNode;
}

// 解析自定义变量组
NodePtr BasicParser::parseCustomVar() {
    // 获取变量组名称
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected variable group name");
        return nullptr;
    }
    
    std::string name = advance().value;
    auto varNode = std::make_shared<CustomVarNode>(
        name, previous().line, previous().column);
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after custom var name");
    
    // 解析变量组内容
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // 检查继承
        if (match(TokenType::KEYWORD_INHERIT)) {
            if (match(TokenType::AT_VAR)) {
                if (check(TokenType::IDENTIFIER)) {
                    varNode->addInheritedVarGroup(advance().value);
                }
            }
            consume(TokenType::SEMICOLON, "Expected ';' after inherit");
        }
        // 检查变量定义
        else if (check(TokenType::IDENTIFIER)) {
            std::string varName = advance().value;
            consume(TokenType::COLON, "Expected ':' after variable name");
            
            std::string varValue;
            if (check(TokenType::STRING_LITERAL) || 
                check(TokenType::STRING_LITERAL_SINGLE)) {
                varValue = parseStringLiteral();
            } else {
                // 收集值直到分号
                while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
                    varValue += advance().value;
                    if (!check(TokenType::SEMICOLON)) {
                        varValue += " ";
                    }
                }
            }
            
            consume(TokenType::SEMICOLON, "Expected ';' after variable value");
            
            varNode->setVariable(varName, varValue);
        }
        else {
            advance(); // 跳过未知token
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after custom var content");
    
    return varNode;
}

NodePtr BasicParser::parseTemplateDefinition() {
    Token templateToken = previous();
    
    // 解析 @Style, @Element, 或 @Var
    if (match(TokenType::AT_STYLE)) {
        return parseTemplateStyle();
    } else if (match(TokenType::AT_ELEMENT)) {
        return parseTemplateElement();
    } else if (match(TokenType::AT_VAR)) {
        return parseTemplateVar();
    } else {
        error("Expected @Style, @Element, or @Var after [Template]");
        return nullptr;
    }
}

// 解析模板样式
NodePtr BasicParser::parseTemplateStyle() {
    // 获取模板名称
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected template style name");
        return nullptr;
    }
    
    std::string name = advance().value;
    auto templateNode = std::make_shared<TemplateStyleNode>(
        name, previous().line, previous().column);
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after template style name");
    
    // 解析模板内容 - 模板主要包含结构而非具体值
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (check(TokenType::IDENTIFIER)) {
            // 解析属性模板
            std::string property = advance().value;
            consume(TokenType::COLON, "Expected ':' after property name");
            
            // 模板值可能是占位符
            std::string value;
            while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
                value += advance().value;
                if (!check(TokenType::SEMICOLON)) {
                    value += " ";
                }
            }
            
            consume(TokenType::SEMICOLON, "Expected ';' after property value");
            
            auto propNode = std::make_shared<StylePropertyNode>(
                property, value, previous().line, previous().column);
            templateNode->addChild(propNode);
        } else {
            advance();
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after template style content");
    
    return templateNode;
}

// 解析模板元素
NodePtr BasicParser::parseTemplateElement() {
    // 获取模板名称
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected template element name");
        return nullptr;
    }
    
    std::string name = advance().value;
    auto templateNode = std::make_shared<TemplateElementNode>(
        name, previous().line, previous().column);
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after template element name");
    
    // 解析模板结构
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto stmt = parseStatement();
        if (stmt) {
            templateNode->addChild(stmt);
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after template element content");
    
    return templateNode;
}

// 解析模板变量
NodePtr BasicParser::parseTemplateVar() {
    // 获取模板名称
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected template var name");
        return nullptr;
    }
    
    std::string name = advance().value;
    auto templateNode = std::make_shared<TemplateVarNode>(
        name, previous().line, previous().column);
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after template var name");
    
    // 解析变量模板
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (check(TokenType::IDENTIFIER)) {
            std::string varName = advance().value;
            consume(TokenType::COLON, "Expected ':' after variable name");
            
            // 跳过值（模板可能只定义结构）
            while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
                advance();
            }
            
            consume(TokenType::SEMICOLON, "Expected ';' after variable");
        } else {
            advance();
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after template var content");
    
    return templateNode;
}

NodePtr BasicParser::parseNamespace() {
    Token namespaceToken = previous();
    
    // 获取命名空间名称
    if (!check(TokenType::IDENTIFIER)) {
        error("Expected namespace name");
        return nullptr;
    }
    
    std::string name = advance().value;
    auto namespaceNode = std::make_shared<NamespaceNode>(
        name, namespaceToken.line, namespaceToken.column);
    
    // 检查是否有嵌套或者内容块
    if (match(TokenType::LEFT_BRACE)) {
        // 解析命名空间内容
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            // 检查嵌套命名空间
            if (match(TokenType::KEYWORD_NAMESPACE)) {
                if (check(TokenType::IDENTIFIER)) {
                    std::string nestedName = advance().value;
                    namespaceNode->addNestedNamespace(nestedName);
                    
                    // 如果嵌套命名空间有内容
                    if (match(TokenType::LEFT_BRACE)) {
                        // 创建嵌套命名空间节点
                        auto nestedNode = std::make_shared<NamespaceNode>(
                            nestedName, previous().line, previous().column);
                        
                        // 解析嵌套内容
                        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                            auto stmt = parseStatement();
                            if (stmt) {
                                nestedNode->addChild(stmt);
                            }
                        }
                        
                        consume(TokenType::RIGHT_BRACE, "Expected '}' after nested namespace");
                        namespaceNode->addChild(nestedNode);
                    }
                }
            }
            // 其他语句
            else {
                auto stmt = parseStatement();
                if (stmt) {
                    namespaceNode->addChild(stmt);
                }
            }
        }
        
        consume(TokenType::RIGHT_BRACE, "Expected '}' after namespace content");
    }
    
    return namespaceNode;
}

NodePtr BasicParser::parseExpect() {
    Token expectToken = previous();
    bool isNot = false;
    
    // 检查是否是 not expect
    if (expectToken.type == TokenType::KEYWORD_NOT) {
        isNot = true;
        // 消费expect
        consume(TokenType::KEYWORD_EXPECT, "Expected 'expect' after 'not'");
        expectToken = previous();
    }
    
    ExpectNode::ExpectType expectType = ExpectNode::EXPECT_PRECISE;
    auto expectNode = std::make_shared<ExpectNode>(
        expectType, isNot, expectToken.line, expectToken.column);
    
    // 解析期盼的内容
    while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
        std::string item;
        
        // 检查是否是类型期盼 [Custom] 或 [Template]
        if (match(TokenType::KEYWORD_CUSTOM) || match(TokenType::KEYWORD_TEMPLATE)) {
            item = previous().value;
            expectType = ExpectNode::EXPECT_TYPE;
            expectNode->setExpectType(expectType);
        }
        // 检查具体的元素或类型
        else if (match(TokenType::AT_ELEMENT) || match(TokenType::AT_STYLE) || 
                 match(TokenType::AT_VAR)) {
            item = previous().value;
            
            // 如果后面跟着标识符，这是精准期盼
            if (check(TokenType::IDENTIFIER)) {
                item += " " + advance().value;
                expectType = ExpectNode::EXPECT_PRECISE;
            }
        }
        // HTML元素名
        else if (check(TokenType::IDENTIFIER)) {
            item = advance().value;
            expectType = ExpectNode::EXPECT_PRECISE;
        }
        // [Custom] @Element XXX 形式
        else if (match(TokenType::LEFT_BRACKET)) {
            if (match(TokenType::KEYWORD_CUSTOM) || match(TokenType::KEYWORD_TEMPLATE)) {
                item = "[" + previous().value + "]";
                consume(TokenType::RIGHT_BRACKET, "Expected ']'");
                
                if (match(TokenType::AT_ELEMENT) || match(TokenType::AT_STYLE) || 
                    match(TokenType::AT_VAR)) {
                    item += " " + previous().value;
                    
                    if (check(TokenType::IDENTIFIER)) {
                        item += " " + advance().value;
                        expectType = ExpectNode::EXPECT_PRECISE;
                    }
                }
            }
        }
        
        if (!item.empty()) {
            expectNode->addExpectedItem(item);
        }
        
        // 检查逗号分隔的多个项
        if (!match(TokenType::COMMA)) {
            break;
        }
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after expect statement");
    
    // 如果是否定期盼，设置类型
    if (isNot) {
        expectNode->setExpectType(ExpectNode::EXPECT_NEGATIVE);
    }
    
    return expectNode;
}

NodePtr BasicParser::parseOrigin() {
    Token originToken = previous();
    
    // 解析 @Html, @Style, 或 @JavaScript
    if (match(TokenType::AT_HTML)) {
        return parseOriginHtml();
    } else if (match(TokenType::AT_STYLE)) {
        return parseOriginStyle();
    } else if (match(TokenType::AT_JAVASCRIPT)) {
        return parseOriginJavaScript();
    } else {
        error("Expected @Html, @Style, or @JavaScript after [Origin]");
        return nullptr;
    }
}

// 解析原始HTML嵌入
NodePtr BasicParser::parseOriginHtml() {
    Token htmlToken = previous();
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after [Origin] @Html");
    
    // 收集原始内容直到匹配的右大括号
    // 这里需要跳过Token解析，直接收集原始文本
    std::string content;
    int braceDepth = 1;
    
    // 暂时简化实现，收集所有token的值
    while (!isAtEnd() && braceDepth > 0) {
        if (check(TokenType::LEFT_BRACE)) {
            braceDepth++;
            advance();
            content += "{";
        } else if (check(TokenType::RIGHT_BRACE)) {
            braceDepth--;
            if (braceDepth > 0) {
                advance();
                content += "}";
            }
        } else {
            // 收集任何其他内容
            Token t = advance();
            content += t.value;
            
            // 保留空格
            if (t.type == TokenType::IDENTIFIER || 
                t.type == TokenType::NUMBER ||
                t.type == TokenType::STRING_LITERAL) {
                content += " ";
            }
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after origin HTML content");
    
    // 去除首尾空白
    size_t start = content.find_first_not_of(" \t\n\r");
    size_t end = content.find_last_not_of(" \t\n\r");
    if (start != std::string::npos && end != std::string::npos) {
        content = content.substr(start, end - start + 1);
    }
    
    return std::make_shared<OriginHtmlNode>(
        content, htmlToken.line, htmlToken.column);
}

// 解析原始Style嵌入
NodePtr BasicParser::parseOriginStyle() {
    Token styleToken = previous();
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after [Origin] @Style");
    
    // 收集原始CSS内容
    std::string content;
    int braceDepth = 1;
    
    while (!isAtEnd() && braceDepth > 0) {
        if (check(TokenType::LEFT_BRACE)) {
            braceDepth++;
            advance();
            content += "{";
        } else if (check(TokenType::RIGHT_BRACE)) {
            braceDepth--;
            if (braceDepth > 0) {
                advance();
                content += "}";
            }
        } else {
            Token t = advance();
            content += t.value;
            
            // 保留必要的空格
            if (t.type != TokenType::SEMICOLON && 
                t.type != TokenType::COLON &&
                t.type != TokenType::LEFT_BRACE &&
                t.type != TokenType::RIGHT_BRACE &&
                !isAtEnd() && 
                peek().type != TokenType::SEMICOLON &&
                peek().type != TokenType::COLON &&
                peek().type != TokenType::RIGHT_BRACE) {
                content += " ";
            }
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after origin style content");
    
    // 去除首尾空白
    size_t start = content.find_first_not_of(" \t\n\r");
    size_t end = content.find_last_not_of(" \t\n\r");
    if (start != std::string::npos && end != std::string::npos) {
        content = content.substr(start, end - start + 1);
    }
    
    return std::make_shared<OriginStyleNode>(
        content, styleToken.line, styleToken.column);
}

// 解析原始JavaScript嵌入
NodePtr BasicParser::parseOriginJavaScript() {
    Token jsToken = previous();
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after [Origin] @JavaScript");
    
    // 收集原始JavaScript内容
    std::string content;
    int braceDepth = 1;
    
    while (!isAtEnd() && braceDepth > 0) {
        if (check(TokenType::LEFT_BRACE)) {
            braceDepth++;
            advance();
            content += "{";
        } else if (check(TokenType::RIGHT_BRACE)) {
            braceDepth--;
            if (braceDepth > 0) {
                advance();
                content += "}";
            }
        } else {
            Token t = advance();
            content += t.value;
            
            // 保留必要的空格
            if (t.type != TokenType::SEMICOLON && 
                t.type != TokenType::LEFT_PAREN &&
                t.type != TokenType::RIGHT_PAREN &&
                t.type != TokenType::DOT &&
                !isAtEnd()) {
                Token next = peek();
                if (next.type != TokenType::SEMICOLON &&
                    next.type != TokenType::LEFT_PAREN &&
                    next.type != TokenType::RIGHT_PAREN &&
                    next.type != TokenType::DOT &&
                    next.type != TokenType::RIGHT_BRACE) {
                    content += " ";
                }
            }
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after origin JavaScript content");
    
    // 去除首尾空白
    size_t start = content.find_first_not_of(" \t\n\r");
    size_t end = content.find_last_not_of(" \t\n\r");
    if (start != std::string::npos && end != std::string::npos) {
        content = content.substr(start, end - start + 1);
    }
    
    return std::make_shared<OriginJsNode>(
        content, jsToken.line, jsToken.column);
}

NodePtr BasicParser::parseAddOperation() {
    Token addToken = previous();
    
    // 解析要添加的目标
    std::string target;
    
    // 可能是 @Style, @Element, @Var
    if (match(TokenType::AT_STYLE) || match(TokenType::AT_ELEMENT) || 
        match(TokenType::AT_VAR)) {
        target = previous().value + " ";
        
        if (check(TokenType::IDENTIFIER)) {
            target += advance().value;
        }
    } else if (check(TokenType::IDENTIFIER)) {
        target = advance().value;
    } else {
        error("Expected target for add operation");
        return nullptr;
    }
    
    auto addNode = std::make_shared<AddNode>(
        target, addToken.line, addToken.column);
    
    // 检查是否有内容块
    if (match(TokenType::LEFT_BRACE)) {
        // 解析添加的内容
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            auto stmt = parseStatement();
            if (stmt) {
                addNode->addChild(stmt);
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' after add content");
    } else {
        consume(TokenType::SEMICOLON, "Expected ';' after add operation");
    }
    
    return addNode;
}

NodePtr BasicParser::parseDeleteOperation() {
    Token deleteToken = previous();
    
    // 解析要删除的目标
    std::string target;
    
    // 可能是 @Style, @Element, @Var
    if (match(TokenType::AT_STYLE) || match(TokenType::AT_ELEMENT) || 
        match(TokenType::AT_VAR)) {
        target = previous().value + " ";
        
        if (check(TokenType::IDENTIFIER)) {
            target += advance().value;
        }
    } else if (check(TokenType::IDENTIFIER)) {
        target = advance().value;
    } else {
        error("Expected target for delete operation");
        return nullptr;
    }
    
    auto deleteNode = std::make_shared<DeleteNode>(
        target, deleteToken.line, deleteToken.column);
    
    consume(TokenType::SEMICOLON, "Expected ';' after delete operation");
    
    return deleteNode;
}

NodePtr BasicParser::parseInheritOperation() {
    Token inheritToken = previous();
    
    // 解析要继承的目标
    std::string target;
    
    // 可能是 @Style, @Element, @Var
    if (match(TokenType::AT_STYLE) || match(TokenType::AT_ELEMENT) || 
        match(TokenType::AT_VAR)) {
        target = previous().value + " ";
        
        if (check(TokenType::IDENTIFIER)) {
            target += advance().value;
        }
    } else if (check(TokenType::IDENTIFIER)) {
        target = advance().value;
    } else {
        error("Expected target for inherit operation");
        return nullptr;
    }
    
    auto inheritNode = std::make_shared<InheritNode>(
        target, inheritToken.line, inheritToken.column);
    
    consume(TokenType::SEMICOLON, "Expected ';' after inherit operation");
    
    return inheritNode;
}

bool BasicParser::isHtmlElement(const std::string& name) {
    return HTML_ELEMENTS.find(name) != HTML_ELEMENTS.end();
}

} // namespace chtl