#pragma once
#include "../common/Token.h"
#include "../common/Context.h"
#include <string>
#include <vector>
#include <memory>

namespace chtl {

/**
 * 基础词法分析器
 * 负责处理CHTL的基础语法，不包括Config机制的自定义关键字
 * 严重依赖状态机与上下文推导
 */
class BasicLexer {
public:
    explicit BasicLexer(const std::string& input);
    virtual ~BasicLexer() = default;
    
    // 主要接口
    std::vector<Token> tokenize();
    Token nextToken();
    bool hasNext() const;
    
    // 位置信息
    Position getCurrentPosition() const;
    size_t getCurrentOffset() const;
    
    // 错误处理
    bool hasError() const;
    std::string getLastError() const;
    void clearError();
    
    // 调试信息
    std::string getDebugInfo() const;
    
protected:
    // 核心tokenization方法
    virtual Token tokenizeNext();
    
    // 字符处理
    char currentChar() const;
    char peekChar(int offset = 1) const;
    void advance();
    void skipWhitespace();
    void skipNewlines();
    
    // 状态机相关
    virtual void processStateTransition(char c);
    virtual TokenType inferTokenType(const std::string& value) const;
    
    // 具体Token识别方法
    Token readIdentifier();
    Token readStringDoubleQuote();
    Token readStringSingleQuote();
    Token readStringNoQuote();
    Token readNumber();
    Token readComment();
    Token readCssSelector();
    Token readAttribute();
    Token readIndex();
    
    // 上下文推导辅助
    virtual bool shouldTreatAsHtmlTag(const std::string& identifier) const;
    virtual bool shouldTreatAsCssSelector(const std::string& identifier) const;
    virtual bool shouldTreatAsCustomElement(const std::string& identifier) const;
    virtual bool shouldTreatAsVariable(const std::string& identifier) const;
    
    // 特殊语法处理
    Token handleAtSymbol();
    Token handleAmpersand();
    Token handleDot();
    Token handleHash();
    Token handleColon();
    Token handleLeftBracket();
    
    // 错误处理
    void setError(const std::string& message);
    Token createErrorToken(const std::string& message);
    
private:
    // 输入数据
    std::string input_;
    size_t position_;
    Position currentPosition_;
    
    // 状态管理
    std::unique_ptr<ContextManager> contextManager_;
    std::unique_ptr<StateMachine> stateMachine_;
    
    // 当前解析状态
    std::string currentValue_;
    LexerState currentState_;
    
    // 错误处理
    bool hasError_;
    std::string lastError_;
    
    // 辅助方法
    void updatePosition(char c);
    bool isAtEnd() const;
    std::string getCurrentToken() const;
    
    // 预读辅助
    std::string peekWord(size_t maxLength = 20) const;
    bool matchKeyword(const std::string& keyword) const;
    bool matchSequence(const std::string& sequence) const;
    
    // Token创建辅助
    Token createToken(TokenType type, const std::string& value = "") const;
    Token createToken(TokenType type, const std::string& value, const Position& pos) const;
    
    // 状态重置
    void resetCurrentToken();
    void saveCurrentState();
    void restoreState();
    
    // 特殊字符序列处理
    bool tryReadMultiCharOperator();
    bool tryReadBracketSequence();
    bool tryReadCommentSequence();
    
    // 上下文敏感的处理
    void handleContextSwitch(TokenType tokenType);
    void updateContextFromToken(const Token& token);
    
    // 验证方法
    bool isValidIdentifierStart(char c) const;
    bool isValidIdentifierContinue(char c) const;
    bool isValidInCurrentContext(const std::string& value) const;
};

/**
 * 词法分析器工厂
 * 用于创建不同类型的词法分析器
 */
class LexerFactory {
public:
    // 创建基础词法分析器
    static std::unique_ptr<BasicLexer> createBasicLexer(const std::string& input);
    
    // 创建配置词法分析器
    static std::unique_ptr<BasicLexer> createConfigLexer(const std::string& input);
    
    // 根据上下文自动选择词法分析器
    static std::unique_ptr<BasicLexer> createLexer(const std::string& input, ParseContext initialContext);
};

} // namespace chtl