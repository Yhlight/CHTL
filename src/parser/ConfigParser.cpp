#include "ConfigParser.h"
#include <algorithm>
#include <sstream>

namespace chtl {

ConfigParser::ConfigParser() 
    : BasicParser(), configApplied(false) {
    setDefaultConfig();
}

ConfigParser::ConfigParser(std::shared_ptr<ChtlLoader> fileLoader)
    : BasicParser(fileLoader), configApplied(false) {
    setDefaultConfig();
}

void ConfigParser::setDefaultConfig() {
    // 设置默认配置值
    configMap["INDEX_INITIAL_COUNT"] = "0";
    configMap["DEBUG_MODE"] = "false";
    configMap["DISABLE_NAME_GROUP"] = "false";
    configMap["OPTION_COUNT"] = "1";
    
    // 默认关键字映射
    configMap["CUSTOM_STYLE"] = "@Style";
    configMap["CUSTOM_ELEMENT"] = "@Element";
    configMap["CUSTOM_VAR"] = "@Var";
    configMap["TEMPLATE_STYLE"] = "@Style";
    configMap["TEMPLATE_ELEMENT"] = "@Element";
    configMap["TEMPLATE_VAR"] = "@Var";
    configMap["ORIGIN_HTML"] = "@Html";
    configMap["ORIGIN_STYLE"] = "@Style";
    configMap["ORIGIN_JAVASCRIPT"] = "@JavaScript";
    configMap["KEYWORD_ADD"] = "add";
    configMap["KEYWORD_DELETE"] = "delete";
    configMap["KEYWORD_INHERIT"] = "inherit";
    configMap["KEYWORD_FROM"] = "from";
    configMap["KEYWORD_AS"] = "as";
    configMap["KEYWORD_TEXT"] = "text";
    configMap["KEYWORD_STYLE"] = "style";
    configMap["KEYWORD_CUSTOM"] = "[Custom]";
    configMap["KEYWORD_TEMPLATE"] = "[Template]";
    configMap["KEYWORD_ORIGIN"] = "[Origin]";
    configMap["KEYWORD_IMPORT"] = "[Import]";
    configMap["KEYWORD_NAMESPACE"] = "[Namespace]";
    configMap["KEYWORD_CONFIGURATION"] = "[Configuration]";
}

NodePtr ConfigParser::parse(const std::vector<Token>& tokenList) {
    tokens = tokenList;
    current = 0;
    
    auto root = std::make_shared<RootNode>();
    
    // 第一遍扫描：查找[Configuration]块
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].type == TokenType::KEYWORD_CONFIGURATION) {
            // 临时解析配置块
            current = i + 1; // 移动到[Configuration]之后，这样previous()会返回[Configuration]
            auto configNode = BasicParser::parseConfiguration();
            
            if (configNode) {
                auto config = std::dynamic_pointer_cast<ConfigurationNode>(configNode);
                if (config) {
                    applyConfiguration(config.get());
                    configApplied = true;
                    root->addChild(configNode);
                }
            }
            break;
        }
    }
    
    // 第二遍解析：使用配置驱动的解析
    current = 0;
    while (!isAtEnd()) {
        try {
            // 跳过已经解析的Configuration块
            if (check(TokenType::KEYWORD_CONFIGURATION)) {
                // 找到对应的右大括号
                advance(); // [Configuration]
                int braceCount = 0;
                while (!isAtEnd()) {
                    if (check(TokenType::LEFT_BRACE)) {
                        braceCount++;
                    } else if (check(TokenType::RIGHT_BRACE)) {
                        braceCount--;
                        if (braceCount == 0) {
                            advance();
                            break;
                        }
                    }
                    advance();
                }
                continue;
            }
            
            auto stmt = parseStatement();
            if (stmt) {
                root->addChild(stmt);
            }
        } catch (const ParseError& e) {
            synchronize();
        }
    }
    
    return root;
}

