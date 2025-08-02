#ifndef CHTL_CONFIG_LEXER_H
#define CHTL_CONFIG_LEXER_H

#include "BasicLexer.h"
#include "../node/Config.h"
#include <map>
#include <set>
#include <vector>

namespace chtl {

// 配置项
struct ConfigItem {
    std::string key;
    std::string value;
    std::vector<std::string> options;  // 组选项
};

// 配置信息
struct Configuration {
    std::map<std::string, ConfigItem> items;
    bool hasNameBlock = false;
    bool disableNameGroup = false;
    int optionCount = 1;
    
    // 获取配置值
    std::string getValue(const std::string& key) const {
        auto it = items.find(key);
        if (it != items.end()) {
            return it->second.value;
        }
        return "";
    }
    
    // 获取组选项
    std::vector<std::string> getOptions(const std::string& key) const {
        auto it = items.find(key);
        if (it != items.end()) {
            return it->second.options;
        }
        return {};
    }
};

// ConfigLexer - 支持配置驱动的词法分析
class ConfigLexer : public BasicLexer {
private:
    Configuration config;
    std::map<std::string, TokenType> customKeywordMap;  // 自定义关键字映射
    std::set<std::string> allPossibleKeywords;          // 所有可能的关键字
    
    // 初始化自定义关键字映射
    void initializeCustomKeywords();
    
    // 检查是否是自定义关键字
    bool isCustomKeyword(const std::string& str) const;
    
    // 获取自定义关键字的Token类型
    TokenType getCustomKeywordType(const std::string& str) const;
    
    // 处理配置驱动的标识符
    void handleConfigDrivenIdentifier(const std::string& identifier);
    
    // 更新关键字映射（基于配置）
    void updateKeywordMappings();
    
protected:
    // 重写父类方法以支持配置驱动
    void handleIdentifierState() override;
    void scanIdentifier() override;
    
public:
    ConfigLexer();
    ~ConfigLexer() override = default;
    
    // 设置配置
    void setConfiguration(const Configuration& cfg);
    
    // 获取配置
    const Configuration& getConfiguration() const { return config; }
    
    // 主要的词法分析接口（重写以支持配置）
    std::vector<Token> tokenize(const std::string& input) override;
    
    // 从配置节点构建配置
    void buildConfigFromNode(const ConfigurationNode* node);
};

// 配置解析器 - 专门用于解析[Configuration]块
class ConfigurationParser {
private:
    std::vector<Token> tokens;
    size_t current;
    
    Token peek(size_t offset = 0) const;
    Token advance();
    bool isAtEnd() const;
    bool match(TokenType type);
    bool check(TokenType type) const;
    Token consume(TokenType type, const std::string& message);
    
public:
    ConfigurationParser();
    
    // 解析配置块
    Configuration parseConfiguration(const std::string& configBlock);
    
    // 解析配置项
    ConfigItem parseConfigItem();
    
    // 解析组选项
    std::vector<std::string> parseOptions();
};

} // namespace chtl

#endif // CHTL_CONFIG_LEXER_H