#include "ConfigParser.h"
#include "../common/Token.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace chtl {

// ConfigParser实现
ConfigParser::ConfigParser(std::vector<Token> tokens)
    : BasicParser(std::move(tokens)), configurationMode_(false) {
}

std::shared_ptr<Node> ConfigParser::parse() {
    // 首先检查是否有Configuration块
    if (hasConfigurationBlock()) {
        return parseWithConfig();
    } else {
        // 如果没有Configuration，使用BasicParser的标准解析
        return BasicParser::parse();
    }
}

std::shared_ptr<Node> ConfigParser::parseWithConfig() {
    auto root = std::make_shared<Node>(NodeType::ROOT);
    
    // 首先解析Configuration块
    if (match(TokenType::CONFIGURATION)) {
        auto configNode = parseConfiguration();
        if (configNode) {
            root->addChild(configNode);
            loadConfiguration(configNode);
            applyCustomKeywordMappings();
        }
    }
    
    // 然后解析剩余的CHTL代码（使用配置后的关键字映射）
    setConfigurationMode(true);
    
    while (!isAtEnd()) {
        try {
            auto node = parseStatement();
            if (node) {
                root->addChild(node);
            }
        } catch (const std::exception& e) {
            reportConfigError(e.what(), peek());
            synchronizeAfterConfigError();
        }
    }
    
    return root;
}

std::shared_ptr<ConfigurationNode> ConfigParser::parseConfiguration() {
    if (!match(TokenType::CONFIGURATION)) {
        reportConfigError("Expected [Configuration]", peek());
        return nullptr;
    }
    
    auto configNode = std::make_shared<ConfigurationNode>(previous().position);
    
    if (!match(TokenType::LEFT_BRACE)) {
        reportConfigError("Expected '{' after [Configuration]", peek());
        return nullptr;
    }
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match(TokenType::CONFIG_OPTION_LIST)) {
            // 解析[Name]块
            auto nameBlock = parseNameBlock();
            if (nameBlock) {
                configNode->addNameBlock(nameBlock);
            }
        } else {
            // 解析配置选项
            auto option = parseConfigOption();
            if (option) {
                configNode->addConfigOption(option);
            }
        }
    }
    
    if (!match(TokenType::RIGHT_BRACE)) {
        reportConfigError("Expected '}' after Configuration block", peek());
    }
    
    return configNode;
}

std::shared_ptr<ConfigOptionNode> ConfigParser::parseConfigOption() {
    if (!match(TokenType::IDENTIFIER)) {
        reportConfigError("Expected configuration option name", peek());
        return nullptr;
    }
    
    std::string optionName = previous().value;
    auto optionType = parseOptionType(optionName);
    
    if (!match(TokenType::EQUAL)) {
        reportConfigError("Expected '=' after option name", peek());
        return nullptr;
    }
    
    // 解析选项值
    ConfigOptionNode::ConfigValue value;
    if (match(TokenType::NUMBER)) {
        value = ConfigOptionNode::ConfigValue(std::stoi(previous().value));
    } else if (match(TokenType::IDENTIFIER)) {
        std::string val = previous().value;
        if (val == "true") {
            value = ConfigOptionNode::ConfigValue(true);
        } else if (val == "false") {
            value = ConfigOptionNode::ConfigValue(false);
        } else {
            value = ConfigOptionNode::ConfigValue(val);
        }
    } else {
        reportConfigError("Expected option value", peek());
        return nullptr;
    }
    
    if (!match(TokenType::SEMICOLON)) {
        reportConfigError("Expected ';' after option value", peek());
    }
    
    return std::make_shared<ConfigOptionNode>(optionType, value, previous().position);
}

