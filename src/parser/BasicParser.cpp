#include "BasicParser.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace chtl {

// ParseError 实现
std::string ParseError::toString() const {
    std::ostringstream oss;
    oss << "ParseError at line " << position_.line << ", column " << position_.column;
    oss << " (" << static_cast<int>(type_) << "): " << message_;
    return oss.str();
}

// BasicParser 实现
BasicParser::BasicParser(const std::vector<Token>& tokens)
    : tokens_(tokens), current_(0), panicMode_(false) {
    initializeContextStack();
}

std::shared_ptr<Node> BasicParser::parse() {
    return parseDocument();
}

std::shared_ptr<Node> BasicParser::parseDocument() {
    auto document = std::make_shared<Node>(NodeType::DOCUMENT, createNodePosition(getCurrentToken()));
    document->setName("document");
    
    pushContext(ParseContext::GLOBAL);
    
    // 解析文档中的所有语句
    while (!isAtEnd()) {
        try {
            auto statement = parseStatement();
            if (statement) {
                document->addChild(statement);
            }
        } catch (const std::exception& e) {
            error(ParseError::Type::INVALID_SYNTAX, e.what());
            synchronize();
        }
    }
    
    popContext();
    return document;
}

const std::vector<ParseError>& BasicParser::getErrors() const {
    return errors_;
}

bool BasicParser::hasErrors() const {
    return !errors_.empty();
}

void BasicParser::clearErrors() {
    errors_.clear();
}

bool BasicParser::isAtEnd() const {
    return current_ >= tokens_.size() || peek().type == TokenType::EOF_TOKEN;
}

size_t BasicParser::getCurrentPosition() const {
    return current_;
}

const Token& BasicParser::getCurrentToken() const {
    return peek();
}

const Token& BasicParser::peek(size_t offset) const {
    size_t index = current_ + offset;
    if (index >= tokens_.size()) {
        static Token eofToken{TokenType::EOF_TOKEN, "", Position()};
        return eofToken;
    }
    return tokens_[index];
}

const Token& BasicParser::advance() {
    if (!isAtEnd()) {
        current_++;
    }
    return previous();
}

const Token& BasicParser::previous() const {
    if (current_ == 0) {
        static Token defaultToken{TokenType::UNKNOWN, "", Position()};
        return defaultToken;
    }
    return tokens_[current_ - 1];
}

bool BasicParser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool BasicParser::match(std::initializer_list<TokenType> types) {
    for (auto type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool BasicParser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool BasicParser::checkSequence(std::initializer_list<TokenType> types) const {
    size_t i = 0;
    for (auto type : types) {
        if (peek(i).type != type) {
            return false;
        }
        i++;
    }
    return true;
}

Token BasicParser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    
    error(ParseError::Type::MISSING_TOKEN, message);
    return getCurrentToken();
}

void BasicParser::synchronize() {
    panicMode_ = false;
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        if (previous().type == TokenType::NEWLINE) return;
        
        switch (peek().type) {
            case TokenType::LEFT_BRACE:
            case TokenType::RIGHT_BRACE:
            case TokenType::HTML_TAG:
            case TokenType::TEXT:
            case TokenType::STYLE:
            case TokenType::COMMENT_MULTI:
            case TokenType::COMMENT_SINGLE:
            case TokenType::COMMENT_GENERATOR:
                return;
            default:
                break;
        }
        
        advance();
    }
}

void BasicParser::error(ParseError::Type type, const std::string& message) {
    error(type, message, getCurrentToken().position);
}

void BasicParser::error(ParseError::Type type, const std::string& message, const Position& position) {
    if (panicMode_) return;
    panicMode_ = true;
    
    errors_.emplace_back(type, message, position);
}

bool BasicParser::recover(std::initializer_list<TokenType> syncTokens) {
    while (!isAtEnd()) {
        for (auto token : syncTokens) {
            if (check(token)) {
                panicMode_ = false;
                return true;
            }
        }
        advance();
    }
    return false;
}

