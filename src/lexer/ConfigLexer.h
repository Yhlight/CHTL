#pragma once
#include "BasicLexer.h"
#include <unordered_map>
#include <vector>

namespace chtl {

/**
 * 配置词法分析器
 * 专门处理Config机制的自定义关键字和配置语法
 * 支持ASCII范围内任意字符组合的自定义关键字
 */
class ConfigLexer : public BasicLexer {
public:
    explicit ConfigLexer(const std::string& input);
    virtual ~ConfigLexer() = default;
    
    // 配置管理
    void applyConfiguration(const std::unordered_map<std::string, std::string>& config);
    void applyConfigurationList(const std::unordered_map<std::string, std::vector<std::string>>& configLists);
    
    // 自定义关键字支持
    void setCustomKeyword(const std::string& configKey, const std::string& keyword);
    void setCustomKeywordList(const std::string& configKey, const std::vector<std::string>& keywords);
    
    // 配置状态查询
    bool isInConfigurationMode() const;
    bool hasCustomKeywords() const;
    std::string getCurrentConfigKey() const;
    
protected:
    // 重写基类方法以支持配置语法
    Token tokenizeNext() override;
    TokenType inferTokenType(const std::string& value) const override;
    
    // 配置特有的Token识别
    Token readConfigurationBlock();
    Token readConfigKey();
    Token readConfigValue();
    Token readConfigOptionList();
    Token readNameBlock();
    
    // 自定义关键字处理
    bool isCustomKeyword(const std::string& value) const;
    TokenType getCustomKeywordType(const std::string& value) const;
    
    // 上下文推导重写
    bool shouldTreatAsHtmlTag(const std::string& identifier) const override;
    bool shouldTreatAsCssSelector(const std::string& identifier) const override;
    bool shouldTreatAsCustomElement(const std::string& identifier) const override;
    bool shouldTreatAsVariable(const std::string& identifier) const override;
    
    // 配置语法特有处理
    Token handleConfigEquals();
    Token handleConfigColon();
    Token handleConfigSemicolon();
    Token handleConfigBracket();
    Token handleConfigIdentifier();
    
private:
    // 配置状态
    bool inConfigurationMode_;
    bool inNameBlock_;
    std::string currentConfigKey_;
    
    // 自定义关键字映射
    std::unordered_map<std::string, TokenType> customKeywords_;
    std::unordered_map<std::string, std::vector<std::string>> customKeywordLists_;
    
    // 配置解析状态
    enum class ConfigParseState {
        NORMAL,
        IN_CONFIG_BLOCK,
        IN_NAME_BLOCK,
        EXPECTING_CONFIG_KEY,
        EXPECTING_CONFIG_VALUE,
        EXPECTING_OPTION_LIST,
        IN_OPTION_LIST
    };
    
    ConfigParseState configState_;
    
    // ASCII范围检查
    bool isValidAsciiIdentifier(const std::string& identifier) const;
    bool isValidCustomKeywordChar(char c) const;
    
    // 配置语法验证
    bool isValidConfigKey(const std::string& key) const;
    bool isValidConfigValue(const std::string& value) const;
    
    // 选项列表处理
    std::vector<std::string> parseOptionList(const std::string& listContent) const;
    bool isValidOptionListFormat(const std::string& content) const;
    
    // 配置应用
    void updateKeywordRegistry();
    void processConfigurationPair(const std::string& key, const std::string& value);
    void processConfigurationList(const std::string& key, const std::vector<std::string>& values);
    
    // 错误处理
    Token createConfigError(const std::string& message);
    void validateConfigSyntax(const std::string& key, const std::string& value);
    
    // 状态管理
    void enterConfigurationMode();
    void exitConfigurationMode();
    void enterNameBlock();
    void exitNameBlock();
    void setConfigParseState(ConfigParseState state);
    
    // 调试和验证
    std::string getConfigStateString() const;
    void logConfigChange(const std::string& key, const std::string& value) const;
};

/**
 * 配置解析器
 * 专门解析配置块内容
 */
class ConfigurationParser {
public:
    ConfigurationParser();
    
    // 解析配置块
    std::unordered_map<std::string, std::string> parseConfigBlock(const std::string& content);
    std::unordered_map<std::string, std::vector<std::string>> parseConfigListBlock(const std::string& content);
    
    // 解析Name块
    std::unordered_map<std::string, std::string> parseNameBlock(const std::string& content);
    std::unordered_map<std::string, std::vector<std::string>> parseNameListBlock(const std::string& content);
    
    // 验证配置
    bool validateConfiguration(const std::unordered_map<std::string, std::string>& config) const;
    bool validateConfigurationList(const std::unordered_map<std::string, std::vector<std::string>>& config) const;
    
    // 默认配置
    std::unordered_map<std::string, std::string> getDefaultConfiguration() const;
    std::unordered_map<std::string, std::vector<std::string>> getDefaultConfigurationLists() const;
    
private:
    // 解析辅助
    std::pair<std::string, std::string> parseConfigLine(const std::string& line) const;
    std::pair<std::string, std::vector<std::string>> parseConfigListLine(const std::string& line) const;
    
    // 值处理
    std::string cleanConfigValue(const std::string& value) const;
    std::vector<std::string> parseValueList(const std::string& listStr) const;
    
    // 验证方法
    bool isValidConfigKeyName(const std::string& key) const;
    bool isValidConfigValueFormat(const std::string& value) const;
    bool isValidKeywordFormat(const std::string& keyword) const;
    
    // 预定义配置键
    std::unordered_set<std::string> validConfigKeys_;
    std::unordered_set<std::string> listConfigKeys_;
    
    void initializeValidKeys();
};

/**
 * 关键字冲突解决器
 * 处理自定义关键字与默认关键字的冲突
 */
class KeywordConflictResolver {
public:
    KeywordConflictResolver();
    
    // 冲突检测
    bool hasConflict(const std::string& customKeyword) const;
    std::vector<std::string> detectConflicts(const std::unordered_map<std::string, std::string>& customKeywords) const;
    
    // 冲突解决
    std::string resolveConflict(const std::string& customKeyword, const std::string& context) const;
    std::unordered_map<std::string, std::string> resolveAllConflicts(
        const std::unordered_map<std::string, std::string>& customKeywords) const;
    
    // 优先级管理
    void setKeywordPriority(const std::string& keyword, int priority);
    int getKeywordPriority(const std::string& keyword) const;
    
private:
    std::unordered_set<std::string> defaultKeywords_;
    std::unordered_map<std::string, int> keywordPriorities_;
    
    void initializeDefaultKeywords();
    std::string generateAlternativeKeyword(const std::string& original) const;
};

} // namespace chtl