std::shared_ptr<NameBlockNode> ConfigParser::parseNameBlock() {
    // [Name]已经被匹配了
    std::string groupName;
    if (match(TokenType::IDENTIFIER)) {
        groupName = previous().value;
    } else {
        reportConfigError("Expected group name after [Name]", peek());
        return nullptr;
    }
    
    auto nameBlock = std::make_shared<NameBlockNode>(groupName, previous().position);
    
    if (!match(TokenType::LEFT_BRACE)) {
        reportConfigError("Expected '{' after Name group declaration", peek());
        return nullptr;
    }
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (check(TokenType::COMMENT_SINGLE) || check(TokenType::COMMENT_MULTI)) {
            advance(); // 跳过注释
            continue;
        }
        
        // 解析选项行：name = value;
        std::string optionName, optionValue;
        
        if (match(TokenType::IDENTIFIER)) {
            optionName = previous().value;
        } else if (match(TokenType::STRING_NO_QUOTE)) {
            optionName = previous().value; // 自定义关键字（可能是中文）
        } else {
            reportConfigError("Expected option name or custom keyword", peek());
            synchronizeAfterConfigError();
            continue;
        }
        
        if (!match(TokenType::EQUAL)) {
            reportConfigError("Expected '=' after option name", peek());
            synchronizeAfterConfigError();
            continue;
        }
        
        if (match(TokenType::STRING_DOUBLE_QUOTE)) {
            optionValue = previous().value;
        } else if (match(TokenType::IDENTIFIER)) {
            optionValue = previous().value;
        } else {
            reportConfigError("Expected option value", peek());
            synchronizeAfterConfigError();
            continue;
        }
        
        if (!match(TokenType::SEMICOLON)) {
            reportConfigError("Expected ';' after option value", peek());
        }
        
        // 判断是自定义关键字还是组选项
        if (isValidCustomKeyword(optionName)) {
            nameBlock->addCustomKeyword(optionName, optionValue);
            addCustomKeywordMapping(optionName, optionValue);
        } else {
            nameBlock->addGroupOption(optionName, optionValue);
        }
    }
    
    if (!match(TokenType::RIGHT_BRACE)) {
        reportConfigError("Expected '}' after Name block", peek());
    }
    
    return nameBlock;
}

bool ConfigParser::parseCustomKeywordMapping(const std::string& line, 
                                           std::string& keyword, 
                                           std::string& mapping) {
    // 简单的关键字映射解析：keyword = "mapping";
    std::regex mappingRegex(R"(\s*([^=]+)\s*=\s*\"([^\"]+)\"\s*;?)");
    std::smatch match;
    
    if (std::regex_match(line, match, mappingRegex)) {
        keyword = match[1].str();
        mapping = match[2].str();
        
        // 去除首尾空白
        keyword.erase(0, keyword.find_first_not_of(" \t"));
        keyword.erase(keyword.find_last_not_of(" \t") + 1);
        mapping.erase(0, mapping.find_first_not_of(" \t"));
        mapping.erase(mapping.find_last_not_of(" \t") + 1);
        
        return true;
    }
    
    return false;
}

bool ConfigParser::applyCustomKeywordMappings() {
    // 将自定义关键字映射应用到词法分析结果
    // 这里可能需要重新词法分析某些部分，或者在解析时动态处理
    processCustomKeywords();
    return true;
}

TokenType ConfigParser::resolveCustomKeyword(const std::string& keyword) const {
    auto it = customKeywordMappings_.find(keyword);
    if (it != customKeywordMappings_.end()) {
        // 根据映射值确定Token类型
        const std::string& mapping = it->second;
        
        // HTML标签映射
        if (mapping == "div" || mapping == "span" || mapping == "button" || 
            mapping == "container" || mapping == "card") {
            return TokenType::HTML_TAG;
        }
        
        // CSS属性映射
        if (mapping == "primary-color" || mapping == "background-color" || 
            mapping == "text-color" || mapping == "secondary-color") {
            return TokenType::IDENTIFIER;
        }
        
        // 其他映射
        return TokenType::IDENTIFIER;
    }
    
    return TokenType::UNKNOWN;
}

std::string ConfigParser::mapCustomKeyword(const std::string& keyword) const {
    auto it = customKeywordMappings_.find(keyword);
    return it != customKeywordMappings_.end() ? it->second : keyword;
}

void ConfigParser::setConfigurationMode(bool enabled) {
    configurationMode_ = enabled;
}

bool ConfigParser::isConfigurationMode() const {
    return configurationMode_;
}

void ConfigParser::loadConfiguration(std::shared_ptr<ConfigurationNode> config) {
    currentConfiguration_ = config;
    applyConfiguration();
}

void ConfigParser::addCustomKeywordMapping(const std::string& keyword, const std::string& mapping) {
    customKeywordMappings_[keyword] = mapping;
}

void ConfigParser::removeCustomKeywordMapping(const std::string& keyword) {
    customKeywordMappings_.erase(keyword);
}

std::unordered_map<std::string, std::string> ConfigParser::getCustomKeywordMappings() const {
    return customKeywordMappings_;
}