std::shared_ptr<Node> BasicParser::parseStatement() {
    // 跳过换行符和其他空白Token
    while (match({TokenType::NEWLINE, TokenType::WHITESPACE})) {
        // 继续
    }
    
    if (isAtEnd()) {
        return nullptr;
    }
    
    // 根据当前Token判断语句类型
    if (isElementStart()) {
        return parseElement();
    }
    
    if (isTextStart()) {
        return parseTextBlock();
    }
    
    if (isStyleStart()) {
        return parseStyleBlock();
    }
    
    if (check(TokenType::COMMENT_SINGLE) || 
        check(TokenType::COMMENT_MULTI) || 
        check(TokenType::COMMENT_GENERATOR)) {
        return parseComment();
    }
    
    if (isCustomStart()) {
        return parseCustomBlock();
    }
    
    if (isTemplateStart()) {
        return parseTemplateBlock();
    }
    
    if (isConfigStart()) {
        return parseConfigurationBlock();
    }
    
    if (isImportStart()) {
        return parseImportBlock();
    }
    
    if (isNamespaceStart()) {
        return parseNamespaceBlock();
    }
    
    // 如果无法识别，尝试作为标识符处理
    if (check(TokenType::IDENTIFIER)) {
        // 可能是HTML元素
        return parseElement();
    }
    
    error(ParseError::Type::UNEXPECTED_TOKEN, 
          "Unexpected token: " + getCurrentToken().value);
    advance();
    return nullptr;
}

std::shared_ptr<Node> BasicParser::parseElement() {
    return parseHtmlElement();
}

std::shared_ptr<ElementNode> BasicParser::parseHtmlElement() {
    Token tagToken = consume(TokenType::HTML_TAG, "Expected HTML tag");
    if (tagToken.type == TokenType::IDENTIFIER) {
        tagToken = previous(); // 使用IDENTIFIER作为标签名
    }
    
    auto element = std::make_shared<ElementNode>(tagToken.value, createNodePosition(tagToken));
    
    pushContext(ParseContext::ELEMENT);
    
    // 解析元素开始标记
    if (match(TokenType::LEFT_BRACE)) {
        parseElementBody(element);
        consume(TokenType::RIGHT_BRACE, "Expected '}' after element body");
    } else if (isSelfClosingTag(tagToken.value)) {
        // 自闭合标签可以没有body
    } else {
        error(ParseError::Type::MISSING_TOKEN, "Expected '{' after element tag");
    }
    
    popContext();
    return element;
}

void BasicParser::parseElementBody(std::shared_ptr<ElementNode> element) {
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // 跳过换行符
        if (match(TokenType::NEWLINE)) {
            continue;
        }
        
        // 解析属性
        if (check(TokenType::IDENTIFIER) && peek(1).type == TokenType::COLON) {
            auto attr = parseAttribute();
            if (attr) {
                element->addAttribute(attr->getAttributeName(), attr->getAttributeValue());
            }
            continue;
        }
        
        // 解析子语句
        auto child = parseStatement();
        if (child) {
            element->addChild(child);
        }
    }
}

std::shared_ptr<AttributeNode> BasicParser::parseAttribute() {
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected attribute name");
    consume(TokenType::COLON, "Expected ':' after attribute name");
    
    std::string value;
    if (check(TokenType::STRING_DOUBLE_QUOTE) || 
        check(TokenType::STRING_SINGLE_QUOTE) || 
        check(TokenType::STRING_NO_QUOTE)) {
        value = parseStringLiteral();
    } else {
        error(ParseError::Type::INVALID_ATTRIBUTE, "Expected attribute value");
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after attribute");
    
    return std::make_shared<AttributeNode>(nameToken.value, value, createNodePosition(nameToken));
}

std::shared_ptr<Node> BasicParser::parseTextBlock() {
    Token textToken = consume(TokenType::TEXT, "Expected 'text' keyword");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'text'");
    
    std::string content;
    if (check(TokenType::STRING_DOUBLE_QUOTE) || 
        check(TokenType::STRING_SINGLE_QUOTE) || 
        check(TokenType::STRING_NO_QUOTE)) {
        content = parseStringLiteral();
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after text content");
    
    return std::make_shared<TextNode>(content, createNodePosition(textToken));
}

std::shared_ptr<Node> BasicParser::parseStyleBlock() {
    Token styleToken = consume(TokenType::STYLE, "Expected 'style' keyword");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'style'");
    
    auto styleBlock = std::make_shared<StyleBlockNode>(createNodePosition(styleToken));
    
    pushContext(ParseContext::STYLE_BLOCK);
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match(TokenType::NEWLINE)) {
            continue;
        }
        
        // 解析CSS规则或内联样式
        if (check(TokenType::CLASS_SELECTOR) || 
            check(TokenType::ID_SELECTOR) ||
            check(TokenType::PSEUDO_CLASS)) {
            // 明确的CSS选择器
            auto rule = parseCssRule();
            if (rule) {
                styleBlock->addChild(rule);
            }
        } else if (check(TokenType::IDENTIFIER)) {
            // 需要区分是CSS规则还是内联样式
            if (peek(1).type == TokenType::COLON) {
                // 内联样式属性 (property: value;)
                auto property = parseCssProperty();
                if (property) {
                    styleBlock->addInlineProperty(property->getProperty(), property->getValue());
                }
            } else {
                // CSS规则 (selector { ... })
                auto rule = parseCssRule();
                if (rule) {
                    styleBlock->addChild(rule);
                }
            }
        }
    }
    
    popContext();
    consume(TokenType::RIGHT_BRACE, "Expected '}' after style block");
    
    return styleBlock;
}