void ConfigParser::applyConfiguration(ConfigurationNode* config) {
    // 应用配置项
    auto items = config->getConfigItems();
    for (const auto& pair : items) {
        configMap[pair.first] = pair.second;
        
        // 如果是关键字配置，建立动态映射
        if (pair.first.find("KEYWORD_") == 0 || 
            pair.first.find("CUSTOM_") == 0 ||
            pair.first.find("TEMPLATE_") == 0 ||
            pair.first.find("ORIGIN_") == 0) {
            
            // 创建反向映射
            TokenType originalType = configValueToTokenType(pair.first, "");
            if (originalType != TokenType::UNKNOWN) {
                dynamicKeywords[pair.second] = originalType;
            }
        }
    }
    
    // 处理[Name]块
    for (const auto& child : config->getChildren()) {
        auto nameBlock = std::dynamic_pointer_cast<ConfigNameBlockNode>(child);
        if (nameBlock && configMap["DISABLE_NAME_GROUP"] != "true") {
            applyNameBlock(nameBlock.get());
        }
    }
    
    // 处理组选项
    for (const auto& child : config->getChildren()) {
        auto itemNode = std::dynamic_pointer_cast<ConfigItemNode>(child);
        if (itemNode && itemNode->hasOptions()) {
            optionsMap[itemNode->getKey()] = itemNode->getOptions();
        }
    }
}

void ConfigParser::applyNameBlock(ConfigNameBlockNode* nameBlock) {
    // 处理Name块中的配置项
    for (const auto& child : nameBlock->getChildren()) {
        auto item = std::dynamic_pointer_cast<ConfigItemNode>(child);
        if (item) {
            std::string key = item->getKey();
            std::string value = item->getValue();
            
            // Name块中的配置覆盖主配置
            configMap[key] = value;
            
            // 如果有组选项
            if (item->hasOptions()) {
                optionsMap[key] = item->getOptions();
                
                // 为每个选项创建动态映射
                TokenType originalType = configValueToTokenType(key, "");
                if (originalType != TokenType::UNKNOWN) {
                    for (const auto& option : item->getOptions()) {
                        dynamicKeywords[option] = originalType;
                    }
                }
            }
        }
    }
}

TokenType ConfigParser::configValueToTokenType(const std::string& configKey, 
                                               const std::string& value) {
    // 根据配置键确定对应的TokenType
    if (configKey == "KEYWORD_TEXT") return TokenType::KEYWORD_TEXT;
    if (configKey == "KEYWORD_STYLE") return TokenType::KEYWORD_STYLE;
    if (configKey == "KEYWORD_ADD") return TokenType::KEYWORD_ADD;
    if (configKey == "KEYWORD_DELETE") return TokenType::KEYWORD_DELETE;
    if (configKey == "KEYWORD_INHERIT") return TokenType::KEYWORD_INHERIT;
    if (configKey == "KEYWORD_FROM") return TokenType::KEYWORD_FROM;
    if (configKey == "KEYWORD_AS") return TokenType::KEYWORD_AS;
    if (configKey == "KEYWORD_CUSTOM") return TokenType::KEYWORD_CUSTOM;
    if (configKey == "KEYWORD_TEMPLATE") return TokenType::KEYWORD_TEMPLATE;
    if (configKey == "KEYWORD_ORIGIN") return TokenType::KEYWORD_ORIGIN;
    if (configKey == "KEYWORD_IMPORT") return TokenType::KEYWORD_IMPORT;
    if (configKey == "KEYWORD_NAMESPACE") return TokenType::KEYWORD_NAMESPACE;
    if (configKey == "KEYWORD_CONFIGURATION") return TokenType::KEYWORD_CONFIGURATION;
    
    if (configKey == "CUSTOM_STYLE" || configKey == "TEMPLATE_STYLE" || 
        configKey == "ORIGIN_STYLE") return TokenType::AT_STYLE;
    if (configKey == "CUSTOM_ELEMENT" || configKey == "TEMPLATE_ELEMENT") 
        return TokenType::AT_ELEMENT;
    if (configKey == "CUSTOM_VAR" || configKey == "TEMPLATE_VAR") 
        return TokenType::AT_VAR;
    if (configKey == "ORIGIN_HTML") return TokenType::AT_HTML;
    if (configKey == "ORIGIN_JAVASCRIPT") return TokenType::AT_JAVASCRIPT;
    
    return TokenType::UNKNOWN;
}

bool ConfigParser::isDynamicKeyword(const std::string& text) const {
    return dynamicKeywords.find(text) != dynamicKeywords.end();
}