void ConfigParser::reportConfigError(const std::string& message, const Token& token) {
    std::ostringstream oss;
    oss << "Config Error at line " << token.position.line 
        << ", column " << token.position.column 
        << ": " << message;
    configErrors_.push_back(oss.str());
}

std::vector<std::string> ConfigParser::getConfigErrors() const {
    return configErrors_;
}

bool ConfigParser::validateConfigurationSyntax() const {
    // 基础语法验证
    return configErrors_.empty();
}

bool ConfigParser::validateCustomKeywords() const {
    for (const auto& pair : customKeywordMappings_) {
        if (!isValidCustomKeyword(pair.first) || !isValidKeywordMapping(pair.second)) {
            return false;
        }
    }
    return true;
}

// 重写的解析方法
std::shared_ptr<Node> ConfigParser::parseElement() {
    if (configurationMode_) {
        return parseConfigElement();
    }
    return BasicParser::parseElement();
}

std::shared_ptr<Node> ConfigParser::parseStyleBlock() {
    if (configurationMode_) {
        return parseConfigStyleBlock();
    }
    return BasicParser::parseStyleBlock();
}

// parseCustomDefinition方法暂不实现，因为BasicParser中没有这个方法

// Config特定的解析方法
std::shared_ptr<Node> ConfigParser::parseConfigElement() {
    Token token = peek();
    
    // 检查是否是自定义关键字
    if (token.type == TokenType::STRING_NO_QUOTE && isCustomKeyword(token.value)) {
        advance();
        std::string mappedTag = mapCustomKeyword(token.value);
        
        // 创建元素节点，使用映射后的标签名
        auto element = std::make_shared<ElementNode>(mappedTag, token.position);
        
        // 解析元素内容
        if (match(TokenType::LEFT_BRACE)) {
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                auto child = parseStatement();
                if (child) {
                    element->addChild(child);
                }
            }
            
            if (!match(TokenType::RIGHT_BRACE)) {
                reportConfigError("Expected '}' after element content", peek());
            }
        }
        
        return element;
    }
    
    // 否则使用标准解析
    return BasicParser::parseElement();
}

std::shared_ptr<Node> ConfigParser::parseConfigStyleBlock() {
    // 在样式块中也支持自定义关键字映射
    auto styleBlock = BasicParser::parseStyleBlock();
    
    // TODO: 处理样式块中的自定义关键字
    
    return styleBlock;
}

std::shared_ptr<Node> ConfigParser::parseConfigCustomDefinition() {
    // 处理自定义定义中的关键字映射
    // TODO: 实现自定义定义解析
    return nullptr;
}

TokenType ConfigParser::getTokenTypeWithConfig(const std::string& text) const {
    if (configurationMode_ && isCustomKeyword(text)) {
        return resolveCustomKeyword(text);
    }
    return TokenType::IDENTIFIER;
}

bool ConfigParser::isCustomKeyword(const std::string& keyword) const {
    return customKeywordMappings_.find(keyword) != customKeywordMappings_.end();
}

// 私有方法实现
ConfigOptionNode::OptionType ConfigParser::parseOptionType(const std::string& optionName) {
    if (optionName == "INDEX_INITIAL_COUNT") {
        return ConfigOptionNode::OptionType::INDEX_INITIAL_COUNT;
    } else if (optionName == "CUSTOM_STYLE") {
        return ConfigOptionNode::OptionType::CUSTOM_STYLE;
    } else if (optionName == "DISABLE_NAME_GROUP") {
        return ConfigOptionNode::OptionType::DISABLE_NAME_GROUP;
    } else if (optionName == "DEBUG_MODE") {
        return ConfigOptionNode::OptionType::DEBUG_MODE;
    } else if (optionName == "OPTION_COUNT") {
        return ConfigOptionNode::OptionType::OPTION_COUNT;
    }
    
    // 默认返回字符串类型
    return ConfigOptionNode::OptionType::CUSTOM_STYLE;
}

