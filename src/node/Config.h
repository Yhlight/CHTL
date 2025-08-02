#pragma once
#include "Node.h"
#include <vector>
#include <unordered_map>
#include <variant>

namespace chtl {

/**
 * 配置节点
 * 表示[Configuration]配置块
 */
class ConfigurationNode : public Node {
public:
    explicit ConfigurationNode(const NodePosition& position = NodePosition());
    virtual ~ConfigurationNode() = default;
    
    // 配置选项管理
    void addConfigOption(std::shared_ptr<class ConfigOptionNode> option);
    void removeConfigOption(const std::string& optionName);
    std::vector<std::shared_ptr<class ConfigOptionNode>> getConfigOptions() const;
    std::shared_ptr<class ConfigOptionNode> findConfigOption(const std::string& optionName) const;
    
    // Name块管理
    void addNameBlock(std::shared_ptr<class NameBlockNode> nameBlock);
    void removeNameBlock(const std::string& groupName);
    std::vector<std::shared_ptr<class NameBlockNode>> getNameBlocks() const;
    std::shared_ptr<class NameBlockNode> findNameBlock(const std::string& groupName) const;
    
    // 配置验证
    bool validateConfiguration() const;
    std::vector<std::string> getConfigurationErrors() const;
    
    // 配置应用
    bool applyConfiguration();
    bool isConfigurationApplied() const;
    
    // 验证
    bool validate() const override;
    
    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;
    std::string toConfigString() const;
    
    // 克隆
    std::shared_ptr<Node> clone() const override;
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override;
    
private:
    std::vector<std::shared_ptr<class ConfigOptionNode>> configOptions_;
    std::vector<std::shared_ptr<class NameBlockNode>> nameBlocks_;
    bool isApplied_;
};

/**
 * 配置选项节点
 * 表示配置块中的单个选项
 */
class ConfigOptionNode : public Node {
public:
    // 配置选项类型枚举
    enum class OptionType {
        INDEX_INITIAL_COUNT,    // INDEX_INITIAL_COUNT = 数值
        CUSTOM_STYLE,          // CUSTOM_STYLE = 布尔值
        DISABLE_NAME_GROUP,    // DISABLE_NAME_GROUP = 布尔值
        DEBUG_MODE,            // DEBUG_MODE = 布尔值
        OPTION_COUNT          // OPTION_COUNT = 数值
    };
    
    // 配置值类型
    using ConfigValue = std::variant<int, bool, std::string>;
    
    explicit ConfigOptionNode(OptionType type, const ConfigValue& value,
                             const NodePosition& position = NodePosition());
    virtual ~ConfigOptionNode() = default;
    
    // 选项类型管理
    OptionType getOptionType() const;
    void setOptionType(OptionType type);
    
    // 选项名称
    std::string getOptionName() const;
    
    // 值管理
    const ConfigValue& getValue() const;
    void setValue(const ConfigValue& value);
    
    // 类型安全的值访问
    int getIntValue() const;
    bool getBoolValue() const;
    std::string getStringValue() const;
    
    // 值类型检查
    bool isIntValue() const;
    bool isBoolValue() const;
    bool isStringValue() const;
    
    // 默认值管理
    ConfigValue getDefaultValue() const;
    bool isDefaultValue() const;
    void resetToDefault();
    
    // 值验证
    bool isValidValue(const ConfigValue& value) const;
    std::string getValueConstraints() const;
    
    // 验证
    bool validate() const override;
    
    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;
    std::string toConfigLine() const;
    
    // 克隆
    std::shared_ptr<Node> clone() const override;
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override;
    
private:
    OptionType optionType_;
    ConfigValue value_;
    
    std::string optionTypeToString() const;
    ConfigValue getDefaultValueForType(OptionType type) const;
    bool isValidValueForType(OptionType type, const ConfigValue& value) const;
};

/**
 * Name块节点
 * 表示[Name]配置块中的组选项
 */
class NameBlockNode : public Node {
public:
    explicit NameBlockNode(const std::string& groupName = "",
                          const NodePosition& position = NodePosition());
    virtual ~NameBlockNode() = default;
    
