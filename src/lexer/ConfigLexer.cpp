#include "ConfigLexer.h"
#include "../node/Config.h"
#include <algorithm>
#include <stdexcept>

namespace chtl {

ConfigLexer::ConfigLexer() : BasicLexer() {
    initializeCustomKeywords();
}

void ConfigLexer::initializeCustomKeywords() {
    // 初始化默认的自定义关键字映射
    // 这些会被配置覆盖
    customKeywordMap["@Style"] = TokenType::AT_STYLE;
    customKeywordMap["@Element"] = TokenType::AT_ELEMENT;
    customKeywordMap["@Var"] = TokenType::AT_VAR;
    customKeywordMap["@Html"] = TokenType::AT_HTML;
    customKeywordMap["@JavaScript"] = TokenType::AT_JAVASCRIPT;
    customKeywordMap["@Chtl"] = TokenType::AT_CHTL;
    
    customKeywordMap["add"] = TokenType::KEYWORD_ADD;
    customKeywordMap["delete"] = TokenType::KEYWORD_DELETE;
    customKeywordMap["from"] = TokenType::KEYWORD_FROM;
    customKeywordMap["as"] = TokenType::KEYWORD_AS;
    customKeywordMap["inherit"] = TokenType::KEYWORD_INHERIT;
    customKeywordMap["text"] = TokenType::KEYWORD_TEXT;
    customKeywordMap["style"] = TokenType::KEYWORD_STYLE;
    customKeywordMap["expect"] = TokenType::KEYWORD_EXPECT;
    customKeywordMap["not"] = TokenType::KEYWORD_NOT;
    
    customKeywordMap["[Custom]"] = TokenType::KEYWORD_CUSTOM;
    customKeywordMap["[Template]"] = TokenType::KEYWORD_TEMPLATE;
    customKeywordMap["[Origin]"] = TokenType::KEYWORD_ORIGIN;
    customKeywordMap["[Import]"] = TokenType::KEYWORD_IMPORT;
    customKeywordMap["[Namespace]"] = TokenType::KEYWORD_NAMESPACE;
    customKeywordMap["[Configuration]"] = TokenType::KEYWORD_CONFIGURATION;
}

void ConfigLexer::setConfiguration(const Configuration& cfg) {
    config = cfg;
    updateKeywordMappings();
}

void ConfigLexer::updateKeywordMappings() {
    // 清除现有的自定义关键字
    customKeywordMap.clear();
    allPossibleKeywords.clear();
    
    // 基于配置更新关键字映射
    auto updateMapping = [this](const std::string& configKey, TokenType tokenType) {
        std::string value = config.getValue(configKey);
        if (!value.empty()) {
            customKeywordMap[value] = tokenType;
            allPossibleKeywords.insert(value);
        }
        
        // 处理组选项
        if (!config.disableNameGroup && config.hasNameBlock) {
            auto options = config.getOptions(configKey);
            for (const auto& opt : options) {
                customKeywordMap[opt] = tokenType;
                allPossibleKeywords.insert(opt);
            }
        }
    };
    
    // 更新所有配置驱动的关键字
    updateMapping("CUSTOM_STYLE", TokenType::AT_STYLE);
    updateMapping("CUSTOM_ELEMENT", TokenType::AT_ELEMENT);
    updateMapping("CUSTOM_VAR", TokenType::AT_VAR);
    updateMapping("TEMPLATE_STYLE", TokenType::AT_STYLE);
    updateMapping("TEMPLATE_ELEMENT", TokenType::AT_ELEMENT);
    updateMapping("TEMPLATE_VAR", TokenType::AT_VAR);
    updateMapping("ORIGIN_HTML", TokenType::AT_HTML);
    updateMapping("ORIGIN_STYLE", TokenType::AT_STYLE);
    updateMapping("ORIGIN_JAVASCRIPT", TokenType::AT_JAVASCRIPT);
    
    updateMapping("KEYWORD_ADD", TokenType::KEYWORD_ADD);
    updateMapping("KEYWORD_DELETE", TokenType::KEYWORD_DELETE);
    updateMapping("KEYWORD_INHERIT", TokenType::KEYWORD_INHERIT);
    updateMapping("KEYWORD_FROM", TokenType::KEYWORD_FROM);
    updateMapping("KEYWORD_AS", TokenType::KEYWORD_AS);
    updateMapping("KEYWORD_TEXT", TokenType::KEYWORD_TEXT);
    updateMapping("KEYWORD_STYLE", TokenType::KEYWORD_STYLE);
    updateMapping("KEYWORD_CUSTOM", TokenType::KEYWORD_CUSTOM);
    updateMapping("KEYWORD_TEMPLATE", TokenType::KEYWORD_TEMPLATE);
    updateMapping("KEYWORD_ORIGIN", TokenType::KEYWORD_ORIGIN);
    updateMapping("KEYWORD_IMPORT", TokenType::KEYWORD_IMPORT);
    updateMapping("KEYWORD_NAMESPACE", TokenType::KEYWORD_NAMESPACE);
    
    // 如果没有配置，使用默认值
    if (customKeywordMap.empty()) {
        initializeCustomKeywords();
    }
}

bool ConfigLexer::isCustomKeyword(const std::string& str) const {
    return customKeywordMap.find(str) != customKeywordMap.end();
}

TokenType ConfigLexer::getCustomKeywordType(const std::string& str) const {
    auto it = customKeywordMap.find(str);
    if (it != customKeywordMap.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

std::vector<Token> ConfigLexer::tokenize(const std::string& inputStr) {
    // 首先检查是否有配置块
    size_t configPos = inputStr.find("[Configuration]");
    if (configPos != std::string::npos) {
        // 找到配置块的结束位置
        size_t configStart = inputStr.find("{", configPos);
        if (configStart != std::string::npos) {
            int braceCount = 1;
            size_t configEnd = configStart + 1;
            
            while (configEnd < inputStr.length() && braceCount > 0) {
                if (inputStr[configEnd] == '{') {
                    braceCount++;
                } else if (inputStr[configEnd] == '}') {
                    braceCount--;
                }
                configEnd++;
            }
            
            if (braceCount == 0) {
                // 提取配置块内容
                std::string configBlock = inputStr.substr(configPos, configEnd - configPos);
                
                // 解析配置
                ConfigurationParser parser;
                Configuration parsedConfig = parser.parseConfiguration(configBlock);
                setConfiguration(parsedConfig);
            }
        }
    }
    
    // 使用BasicLexer的tokenize，但会调用我们重写的方法
    return BasicLexer::tokenize(inputStr);
}

void ConfigLexer::handleIdentifierState() {
    scanIdentifier();
}

void ConfigLexer::scanIdentifier() {
    std::string value;
    int startColumn = column;
    
    // 首先尝试匹配最长的可能关键字
    std::string longestMatch;
    TokenType longestMatchType = TokenType::IDENTIFIER;
    
    // 读取标识符字符
    while (!isAtEnd() && isIdentifierChar(peek())) {
        value += peek();
        
        // 检查当前值是否是自定义关键字
        if (isCustomKeyword(value)) {
            longestMatch = value;
            longestMatchType = getCustomKeywordType(value);
        }
        
        advance();
    }
    
    // 如果找到了匹配的自定义关键字
    if (!longestMatch.empty()) {
        // 回退到最长匹配之后
        for (size_t i = longestMatch.length(); i < value.length(); i++) {
            position--;
            column--;
        }
        addToken(longestMatchType, longestMatch);
    } else {
        // 检查是否是标准关键字
        TokenType type = getKeywordType(value);
        if (type == TokenType::IDENTIFIER) {
            // 再检查是否是自定义关键字
            type = getCustomKeywordType(value);
        }
        
        // 特殊处理：如果是"style"关键字，设置上下文
        if (type == TokenType::KEYWORD_STYLE || 
            (isCustomKeyword(value) && getCustomKeywordType(value) == TokenType::KEYWORD_STYLE)) {
            context.inStyleBlock = true;
        }
        
        addToken(type, value);
    }
    
    state = LexerState::NORMAL;
}

void ConfigLexer::buildConfigFromNode(const ConfigurationNode* node) {
    if (!node) return;
    
    Configuration newConfig;
    
    // 从节点中提取配置项
    for (const auto& item : node->getConfigItems()) {
        ConfigItem configItem;
        configItem.key = item.first;
        configItem.value = item.second;
        newConfig.items[item.first] = configItem;
    }
    
    // 设置特殊配置
    newConfig.hasNameBlock = node->getHasNameBlock();
    
    auto disableValue = node->getConfigItem("DISABLE_NAME_GROUP");
    newConfig.disableNameGroup = (disableValue == "true");
    
    auto optionCountValue = node->getConfigItem("OPTION_COUNT");
    if (!optionCountValue.empty()) {
        try {
            newConfig.optionCount = std::stoi(optionCountValue);
        } catch (...) {
            newConfig.optionCount = 1;
        }
    }
    
    setConfiguration(newConfig);
}

// ConfigurationParser 实现

ConfigurationParser::ConfigurationParser() : current(0) {
}

Token ConfigurationParser::peek(size_t offset) const {
    size_t pos = current + offset;
    if (pos >= tokens.size()) {
        return Token(TokenType::END_OF_FILE, "", 0, 0);
    }
    return tokens[pos];
}

Token ConfigurationParser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return tokens[current - 1];
}

bool ConfigurationParser::isAtEnd() const {
    return current >= tokens.size() || peek().type == TokenType::END_OF_FILE;
}

bool ConfigurationParser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool ConfigurationParser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token ConfigurationParser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    throw std::runtime_error(message + " at line " + std::to_string(peek().line));
}

Configuration ConfigurationParser::parseConfiguration(const std::string& configBlock) {
    // 使用BasicLexer来tokenize配置块
    BasicLexer lexer;
    tokens = lexer.tokenize(configBlock);
    current = 0;
    
    Configuration config;
    
    // 跳过 [Configuration]
    if (match(TokenType::KEYWORD_CONFIGURATION)) {
        consume(TokenType::LEFT_BRACE, "Expected '{' after [Configuration]");
        
        // 解析配置项
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (match(TokenType::COMMENT_SINGLE) || 
                match(TokenType::COMMENT_MULTI)) {
                // 跳过注释
                continue;
            }
            
            if (check(TokenType::LEFT_BRACKET)) {
                // 可能是 [Name] 块
                advance();
                if (check(TokenType::IDENTIFIER) && peek().value == "Name") {
                    advance();
                    consume(TokenType::RIGHT_BRACKET, "Expected ']' after Name");
                    config.hasNameBlock = true;
                    
                    consume(TokenType::LEFT_BRACE, "Expected '{' after [Name]");
                    
                    // 解析Name块中的配置项
                    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                        if (match(TokenType::COMMENT_SINGLE) || 
                            match(TokenType::COMMENT_MULTI)) {
                            continue;
                        }
                        
                        ConfigItem item = parseConfigItem();
                        config.items[item.key] = item;
                    }
                    
                    consume(TokenType::RIGHT_BRACE, "Expected '}' to close [Name] block");
                }
            } else {
                // 普通配置项
                ConfigItem item = parseConfigItem();
                config.items[item.key] = item;
            }
        }
        
