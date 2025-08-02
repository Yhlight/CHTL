#include "Parser.h"
#include <algorithm>

namespace chtl {

// HTML元素列表
const std::set<std::string> HTML_ELEMENTS = {
    // 文档结构
    "html", "head", "body", "title", "meta", "link", "base",
    
    // 文本内容
    "h1", "h2", "h3", "h4", "h5", "h6", "p", "br", "hr",
    "blockquote", "pre", "code", "em", "strong", "i", "b",
    "u", "s", "small", "mark", "del", "ins", "sub", "sup",
    
    // 列表
    "ul", "ol", "li", "dl", "dt", "dd",
    
    // 表格
    "table", "thead", "tbody", "tfoot", "tr", "th", "td",
    "caption", "col", "colgroup",
    
    // 表单
    "form", "input", "textarea", "button", "select", "option",
    "optgroup", "label", "fieldset", "legend", "datalist",
    "output", "progress", "meter",
    
    // 分组内容
    "div", "span", "header", "footer", "main", "section",
    "article", "aside", "nav", "figure", "figcaption",
    "details", "summary", "dialog",
    
    // 嵌入内容
    "img", "iframe", "embed", "object", "param", "video",
    "audio", "source", "track", "canvas", "map", "area",
    "svg", "math",
    
    // 脚本
    "script", "noscript", "template", "slot",
    
    // 其他
    "a", "abbr", "address", "cite", "dfn", "kbd", "q",
    "samp", "var", "time", "data", "ruby", "rb", "rt",
    "rtc", "rp", "bdi", "bdo", "wbr", "picture"
};

Parser::Parser() : current(0) {
}

Token Parser::peek(size_t offset) const {
    size_t pos = current + offset;
    if (pos >= tokens.size()) {
        return tokens.empty() ? 
            Token(TokenType::END_OF_FILE, "", 0, 0) : 
            tokens.back();
    }
    return tokens[pos];
}

Token Parser::previous() const {
    if (current > 0 && current <= tokens.size()) {
        return tokens[current - 1];
    }
    return Token(TokenType::UNKNOWN, "", 0, 0);
}

Token Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return previous();
}

bool Parser::isAtEnd() const {
    return current >= tokens.size() || 
           peek().type == TokenType::END_OF_FILE;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    
    throw createError(peek(), message);
}

void Parser::error(const std::string& message) {
    throw createError(message);
}

void Parser::error(const Token& token, const std::string& message) {
    throw createError(token, message);
}

ParseError Parser::createError(const std::string& message) {
    Token current = peek();
    return ParseError(message, current.line, current.column);
}

ParseError Parser::createError(const Token& token, const std::string& message) {
    return ParseError(message, token.line, token.column);
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        // 如果前一个token是分号，可能是语句结束
        if (previous().type == TokenType::SEMICOLON) return;
        
        // 如果当前token是某些关键字，可能是新语句开始
        switch (peek().type) {
            case TokenType::KEYWORD_TEXT:
            case TokenType::KEYWORD_STYLE:
            case TokenType::KEYWORD_CUSTOM:
            case TokenType::KEYWORD_TEMPLATE:
            case TokenType::KEYWORD_IMPORT:
            case TokenType::KEYWORD_NAMESPACE:
            case TokenType::KEYWORD_CONFIGURATION:
            case TokenType::RIGHT_BRACE:
                return;
            default:
                break;
        }
        
        advance();
    }
}

bool Parser::isHtmlElement(const std::string& name) const {
    return HTML_ELEMENTS.find(name) != HTML_ELEMENTS.end();
}

bool Parser::isCustomElement(const std::string& name) const {
    return context.customElements.find(name) != context.customElements.end();
}

bool Parser::isTemplateElement(const std::string& name) const {
    return context.templateElements.find(name) != context.templateElements.end();
}

} // namespace chtl