    // 组名称管理
    const std::string& getGroupName() const;
    void setGroupName(const std::string& groupName);
    
    // 组选项管理
    void addGroupOption(const std::string& optionName, const std::string& value);
    void removeGroupOption(const std::string& optionName);
    std::string getGroupOption(const std::string& optionName) const;
    std::unordered_map<std::string, std::string> getAllGroupOptions() const;
    bool hasGroupOption(const std::string& optionName) const;
    
    // 自定义关键字管理
    void addCustomKeyword(const std::string& keyword, const std::string& mapping);
    void removeCustomKeyword(const std::string& keyword);
    std::string getCustomKeywordMapping(const std::string& keyword) const;
    std::unordered_map<std::string, std::string> getAllCustomKeywords() const;
    bool hasCustomKeyword(const std::string& keyword) const;
    
    // 关键字解析
    std::string resolveKeyword(const std::string& keyword) const;
    bool isCustomKeyword(const std::string& keyword) const;
    
    // 验证
    bool validate() const override;
    bool validateGroupOptions() const;
    bool validateCustomKeywords() const;
    
    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;
    std::string toNameBlockString() const;
    
    // 克隆
    std::shared_ptr<Node> clone() const override;
    
    // 访问者模式
    void accept(NodeVisitor& visitor) override;
    
private:
    std::string groupName_;
    std::unordered_map<std::string, std::string> groupOptions_;
    std::unordered_map<std::string, std::string> customKeywords_;
    
    bool isValidGroupName(const std::string& groupName) const;
    bool isValidOptionName(const std::string& optionName) const;
    bool isValidKeyword(const std::string& keyword) const;
    bool isValidKeywordMapping(const std::string& mapping) const;
};

/**
 * 配置管理器
 * 全局管理配置状态和应用
 */
class ConfigurationManager {
public:
    static ConfigurationManager& getInstance();
    
    // 配置注册
    bool registerConfiguration(std::shared_ptr<ConfigurationNode> config);
    void unregisterConfiguration();
    std::shared_ptr<ConfigurationNode> getCurrentConfiguration() const;
    bool hasConfiguration() const;
    
    // 配置选项访问
    int getIndexInitialCount() const;
    bool isCustomStyleEnabled() const;
    bool isNameGroupDisabled() const;
    bool isDebugModeEnabled() const;
    int getOptionCount() const;
    
    // 配置选项设置
    void setIndexInitialCount(int count);
    void setCustomStyleEnabled(bool enabled);
    void setNameGroupDisabled(bool disabled);
    void setDebugModeEnabled(bool enabled);
    void setOptionCount(int count);
    
    // Name块管理
    std::shared_ptr<NameBlockNode> findNameBlock(const std::string& groupName) const;
    std::vector<std::shared_ptr<NameBlockNode>> getAllNameBlocks() const;
    
    // 自定义关键字解析
    std::string resolveCustomKeyword(const std::string& keyword) const;
    bool isCustomKeyword(const std::string& keyword) const;
    std::unordered_map<std::string, std::string> getAllCustomKeywords() const;
    
    // 配置验证
    bool validateAllConfigurations() const;
    std::vector<std::string> getValidationErrors() const;
    
    // 配置应用
    bool applyConfiguration();
    bool isConfigurationApplied() const;
    void resetConfiguration();
    
    // 默认配置
    void loadDefaultConfiguration();
    std::shared_ptr<ConfigurationNode> createDefaultConfiguration() const;
    
    // 配置序列化
    std::string exportConfiguration() const;
    bool importConfiguration(const std::string& configString);
    
    // 统计信息
    size_t getConfigOptionCount() const;
    size_t getNameBlockCount() const;
    size_t getCustomKeywordCount() const;
    