ConfigOptionNode::ConfigValue ConfigParser::parseOptionValue(const std::string& value, 
                                                            ConfigOptionNode::OptionType type) {
    switch (type) {
        case ConfigOptionNode::OptionType::INDEX_INITIAL_COUNT:
        case ConfigOptionNode::OptionType::OPTION_COUNT:
            return ConfigOptionNode::ConfigValue(std::stoi(value));
            
        case ConfigOptionNode::OptionType::CUSTOM_STYLE:
        case ConfigOptionNode::OptionType::DISABLE_NAME_GROUP:
        case ConfigOptionNode::OptionType::DEBUG_MODE:
            if (value == "true") return ConfigOptionNode::ConfigValue(true);
            if (value == "false") return ConfigOptionNode::ConfigValue(false);
            return ConfigOptionNode::ConfigValue(value);
            
        default:
            return ConfigOptionNode::ConfigValue(value);
    }
}

bool ConfigParser::parseNameBlockOption(const std::string& line, 
                                       std::string& optionName, 
                                       std::string& optionValue) {
    std::regex optionRegex(R"(\s*([^=]+)\s*=\s*([^;]+)\s*;?)");
    std::smatch match;
    
    if (std::regex_match(line, match, optionRegex)) {
        optionName = match[1].str();
        optionValue = match[2].str();
        
        // 去除首尾空白和引号
        optionName.erase(0, optionName.find_first_not_of(" \t"));
        optionName.erase(optionName.find_last_not_of(" \t") + 1);
        
        optionValue.erase(0, optionValue.find_first_not_of(" \t\""));
        optionValue.erase(optionValue.find_last_not_of(" \t\"") + 1);
        
        return true;
    }
    
    return false;
}

void ConfigParser::applyConfiguration() {
    if (!currentConfiguration_) return;
    
    auto options = currentConfiguration_->getConfigOptions();
    for (const auto& option : options) {
        switch (option->getOptionType()) {
            case ConfigOptionNode::OptionType::INDEX_INITIAL_COUNT:
                applyIndexInitialCount(option->getIntValue());
                break;
            case ConfigOptionNode::OptionType::CUSTOM_STYLE:
                applyCustomStyleSetting(option->getBoolValue());
                break;
            case ConfigOptionNode::OptionType::DISABLE_NAME_GROUP:
                applyNameGroupSettings(option->getBoolValue());
                break;
            case ConfigOptionNode::OptionType::DEBUG_MODE:
                applyDebugMode(option->getBoolValue());
                break;
            case ConfigOptionNode::OptionType::OPTION_COUNT:
                // 应用选项计数设置
                break;
        }
    }
}

void ConfigParser::applyIndexInitialCount(int count) {
    // 设置索引初始计数
    // TODO: 实现索引初始计数逻辑
}

void ConfigParser::applyCustomStyleSetting(bool enabled) {
    // 启用或禁用自定义样式
    // TODO: 实现自定义样式设置
}

void ConfigParser::applyNameGroupSettings(bool disabled) {
    // 启用或禁用名称组
    // TODO: 实现名称组设置
}

void ConfigParser::applyDebugMode(bool enabled) {
    // 启用或禁用调试模式
    if (enabled) {
        // 在调试模式下提供更详细的错误信息
    }
}

void ConfigParser::processCustomKeywords() {
    // 处理所有的自定义关键字映射
    for (const auto& pair : customKeywordMappings_) {
        // 验证关键字映射的有效性
        if (!isValidCustomKeyword(pair.first) || !isValidKeywordMapping(pair.second)) {
            reportConfigError("Invalid custom keyword mapping: " + pair.first + " = " + pair.second, peek());
        }
    }
}

bool ConfigParser::isValidCustomKeyword(const std::string& keyword) const {
    // 检查关键字是否有效（不与保留字冲突）
    static const std::unordered_set<std::string> reservedWords = {
        "html", "head", "body", "div", "span", "style", "text",
        "[Custom]", "[Template]", "[Origin]", "[Configuration]", "[Import]", "[Namespace]"
    };
    
    return !keyword.empty() && reservedWords.find(keyword) == reservedWords.end();
}

bool ConfigParser::isValidKeywordMapping(const std::string& mapping) const {
    // 检查映射值是否有效
    return !mapping.empty();
}

void ConfigParser::synchronizeAfterConfigError() {
    while (!isAtEnd() && !isConfigSynchronizationPoint()) {
        advance();
    }
}

bool ConfigParser::isConfigSynchronizationPoint() const {
    TokenType type = peek().type;
    return type == TokenType::RIGHT_BRACE || 
           type == TokenType::CONFIGURATION ||
           type == TokenType::CONFIG_OPTION_LIST ||
           type == TokenType::SEMICOLON;
}