std::shared_ptr<CssRuleNode> BasicParser::parseCssRule() {
    auto rule = std::make_shared<CssRuleNode>(createNodePosition(getCurrentToken()));
    
    // 解析选择器
    auto selector = parseCssSelector();
    if (selector) {
        rule->addSelector(selector);
    }
    
    consume(TokenType::LEFT_BRACE, "Expected '{' after CSS selector");
    
    // 解析CSS属性
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (match(TokenType::NEWLINE)) {
            continue;
        }
        
        auto property = parseCssProperty();
        if (property) {
            rule->addProperty(property);
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}' after CSS rule");
    
    return rule;
}

std::shared_ptr<CssSelectorNode> BasicParser::parseCssSelector() {
    if (check(TokenType::CLASS_SELECTOR)) {
        Token token = advance();
        return std::make_shared<CssSelectorNode>(
            CssSelectorNode::SelectorType::CLASS, 
            token.value.substr(1), // 去掉前缀'.'
            createNodePosition(token)
        );
    }
    
    if (check(TokenType::ID_SELECTOR)) {
        Token token = advance();
        return std::make_shared<CssSelectorNode>(
            CssSelectorNode::SelectorType::ID, 
            token.value.substr(1), // 去掉前缀'#'
            createNodePosition(token)
        );
    }
    
    if (check(TokenType::PSEUDO_CLASS)) {
        Token token = advance();
        return std::make_shared<CssSelectorNode>(
            CssSelectorNode::SelectorType::PSEUDO_CLASS, 
            token.value.substr(1), // 去掉前缀':'
            createNodePosition(token)
        );
    }
    
    if (check(TokenType::IDENTIFIER)) {
        Token token = advance();
        return std::make_shared<CssSelectorNode>(
            CssSelectorNode::SelectorType::ELEMENT, 
            token.value,
            createNodePosition(token)
        );
    }
    
    error(ParseError::Type::INVALID_STYLE, "Expected CSS selector");
    return nullptr;
}