TokenType ConfigParser::getDynamicKeywordType(const std::string& text) const {
    auto it = dynamicKeywords.find(text);
    if (it != dynamicKeywords.end()) {
        return it->second;
    }
    return TokenType::UNKNOWN;
}

NodePtr ConfigParser::parseStatement() {
    // 首先检查是否是动态关键字
    if (check(TokenType::IDENTIFIER)) {
        std::string text = peek().value;
        if (isDynamicKeyword(text)) {
            TokenType originalType = getDynamicKeywordType(text);
            advance(); // 消费动态关键字
            return parseDynamicKeyword(text, originalType);
        }
    }
    
    // 否则使用基类的解析
    return BasicParser::parseStatement();
}

NodePtr ConfigParser::parseDynamicKeyword(const std::string& keyword, 
                                         TokenType originalType) {
    // 根据原始类型决定如何解析
    switch (originalType) {
        case TokenType::KEYWORD_TEXT:
            return parseTextNode();
            
        case TokenType::KEYWORD_STYLE:
            return parseStyleNode();
            
        case TokenType::KEYWORD_ADD:
            return parseAddOperation();
            
        case TokenType::KEYWORD_DELETE:
            return parseDeleteOperation();
            
        case TokenType::KEYWORD_INHERIT:
            return parseInheritOperation();
            
        case TokenType::KEYWORD_CUSTOM:
            return parseCustomDefinition();
            
        case TokenType::KEYWORD_TEMPLATE:
            return parseTemplateDefinition();
            
        case TokenType::KEYWORD_ORIGIN:
            return parseOrigin();
            
        case TokenType::KEYWORD_IMPORT:
            return parseImport();
            
        case TokenType::KEYWORD_NAMESPACE:
            return parseNamespace();
            
        default:
            error("Unknown dynamic keyword: " + keyword);
            return nullptr;
    }
}

NodePtr ConfigParser::parseCustomDefinition() {
    // 使用配置的关键字
    std::string customStyle = configMap["CUSTOM_STYLE"];
    std::string customElement = configMap["CUSTOM_ELEMENT"];
    std::string customVar = configMap["CUSTOM_VAR"];
    
    // 检查动态关键字
    if (check(TokenType::IDENTIFIER)) {
        std::string text = peek().value;
        
        if (text == customStyle || 
            (optionsMap.count("CUSTOM_STYLE") && 
             std::find(optionsMap["CUSTOM_STYLE"].begin(), 
                      optionsMap["CUSTOM_STYLE"].end(), text) != 
             optionsMap["CUSTOM_STYLE"].end())) {
            advance();
            return parseCustomStyle();
        }
        
        if (text == customElement || 
            (optionsMap.count("CUSTOM_ELEMENT") && 
             std::find(optionsMap["CUSTOM_ELEMENT"].begin(), 
                      optionsMap["CUSTOM_ELEMENT"].end(), text) != 
             optionsMap["CUSTOM_ELEMENT"].end())) {
            advance();
            return parseCustomElement();
        }
        
        if (text == customVar || 
            (optionsMap.count("CUSTOM_VAR") && 
             std::find(optionsMap["CUSTOM_VAR"].begin(), 
                      optionsMap["CUSTOM_VAR"].end(), text) != 
             optionsMap["CUSTOM_VAR"].end())) {
            advance();
            return parseCustomVar();
        }
    }
    
    // 如果不是动态关键字，使用基类方法
    return BasicParser::parseCustomDefinition();
}

