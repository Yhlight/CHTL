#ifndef CHTL_BASIC_LEXER_H
#define CHTL_BASIC_LEXER_H

#include "Lexer.h"
#include <string>
#include <cctype>

namespace chtl {

// BasicLexer - 处理CHTL的基础语法
class BasicLexer : public Lexer {
protected:
    // 辅助方法
    bool isAlpha(char ch) const;
    bool isDigit(char ch) const;
    bool isAlphaNumeric(char ch) const;
    bool isIdentifierStart(char ch) const;
    bool isIdentifierChar(char ch) const;
    
private:
    
    // 处理特定类型的token
    void scanString(char quote);
    virtual void scanIdentifier();
    void scanNumber();
    void scanComment();
    void scanBracketKeyword();
    void scanAtKeyword();
    void scanStyleSelector();
    void scanIndexAccess();
    void scanUnquotedString();
    
    // 检查是否需要无引号字符串
    bool shouldScanUnquotedString() const;
    
protected:
    // 实现父类的虚函数
    void handleNormalState() override;
    void handleStringState(char quote) override;
    void handleCommentState() override;
    void handleIdentifierState() override;
    void handleNumberState() override;
    void handleBracketKeywordState() override;
    void handleAtKeywordState() override;
    void handleStyleSelectorState() override;

public:
    BasicLexer();
    ~BasicLexer() override = default;
    
    // 主要的词法分析接口
    std::vector<Token> tokenize(const std::string& input) override;
};

} // namespace chtl

#endif // CHTL_BASIC_LEXER_H