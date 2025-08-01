#pragma once
#include <stack>
#include <string>
#include <vector>

// 前向声明
namespace chtl {
    enum class TokenType;
}

namespace chtl {

/**
 * 解析上下文枚举
 * 用于状态机的上下文推导
 */
enum class ParseContext {
    GLOBAL,                 // 全局上下文
    ELEMENT,                // 元素上下文
    STYLE_BLOCK,            // 样式块上下文
    TEXT_BLOCK,             // 文本块上下文
    CUSTOM_BLOCK,           // 自定义块上下文
    TEMPLATE_BLOCK,         // 模板块上下文
    ORIGIN_BLOCK,           // 原始嵌入块上下文
    CONFIG_BLOCK,           // 配置块上下文
    IMPORT_BLOCK,           // 导入块上下文
    NAMESPACE_BLOCK,        // 命名空间块上下文
    
    // 样式相关子上下文
    CSS_SELECTOR,           // CSS选择器上下文
    CSS_PROPERTY,           // CSS属性上下文
    CSS_VALUE,              // CSS值上下文
    
    // 自定义相关子上下文
    CUSTOM_STYLE,           // 自定义样式组上下文
    CUSTOM_ELEMENT,         // 自定义元素上下文
    CUSTOM_VAR,             // 自定义变量组上下文
    
    // 模板相关子上下文
    TEMPLATE_STYLE,         // 模板样式组上下文
    TEMPLATE_ELEMENT,       // 模板元素上下文
    TEMPLATE_VAR,           // 模板变量组上下文
    
    // 特殊上下文
    ATTRIBUTE,              // 属性上下文
    ATTRIBUTE_VALUE,        // 属性值上下文
    INDEX_ACCESS,           // 索引访问上下文
    VARIABLE_CALL,          // 变量调用上下文
    SPECIALIZATION,         // 特例化上下文
    INHERITANCE,            // 继承上下文
    
    // 配置相关子上下文
    CONFIG_KEY,             // 配置键上下文
    CONFIG_VALUE,           // 配置值上下文
    CONFIG_NAME_BLOCK,      // Name配置块上下文
    CONFIG_OPTION_LIST      // 配置选项列表上下文
};

/**
 * 词法分析器状态枚举
 */
enum class LexerState {
    START,                  // 开始状态
    IDENTIFIER,             // 标识符状态
    STRING_DOUBLE,          // 双引号字符串状态
    STRING_SINGLE,          // 单引号字符串状态
    STRING_NO_QUOTE,        // 无引号字符串状态
    NUMBER,                 // 数字状态
    COMMENT_SINGLE,         // 单行注释状态
    COMMENT_MULTI,          // 多行注释状态
    COMMENT_GENERATOR,      // 生成器注释状态
    
    // 特殊符号状态
    DOT,                    // 点号状态 (可能是CSS选择器)
    HASH,                   // 井号状态 (可能是ID选择器)
    AT,                     // @符号状态
    AMPERSAND,              // &符号状态 (伪类伪元素)
    COLON,                  // 冒号状态 (可能是伪类)
    COLON_COLON,            // 双冒号状态 (伪元素)
    
    // 括号相关状态
    LEFT_BRACKET,           // 左方括号状态
    INDEX_CONTENT,          // 索引内容状态
    CONFIG_BRACKET,         // 配置方括号状态
    
    // 配置相关状态
    CONFIG_KEY,             // 配置键状态
    CONFIG_VALUE,           // 配置值状态
    CONFIG_OPTION_START,    // 配置选项开始状态
    CONFIG_OPTION_CONTENT,  // 配置选项内容状态
    
    // 原始嵌入状态
    ORIGIN_HTML,            // 原始HTML状态
    ORIGIN_CSS,             // 原始CSS状态
    ORIGIN_JS,              // 原始JavaScript状态
    
    // 错误状态
    ERROR                   // 错误状态
};

/**
 * 上下文管理器
 * 负责管理解析上下文栈和状态转换
 */
class ContextManager {
public:
    ContextManager();
    
    // 上下文栈操作
    void pushContext(ParseContext context);
    void popContext();
    ParseContext currentContext() const;
    ParseContext previousContext() const;
    bool isInContext(ParseContext context) const;
    size_t contextDepth() const;
    
    // 状态管理
    void setState(LexerState state);
    LexerState currentState() const;
    LexerState previousState() const;
    
    // 上下文查询
    bool isInStyleContext() const;
    bool isInCustomContext() const;
    bool isInTemplateContext() const;
    bool isInConfigContext() const;
    bool isInOriginContext() const;
    
    // 特殊上下文检查
    bool canUseCssSelector() const;
    bool canUseHtmlTag() const;
    bool canUseCustomKeyword() const;
    bool canUseTemplateKeyword() const;
    bool canUseAttribute() const;
    
    // 上下文推导辅助
    bool shouldInferAsHtmlTag(const std::string& identifier) const;
    bool shouldInferAsCssSelector(const std::string& identifier) const;
    bool shouldInferAsCustomElement(const std::string& identifier) const;
    bool shouldInferAsVariable(const std::string& identifier) const;
    
    // 重置
    void reset();
    
    // 调试信息
    std::string getContextStackString() const;
    std::string getStateString() const;
    
private:
    std::stack<ParseContext> contextStack_;
    LexerState currentState_;
    LexerState previousState_;
    
    // 辅助方法
    bool isTopLevelContext() const;
    bool isInBlockContext() const;
};

/**
 * 状态机转换表
 */
class StateMachine {
public:
    StateMachine();
    
    // 状态转换
    LexerState getNextState(LexerState currentState, char input, ParseContext context) const;
    
    // 检查状态转换是否有效
    bool isValidTransition(LexerState from, LexerState to, ParseContext context) const;
    
    // 获取状态对应的Token类型
    TokenType getTokenTypeForState(LexerState state, const std::string& value, ParseContext context) const;
    
private:
    // 状态转换表
    void initializeTransitionTable();
    
    // 基础字符检查
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    
    // 特殊字符检查
    bool isQuote(char c) const;
    bool isSingleQuote(char c) const;
    bool isDoubleQuote(char c) const;
    bool isPunctuation(char c) const;
    
    // 上下文相关的字符检查
    bool canStartIdentifier(char c, ParseContext context) const;
    bool canContinueIdentifier(char c, ParseContext context) const;
    bool isValidInNoQuoteString(char c, ParseContext context) const;
};

} // namespace chtl