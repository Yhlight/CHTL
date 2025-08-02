#include "Context.h"
#include "Token.h"
#include <sstream>
#include <cctype>
#include <vector>

namespace chtl {

// ContextManager 实现
ContextManager::ContextManager() 
    : currentState_(LexerState::START), previousState_(LexerState::START) {
    contextStack_.push(ParseContext::GLOBAL);
}

void ContextManager::pushContext(ParseContext context) {
    contextStack_.push(context);
}

void ContextManager::popContext() {
    if (contextStack_.size() > 1) {  // 保留全局上下文
        contextStack_.pop();
    }
}

ParseContext ContextManager::currentContext() const {
    return contextStack_.empty() ? ParseContext::GLOBAL : contextStack_.top();
}

ParseContext ContextManager::previousContext() const {
    if (contextStack_.size() <= 1) {
        return ParseContext::GLOBAL;
    }
    
    std::stack<ParseContext> tempStack = contextStack_;
    tempStack.pop();
    return tempStack.top();
}

bool ContextManager::isInContext(ParseContext context) const {
    std::stack<ParseContext> tempStack = contextStack_;
    while (!tempStack.empty()) {
        if (tempStack.top() == context) {
            return true;
        }
        tempStack.pop();
    }
    return false;
}

size_t ContextManager::contextDepth() const {
    return contextStack_.size();
}

void ContextManager::setState(LexerState state) {
    previousState_ = currentState_;
    currentState_ = state;
}

LexerState ContextManager::currentState() const {
    return currentState_;
}

LexerState ContextManager::previousState() const {
    return previousState_;
}

bool ContextManager::isInStyleContext() const {
    return isInContext(ParseContext::STYLE_BLOCK) ||
           isInContext(ParseContext::CSS_SELECTOR) ||
           isInContext(ParseContext::CSS_PROPERTY) ||
           isInContext(ParseContext::CSS_VALUE);
}

bool ContextManager::isInCustomContext() const {
    return isInContext(ParseContext::CUSTOM_BLOCK) ||
           isInContext(ParseContext::CUSTOM_STYLE) ||
           isInContext(ParseContext::CUSTOM_ELEMENT) ||
           isInContext(ParseContext::CUSTOM_VAR);
}

bool ContextManager::isInTemplateContext() const {
    return isInContext(ParseContext::TEMPLATE_BLOCK) ||
           isInContext(ParseContext::TEMPLATE_STYLE) ||
           isInContext(ParseContext::TEMPLATE_ELEMENT) ||
           isInContext(ParseContext::TEMPLATE_VAR);
}

bool ContextManager::isInConfigContext() const {
    return isInContext(ParseContext::CONFIG_BLOCK) ||
           isInContext(ParseContext::CONFIG_KEY) ||
           isInContext(ParseContext::CONFIG_VALUE) ||
           isInContext(ParseContext::CONFIG_NAME_BLOCK) ||
           isInContext(ParseContext::CONFIG_OPTION_LIST);
}

bool ContextManager::isInOriginContext() const {
    return isInContext(ParseContext::ORIGIN_BLOCK);
}

bool ContextManager::canUseCssSelector() const {
    return isInStyleContext() || 
           currentContext() == ParseContext::ELEMENT;
}

bool ContextManager::canUseHtmlTag() const {
    return currentContext() == ParseContext::GLOBAL ||
           currentContext() == ParseContext::ELEMENT ||
           isInCustomContext() ||
           isInTemplateContext();
}

bool ContextManager::canUseCustomKeyword() const {
    return currentContext() == ParseContext::GLOBAL ||
           isInCustomContext();
}

bool ContextManager::canUseTemplateKeyword() const {
    return currentContext() == ParseContext::GLOBAL ||
           isInTemplateContext();
}

bool ContextManager::canUseAttribute() const {
    return currentContext() == ParseContext::ELEMENT ||
           isInCustomContext() ||
           isInTemplateContext();
}

bool ContextManager::shouldInferAsHtmlTag(const std::string& identifier) const {
    if (!canUseHtmlTag()) {
        return false;
    }
    
    // 检查是否为有效的HTML标签
    return HtmlTagRegistry::getInstance().isValidHtmlTag(identifier);
}

bool ContextManager::shouldInferAsCssSelector(const std::string& identifier) const {
    if (!canUseCssSelector()) {
        return false;
    }
    
    // 在样式上下文中，标识符更可能是CSS选择器
    return isInStyleContext();
}

bool ContextManager::shouldInferAsCustomElement(const std::string& identifier) const {
    // 检查是否在可以使用自定义元素的上下文中
    return canUseCustomKeyword() && !HtmlTagRegistry::getInstance().isValidHtmlTag(identifier);
}

bool ContextManager::shouldInferAsVariable(const std::string& identifier) const {
    // 变量调用通常出现在属性值或CSS值上下文中
    return currentContext() == ParseContext::ATTRIBUTE_VALUE ||
           currentContext() == ParseContext::CSS_VALUE ||
           isInContext(ParseContext::VARIABLE_CALL);
}

void ContextManager::reset() {
    while (!contextStack_.empty()) {
        contextStack_.pop();
    }
    contextStack_.push(ParseContext::GLOBAL);
    currentState_ = LexerState::START;
    previousState_ = LexerState::START;
}

std::string ContextManager::getContextStackString() const {
    std::ostringstream oss;
    std::stack<ParseContext> tempStack = contextStack_;
    std::vector<std::string> contexts;
    
    while (!tempStack.empty()) {
        ParseContext ctx = tempStack.top();
        tempStack.pop();
        
        switch (ctx) {
            case ParseContext::GLOBAL: contexts.push_back("GLOBAL"); break;
            case ParseContext::ELEMENT: contexts.push_back("ELEMENT"); break;
            case ParseContext::STYLE_BLOCK: contexts.push_back("STYLE_BLOCK"); break;
            case ParseContext::TEXT_BLOCK: contexts.push_back("TEXT_BLOCK"); break;
            case ParseContext::CUSTOM_BLOCK: contexts.push_back("CUSTOM_BLOCK"); break;
            case ParseContext::TEMPLATE_BLOCK: contexts.push_back("TEMPLATE_BLOCK"); break;
            case ParseContext::ORIGIN_BLOCK: contexts.push_back("ORIGIN_BLOCK"); break;
            case ParseContext::CONFIG_BLOCK: contexts.push_back("CONFIG_BLOCK"); break;
            case ParseContext::CSS_SELECTOR: contexts.push_back("CSS_SELECTOR"); break;
            case ParseContext::CSS_PROPERTY: contexts.push_back("CSS_PROPERTY"); break;
            case ParseContext::CSS_VALUE: contexts.push_back("CSS_VALUE"); break;
            case ParseContext::CUSTOM_STYLE: contexts.push_back("CUSTOM_STYLE"); break;
            case ParseContext::CUSTOM_ELEMENT: contexts.push_back("CUSTOM_ELEMENT"); break;
            case ParseContext::CUSTOM_VAR: contexts.push_back("CUSTOM_VAR"); break;
            case ParseContext::ATTRIBUTE: contexts.push_back("ATTRIBUTE"); break;
            case ParseContext::ATTRIBUTE_VALUE: contexts.push_back("ATTRIBUTE_VALUE"); break;
            default: contexts.push_back("UNKNOWN"); break;
        }
    }
    
    // 反转顺序（栈顶到栈底）
    for (int i = contexts.size() - 1; i >= 0; --i) {
        if (i < contexts.size() - 1) oss << " -> ";
        oss << contexts[i];
    }
    
    return oss.str();
}

std::string ContextManager::getStateString() const {
    switch (currentState_) {
        case LexerState::START: return "START";
        case LexerState::IDENTIFIER: return "IDENTIFIER";
        case LexerState::STRING_DOUBLE: return "STRING_DOUBLE";
        case LexerState::STRING_SINGLE: return "STRING_SINGLE";
        case LexerState::STRING_NO_QUOTE: return "STRING_NO_QUOTE";
        case LexerState::NUMBER: return "NUMBER";
        case LexerState::COMMENT_SINGLE: return "COMMENT_SINGLE";
        case LexerState::COMMENT_MULTI: return "COMMENT_MULTI";
        case LexerState::DOT: return "DOT";
        case LexerState::HASH: return "HASH";
        case LexerState::AT: return "AT";
        case LexerState::COLON: return "COLON";
        case LexerState::LEFT_BRACKET: return "LEFT_BRACKET";
        case LexerState::CONFIG_KEY: return "CONFIG_KEY";
        case LexerState::CONFIG_VALUE: return "CONFIG_VALUE";
        case LexerState::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

bool ContextManager::isTopLevelContext() const {
    return currentContext() == ParseContext::GLOBAL;
}

bool ContextManager::isInBlockContext() const {
    ParseContext ctx = currentContext();
    return ctx == ParseContext::STYLE_BLOCK ||
           ctx == ParseContext::TEXT_BLOCK ||
           ctx == ParseContext::CUSTOM_BLOCK ||
           ctx == ParseContext::TEMPLATE_BLOCK ||
           ctx == ParseContext::ORIGIN_BLOCK ||
           ctx == ParseContext::CONFIG_BLOCK;
}

// StateMachine 实现
StateMachine::StateMachine() {
    initializeTransitionTable();
}

void StateMachine::initializeTransitionTable() {
    // 状态转换表在实际实现中会更复杂
    // 这里提供基础框架
}

LexerState StateMachine::getNextState(LexerState currentState, char input, ParseContext context) const {
    switch (currentState) {
        case LexerState::START:
            if (isAlpha(input) || input == '_') return LexerState::IDENTIFIER;
            if (isDigit(input)) return LexerState::NUMBER;
            if (input == '"') return LexerState::STRING_DOUBLE;
            if (input == '\'') return LexerState::STRING_SINGLE;
            if (input == '.') return LexerState::DOT;
            if (input == '#') return LexerState::HASH;
            if (input == '@') return LexerState::AT;
            if (input == ':') return LexerState::COLON;
            if (input == '[') return LexerState::LEFT_BRACKET;
            if (input == '/') {
                // 需要预读下一个字符判断是否为注释
                return LexerState::START; // 暂时返回START，实际需要更复杂的处理
            }
            break;
            
        case LexerState::IDENTIFIER:
            if (isAlphaNumeric(input) || input == '_' || input == '-') {
                return LexerState::IDENTIFIER;
            }
            return LexerState::START;
            
        case LexerState::NUMBER:
            if (isDigit(input) || input == '.') {
                return LexerState::NUMBER;
            }
            return LexerState::START;
            
        case LexerState::STRING_DOUBLE:
            if (input == '"') return LexerState::START;
            return LexerState::STRING_DOUBLE;
            
        case LexerState::STRING_SINGLE:
            if (input == '\'') return LexerState::START;
            return LexerState::STRING_SINGLE;
            
        case LexerState::DOT:
            if (isAlpha(input)) return LexerState::IDENTIFIER; // CSS类选择器
            return LexerState::START;
            
        case LexerState::HASH:
            if (isAlphaNumeric(input)) return LexerState::IDENTIFIER; // CSS ID选择器
            return LexerState::START;
            
        case LexerState::AT:
            if (isAlpha(input)) return LexerState::IDENTIFIER; // @关键字
            return LexerState::START;
            
        case LexerState::COLON:
            if (input == ':') return LexerState::COLON_COLON; // 伪元素
            if (isAlpha(input)) return LexerState::IDENTIFIER; // 伪类
            return LexerState::START;
            
        default:
            return LexerState::START;
    }
    
    return LexerState::START;
}

bool StateMachine::isValidTransition(LexerState from, LexerState to, ParseContext context) const {
    // 实现状态转换验证逻辑
    // 这里需要根据CHTL语法规则进行详细实现
    return true; // 简化实现
}

TokenType StateMachine::getTokenTypeForState(LexerState state, const std::string& value, ParseContext context) const {
    switch (state) {
        case LexerState::IDENTIFIER:
            // 根据上下文和关键字注册表推导Token类型
            if (KeywordRegistry::getInstance().isKeyword(value)) {
                return KeywordRegistry::getInstance().getKeywordType(value);
            }
            if (HtmlTagRegistry::getInstance().isValidHtmlTag(value)) {
                return TokenType::HTML_TAG;
            }
            return TokenType::IDENTIFIER;
            
        case LexerState::NUMBER:
            return TokenType::NUMBER;
            
        case LexerState::STRING_DOUBLE:
            return TokenType::STRING_DOUBLE_QUOTE;
            
        case LexerState::STRING_SINGLE:
            return TokenType::STRING_SINGLE_QUOTE;
            
        case LexerState::STRING_NO_QUOTE:
            return TokenType::STRING_NO_QUOTE;
            
        default:
            return TokenType::UNKNOWN;
    }
}

// 字符检查方法实现
bool StateMachine::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool StateMachine::isDigit(char c) const {
    return std::isdigit(c);
}

bool StateMachine::isAlphaNumeric(char c) const {
    return std::isalnum(c) || c == '_';
}

bool StateMachine::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool StateMachine::isNewline(char c) const {
    return c == '\n';
}

bool StateMachine::isQuote(char c) const {
    return c == '"' || c == '\'';
}

bool StateMachine::isSingleQuote(char c) const {
    return c == '\'';
}

bool StateMachine::isDoubleQuote(char c) const {
    return c == '"';
}

bool StateMachine::isPunctuation(char c) const {
    return std::ispunct(c);
}

bool StateMachine::canStartIdentifier(char c, ParseContext context) const {
    if (isAlpha(c)) return true;
    
    // 在配置上下文中，允许更多字符作为标识符开始
    if (context == ParseContext::CONFIG_BLOCK ||
        context == ParseContext::CONFIG_KEY ||
        context == ParseContext::CONFIG_VALUE) {
        return std::isprint(c) && c != ' ' && c != '\t' && c != '\n' && c != '\r';
    }
    
    return false;
}

bool StateMachine::canContinueIdentifier(char c, ParseContext context) const {
    if (isAlphaNumeric(c) || c == '-') return true;
    
    // 在配置上下文中的特殊处理
    if (context == ParseContext::CONFIG_BLOCK ||
        context == ParseContext::CONFIG_KEY ||
        context == ParseContext::CONFIG_VALUE) {
        return std::isprint(c) && c != ' ' && c != '\t' && c != '\n' && c != '\r' &&
               c != ';' && c != '=' && c != ':' && c != '{' && c != '}';
    }
    
    return false;
}

bool StateMachine::isValidInNoQuoteString(char c, ParseContext context) const {
    // 在无引号字符串中，排除特殊分隔符
    return std::isprint(c) && c != ';' && c != ':' && c != '=' && c != '{' && c != '}' &&
           c != '(' && c != ')' && c != '[' && c != ']' && c != ',' && !isWhitespace(c);
}

} // namespace chtl