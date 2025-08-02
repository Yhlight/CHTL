#ifndef CHTL_CONFIG_PARSER_H
#define CHTL_CONFIG_PARSER_H

#include "BasicParser.h"
#include <map>
#include <string>
#include <vector>

namespace chtl {

// 配置驱动的解析器
class ConfigParser : public BasicParser {
private:
    // 配置映射表
    std::map<std::string, std::string> configMap;
    
    // 组选项映射表
    std::map<std::string, std::vector<std::string>> optionsMap;
    
    // 是否已经应用了配置
    bool configApplied;
    
    // 动态关键字映射
    std::map<std::string, TokenType> dynamicKeywords;
    
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
};

} // namespace chtl

#endif // CHTL_CONFIG_PARSER_H