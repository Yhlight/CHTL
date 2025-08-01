#include "BasicLexer.h"
#include <algorithm>
#include <sstream>

namespace chtl {

// BasicLexer 实现
BasicLexer::BasicLexer(const std::string& input) 
    : input_(input), position_(0), currentPosition_(1, 1, 0),
      currentState_(LexerState::START), hasError_(false) {
    contextManager_ = std::make_unique<ContextManager>();
    stateMachine_ = std::make_unique<StateMachine>();
}

std::vector<Token> BasicLexer::tokenize() {
    std::vector<Token> tokens;
    
    while (hasNext()) {
        Token token = nextToken();
        if (token.type != TokenType::WHITESPACE && token.type != TokenType::NEWLINE) {
            tokens.push_back(token);
        }
        
        if (hasError()) {
            break;
        }
    }
    
    // 添加EOF token
    if (!hasError()) {
        tokens.push_back(createToken(TokenType::EOF_TOKEN));
    }
    
    return tokens;
}

Token BasicLexer::nextToken() {
    if (hasError_ || isAtEnd()) {
        return createToken(TokenType::EOF_TOKEN);
    }
    
    return tokenizeNext();
}

bool BasicLexer::hasNext() const {
    return !isAtEnd() && !hasError_;
}

Position BasicLexer::getCurrentPosition() const {
    return currentPosition_;
}

size_t BasicLexer::getCurrentOffset() const {
    return position_;
}

bool BasicLexer::hasError() const {
    return hasError_;
}

std::string BasicLexer::getLastError() const {
    return lastError_;
}

void BasicLexer::clearError() {
    hasError_ = false;
    lastError_.clear();
}

std::string BasicLexer::getDebugInfo() const {
    std::ostringstream oss;
    oss << "Position: " << position_ << "/" << input_.size() << "\n";
    oss << "Line: " << currentPosition_.line << ", Column: " << currentPosition_.column << "\n";
    oss << "Current State: " << contextManager_->getStateString() << "\n";
    oss << "Context Stack: " << contextManager_->getContextStackString() << "\n";
    if (hasError_) {
        oss << "Error: " << lastError_ << "\n";
    }
    return oss.str();
}

Token BasicLexer::tokenizeNext() {
    skipWhitespace();
    
    // 处理换行符
    if (!isAtEnd() && currentChar() == '\n') {
        Position tokenStart = currentPosition_;
        advance();
        return createToken(TokenType::NEWLINE, "\n", tokenStart);
    }
    
    if (isAtEnd()) {
        return createToken(TokenType::EOF_TOKEN);
    }
    
    char c = currentChar();
    Position tokenStart = currentPosition_;
    
    // 根据当前字符决定如何处理
    switch (c) {
        case '/':
            if (peekChar() == '/') {
                return readComment();
            } else if (peekChar() == '*') {
                return readComment();
            }
            break;
            
        case '-':
            if (peekChar() == '-') {
                return readComment();
            }
            break;
            
        case '"':
            return readStringDoubleQuote();
            
        case '\'':
            return readStringSingleQuote();
            
        case '.':
            return handleDot();
            
        case '#':
            return handleHash();
            
        case '@':
            return handleAtSymbol();
            
        case '&':
            return handleAmpersand();
            
        case ':':
            return handleColon();
            
        case '[':
            return handleLeftBracket();
            
        case ';':
            advance();
            return createToken(TokenType::SEMICOLON, ";", tokenStart);
            
        case '=':
            advance();
            return createToken(TokenType::EQUAL, "=", tokenStart);
            
        case '{':
            advance();
            return createToken(TokenType::LEFT_BRACE, "{", tokenStart);
            
        case '}':
            advance();
            return createToken(TokenType::RIGHT_BRACE, "}", tokenStart);
            
        case ']':
            advance();
            return createToken(TokenType::RIGHT_BRACKET, "]", tokenStart);
            
        case '(':
            advance();
            return createToken(TokenType::LEFT_PAREN, "(", tokenStart);
            
        case ')':
            advance();
            return createToken(TokenType::RIGHT_PAREN, ")", tokenStart);
            
        case ',':
            advance();
            return createToken(TokenType::COMMA, ",", tokenStart);
            
        default:
            if (std::isdigit(c)) {
                return readNumber();
            } else if (std::isalpha(c) || c == '_') {
                return readIdentifier();
            } else {
                // 处理无引号字符串或错误
                return readStringNoQuote();
            }
    }
    
    // 如果到达这里，说明遇到了未知字符
    advance();
    return createErrorToken("Unexpected character: " + std::string(1, c));
}

Token BasicLexer::readIdentifier() {
    Position startPos = currentPosition_;
    std::string value;
    
    while (!isAtEnd() && (std::isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-')) {
        value += currentChar();
        advance();
    }
    
    // 使用上下文推导确定Token类型
    TokenType type = inferTokenType(value);
    
    Token token = createToken(type, value, startPos);
    updateContextFromToken(token);
    
    return token;
}

Token BasicLexer::readStringDoubleQuote() {
    Position startPos = currentPosition_;
    std::string value;
    
    advance(); // 跳过开始的引号
    
    while (!isAtEnd() && currentChar() != '"') {
        if (currentChar() == '\\' && peekChar() == '"') {
            // 处理转义的引号
            advance(); // 跳过反斜杠
            value += currentChar();
            advance();
        } else {
            value += currentChar();
            advance();
        }
    }
    
    if (isAtEnd()) {
        return createErrorToken("Unterminated string literal");
    }
    
    advance(); // 跳过结束的引号
    return createToken(TokenType::STRING_DOUBLE_QUOTE, value, startPos);
}

Token BasicLexer::readStringSingleQuote() {
    Position startPos = currentPosition_;
    std::string value;
    
    advance(); // 跳过开始的引号
    
    while (!isAtEnd() && currentChar() != '\'') {
        if (currentChar() == '\\' && peekChar() == '\'') {
            // 处理转义的引号
            advance(); // 跳过反斜杠
            value += currentChar();
            advance();
        } else {
            value += currentChar();
            advance();
        }
    }
    
    if (isAtEnd()) {
        return createErrorToken("Unterminated string literal");
    }
    
    advance(); // 跳过结束的引号
    return createToken(TokenType::STRING_SINGLE_QUOTE, value, startPos);
}

Token BasicLexer::readStringNoQuote() {
    Position startPos = currentPosition_;
    std::string value;
    
    // 读取直到遇到分隔符或空白字符
    while (!isAtEnd()) {
        char c = currentChar();
        if (std::isspace(c) || c == ';' || c == ':' || c == '=' || 
            c == '{' || c == '}' || c == '(' || c == ')' || 
            c == '[' || c == ']' || c == ',') {
            break;
        }
        value += c;
        advance();
    }
    
    if (value.empty()) {
        return createErrorToken("Empty identifier");
    }
    
    return createToken(TokenType::STRING_NO_QUOTE, value, startPos);
}

Token BasicLexer::readNumber() {
    Position startPos = currentPosition_;
    std::string value;
    bool hasDecimalPoint = false;
    
    while (!isAtEnd()) {
        char c = currentChar();
        if (std::isdigit(c)) {
            value += c;
            advance();
        } else if (c == '.' && !hasDecimalPoint) {
            hasDecimalPoint = true;
            value += c;
            advance();
        } else {
            break;
        }
    }
    
    return createToken(TokenType::NUMBER, value, startPos);
}

Token BasicLexer::readComment() {
    Position startPos = currentPosition_;
    std::string value;
    
    if (currentChar() == '/' && peekChar() == '/') {
        // 单行注释
        advance(); // 跳过第一个 /
        advance(); // 跳过第二个 /
        
        while (!isAtEnd() && currentChar() != '\n') {
            value += currentChar();
            advance();
        }
        
        return createToken(TokenType::COMMENT_SINGLE, value, startPos);
    } else if (currentChar() == '/' && peekChar() == '*') {
        // 多行注释
        advance(); // 跳过 /
        advance(); // 跳过 *
        
        while (!isAtEnd()) {
            if (currentChar() == '*' && peekChar() == '/') {
                advance(); // 跳过 *
                advance(); // 跳过 /
                break;
            }
            value += currentChar();
            advance();
        }
        
        return createToken(TokenType::COMMENT_MULTI, value, startPos);
    } else if (currentChar() == '-' && peekChar() == '-') {
        // 生成器注释
        advance(); // 跳过第一个 -
        advance(); // 跳过第二个 -
        
        while (!isAtEnd() && currentChar() != '\n') {
            value += currentChar();
            advance();
        }
        
        return createToken(TokenType::COMMENT_GENERATOR, value, startPos);
    }
    
    return createErrorToken("Invalid comment syntax");
}

Token BasicLexer::handleDot() {
    Position startPos = currentPosition_;
    advance(); // 跳过 .
    
    if (!isAtEnd() && (std::isalpha(currentChar()) || currentChar() == '_')) {
        // CSS类选择器 .className
        std::string className;
        while (!isAtEnd() && (std::isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-')) {
            className += currentChar();
            advance();
        }
        return createToken(TokenType::CLASS_SELECTOR, "." + className, startPos);
    }
    
    return createToken(TokenType::DOT, ".", startPos);
}

Token BasicLexer::handleHash() {
    Position startPos = currentPosition_;
    advance(); // 跳过 #
    
    if (!isAtEnd() && (std::isalpha(currentChar()) || currentChar() == '_')) {
        // CSS ID选择器 #idName
        std::string idName;
        while (!isAtEnd() && (std::isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-')) {
            idName += currentChar();
            advance();
        }
        return createToken(TokenType::ID_SELECTOR, "#" + idName, startPos);
    }
    
    return createToken(TokenType::HASH, "#", startPos);
}

Token BasicLexer::handleAtSymbol() {
    Position startPos = currentPosition_;
    advance(); // 跳过 @
    
    if (!isAtEnd() && std::isalpha(currentChar())) {
        // @关键字，如 @Style, @Element 等
        std::string keyword;
        while (!isAtEnd() && (std::isalnum(currentChar()) || currentChar() == '_')) {
            keyword += currentChar();
            advance();
        }
        
        std::string fullKeyword = "@" + keyword;
        TokenType type = KeywordRegistry::getInstance().getKeywordType(fullKeyword);
        if (type != TokenType::UNKNOWN) {
            return createToken(type, fullKeyword, startPos);
        }
        
        return createToken(TokenType::IDENTIFIER, fullKeyword, startPos);
    }
    
    return createToken(TokenType::AT, "@", startPos);
}

Token BasicLexer::handleAmpersand() {
    Position startPos = currentPosition_;
    advance(); // 跳过 &
    
    if (!isAtEnd() && currentChar() == ':') {
        // 伪类选择器 &:hover
        advance(); // 跳过 :
        if (!isAtEnd() && currentChar() == ':') {
            // 伪元素选择器 &::before
            advance(); // 跳过第二个 :
            std::string pseudoElement;
            while (!isAtEnd() && (std::isalnum(currentChar()) || currentChar() == '-')) {
                pseudoElement += currentChar();
                advance();
            }
            return createToken(TokenType::PSEUDO_ELEMENT, "&::" + pseudoElement, startPos);
        } else {
            // 伪类选择器
            std::string pseudoClass;
            while (!isAtEnd() && (std::isalnum(currentChar()) || currentChar() == '-')) {
                pseudoClass += currentChar();
                advance();
            }
            return createToken(TokenType::PSEUDO_CLASS, "&:" + pseudoClass, startPos);
        }
    }
    
    return createToken(TokenType::AMPERSAND, "&", startPos);
}

Token BasicLexer::handleColon() {
    Position startPos = currentPosition_;
    advance(); // 跳过第一个 :
    
    if (!isAtEnd() && currentChar() == ':') {
        // 双冒号，可能是伪元素
        advance(); // 跳过第二个 :
        if (!isAtEnd() && std::isalpha(currentChar())) {
            std::string pseudoElement;
            while (!isAtEnd() && (std::isalnum(currentChar()) || currentChar() == '-')) {
                pseudoElement += currentChar();
                advance();
            }
            return createToken(TokenType::PSEUDO_ELEMENT, "::" + pseudoElement, startPos);
        }
    } else if (!isAtEnd() && std::isalpha(currentChar())) {
        // 单冒号，可能是伪类
        std::string pseudoClass;
        while (!isAtEnd() && (std::isalnum(currentChar()) || currentChar() == '-')) {
            pseudoClass += currentChar();
            advance();
        }
        return createToken(TokenType::PSEUDO_CLASS, ":" + pseudoClass, startPos);
    }
    
    return createToken(TokenType::COLON, ":", startPos);
}

Token BasicLexer::handleLeftBracket() {
    Position startPos = currentPosition_;
    advance(); // 跳过 [
    
    // 检查是否为配置块或特殊语法
    std::string content;
    while (!isAtEnd() && currentChar() != ']') {
        content += currentChar();
        advance();
    }
    
    if (isAtEnd()) {
        return createErrorToken("Unterminated bracket expression");
    }
    
    advance(); // 跳过 ]
    
    std::string fullContent = "[" + content + "]";
    
    // 检查是否为关键字
    TokenType type = KeywordRegistry::getInstance().getKeywordType(fullContent);
    if (type != TokenType::UNKNOWN) {
        Token token = createToken(type, fullContent, startPos);
        updateContextFromToken(token);
        return token;
    }
    
    // 检查是否为索引访问
    if (std::all_of(content.begin(), content.end(), [](char c) { 
        return std::isdigit(c) || std::isspace(c); 
    })) {
        return createToken(TokenType::INDEX, fullContent, startPos);
    }
    
    // 默认作为配置选项列表处理
    return createToken(TokenType::CONFIG_OPTION_LIST, fullContent, startPos);
}

TokenType BasicLexer::inferTokenType(const std::string& value) const {
    // 首先检查是否为关键字
    if (KeywordRegistry::getInstance().isKeyword(value)) {
        return KeywordRegistry::getInstance().getKeywordType(value);
    }
    
    // 根据上下文推导
    if (contextManager_->shouldInferAsHtmlTag(value)) {
        return TokenType::HTML_TAG;
    }
    
    if (contextManager_->shouldInferAsCssSelector(value)) {
        return TokenType::CLASS_SELECTOR; // 需要根据具体情况细化
    }
    
    if (contextManager_->shouldInferAsCustomElement(value)) {
        return TokenType::CUSTOM_ELEMENT;
    }
    
    if (contextManager_->shouldInferAsVariable(value)) {
        return TokenType::CUSTOM_VAR;
    }
    
    return TokenType::IDENTIFIER;
}

void BasicLexer::updateContextFromToken(const Token& token) {
    switch (token.type) {
        case TokenType::STYLE:
            contextManager_->pushContext(ParseContext::STYLE_BLOCK);
            break;
            
        case TokenType::TEXT:
            contextManager_->pushContext(ParseContext::TEXT_BLOCK);
            break;
            
        case TokenType::CUSTOM:
            contextManager_->pushContext(ParseContext::CUSTOM_BLOCK);
            break;
            
        case TokenType::TEMPLATE:
            contextManager_->pushContext(ParseContext::TEMPLATE_BLOCK);
            break;
            
        case TokenType::ORIGIN:
            contextManager_->pushContext(ParseContext::ORIGIN_BLOCK);
            break;
            
        case TokenType::CONFIGURATION:
            contextManager_->pushContext(ParseContext::CONFIG_BLOCK);
            break;
            
        case TokenType::HTML_TAG:
            contextManager_->pushContext(ParseContext::ELEMENT);
            break;
            
        case TokenType::RIGHT_BRACE:
            contextManager_->popContext();
            break;
            
        default:
            break;
    }
}

void BasicLexer::processStateTransition(char c) {
    // 基础实现 - 在子类中可以重写
    (void)c; // 避免未使用参数警告
}

bool BasicLexer::shouldTreatAsHtmlTag(const std::string& identifier) const {
    return contextManager_->shouldInferAsHtmlTag(identifier);
}

bool BasicLexer::shouldTreatAsCssSelector(const std::string& identifier) const {
    return contextManager_->shouldInferAsCssSelector(identifier);
}

bool BasicLexer::shouldTreatAsCustomElement(const std::string& identifier) const {
    return contextManager_->shouldInferAsCustomElement(identifier);
}

bool BasicLexer::shouldTreatAsVariable(const std::string& identifier) const {
    return contextManager_->shouldInferAsVariable(identifier);
}

char BasicLexer::currentChar() const {
    return isAtEnd() ? '\0' : input_[position_];
}

char BasicLexer::peekChar(int offset) const {
    size_t peekPos = position_ + offset;
    return (peekPos >= input_.size()) ? '\0' : input_[peekPos];
}

void BasicLexer::advance() {
    if (!isAtEnd()) {
        updatePosition(input_[position_]);
        ++position_;
    }
}

void BasicLexer::skipWhitespace() {
    while (!isAtEnd() && (currentChar() == ' ' || currentChar() == '\t' || currentChar() == '\r')) {
        advance();
    }
}

void BasicLexer::updatePosition(char c) {
    if (c == '\n') {
        currentPosition_.line++;
        currentPosition_.column = 1;
    } else {
        currentPosition_.column++;
    }
    currentPosition_.offset = position_;
}

bool BasicLexer::isAtEnd() const {
    return position_ >= input_.size();
}

Token BasicLexer::createToken(TokenType type, const std::string& value) const {
    return createToken(type, value, currentPosition_);
}

Token BasicLexer::createToken(TokenType type, const std::string& value, const Position& pos) const {
    Token token(type, value, pos);
    
    // 设置上下文信息
    token.isInStyleBlock = contextManager_->isInStyleContext();
    token.isInCustomBlock = contextManager_->isInCustomContext();
    token.isInTemplateBlock = contextManager_->isInTemplateContext();
    token.isInConfigBlock = contextManager_->isInConfigContext();
    token.isInOriginBlock = contextManager_->isInOriginContext();
    
    return token;
}

void BasicLexer::setError(const std::string& message) {
    hasError_ = true;
    lastError_ = message + " at line " + std::to_string(currentPosition_.line) + 
                ", column " + std::to_string(currentPosition_.column);
}

Token BasicLexer::createErrorToken(const std::string& message) {
    setError(message);
    return createToken(TokenType::UNKNOWN, message);
}

// LexerFactory 实现
std::unique_ptr<BasicLexer> LexerFactory::createBasicLexer(const std::string& input) {
    return std::make_unique<BasicLexer>(input);
}

std::unique_ptr<BasicLexer> LexerFactory::createConfigLexer(const std::string& input) {
    // ConfigLexer将在后续实现
    return std::make_unique<BasicLexer>(input);
}

std::unique_ptr<BasicLexer> LexerFactory::createLexer(const std::string& input, ParseContext initialContext) {
    auto lexer = std::make_unique<BasicLexer>(input);
    // 根据初始上下文进行配置
    return lexer;
}

} // namespace chtl