NodePtr ConfigParser::parseTemplateDefinition() {
    // 类似于parseCustomDefinition的实现
    std::string templateStyle = configMap["TEMPLATE_STYLE"];
    std::string templateElement = configMap["TEMPLATE_ELEMENT"];
    std::string templateVar = configMap["TEMPLATE_VAR"];
    
    if (check(TokenType::IDENTIFIER)) {
        std::string text = peek().value;
        
        if (text == templateStyle || 
            (optionsMap.count("TEMPLATE_STYLE") && 
             std::find(optionsMap["TEMPLATE_STYLE"].begin(), 
                      optionsMap["TEMPLATE_STYLE"].end(), text) != 
             optionsMap["TEMPLATE_STYLE"].end())) {
            advance();
            return parseTemplateStyle();
        }
        
        if (text == templateElement || 
            (optionsMap.count("TEMPLATE_ELEMENT") && 
             std::find(optionsMap["TEMPLATE_ELEMENT"].begin(), 
                      optionsMap["TEMPLATE_ELEMENT"].end(), text) != 
             optionsMap["TEMPLATE_ELEMENT"].end())) {
            advance();
            return parseTemplateElement();
        }
        
        if (text == templateVar || 
            (optionsMap.count("TEMPLATE_VAR") && 
             std::find(optionsMap["TEMPLATE_VAR"].begin(), 
                      optionsMap["TEMPLATE_VAR"].end(), text) != 
             optionsMap["TEMPLATE_VAR"].end())) {
            advance();
            return parseTemplateVar();
        }
    }
    
    return BasicParser::parseTemplateDefinition();
}

NodePtr ConfigParser::parseOrigin() {
    // 使用配置的关键字
    std::string originHtml = configMap["ORIGIN_HTML"];
    std::string originStyle = configMap["ORIGIN_STYLE"];
    std::string originJs = configMap["ORIGIN_JAVASCRIPT"];
    
    if (check(TokenType::IDENTIFIER)) {
        std::string text = peek().value;
        
        if (text == originHtml || 
            (optionsMap.count("ORIGIN_HTML") && 
             std::find(optionsMap["ORIGIN_HTML"].begin(), 
                      optionsMap["ORIGIN_HTML"].end(), text) != 
             optionsMap["ORIGIN_HTML"].end())) {
            advance();
            return parseOriginHtml();
        }
        
        if (text == originStyle || 
            (optionsMap.count("ORIGIN_STYLE") && 
             std::find(optionsMap["ORIGIN_STYLE"].begin(), 
                      optionsMap["ORIGIN_STYLE"].end(), text) != 
             optionsMap["ORIGIN_STYLE"].end())) {
            advance();
            return parseOriginStyle();
        }
        
        if (text == originJs || 
            (optionsMap.count("ORIGIN_JAVASCRIPT") && 
             std::find(optionsMap["ORIGIN_JAVASCRIPT"].begin(), 
                      optionsMap["ORIGIN_JAVASCRIPT"].end(), text) != 
             optionsMap["ORIGIN_JAVASCRIPT"].end())) {
            advance();
            return parseOriginJavaScript();
        }
    }
    
    return BasicParser::parseOrigin();
}

bool ConfigParser::matchDynamic(TokenType expectedType) {
    if (check(TokenType::IDENTIFIER)) {
        std::string text = peek().value;
        
        // 检查是否匹配任何配置的关键字
        for (const auto& pair : configMap) {
            if (configValueToTokenType(pair.first, "") == expectedType) {
                if (text == pair.second) {
                    advance();
                    return true;
                }
                
                // 检查组选项
                if (optionsMap.count(pair.first)) {
                    const auto& options = optionsMap[pair.first];
                    if (std::find(options.begin(), options.end(), text) != options.end()) {
                        advance();
                        return true;
                    }
                }
            }
        }
    }
    
    // 使用基类的match
    return match(expectedType);
}

Token ConfigParser::consumeDynamic(TokenType expectedType, const std::string& message) {
    if (matchDynamic(expectedType)) {
        return previous();
    }
    
    // 使用基类的consume
    return consume(expectedType, message);
}

std::string ConfigParser::getConfig(const std::string& key) const {
    auto it = configMap.find(key);
    if (it != configMap.end()) {
        return it->second;
    }
    return "";
}

std::vector<std::string> ConfigParser::getOptions(const std::string& key) const {
    auto it = optionsMap.find(key);
    if (it != optionsMap.end()) {
        return it->second;
    }
    return std::vector<std::string>();
}

bool ConfigParser::isDebugMode() const {
    return getConfig("DEBUG_MODE") == "true";
}

int ConfigParser::getIndexInitialCount() const {
    std::string value = getConfig("INDEX_INITIAL_COUNT");
    if (!value.empty()) {
        try {
            return std::stoi(value);
        } catch (...) {
            return 0;
        }
    }
    return 0;
}

} // namespace chtl