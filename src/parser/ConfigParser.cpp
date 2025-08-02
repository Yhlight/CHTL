#include "ConfigParser.h"
#include "../lexer/BasicLexer.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <chrono>

namespace chtl {

// ConfigCache静态成员定义
std::unordered_map<size_t, ConfigCache::CacheEntry> ConfigCache::cache;
static bool cacheEnabled = true;

bool ConfigCache::get(size_t hash, CacheEntry& entry) {
    if (!cacheEnabled) return false;
    
    auto it = cache.find(hash);
    if (it != cache.end()) {
        entry = it->second;
        return true;
    }
    return false;
}

void ConfigCache::put(size_t hash, const CacheEntry& entry) {
    if (!cacheEnabled) return;
    
    // 简单的LRU：如果缓存满了，删除最早的条目
    if (cache.size() >= MAX_CACHE_SIZE) {
        cache.erase(cache.begin());
    }
    
    cache[hash] = entry;
}

void ConfigCache::clear() {
    cache.clear();
}

size_t ConfigCache::computeHash(const std::vector<Token>& tokens) {
    size_t hash = 0;
    
    // 只对[Configuration]块计算哈希
    bool inConfig = false;
    int braceCount = 0;
    
    for (const auto& token : tokens) {
        if (token.type == TokenType::KEYWORD_CONFIGURATION) {
            inConfig = true;
        }
        
        if (inConfig) {
            // 使用token类型和值计算哈希
            hash ^= std::hash<int>{}(static_cast<int>(token.type)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            hash ^= std::hash<std::string>{}(token.value) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            
            if (token.type == TokenType::LEFT_BRACE) {
                braceCount++;
            } else if (token.type == TokenType::RIGHT_BRACE) {
                braceCount--;
                if (braceCount == 0 && inConfig) {
                    break;  // 配置块结束
                }
            }
        }
    }
    
    return hash;
}

ConfigParser::ConfigParser() 
    : BasicParser(), configApplied(false), 
      dynamicKeywordLookups(0), dynamicKeywordHits(0) {
    setDefaultConfig();
}

ConfigParser::ConfigParser(std::shared_ptr<ChtlLoader> fileLoader)
    : BasicParser(fileLoader), configApplied(false),
      dynamicKeywordLookups(0), dynamicKeywordHits(0) {
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
    auto startTime = std::chrono::high_resolution_clock::now();
    
    tokens = tokenList;
    current = 0;
    
    auto root = std::make_shared<RootNode>();
    
    // 计算配置哈希
    size_t configHash = ConfigCache::computeHash(tokens);
    
    // 尝试从缓存获取配置
    ConfigCache::CacheEntry cacheEntry;
    if (ConfigCache::get(configHash, cacheEntry)) {
        // 使用缓存的配置
        configMap = cacheEntry.configMap;
        optionsMap = cacheEntry.optionsMap;
        dynamicKeywords = cacheEntry.dynamicKeywords;
        configApplied = true;
        
        if (isDebugMode()) {
            std::cout << "[ConfigParser] Using cached configuration" << std::endl;
        }
    } else {
        // 第一遍扫描：查找[Configuration]块
        for (size_t i = 0; i < tokens.size(); ++i) {
            if (tokens[i].type == TokenType::KEYWORD_CONFIGURATION) {
                // 临时解析配置块
                current = i + 1; // 移动到[Configuration]之后，这样previous()会返回[Configuration]
                auto configNode = BasicParser::parseConfiguration();
                
                if (configNode) {
                    auto config = std::dynamic_pointer_cast<ConfigurationNode>(configNode);
                    if (config) {
                        // 验证配置
                        validateConfiguration(config.get());
                        
                        if (validationResult.isValid) {
                            applyConfiguration(config.get());
                            configApplied = true;
                            root->addChild(configNode);
                            
                            // 缓存配置
                            cacheEntry.configNode = config;
                            cacheEntry.configMap = configMap;
                            cacheEntry.optionsMap = optionsMap;
                            cacheEntry.dynamicKeywords = dynamicKeywords;
                            cacheEntry.hash = configHash;
                            ConfigCache::put(configHash, cacheEntry);
                        } else {
                            // 报告验证错误但继续解析
                            if (isDebugMode()) {
                                std::cout << "[ConfigParser] Configuration validation failed:" << std::endl;
                                for (const auto& err : validationResult.errors) {
                                    std::cout << "  ERROR: " << err << std::endl;
                                }
                                for (const auto& warn : validationResult.warnings) {
                                    std::cout << "  WARNING: " << warn << std::endl;
                                }
                            }
                            // 仍然应用配置，但标记为无效
                            applyConfiguration(config.get());
                            configApplied = true;
                            root->addChild(configNode);
                        }
                    }
                }
                break;
            }
        }
    }
    
    // 优化动态关键字映射
    if (configApplied) {
        optimizeDynamicKeywordMap();
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
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    if (isDebugMode()) {
        std::cout << "[ConfigParser] Parse time: " << duration.count() << " microseconds" << std::endl;
        printPerformanceStats();
    }
    
    return root;
}

void ConfigParser::validateConfiguration(ConfigurationNode* config) {
    validationResult = ConfigValidationResult();  // 重置验证结果
    
    // 获取所有配置项
    auto items = config->getConfigItems();
    
    // 1. 检查必需的配置项
    std::vector<std::string> requiredKeys = {
        "INDEX_INITIAL_COUNT", "DEBUG_MODE"
    };
    
    for (const auto& key : requiredKeys) {
        if (items.find(key) == items.end()) {
            validationResult.addWarning("Missing recommended configuration: " + key);
        }
    }
    
    // 2. 验证配置值
    for (const auto& pair : items) {
        if (!isValidConfigValue(pair.first, pair.second)) {
            validationResult.addError("Invalid value for " + pair.first + ": " + pair.second);
        }
    }
    
    // 3. 检查关键字冲突
    checkKeywordConflicts();
    
    // 4. 检查循环定义
    checkCircularDefinitions();
    
    // 5. 验证组选项
    for (const auto& child : config->getChildren()) {
        auto itemNode = std::dynamic_pointer_cast<ConfigItemNode>(child);
        if (itemNode && itemNode->hasOptions()) {
            auto options = itemNode->getOptions();
            if (options.empty()) {
                validationResult.addError("Empty option group for " + itemNode->getKey());
            }
            
            // 检查重复选项
            std::unordered_set<std::string> seen;
            for (const auto& opt : options) {
                if (seen.count(opt)) {
                    validationResult.addWarning("Duplicate option '" + opt + "' in " + itemNode->getKey());
                }
                seen.insert(opt);
            }
        }
    }
}

bool ConfigParser::isValidConfigValue(const std::string& key, const std::string& value) {
    // 验证布尔值
    if (key == "DEBUG_MODE" || key == "DISABLE_NAME_GROUP") {
        return value == "true" || value == "false";
    }
    
    // 验证数字值
    if (key == "INDEX_INITIAL_COUNT" || key == "OPTION_COUNT") {
        try {
            std::stoi(value);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // 验证关键字不为空
    if (key.find("KEYWORD_") == 0 || key.find("CUSTOM_") == 0 || 
        key.find("TEMPLATE_") == 0 || key.find("ORIGIN_") == 0) {
        return !value.empty();
    }
    
    return true;
}

void ConfigParser::checkKeywordConflicts() {
    std::unordered_map<std::string, std::vector<std::string>> keywordUsage;
    
    // 收集所有关键字使用情况
    for (const auto& pair : configMap) {
        if (pair.first.find("KEYWORD_") == 0 || pair.first.find("CUSTOM_") == 0 ||
            pair.first.find("TEMPLATE_") == 0 || pair.first.find("ORIGIN_") == 0) {
            keywordUsage[pair.second].push_back(pair.first);
        }
    }
    
    // 检查组选项中的关键字
    for (const auto& pair : optionsMap) {
        for (const auto& opt : pair.second) {
            keywordUsage[opt].push_back(pair.first + " (option)");
        }
    }
    
    // 报告冲突
    for (const auto& pair : keywordUsage) {
        if (pair.second.size() > 1) {
            std::stringstream ss;
            ss << "Keyword '" << pair.first << "' is used by multiple configurations: ";
            for (size_t i = 0; i < pair.second.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << pair.second[i];
            }
            validationResult.addWarning(ss.str());
        }
    }
}

void ConfigParser::checkCircularDefinitions() {
    // 检查配置值是否引用了其他配置键
    for (const auto& pair : configMap) {
        // 简单检查：配置值是否包含其他配置键
        for (const auto& other : configMap) {
            if (pair.first != other.first && pair.second.find(other.first) != std::string::npos) {
                validationResult.addWarning("Potential circular reference: " + 
                    pair.first + " references " + other.first);
            }
        }
    }
}

void ConfigParser::optimizeDynamicKeywordMap() {
    // 构建反向映射以加速查找
    reverseKeywordMap.clear();
    
    for (const auto& pair : dynamicKeywords) {
        reverseKeywordMap[pair.second].insert(pair.first);
    }
    
    // 预分配桶大小以减少哈希冲突
    dynamicKeywords.reserve(dynamicKeywords.size() * 2);
}

void ConfigParser::printPerformanceStats() const {
    std::cout << "[ConfigParser Performance Stats]" << std::endl;
    std::cout << "  Dynamic keyword lookups: " << dynamicKeywordLookups << std::endl;
    std::cout << "  Dynamic keyword hits: " << dynamicKeywordHits << std::endl;
    
    if (dynamicKeywordLookups > 0) {
        double hitRate = (double)dynamicKeywordHits / dynamicKeywordLookups * 100;
        std::cout << "  Hit rate: " << hitRate << "%" << std::endl;
    }
    
    std::cout << "  Total dynamic keywords: " << dynamicKeywords.size() << std::endl;
    std::cout << "  Total configurations: " << configMap.size() << std::endl;
}

void ConfigParser::enableCache(bool enable) {
    cacheEnabled = enable;
    if (!enable) {
        ConfigCache::clear();
    }
}

void ConfigParser::warmupCache(const std::string& configContent) {
    // 预热缓存：解析配置并存储
    BasicLexer lexer;
    auto tokens = lexer.tokenize(configContent);
    
    ConfigParser parser;
    parser.parse(tokens);
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
        
        // 使用反向映射快速查找
        auto it = reverseKeywordMap.find(expectedType);
        if (it != reverseKeywordMap.end() && it->second.count(text)) {
            advance();
            return true;
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