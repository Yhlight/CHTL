#pragma once
#include "BasicParser.h"
#include "../node/Config.h"
#include <unordered_map>
#include <unordered_set>

namespace chtl {

/**
 * 配置解析器
 * 专门处理Config机制的语法分析，支持自定义关键字映射
 */
class ConfigParser : public BasicParser {
public:
    explicit ConfigParser(std::vector<Token> tokens);
    virtual ~ConfigParser() = default;

    // 主要解析方法
    std::shared_ptr<Node> parse();
    std::shared_ptr<Node> parseWithConfig();

    // Configuration语法解析
    std::shared_ptr<ConfigurationNode> parseConfiguration();
    std::shared_ptr<ConfigOptionNode> parseConfigOption();
    std::shared_ptr<NameBlockNode> parseNameBlock();

    // 自定义关键字解析
    bool parseCustomKeywordMapping(const std::string& line, 
                                  std::string& keyword, 
                                  std::string& mapping);
    bool applyCustomKeywordMappings();

    // 配置驱动的Token解析
    TokenType resolveCustomKeyword(const std::string& keyword) const;
    std::string mapCustomKeyword(const std::string& keyword) const;

    // 配置状态管理
    void setConfigurationMode(bool enabled);
    bool isConfigurationMode() const;
    void loadConfiguration(std::shared_ptr<ConfigurationNode> config);

    // 自定义关键字管理
    void addCustomKeywordMapping(const std::string& keyword, const std::string& mapping);
    void removeCustomKeywordMapping(const std::string& keyword);
    std::unordered_map<std::string, std::string> getCustomKeywordMappings() const;

    // Config特定的错误处理
    void reportConfigError(const std::string& message, const Token& token);
    std::vector<std::string> getConfigErrors() const;

    // 验证方法
    bool validateConfigurationSyntax() const;
    bool validateCustomKeywords() const;

protected:
    // 重写基类的解析方法以支持Config机制
    std::shared_ptr<Node> parseElement();
    std::shared_ptr<Node> parseStyleBlock();

    // Config特定的解析方法
    std::shared_ptr<Node> parseConfigElement();
    std::shared_ptr<Node> parseConfigStyleBlock();
    std::shared_ptr<Node> parseConfigCustomDefinition();

    // 关键字解析增强
    TokenType getTokenTypeWithConfig(const std::string& text) const;
    bool isCustomKeyword(const std::string& keyword) const;

private:
    // 配置状态
    bool configurationMode_;
    std::shared_ptr<ConfigurationNode> currentConfiguration_;
    
    // 辅助方法
    bool hasConfigurationBlock();

    // 自定义关键字映射表
    std::unordered_map<std::string, std::string> customKeywordMappings_;

    // Config特定的错误列表
    std::vector<std::string> configErrors_;

    // 配置选项解析
    ConfigOptionNode::OptionType parseOptionType(const std::string& optionName);
    ConfigOptionNode::ConfigValue parseOptionValue(const std::string& value, 
                                                  ConfigOptionNode::OptionType type);

    // Name块解析辅助方法
    bool parseNameBlockOption(const std::string& line, 
                             std::string& optionName, 
                             std::string& optionValue);

    // 配置应用
    void applyConfiguration();
    void applyIndexInitialCount(int count);
    void applyCustomStyleSetting(bool enabled);
    void applyNameGroupSettings(bool disabled);
    void applyDebugMode(bool enabled);

    // 自定义关键字处理
    void processCustomKeywords();
    bool isValidCustomKeyword(const std::string& keyword) const;
    bool isValidKeywordMapping(const std::string& mapping) const;

    // 错误恢复
    void synchronizeAfterConfigError();
    bool isConfigSynchronizationPoint() const;
};

/**
 * 配置解析工厂
 * 根据不同的配置创建相应的解析器
 */
class ConfigParserFactory {
public:
    // 创建标准配置解析器
    static std::unique_ptr<ConfigParser> createStandardParser(std::vector<Token> tokens);

