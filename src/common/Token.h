#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace chtl {

/**
 * Token类型枚举
 * 严格按照CHTL语法文档定义所有Token类型
 */
enum class TokenType {
    // 基础Token
    UNKNOWN = 0,
    EOF_TOKEN,
    NEWLINE,
    WHITESPACE,
    
    // 标识符和字面量
    IDENTIFIER,                 // 标识符
    STRING_DOUBLE_QUOTE,        // "string"
    STRING_SINGLE_QUOTE,        // 'string'
    STRING_NO_QUOTE,            // 无引号字符串
    NUMBER,                     // 数字
    
    // 运算符和分隔符
    SEMICOLON,                  // ;
    COLON,                      // :
    EQUAL,                      // =
    LEFT_BRACE,                 // {
    RIGHT_BRACE,                // }
    LEFT_BRACKET,               // [
    RIGHT_BRACKET,              // ]
    LEFT_PAREN,                 // (
    RIGHT_PAREN,                // )
    DOT,                        // .
    COMMA,                      // ,
    AMPERSAND,                  // &
    HASH,                       // #
    AT,                         // @
    
    // 注释
    COMMENT_SINGLE,             // //
    COMMENT_MULTI,              // /* */
    COMMENT_GENERATOR,          // --
    
    // 基础关键字
    TEXT,                       // text
    STYLE,                      // style
    
    // 自定义关键字(可配置)
    CUSTOM,                     // [Custom]
    TEMPLATE,                   // [Template]
    ORIGIN,                     // [Origin]
    CONFIGURATION,              // [Configuration]
    IMPORT,                     // [Import]
    NAMESPACE,                  // [Namespace]
    
    // 操作符关键字
    ADD,                        // add
    DELETE,                     // delete
    INHERIT,                    // inherit
    FROM,                       // from
    AS,                         // as
    EXCEPT,                     // except
    NOT,                        // not
    
    // 原始嵌入关键字
    ORIGIN_HTML,                // @Html
    ORIGIN_STYLE,               // @Style
    ORIGIN_JAVASCRIPT,          // @JavaScript
    
    // 自定义类型关键字
    CUSTOM_STYLE,               // @Style
    CUSTOM_ELEMENT,             // @Element
    CUSTOM_VAR,                 // @Var
    
    // 模板类型关键字
    TEMPLATE_STYLE,             // @Style (在Template上下文中)
    TEMPLATE_ELEMENT,           // @Element (在Template上下文中)
    TEMPLATE_VAR,               // @Var (在Template上下文中)
    
    // HTML标签
    HTML_TAG,                   // 任意HTML标签名
    
    // CSS选择器相关
    CLASS_SELECTOR,             // .class
    ID_SELECTOR,                // #id
    PSEUDO_CLASS,               // :hover, :active等
    PSEUDO_ELEMENT,             // ::before, ::after等
    
    // 配置相关
    CONFIG_KEY,                 // 配置键
    CONFIG_VALUE,               // 配置值
    CONFIG_OPTION_LIST,         // [option1, option2, ...]
    
    // 索引访问
    INDEX,                      // [0], [1], [index]等
    
    // 特殊状态
    CONTEXT_SWITCH,             // 上下文切换标记
    STATE_CHANGE                // 状态变化标记
};

/**
 * Token位置信息
 */
struct Position {
    size_t line;
    size_t column;
    size_t offset;
    
    Position(size_t l = 1, size_t c = 1, size_t o = 0) 
        : line(l), column(c), offset(o) {}
};

/**
 * Token结构
 */
struct Token {
    TokenType type;
    std::string value;
    Position position;
    
    // 上下文信息
    bool isInStyleBlock = false;
    bool isInCustomBlock = false;
    bool isInTemplateBlock = false;
    bool isInConfigBlock = false;
    bool isInOriginBlock = false;
    
    Token(TokenType t = TokenType::UNKNOWN, 
          const std::string& v = "", 
          const Position& pos = Position())
        : type(t), value(v), position(pos) {}
};

/**
 * 关键字映射表
 * 用于快速查找和识别关键字
 */
class KeywordRegistry {
public:
    static KeywordRegistry& getInstance();
    
    // 检查是否为关键字
    bool isKeyword(const std::string& word) const;
    
    // 获取关键字对应的Token类型
    TokenType getKeywordType(const std::string& word) const;
    
    // 更新自定义关键字(用于Config机制)
    void updateCustomKeyword(const std::string& configKey, const std::string& keyword);
    
    // 重置为默认关键字
    void resetToDefault();
    
private:
    KeywordRegistry();
    void initializeDefaultKeywords();
    
    std::unordered_map<std::string, TokenType> keywords_;
    std::unordered_map<std::string, TokenType> defaultKeywords_;
};

/**
 * HTML标签注册表
 */
class HtmlTagRegistry {
public:
    static HtmlTagRegistry& getInstance();
    
    // 检查是否为有效的HTML标签
    bool isValidHtmlTag(const std::string& tag) const;
    
    // 检查是否为自闭合标签
    bool isSelfClosingTag(const std::string& tag) const;
    
private:
    HtmlTagRegistry();
    void initializeHtmlTags();
    
    std::unordered_set<std::string> htmlTags_;
    std::unordered_set<std::string> selfClosingTags_;
};

/**
 * Token工具函数
 */
class TokenUtils {
public:
    // 将TokenType转换为字符串
    static std::string tokenTypeToString(TokenType type);
    
    // 检查Token是否为字面量
    static bool isLiteral(TokenType type);
    
    // 检查Token是否为运算符
    static bool isOperator(TokenType type);
    
    // 检查Token是否为关键字
    static bool isKeyword(TokenType type);
    
    // 检查Token是否为分隔符
    static bool isDelimiter(TokenType type);
    
    // 检查Token是否需要上下文推导
    static bool needsContextInference(TokenType type);
};

} // namespace chtl