        consume(TokenType::RIGHT_BRACE, "Expected '}' to close [Configuration]");
    }
    
    // 处理特殊配置项
    auto disableIt = config.items.find("DISABLE_NAME_GROUP");
    if (disableIt != config.items.end()) {
        config.disableNameGroup = (disableIt->second.value == "true");
    }
    
    auto optionIt = config.items.find("OPTION_COUNT");
    if (optionIt != config.items.end()) {
        try {
            config.optionCount = std::stoi(optionIt->second.value);
        } catch (...) {
            config.optionCount = 1;
        }
    }
    
    return config;
}

ConfigItem ConfigurationParser::parseConfigItem() {
    ConfigItem item;
    
    // 获取配置键
    Token key = consume(TokenType::IDENTIFIER, "Expected configuration key");
    item.key = key.value;
    
    // 期待 = 或 :
    if (!match(TokenType::EQUALS) && !match(TokenType::COLON)) {
        throw std::runtime_error("Expected '=' or ':' after configuration key");
    }
    
    // 检查是否是组选项
    if (check(TokenType::LEFT_BRACKET)) {
        item.options = parseOptions();
    } else {
        // 获取配置值
        if (check(TokenType::STRING_LITERAL) || 
            check(TokenType::STRING_LITERAL_SINGLE)) {
            item.value = advance().value;
        } else if (check(TokenType::NUMBER)) {
            item.value = advance().value;
        } else if (check(TokenType::IDENTIFIER)) {
            item.value = advance().value;
        } else if (check(TokenType::AT_STYLE) || 
                   check(TokenType::AT_ELEMENT) ||
                   check(TokenType::AT_VAR) ||
                   check(TokenType::AT_HTML) ||
                   check(TokenType::AT_JAVASCRIPT) ||
                   check(TokenType::AT_CHTL)) {
            // 处理 @前缀的关键字
            item.value = advance().value;
        } else if (check(TokenType::KEYWORD_TEXT) ||
                   check(TokenType::KEYWORD_STYLE) ||
                   check(TokenType::KEYWORD_ADD) ||
                   check(TokenType::KEYWORD_DELETE) ||
                   check(TokenType::KEYWORD_FROM) ||
                   check(TokenType::KEYWORD_AS) ||
                   check(TokenType::KEYWORD_INHERIT)) {
            // 处理普通关键字作为值
            item.value = advance().value;
        } else if (check(TokenType::AT)) {
            // 处理 @Style 这样的值（如果不是已知的@关键字）
            advance();
            if (check(TokenType::IDENTIFIER)) {
                item.value = "@" + advance().value;
            }
        } else if (check(TokenType::LEFT_BRACKET)) {
            // 处理 [Custom] 这样的值
            advance();
            if (check(TokenType::IDENTIFIER)) {
                std::string bracketContent = advance().value;
                consume(TokenType::RIGHT_BRACKET, "Expected ']'");
                item.value = "[" + bracketContent + "]";
            }
        } else {
            throw std::runtime_error("Expected configuration value");
        }
    }
    
    // 消费分号（如果有）
    match(TokenType::SEMICOLON);
    
    return item;
}

std::vector<std::string> ConfigurationParser::parseOptions() {
    std::vector<std::string> options;
    
    consume(TokenType::LEFT_BRACKET, "Expected '['");
    
    while (!check(TokenType::RIGHT_BRACKET) && !isAtEnd()) {
        std::string option;
        
        if (check(TokenType::AT)) {
            // @前缀选项
            advance();
            if (check(TokenType::IDENTIFIER)) {
                option = "@" + advance().value;
            }
        } else if (check(TokenType::IDENTIFIER)) {
            option = advance().value;
        } else if (check(TokenType::STRING_LITERAL) || 
                   check(TokenType::STRING_LITERAL_SINGLE)) {
            option = advance().value;
        }
        
        if (!option.empty()) {
            options.push_back(option);
        }
        
        // 消费逗号（如果有）
        if (!match(TokenType::COMMA)) {
            break;
        }
    }
    
    consume(TokenType::RIGHT_BRACKET, "Expected ']'");
    
    return options;
}

} // namespace chtl