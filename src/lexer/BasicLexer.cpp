#include "BasicLexer.h"
#include <cctype>
#include <algorithm>

namespace chtl {

BasicLexer::BasicLexer() : Lexer() {
}

bool BasicLexer::isAlpha(char ch) const {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool BasicLexer::isDigit(char ch) const {
    return ch >= '0' && ch <= '9';
}

bool BasicLexer::isAlphaNumeric(char ch) const {
    return isAlpha(ch) || isDigit(ch);
}

bool BasicLexer::isIdentifierStart(char ch) const {
    return isAlpha(ch) || ch == '_';
}

bool BasicLexer::isIdentifierChar(char ch) const {
    return isAlphaNumeric(ch) || ch == '_' || ch == '-';
}

std::vector<Token> BasicLexer::tokenize(const std::string& inputStr) {
    input = inputStr;
    position = 0;
    line = 1;
    column = 1;
    state = LexerState::NORMAL;
    tokens.clear();
    
    while (!isAtEnd()) {
        switch (state) {
            case LexerState::NORMAL:
                handleNormalState();
                break;
            case LexerState::IN_STRING_DOUBLE:
                handleStringState('"');
                break;
            case LexerState::IN_STRING_SINGLE:
                handleStringState('\'');
                break;
            case LexerState::IN_COMMENT_SINGLE:
            case LexerState::IN_COMMENT_MULTI:
            case LexerState::IN_COMMENT_GENERATOR:
                handleCommentState();
                break;
            case LexerState::IN_IDENTIFIER:
                handleIdentifierState();
                break;
            case LexerState::IN_NUMBER:
                handleNumberState();
                break;
            case LexerState::IN_BRACKET_KEYWORD:
                handleBracketKeywordState();
                break;
            case LexerState::IN_AT_KEYWORD:
                handleAtKeywordState();
                break;
            case LexerState::IN_STYLE_SELECTOR:
                handleStyleSelectorState();
                break;
            case LexerState::IN_UNQUOTED_STRING:
                scanUnquotedString();
                break;
            case LexerState::IN_INDEX_ACCESS:
                scanIndexAccess();
                break;
        }
    }
    
    // 添加EOF token
    addToken(TokenType::END_OF_FILE);
    return tokens;
}

void BasicLexer::handleNormalState() {
    skipWhitespace();
    if (isAtEnd()) return;
    
    char ch = peek();
    
    // 处理注释
    if (ch == '/') {
        if (peek(1) == '/') {
            state = LexerState::IN_COMMENT_SINGLE;
            return;
        } else if (peek(1) == '*') {
            state = LexerState::IN_COMMENT_MULTI;
            return;
        }
    }
    
    // 处理生成器注释
    if (ch == '-' && peek(1) == '-') {
        state = LexerState::IN_COMMENT_GENERATOR;
        return;
    }
    
    // 处理字符串
    if (ch == '"') {
        state = LexerState::IN_STRING_DOUBLE;
        advance(); // 跳过引号
        return;
    }
    if (ch == '\'') {
        state = LexerState::IN_STRING_SINGLE;
        advance(); // 跳过引号
        return;
    }
    
    // 处理方括号关键字
    if (ch == '[') {
        state = LexerState::IN_BRACKET_KEYWORD;
        return;
    }
    
    // 处理@前缀关键字
    if (ch == '@') {
        state = LexerState::IN_AT_KEYWORD;
        return;
    }
    
    // 处理样式选择器（在style块中）
    if (context.inStyleBlock && (ch == '.' || ch == '#' || ch == '&')) {
        state = LexerState::IN_STYLE_SELECTOR;
        return;
    }
    
    // 处理单字符符号
    switch (ch) {
        case '{':
            advance();
            addToken(TokenType::LEFT_BRACE, "{");
            context.braceDepth++;
            return;
        case '}':
            advance();
            addToken(TokenType::RIGHT_BRACE, "}");
            context.braceDepth--;
            return;
        case '(':
            advance();
            addToken(TokenType::LEFT_PAREN, "(");
            context.parenDepth++;
            return;
        case ')':
            advance();
            addToken(TokenType::RIGHT_PAREN, ")");
            context.parenDepth--;
            return;
        case ';':
            advance();
            addToken(TokenType::SEMICOLON, ";");
            context.inAttributeValue = false;
            return;
        case ':':
            advance();
            addToken(TokenType::COLON, ":");
            context.inAttributeValue = true;
            return;
        case '=':
            advance();
            addToken(TokenType::EQUALS, "=");
            context.inAttributeValue = true;
            return;
        case '.':
            // 如果不在style块中，点号是普通符号
            if (!context.inStyleBlock) {
                advance();
                addToken(TokenType::DOT, ".");
                return;
            }
            break;
                        case ',':
                    advance();
                    addToken(TokenType::COMMA, ",");
                    return;
                    
                case '/':
                    advance();
                    addToken(TokenType::SLASH, "/");
                    return;
                    
                case '*':
                    advance();
                    addToken(TokenType::ASTERISK, "*");
                    return;
            }
    
    // 处理数字
    if (isDigit(ch)) {
        state = LexerState::IN_NUMBER;
        return;
    }
    
    // 处理标识符
    if (isIdentifierStart(ch)) {
        state = LexerState::IN_IDENTIFIER;
        return;
    }
    
    // 处理无引号字符串（在属性值位置）
    if (shouldScanUnquotedString()) {
        state = LexerState::IN_UNQUOTED_STRING;
        return;
    }
    
    // 未知字符
    reportError("Unexpected character: " + std::string(1, ch));
}

void BasicLexer::handleStringState(char quote) {
    scanString(quote);
}

void BasicLexer::handleCommentState() {
    scanComment();
}

void BasicLexer::handleIdentifierState() {
    scanIdentifier();
}

void BasicLexer::handleNumberState() {
    scanNumber();
}

void BasicLexer::handleBracketKeywordState() {
    scanBracketKeyword();
}

void BasicLexer::handleAtKeywordState() {
    scanAtKeyword();
}

void BasicLexer::handleStyleSelectorState() {
    scanStyleSelector();
}

void BasicLexer::scanString(char quote) {
    std::string value;
    int startLine = line;
    int startColumn = column;
    
    while (!isAtEnd() && peek() != quote) {
        if (peek() == '\\' && peek(1) == quote) {
            // 转义引号
            advance(); // 跳过反斜杠
            value += advance();
        } else if (peek() == '\n') {
            reportError("Unterminated string at line " + std::to_string(startLine));
        } else {
            value += advance();
        }
    }
    
    if (isAtEnd()) {
        reportError("Unterminated string");
    }
    
    advance(); // 跳过结束引号
    
    TokenType type = (quote == '"') ? TokenType::STRING_LITERAL : TokenType::STRING_LITERAL_SINGLE;
    addToken(type, value);
    state = LexerState::NORMAL;
}

void BasicLexer::scanIdentifier() {
    std::string value;
    int startColumn = column;
    
    while (!isAtEnd() && isIdentifierChar(peek())) {
        value += advance();
    }
    
    // 检查是否是关键字
    TokenType type = getKeywordType(value);
    
    // 特殊处理：如果是"style"关键字，设置上下文
    if (type == TokenType::KEYWORD_STYLE) {
        context.inStyleBlock = true;
    }
    
    addToken(type, value);
    state = LexerState::NORMAL;
}

void BasicLexer::scanNumber() {
    std::string value;
    
    while (!isAtEnd() && isDigit(peek())) {
        value += advance();
    }
    
    // 处理小数点
    if (peek() == '.' && isDigit(peek(1))) {
        value += advance(); // 添加点号
        while (!isAtEnd() && isDigit(peek())) {
            value += advance();
        }
    }
    
    // 处理单位（如px, rem等）
    if (isAlpha(peek())) {
        while (!isAtEnd() && isAlpha(peek())) {
            value += advance();
        }
    }
    
    addToken(TokenType::NUMBER, value);
    state = LexerState::NORMAL;
}

void BasicLexer::scanComment() {
    std::string value;
    
    if (state == LexerState::IN_COMMENT_SINGLE) {
        advance(); advance(); // 跳过 //
        while (!isAtEnd() && peek() != '\n') {
            value += advance();
        }
        addToken(TokenType::COMMENT_SINGLE, value);
    } else if (state == LexerState::IN_COMMENT_MULTI) {
        advance(); advance(); // 跳过 /*
        while (!isAtEnd()) {
            if (peek() == '*' && peek(1) == '/') {
                advance(); advance(); // 跳过 */
                break;
            }
            value += advance();
        }
        addToken(TokenType::COMMENT_MULTI, value);
    } else if (state == LexerState::IN_COMMENT_GENERATOR) {
        advance(); advance(); // 跳过 --
        while (!isAtEnd() && peek() != '\n') {
            value += advance();
        }
        addToken(TokenType::COMMENT_GENERATOR, value);
    }
    
    state = LexerState::NORMAL;
}

void BasicLexer::scanBracketKeyword() {
    std::string value;
    advance(); // 跳过 [
    
    while (!isAtEnd() && peek() != ']') {
        value += advance();
    }
    
    if (isAtEnd()) {
        reportError("Unterminated bracket keyword");
    }
    
    advance(); // 跳过 ]
    
    // 完整的方括号关键字
    std::string fullKeyword = "[" + value + "]";
    TokenType type = getKeywordType(fullKeyword);
    
    // 检查是否是索引访问（纯数字）
    bool isIndex = true;
    for (char ch : value) {
        if (!isDigit(ch)) {
            isIndex = false;
            break;
        }
    }
    
    if (isIndex && !value.empty()) {
        addToken(TokenType::INDEX_ACCESS, value);
    } else {
        addToken(type, fullKeyword);
    }
    
    state = LexerState::NORMAL;
}

void BasicLexer::scanAtKeyword() {
    std::string value = "@";
    advance(); // 跳过 @
    
    // 读取@后面的标识符
    while (!isAtEnd() && isIdentifierChar(peek())) {
        value += advance();
    }
    
    TokenType type = getKeywordType(value);
    addToken(type, value);
    state = LexerState::NORMAL;
}

void BasicLexer::scanStyleSelector() {
    std::string value;
    char firstChar = advance(); // 获取第一个字符（. # &）
    value += firstChar;
    
    if (firstChar == '&') {
        // 处理 &:hover 或 &::after
        if (peek() == ':') {
            value += advance();
            if (peek() == ':') {
                value += advance();
                // 伪元素选择器
                while (!isAtEnd() && isIdentifierChar(peek())) {
                    value += advance();
                }
                addToken(TokenType::STYLE_SELECTOR_PSEUDO_ELEMENT, value);
            } else {
                // 伪类选择器
                while (!isAtEnd() && isIdentifierChar(peek())) {
                    value += advance();
                }
                addToken(TokenType::STYLE_SELECTOR_PSEUDO, value);
            }
        } else {
            addToken(TokenType::AMPERSAND, "&");
        }
    } else if (firstChar == '.') {
        // 类选择器
        while (!isAtEnd() && isIdentifierChar(peek())) {
            value += advance();
        }
        addToken(TokenType::STYLE_SELECTOR_CLASS, value);
    } else if (firstChar == '#') {
        // ID选择器
        while (!isAtEnd() && isIdentifierChar(peek())) {
            value += advance();
        }
        addToken(TokenType::STYLE_SELECTOR_ID, value);
    }
    
    state = LexerState::NORMAL;
}

void BasicLexer::scanIndexAccess() {
    std::string value;
    advance(); // 跳过 [
    
    while (!isAtEnd() && isDigit(peek())) {
        value += advance();
    }
    
    if (peek() != ']') {
        reportError("Expected ']' after index");
    }
    
    advance(); // 跳过 ]
    addToken(TokenType::INDEX_ACCESS, value);
    state = LexerState::NORMAL;
}

void BasicLexer::scanUnquotedString() {
    std::string value;
    
    // 无引号字符串读取到分号、花括号或换行为止
    while (!isAtEnd()) {
        char ch = peek();
        if (ch == ';' || ch == '{' || ch == '}' || ch == '\n') {
            break;
        }
        value += advance();
    }
    
    // 去除尾部空白
    while (!value.empty() && std::isspace(value.back())) {
        value.pop_back();
    }
    
    addToken(TokenType::UNQUOTED_STRING, value);
    state = LexerState::NORMAL;
}

bool BasicLexer::shouldScanUnquotedString() const {
    // 在属性值位置，且不是其他已知的token类型
    return context.inAttributeValue && 
           !isDigit(peek()) && 
           !isIdentifierStart(peek()) &&
           peek() != '"' && 
           peek() != '\'' &&
           peek() != '{' &&
           peek() != '}';
}

} // namespace chtl