bool ConfigParser::hasConfigurationBlock() {
    // 检查Token流中是否包含Configuration块
    for (size_t i = 0; i < tokens_.size(); ++i) {
        if (tokens_[i].type == TokenType::CONFIGURATION) {
            return true;
        }
    }
    return false;
}

// ConfigParserFactory实现
std::unique_ptr<ConfigParser> ConfigParserFactory::createStandardParser(std::vector<Token> tokens) {
    return std::make_unique<ConfigParser>(std::move(tokens));
}

std::unique_ptr<ConfigParser> ConfigParserFactory::createCustomKeywordParser(
    std::vector<Token> tokens,
    const std::unordered_map<std::string, std::string>& keywordMappings) {
    
    auto parser = std::make_unique<ConfigParser>(std::move(tokens));
    for (const auto& pair : keywordMappings) {
        parser->addCustomKeywordMapping(pair.first, pair.second);
    }
    return parser;
}

std::unique_ptr<ConfigParser> ConfigParserFactory::createDebugParser(std::vector<Token> tokens) {
    auto parser = std::make_unique<ConfigParser>(std::move(tokens));
    // 设置调试模式相关配置
    return parser;
}

std::unique_ptr<ConfigParser> ConfigParserFactory::createFromConfigFile(
    std::vector<Token> tokens,
    const std::string& configFilePath) {
    
    auto keywordMappings = loadKeywordMappings(configFilePath);
    return createCustomKeywordParser(std::move(tokens), keywordMappings);
}

std::unordered_map<std::string, std::string> ConfigParserFactory::loadKeywordMappings(
    const std::string& configFilePath) {
    
    std::unordered_map<std::string, std::string> mappings;
    // TODO: 从配置文件加载关键字映射
    return mappings;
}

// ConfigParseState实现
ConfigParseState::ConfigParseState() 
    : currentState_(State::INITIAL), optionCount_(0), nameBlockCount_(0), customKeywordCount_(0) {
}

ConfigParseState::State ConfigParseState::getCurrentState() const {
    return currentState_;
}

void ConfigParseState::setState(State newState) {
    currentState_ = newState;
}

void ConfigParseState::pushState(State state) {
    stateStack_.push_back(currentState_);
    currentState_ = state;
}

ConfigParseState::State ConfigParseState::popState() {
    if (!stateStack_.empty()) {
        currentState_ = stateStack_.back();
        stateStack_.pop_back();
    }
    return currentState_;
}

void ConfigParseState::incrementOptionCount() {
    ++optionCount_;
}

void ConfigParseState::incrementNameBlockCount() {
    ++nameBlockCount_;
}

void ConfigParseState::incrementCustomKeywordCount() {
    ++customKeywordCount_;
}

size_t ConfigParseState::getOptionCount() const {
    return optionCount_;
}

size_t ConfigParseState::getNameBlockCount() const {
    return nameBlockCount_;
}

size_t ConfigParseState::getCustomKeywordCount() const {
    return customKeywordCount_;
}

void ConfigParseState::addError(const std::string& error) {
    errors_.push_back(error);
}

std::vector<std::string> ConfigParseState::getErrors() const {
    return errors_;
}

bool ConfigParseState::hasErrors() const {
    return !errors_.empty();
}

void ConfigParseState::reset() {
    currentState_ = State::INITIAL;
    stateStack_.clear();
    optionCount_ = 0;
    nameBlockCount_ = 0;
    customKeywordCount_ = 0;
    errors_.clear();
}

// ConfigValidator实现
const std::unordered_set<std::string> ConfigValidator::validConfigOptions_ = {
    "INDEX_INITIAL_COUNT", "CUSTOM_STYLE", "DISABLE_NAME_GROUP", "DEBUG_MODE", "OPTION_COUNT"
};

const std::unordered_set<std::string> ConfigValidator::validNameBlockOptions_ = {
    "prefix", "suffix", "namespace", "default_class", "type"
};

const std::unordered_set<std::string> ConfigValidator::reservedKeywords_ = {
    "html", "head", "body", "div", "span", "style", "text",
    "class", "id", "src", "href", "alt", "title"
};

ConfigValidator::ConfigValidator() {
}