    // 创建自定义关键字解析器
    static std::unique_ptr<ConfigParser> createCustomKeywordParser(
        std::vector<Token> tokens,
        const std::unordered_map<std::string, std::string>& keywordMappings);

    // 创建调试模式解析器
    static std::unique_ptr<ConfigParser> createDebugParser(std::vector<Token> tokens);

    // 从配置文件创建解析器
    static std::unique_ptr<ConfigParser> createFromConfigFile(
        std::vector<Token> tokens,
        const std::string& configFilePath);

private:
    ConfigParserFactory() = default;
    ~ConfigParserFactory() = default;

    static std::unordered_map<std::string, std::string> loadKeywordMappings(
        const std::string& configFilePath);
};

/**
 * 配置解析状态管理器
 * 管理配置解析过程中的状态信息
 */
class ConfigParseState {
public:
    explicit ConfigParseState();
    ~ConfigParseState() = default;

    // 解析状态
    enum class State {
        INITIAL,            // 初始状态
        IN_CONFIGURATION,   // 在Configuration块中
        IN_NAME_BLOCK,      // 在Name块中
        IN_OPTION_PARSING,  // 在解析配置选项
        IN_CUSTOM_PARSING,  // 在解析自定义关键字
        ERROR_RECOVERY,     // 错误恢复状态
        COMPLETED          // 解析完成
    };

    // 状态管理
    State getCurrentState() const;
    void setState(State newState);
    void pushState(State state);
    State popState();

    // 配置项计数
    void incrementOptionCount();
    void incrementNameBlockCount();
    void incrementCustomKeywordCount();

    // 统计信息
    size_t getOptionCount() const;
    size_t getNameBlockCount() const;
    size_t getCustomKeywordCount() const;

    // 错误状态
    void addError(const std::string& error);
    std::vector<std::string> getErrors() const;
    bool hasErrors() const;

    // 重置状态
    void reset();

private:
    State currentState_;
    std::vector<State> stateStack_;
    
    size_t optionCount_;
    size_t nameBlockCount_;
    size_t customKeywordCount_;
    
    std::vector<std::string> errors_;
};

/**
 * 配置验证器
 * 验证配置语法和语义的正确性
 */
class ConfigValidator {
public:
    explicit ConfigValidator();
    ~ConfigValidator() = default;

    // 验证配置节点
    bool validateConfiguration(std::shared_ptr<ConfigurationNode> config);
    bool validateConfigOption(std::shared_ptr<ConfigOptionNode> option);
    bool validateNameBlock(std::shared_ptr<NameBlockNode> nameBlock);

    // 验证自定义关键字
    bool validateCustomKeyword(const std::string& keyword, const std::string& mapping);
    bool validateKeywordMapping(const std::unordered_map<std::string, std::string>& mappings);

    // 验证配置选项值
    bool validateOptionValue(ConfigOptionNode::OptionType type, 
                           const ConfigOptionNode::ConfigValue& value);

    // 验证Name块选项
    bool validateNameBlockOption(const std::string& optionName, const std::string& optionValue);

    // 获取验证错误
    std::vector<std::string> getValidationErrors() const;
    void clearErrors();

private:
    std::vector<std::string> validationErrors_;

    // 验证辅助方法
    bool isValidOptionName(const std::string& optionName);
    bool isValidOptionValue(const std::string& value, ConfigOptionNode::OptionType type);
    bool isValidKeywordName(const std::string& keyword);
    bool isValidMappingValue(const std::string& mapping);

    // 预定义的有效选项
    static const std::unordered_set<std::string> validConfigOptions_;
    static const std::unordered_set<std::string> validNameBlockOptions_;
    static const std::unordered_set<std::string> reservedKeywords_;

    void addError(const std::string& error);
};

} // namespace chtl