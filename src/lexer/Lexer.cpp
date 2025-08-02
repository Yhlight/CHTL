#include "Lexer.h"
#include <iostream>
#include <stdexcept>

namespace chtl {

Lexer::Lexer() : position(0), line(1), column(1), state(LexerState::NORMAL) {
}

char Lexer::peek(size_t offset) const {
    size_t pos = position + offset;
    if (pos >= input.length()) {
        return '\0';
    }
    return input[pos];
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    
    char ch = input[position++];
    if (ch == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return ch;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char ch = peek();
        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
            advance();
        } else {
            break;
        }
    }
}

bool Lexer::isAtEnd() const {
    return position >= input.length();
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (peek() != expected) return false;
    advance();
    return true;
}

bool Lexer::matchString(const std::string& expected) {
    for (size_t i = 0; i < expected.length(); i++) {
        if (peek(i) != expected[i]) {
            return false;
        }
    }
    
    // 匹配成功，前进相应的字符数
    for (size_t i = 0; i < expected.length(); i++) {
        advance();
    }
    return true;
}

void Lexer::addToken(TokenType type, const std::string& value) {
    tokens.push_back(Token(type, value, line, column - value.length()));
}

void Lexer::addToken(TokenType type) {
    addToken(type, "");
}

void Lexer::reportError(const std::string& message) {
    std::cerr << "Lexer Error at line " << line << ", column " << column 
              << ": " << message << std::endl;
    throw std::runtime_error("Lexer error: " + message);
}

} // namespace chtl