    // 调试信息
    std::string getDebugInfo() const;
    
    // 清理
    void clear();
    
private:
    ConfigurationManager() = default;
    ~ConfigurationManager() = default;
    ConfigurationManager(const ConfigurationManager&) = delete;
    ConfigurationManager& operator=(const ConfigurationManager&) = delete;
    
    std::shared_ptr<ConfigurationNode> currentConfiguration_;
    bool isApplied_;
    
    // 默认配置值
    static const int DEFAULT_INDEX_INITIAL_COUNT = 0;
    static const bool DEFAULT_CUSTOM_STYLE = true;
    static const bool DEFAULT_DISABLE_NAME_GROUP = false;
    static const bool DEFAULT_DEBUG_MODE = false;
    static const int DEFAULT_OPTION_COUNT = 10;
    
    void initializeDefaults();
};

/**
 * 配置解析器
 * 用于解析配置语法
 */
class ConfigurationParser {
public:
    explicit ConfigurationParser();
    ~ConfigurationParser() = default;
    
    // 解析配置块
    std::shared_ptr<ConfigurationNode> parseConfiguration(const std::string& configText);
    std::shared_ptr<ConfigOptionNode> parseConfigOption(const std::string& optionLine);
    std::shared_ptr<NameBlockNode> parseNameBlock(const std::string& nameBlockText);
    
    // 解析配置选项
    ConfigOptionNode::OptionType parseOptionType(const std::string& optionName);
    ConfigOptionNode::ConfigValue parseOptionValue(const std::string& valueString, 
                                                  ConfigOptionNode::OptionType type);
    
    // 解析Name块选项
    std::pair<std::string, std::string> parseGroupOption(const std::string& optionLine);
    std::pair<std::string, std::string> parseCustomKeyword(const std::string& keywordLine);
    
    // 语法验证
    bool isValidConfigurationSyntax(const std::string& configText);
    bool isValidOptionSyntax(const std::string& optionLine);
    bool isValidNameBlockSyntax(const std::string& nameBlockText);
    
    // 错误处理
    std::string getLastError() const;
    bool hasError() const;
    void clearErrors();
    std::vector<std::string> getAllErrors() const;
    
private:
    std::vector<std::string> errors_;
    
    void addError(const std::string& error);
    std::string trim(const std::string& str) const;
    std::vector<std::string> splitLines(const std::string& text) const;
    bool isCommentLine(const std::string& line) const;
    bool isEmptyLine(const std::string& line) const;
};

/**
 * 配置工厂
 * 用于创建常用配置
 */
class ConfigurationFactory {
public:
    // 创建默认配置
    static std::shared_ptr<ConfigurationNode> createDefaultConfiguration();
    
    // 创建开发模式配置
    static std::shared_ptr<ConfigurationNode> createDevelopmentConfiguration();
    
    // 创建生产模式配置
    static std::shared_ptr<ConfigurationNode> createProductionConfiguration();
    
    // 创建测试模式配置
    static std::shared_ptr<ConfigurationNode> createTestConfiguration();
    
    // 创建配置选项
    static std::shared_ptr<ConfigOptionNode> createIndexInitialCountOption(int count);
    static std::shared_ptr<ConfigOptionNode> createCustomStyleOption(bool enabled);
    static std::shared_ptr<ConfigOptionNode> createDisableNameGroupOption(bool disabled);
    static std::shared_ptr<ConfigOptionNode> createDebugModeOption(bool enabled);
    static std::shared_ptr<ConfigOptionNode> createOptionCountOption(int count);
    
    // 创建Name块
    static std::shared_ptr<NameBlockNode> createBasicNameBlock(const std::string& groupName);
    static std::shared_ptr<NameBlockNode> createCustomKeywordNameBlock(
        const std::string& groupName,
        const std::unordered_map<std::string, std::string>& keywords);
    
private:
    ConfigurationFactory() = default;
    ~ConfigurationFactory() = default;
};

} // namespace chtl