bool ConfigValidator::validateConfiguration(std::shared_ptr<ConfigurationNode> config) {
    if (!config) {
        addError("Configuration node is null");
        return false;
    }
    
    bool isValid = true;
    
    // 验证配置选项
    auto options = config->getConfigOptions();
    for (const auto& option : options) {
        if (!validateConfigOption(option)) {
            isValid = false;
        }
    }
    
    // 验证Name块
    auto nameBlocks = config->getNameBlocks();
    for (const auto& nameBlock : nameBlocks) {
        if (!validateNameBlock(nameBlock)) {
            isValid = false;
        }
    }
    
    return isValid;
}

bool ConfigValidator::validateConfigOption(std::shared_ptr<ConfigOptionNode> option) {
    if (!option) {
        addError("Config option node is null");
        return false;
    }
    
    return validateOptionValue(option->getOptionType(), option->getValue());
}

bool ConfigValidator::validateNameBlock(std::shared_ptr<NameBlockNode> nameBlock) {
    if (!nameBlock) {
        addError("Name block node is null");
        return false;
    }
    
    bool isValid = true;
    
    // 验证组选项
    auto groupOptions = nameBlock->getAllGroupOptions();
    for (const auto& pair : groupOptions) {
        if (!validateNameBlockOption(pair.first, pair.second)) {
            isValid = false;
        }
    }
    
    // 验证自定义关键字
    auto customKeywords = nameBlock->getAllCustomKeywords();
    for (const auto& pair : customKeywords) {
        if (!validateCustomKeyword(pair.first, pair.second)) {
            isValid = false;
        }
    }
    
    return isValid;
}

bool ConfigValidator::validateCustomKeyword(const std::string& keyword, const std::string& mapping) {
    if (!isValidKeywordName(keyword)) {
        addError("Invalid keyword name: " + keyword);
        return false;
    }
    
    if (!isValidMappingValue(mapping)) {
        addError("Invalid mapping value: " + mapping);
        return false;
    }
    
    return true;
}

bool ConfigValidator::validateKeywordMapping(const std::unordered_map<std::string, std::string>& mappings) {
    bool isValid = true;
    
    for (const auto& pair : mappings) {
        if (!validateCustomKeyword(pair.first, pair.second)) {
            isValid = false;
        }
    }
    
    return isValid;
}

bool ConfigValidator::validateOptionValue(ConfigOptionNode::OptionType type, 
                                        const ConfigOptionNode::ConfigValue& value) {
    switch (type) {
        case ConfigOptionNode::OptionType::INDEX_INITIAL_COUNT:
        case ConfigOptionNode::OptionType::OPTION_COUNT:
            if (!value.isInt()) {
                addError("Expected integer value for numeric option");
                return false;
            }
            if (value.asInt() < 0) {
                addError("Numeric option value must be non-negative");
                return false;
            }
            break;
            
        case ConfigOptionNode::OptionType::CUSTOM_STYLE:
        case ConfigOptionNode::OptionType::DISABLE_NAME_GROUP:
        case ConfigOptionNode::OptionType::DEBUG_MODE:
            if (!value.isBool()) {
                addError("Expected boolean value for boolean option");
                return false;
            }
            break;
    }
    
    return true;
}

bool ConfigValidator::validateNameBlockOption(const std::string& optionName, const std::string& optionValue) {
    if (validNameBlockOptions_.find(optionName) == validNameBlockOptions_.end()) {
        addError("Unknown name block option: " + optionName);
        return false;
    }
    
    if (optionValue.empty()) {
        addError("Name block option value cannot be empty: " + optionName);
        return false;
    }
    
    return true;
}

std::vector<std::string> ConfigValidator::getValidationErrors() const {
    return validationErrors_;
}

void ConfigValidator::clearErrors() {
    validationErrors_.clear();
}

bool ConfigValidator::isValidOptionName(const std::string& optionName) {
    return validConfigOptions_.find(optionName) != validConfigOptions_.end();
}

bool ConfigValidator::isValidOptionValue(const std::string& value, ConfigOptionNode::OptionType type) {
    // 基本的值验证
    return !value.empty();
}

bool ConfigValidator::isValidKeywordName(const std::string& keyword) {
    if (keyword.empty()) return false;
    
    // 检查是否与保留关键字冲突
    if (reservedKeywords_.find(keyword) != reservedKeywords_.end()) {
        return false;
    }
    
    // 允许中文字符和其他Unicode字符
    return true;
}

bool ConfigValidator::isValidMappingValue(const std::string& mapping) {
    return !mapping.empty();
}

void ConfigValidator::addError(const std::string& error) {
    validationErrors_.push_back(error);
}

} // namespace chtl