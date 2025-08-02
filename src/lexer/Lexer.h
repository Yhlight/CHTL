#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include <vector>
#include <memory>
#include "../common/Token.h"

namespace chtl {

// Lexer状态枚举
enum class LexerState {
    NORMAL,                    // 正常状态
    IN_STRING_DOUBLE,          // 在双引号字符串中
    IN_STRING_SINGLE,          // 在单引号字符串中
    IN_COMMENT_SINGLE,         // 在单行注释中
    IN_COMMENT_MULTI,          // 在多行注释中
    IN_COMMENT_GENERATOR,      // 在生成器注释中
    IN_IDENTIFIER,             // 在标识符中
    IN_NUMBER,                 // 在数字中
    IN_BRACKET_KEYWORD,        // 在方括号关键字中
    IN_AT_KEYWORD,             // 在@前缀关键字中
    IN_STYLE_SELECTOR,         // 在样式选择器中
    IN_UNQUOTED_STRING,        // 在无引号字符串中
    IN_INDEX_ACCESS,           // 在索引访问中
};

// Lexer上下文，用于辅助状态机进行上下文推导
struct LexerContext {
    bool inStyleBlock = false;      // 是否在style块中
    bool inAttributeValue = false;  // 是否在属性值位置
    bool expectKeyword = false;     // 是否期待关键字
    int braceDepth = 0;             // 花括号深度
    int bracketDepth = 0;           // 方括号深度
    int parenDepth = 0;             // 圆括号深度
    
    // 用于处理配置驱动的关键字
    std::map<std::string, TokenType> configKeywords;
};

// Lexer基类
class Lexer {
protected:
    std::string input;              // 输入字符串
    size_t position;                // 当前位置
    int line;                       // 当前行号
    int column;                     // 当前列号
    LexerState state;               // 当前状态
    LexerContext context;           // 上下文信息
    std::vector<Token> tokens;      // Token列表
    
    // 辅助方法
    char peek(size_t offset = 0) const;
    char advance();
    void skipWhitespace();
    bool isAtEnd() const;
    bool match(char expected);
    bool matchString(const std::string& expected);
    
    // 状态处理方法
    virtual void handleNormalState() = 0;
    virtual void handleStringState(char quote) = 0;
    virtual void handleCommentState() = 0;
    virtual void handleIdentifierState() = 0;
    virtual void handleNumberState() = 0;
    virtual void handleBracketKeywordState() = 0;
    virtual void handleAtKeywordState() = 0;
    virtual void handleStyleSelectorState() = 0;
    
    // 创建Token
    void addToken(TokenType type, const std::string& value);
    void addToken(TokenType type);
    
    // 错误处理
    void reportError(const std::string& message);

public:
    Lexer();
    virtual ~Lexer() = default;
    
    // 主要接口
    virtual std::vector<Token> tokenize(const std::string& input) = 0;
    
    // 获取和设置上下文
    LexerContext& getContext() { return context; }
    const LexerContext& getContext() const { return context; }
};

} // namespace chtl

#endif // CHTL_LEXER_H