std::shared_ptr<CssPropertyNode> BasicParser::parseCssProperty() {
    Token propertyToken = consume(TokenType::IDENTIFIER, "Expected CSS property name");
    consume(TokenType::COLON, "Expected ':' after CSS property");
    
    std::string value;
    if (check(TokenType::STRING_DOUBLE_QUOTE) || 
        check(TokenType::STRING_SINGLE_QUOTE) || 
        check(TokenType::STRING_NO_QUOTE)) {
        value = parseStringLiteral();
    } else if (check(TokenType::IDENTIFIER) || check(TokenType::NUMBER)) {
        value = advance().value;
    } else {
        error(ParseError::Type::INVALID_STYLE, "Expected CSS property value");
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after CSS property");
    
    return std::make_shared<CssPropertyNode>(propertyToken.value, value, createNodePosition(propertyToken));
}

std::shared_ptr<Node> BasicParser::parseComment() {
    CommentNode::CommentType type = CommentNode::CommentType::SINGLE_LINE;
    std::string content;
    Token commentToken;
    
    if (check(TokenType::COMMENT_SINGLE)) {
        commentToken = advance();
        type = CommentNode::CommentType::SINGLE_LINE;
        content = commentToken.value;
    } else if (check(TokenType::COMMENT_MULTI)) {
        commentToken = advance();
        type = CommentNode::CommentType::MULTI_LINE;
        content = commentToken.value;
    } else if (check(TokenType::COMMENT_GENERATOR)) {
        commentToken = advance();
        type = CommentNode::CommentType::GENERATOR;
        content = commentToken.value;
    } else {
        error(ParseError::Type::UNEXPECTED_TOKEN, "Expected comment token");
        return nullptr;
    }
    
    return std::make_shared<CommentNode>(content, type, createNodePosition(commentToken));
}

std::string BasicParser::parseStringLiteral() {
    if (check(TokenType::STRING_DOUBLE_QUOTE) || 
        check(TokenType::STRING_SINGLE_QUOTE) || 
        check(TokenType::STRING_NO_QUOTE)) {
        return advance().value;
    }
    
    error(ParseError::Type::INVALID_SYNTAX, "Expected string literal");
    return "";
}

// 实用方法实现
bool BasicParser::isElementStart() const {
    return check(TokenType::HTML_TAG) || 
           (check(TokenType::IDENTIFIER) && isValidHtmlTag(peek().value));
}

bool BasicParser::isStyleStart() const {
    return check(TokenType::STYLE);
}

bool BasicParser::isTextStart() const {
    return check(TokenType::TEXT);
}

bool BasicParser::isCustomStart() const {
    return check(TokenType::CUSTOM);
}

bool BasicParser::isTemplateStart() const {
    return check(TokenType::TEMPLATE);
}

bool BasicParser::isConfigStart() const {
    return check(TokenType::CONFIGURATION);
}

bool BasicParser::isImportStart() const {
    return check(TokenType::IMPORT);
}

bool BasicParser::isNamespaceStart() const {
    return check(TokenType::NAMESPACE);
}

bool BasicParser::isValidHtmlTag(const std::string& tagName) const {
    return HtmlTagRegistry::getInstance().isValidHtmlTag(tagName);
}

bool BasicParser::isSelfClosingTag(const std::string& tagName) const {
    return HtmlTagRegistry::getInstance().isSelfClosingTag(tagName);
}

void BasicParser::pushContext(ParseContext context) {
    contextStack_.push(context);
}

void BasicParser::popContext() {
    if (!contextStack_.empty()) {
        contextStack_.pop();
    }
}

ParseContext BasicParser::getCurrentContext() const {
    return contextStack_.empty() ? ParseContext::GLOBAL : contextStack_.top();
}

bool BasicParser::isInContext(ParseContext context) const {
    return getCurrentContext() == context;
}

bool BasicParser::shouldTreatAsCustomElement(const std::string& identifier) const {
    (void)identifier; // 避免未使用参数警告
    return false; // 基础Parser不处理自定义元素
}

bool BasicParser::shouldTreatAsStyleGroup(const std::string& identifier) const {
    (void)identifier;
    return false;
}

bool BasicParser::shouldTreatAsVariable(const std::string& identifier) const {
    (void)identifier;
    return false;
}

void BasicParser::initializeContextStack() {
    // 清空上下文栈
    while (!contextStack_.empty()) {
        contextStack_.pop();
    }
    // 初始化为全局上下文
    contextStack_.push(ParseContext::GLOBAL);
}

NodePosition BasicParser::createNodePosition(const Token& startToken, const Token& endToken) const {
    return NodePosition(startToken.position, endToken.position);
}

NodePosition BasicParser::createNodePosition(const Token& token) const {
    return NodePosition(token.position, token.position);
}

// 占位符实现 - 在后续版本中完善
std::shared_ptr<Node> BasicParser::parseCustomBlock() {
    error(ParseError::Type::INVALID_SYNTAX, "Custom blocks not yet implemented");
    synchronize();
    return nullptr;
}

std::shared_ptr<Node> BasicParser::parseTemplateBlock() {
    error(ParseError::Type::INVALID_SYNTAX, "Template blocks not yet implemented");
    synchronize();
    return nullptr;
}

std::shared_ptr<Node> BasicParser::parseConfigurationBlock() {
    error(ParseError::Type::INVALID_SYNTAX, "Configuration blocks not yet implemented");
    synchronize();
    return nullptr;
}

std::shared_ptr<Node> BasicParser::parseImportBlock() {
    error(ParseError::Type::INVALID_SYNTAX, "Import blocks not yet implemented");
    synchronize();
    return nullptr;
}

std::shared_ptr<Node> BasicParser::parseNamespaceBlock() {
    error(ParseError::Type::INVALID_SYNTAX, "Namespace blocks not yet implemented");
    synchronize();
    return nullptr;
}

// ParserFactory 实现
std::unique_ptr<BasicParser> ParserFactory::createBasicParser(const std::vector<Token>& tokens) {
    return std::make_unique<BasicParser>(tokens);
}

std::shared_ptr<Node> ParserFactory::parseTokens(const std::vector<Token>& tokens) {
    auto parser = createBasicParser(tokens);
    return parser->parse();
}

} // namespace chtl