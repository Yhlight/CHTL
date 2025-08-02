#ifndef CHTL_CONFIG_PARSER_H
#define CHTL_CONFIG_PARSER_H

#include "BasicParser.h"
#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace chtl {

// 配置验证结果
struct ConfigValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ConfigValidationResult() : isValid(true) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        isValid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// 配置缓存
class ConfigCache {
public:
    struct CacheEntry {
        std::shared_ptr<ConfigurationNode> configNode;
        std::map<std::string, std::string> configMap;
        std::map<std::string, std::vector<std::string>> optionsMap;
        std::unordered_map<std::string, TokenType> dynamicKeywords;
        size_t hash;
    };
    
private:
    static std::unordered_map<size_t, CacheEntry> cache;
    static const size_t MAX_CACHE_SIZE = 100;
    
public:
    static bool get(size_t hash, CacheEntry& entry);
    static void put(size_t hash, const CacheEntry& entry);
    static void clear();
    static size_t computeHash(const std::vector<Token>& tokens);
};

// 配置驱动的解析器
class ConfigParser : public BasicParser {
private:
    // 配置映射表
    std::map<std::string, std::string> configMap;
    
    // 组选项映射表
    std::map<std::string, std::vector<std::string>> optionsMap;
    
    // 是否已经应用了配置
    bool configApplied;
    
    // 动态关键字映射 - 使用unordered_map提升查找性能
    std::unordered_map<std::string, TokenType> dynamicKeywords;
    
    // 反向映射 - TokenType到所有可能的关键字
    std::unordered_map<TokenType, std::unordered_set<std::string>> reverseKeywordMap;
    
    // 配置验证结果
    ConfigValidationResult validationResult;
    
    // 性能统计
    mutable size_t dynamicKeywordLookups;
    mutable size_t dynamicKeywordHits;
    
    // 将配置值转换为TokenType
    TokenType configValueToTokenType(const std::string& configKey, const std::string& value);
    
    // 应用配置到解析器
    void applyConfiguration(ConfigurationNode* config);
    
    // 处理[Name]块
    void applyNameBlock(ConfigNameBlockNode* nameBlock);
    
    // 重映射关键字
    void remapKeyword(const std::string& oldKeyword, const std::string& newKeyword);
    
    // 检查是否是动态关键字
    bool isDynamicKeyword(const std::string& text) const;
    
    // 获取动态关键字类型
    TokenType getDynamicKeywordType(const std::string& text) const;
    
    // 验证配置
    void validateConfiguration(ConfigurationNode* config);
    
    // 检查关键字冲突
    void checkKeywordConflicts();
    
    // 检查循环定义
    void checkCircularDefinitions();
    
    // 验证配置值
    bool isValidConfigValue(const std::string& key, const std::string& value);
    
    // 优化动态关键字映射
    void optimizeDynamicKeywordMap();
    
protected:
    // 重写parseStatement以支持动态关键字
    NodePtr parseStatement() override;
    
    // 重写各种解析方法以支持配置驱动
    NodePtr parseCustomDefinition() override;
    NodePtr parseTemplateDefinition() override;
    NodePtr parseOrigin() override;
    
    // 解析动态关键字
    NodePtr parseDynamicKeyword(const std::string& keyword, TokenType originalType);
    
    // 检查并匹配动态Token
    bool matchDynamic(TokenType expectedType);
    
    // 消费动态Token
    Token consumeDynamic(TokenType expectedType, const std::string& message);
    
public:
    ConfigParser();
    explicit ConfigParser(std::shared_ptr<ChtlLoader> fileLoader);
    virtual ~ConfigParser() = default;
    
    // 主解析接口 - 重写以先查找配置
    NodePtr parse(const std::vector<Token>& tokenList) override;
    
    // 设置默认配置
    void setDefaultConfig();
    
    // 获取配置值
    std::string getConfig(const std::string& key) const;
    
    // 获取组选项
    std::vector<std::string> getOptions(const std::string& key) const;
    
    // 判断是否启用调试模式
    bool isDebugMode() const;
    
    // 获取索引起始值
    int getIndexInitialCount() const;
    
    // 获取验证结果
    const ConfigValidationResult& getValidationResult() const { return validationResult; }
    
    // 打印性能统计
    void printPerformanceStats() const;
    
    // 启用/禁用缓存
    static void enableCache(bool enable);
    
    // 预热缓存
    void warmupCache(const std::string& configContent);
};

} // namespace chtl

#endif // CHTL_CONFIG_PARSER_H