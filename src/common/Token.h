#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <map>

namespace chtl {

// Token类型枚举
enum class TokenType {
    // 特殊标记
    END_OF_FILE,
    UNKNOWN,
    
    // 字面量
    IDENTIFIER,          // 标识符
    STRING_LITERAL,      // 字符串字面量（双引号）
    STRING_LITERAL_SINGLE, // 字符串字面量（单引号）
    UNQUOTED_STRING,     // 无引号字符串
    NUMBER,              // 数字
    
    // 注释
    COMMENT_SINGLE,      // // 注释
    COMMENT_MULTI,       // /* */ 注释
    COMMENT_GENERATOR,   // -- 生成器注释
    
    // 符号
    LEFT_BRACE,          // {
    RIGHT_BRACE,         // }
    LEFT_BRACKET,        // [
    RIGHT_BRACKET,       // ]
    LEFT_PAREN,          // (
    RIGHT_PAREN,         // )
    SEMICOLON,           // ;
    COLON,               // :
    EQUALS,              // =
    DOT,                 // .
    AT,                  // @
    AMPERSAND,           // &
    COMMA,               // ,
    SLASH,               // /
    ASTERISK,            // *
    
    // 字面量
    STRING,              // 字符串字面量
    COMMENT,             // 注释
    
    // 基础关键字
    KEYWORD_TEXT,        // text
    KEYWORD_STYLE,       // style
    KEYWORD_EXPECT,      // expect
    KEYWORD_NOT,         // not
    
    // 操作符关键字
    KEYWORD_ADD,         // add
    KEYWORD_DELETE,      // delete
    KEYWORD_FROM,        // from
    KEYWORD_AS,          // as
    KEYWORD_INHERIT,     // inherit
    
    // 配置关键字（方括号组合）
    KEYWORD_CUSTOM,      // [Custom]
    KEYWORD_TEMPLATE,    // [Template]
    KEYWORD_ORIGIN,      // [Origin]
    KEYWORD_IMPORT,      // [Import]
    KEYWORD_NAMESPACE,   // [Namespace]
    KEYWORD_CONFIGURATION, // [Configuration]
    
    // 配置驱动的关键字标记
    CONFIG_KEYWORD,      // 用于标记配置驱动的自定义关键字
    
    // @前缀关键字
    AT_STYLE,            // @Style
    AT_ELEMENT,          // @Element
    AT_VAR,              // @Var
    AT_HTML,             // @Html
    AT_JAVASCRIPT,       // @JavaScript
    AT_CHTL,             // @Chtl
    
    // 特殊标记
    STYLE_SELECTOR_CLASS,    // .className
    STYLE_SELECTOR_ID,       // #idName
    STYLE_SELECTOR_PSEUDO,   // :hover等
    STYLE_SELECTOR_PSEUDO_ELEMENT, // ::after等
    
    // 索引访问
    INDEX_ACCESS,        // [数字]
};

// Token结构体
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token() : type(TokenType::UNKNOWN), line(0), column(0) {}
    
    Token(TokenType t, const std::string& v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
};

// Token类型到字符串的映射
extern const std::map<TokenType, std::string> tokenTypeToString;

// 关键字到Token类型的映射
extern const std::map<std::string, TokenType> keywordToTokenType;

// 判断是否是关键字
bool isKeyword(const std::string& str);

// 获取关键字的Token类型
TokenType getKeywordType(const std::string& str);

// Token类型转字符串
std::string tokenTypeToStr(TokenType type);

} // namespace chtl

#endif // CHTL_TOKEN_H