#pragma once
#include "../common/Token.h"
#include "../common/Context.h"
#include "../node/Node.h"
#include "../node/ElementNode.h"
#include "../node/StyleNode.h"
#include "../node/CustomNode.h"
#include <vector>
#include <stack>
#include <memory>

namespace chtl {

/**
 * 解析错误类
 * 包含错误信息和位置
 */
class ParseError {
public:
    enum class Type {
        UNEXPECTED_TOKEN,
        MISSING_TOKEN,
        INVALID_SYNTAX,
        INVALID_ATTRIBUTE,
        INVALID_STYLE,
        INVALID_CUSTOM,
        UNMATCHED_BRACE,
        UNEXPECTED_EOF,
        SEMANTIC_ERROR
    };
    
    ParseError(Type type, const std::string& message, const Position& position)
        : type_(type), message_(message), position_(position) {}
    
    Type getType() const { return type_; }
    const std::string& getMessage() const { return message_; }
    const Position& getPosition() const { return position_; }
    
    std::string toString() const;
    
private:
    Type type_;
    std::string message_;
    Position position_;
};

/**
 * 基础语法分析器
 * 将Token序列解析为AST
 * 严格按照CHTL语法文档实现
 */
class BasicParser {
public:
    explicit BasicParser(const std::vector<Token>& tokens);
    virtual ~BasicParser() = default;
    
    // 主要解析接口
    std::shared_ptr<Node> parse();
    std::shared_ptr<Node> parseDocument();
    
    // 错误处理
    const std::vector<ParseError>& getErrors() const;
    bool hasErrors() const;
    void clearErrors();
    
    // 解析状态
    bool isAtEnd() const;
    size_t getCurrentPosition() const;
    const Token& getCurrentToken() const;
    
protected:
    // Token管理
    const Token& peek(size_t offset = 0) const;
    const Token& advance();
    const Token& previous() const;
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType type) const;
    bool checkSequence(std::initializer_list<TokenType> types) const;
    
    // 消费Token
    Token consume(TokenType type, const std::string& message);
    void synchronize();
    
    // 错误处理
    void error(ParseError::Type type, const std::string& message);
    void error(ParseError::Type type, const std::string& message, const Position& position);
    bool recover(std::initializer_list<TokenType> syncTokens);
    
    // 主要解析方法
    virtual std::shared_ptr<Node> parseStatement();
    virtual std::shared_ptr<Node> parseElement();
    virtual std::shared_ptr<Node> parseTextBlock();
    virtual std::shared_ptr<Node> parseStyleBlock();
    virtual std::shared_ptr<Node> parseComment();
    virtual std::shared_ptr<Node> parseCustomBlock();
    virtual std::shared_ptr<Node> parseTemplateBlock();
    virtual std::shared_ptr<Node> parseConfigurationBlock();
    virtual std::shared_ptr<Node> parseImportBlock();
    virtual std::shared_ptr<Node> parseNamespaceBlock();
    
    // 元素解析
    std::shared_ptr<ElementNode> parseHtmlElement();
    void parseElementAttributes(std::shared_ptr<ElementNode> element);
    std::shared_ptr<AttributeNode> parseAttribute();
    void parseElementBody(std::shared_ptr<ElementNode> element);
    
    // 样式解析
    std::shared_ptr<StyleBlockNode> parseElementStyleBlock();
    std::shared_ptr<CssRuleNode> parseCssRule();
    std::shared_ptr<CssSelectorNode> parseCssSelector();
    std::shared_ptr<CssPropertyNode> parseCssProperty();
    void parseInlineStyles(std::shared_ptr<StyleBlockNode> styleBlock);
    
    // 文本解析
    std::shared_ptr<TextNode> parseTextContent();
    std::string parseStringLiteral();
    std::string parseUnquotedString();
    
    // 注释解析
    std::shared_ptr<CommentNode> parseCommentBlock();
    
    // 自定义语法解析
    std::shared_ptr<CustomStyleNode> parseCustomStyle();
    std::shared_ptr<CustomElementNode> parseCustomElement();
    std::shared_ptr<CustomVarNode> parseCustomVar();
    
    // 模板语法解析
    std::shared_ptr<TemplateStyleNode> parseTemplateStyle();
    std::shared_ptr<TemplateElementNode> parseTemplateElement();
    std::shared_ptr<TemplateVarNode> parseTemplateVar();
    
    // 变量和调用解析
    std::shared_ptr<VariableCallNode> parseVariableCall();
    std::shared_ptr<Node> parseStyleGroupCall();
    
    // 配置解析
    std::shared_ptr<Node> parseConfigOption();
    std::shared_ptr<Node> parseNameBlock();
    
    // 实用方法
    bool isElementStart() const;
    bool isStyleStart() const;
    bool isTextStart() const;
    bool isCustomStart() const;
    bool isTemplateStart() const;
    bool isConfigStart() const;
    bool isImportStart() const;
    bool isNamespaceStart() const;
    
    // HTML标签检查
    bool isValidHtmlTag(const std::string& tagName) const;
    bool isSelfClosingTag(const std::string& tagName) const;
    
    // 上下文管理
    void pushContext(ParseContext context);
    void popContext();
    ParseContext getCurrentContext() const;
    bool isInContext(ParseContext context) const;
    
    // 虚方法供子类重写
    virtual bool shouldTreatAsCustomElement(const std::string& identifier) const;
    virtual bool shouldTreatAsStyleGroup(const std::string& identifier) const;
    virtual bool shouldTreatAsVariable(const std::string& identifier) const;
    
private:
    // Token流
    std::vector<Token> tokens_;
    size_t current_;
    
    // 错误收集
    std::vector<ParseError> errors_;
    
    // 上下文栈
    std::stack<ParseContext> contextStack_;
    
    // 解析状态
    bool panicMode_;
    
    // 辅助方法
    void initializeContextStack();
    NodePosition createNodePosition(const Token& startToken, const Token& endToken) const;
    NodePosition createNodePosition(const Token& token) const;
    
    // 错误恢复辅助
    void skipToNextStatement();
    void skipToMatchingBrace();
    bool findSynchronizationPoint();
    
    // 语法验证辅助
    bool validateElementStructure(std::shared_ptr<ElementNode> element);
    bool validateStyleStructure(std::shared_ptr<StyleBlockNode> styleBlock);
    bool validateAttributeSyntax(const std::string& name, const std::string& value);
    
    // 语义分析辅助
    void performSemanticAnalysis(std::shared_ptr<Node> node);
    void checkDuplicateAttributes(std::shared_ptr<ElementNode> element);
    void checkValidElementNesting(std::shared_ptr<ElementNode> parent, std::shared_ptr<ElementNode> child);
};

/**
 * Parser工厂类
 * 用于创建不同类型的Parser
 */
class ParserFactory {
public:
    static std::unique_ptr<BasicParser> createBasicParser(const std::vector<Token>& tokens);
    static std::unique_ptr<class ConfigParser> createConfigParser(const std::vector<Token>& tokens);
    
    // 便利方法
    static std::shared_ptr<Node> parseTokens(const std::vector<Token>& tokens);
    static std::shared_ptr<Node> parseWithConfig(const std::vector<Token>& tokens, 
                                                 const std::unordered_map<std::string, std::string>& config);
};

/**
 * 解析结果类
 * 包含解析生成的AST和错误信息
 */
class ParseResult {
public:
    ParseResult(std::shared_ptr<Node> ast, const std::vector<ParseError>& errors)
        : ast_(ast), errors_(errors), success_(errors.empty()) {}
    
    std::shared_ptr<Node> getAst() const { return ast_; }
    const std::vector<ParseError>& getErrors() const { return errors_; }
    bool isSuccess() const { return success_; }
    
    // 错误统计
    size_t getErrorCount() const { return errors_.size(); }
    std::vector<ParseError> getErrorsByType(ParseError::Type type) const;
    
    // 结果输出
    std::string getErrorSummary() const;
    void printErrors() const;
    
private:
    std::shared_ptr<Node> ast_;
    std::vector<ParseError> errors_;
    bool success_;
};

/**
 * 解析工具类
 * 提供解析相关的实用功能
 */
class ParseUtils {
public:
    // Token序列验证
    static bool validateTokenSequence(const std::vector<Token>& tokens);
    static std::vector<std::string> getValidationIssues(const std::vector<Token>& tokens);
    
    // 解析统计
    static std::unordered_map<NodeType, size_t> getParseStatistics(std::shared_ptr<Node> ast);
    static size_t calculateParseComplexity(std::shared_ptr<Node> ast);
    
    // 错误分析
    static std::string analyzeErrors(const std::vector<ParseError>& errors);
    static std::vector<std::string> getSuggestions(const ParseError& error);
    
    // AST验证
    static bool validateAst(std::shared_ptr<Node> ast);
    static std::vector<std::string> getAstValidationErrors(std::shared_ptr<Node> ast);
    
    // 调试工具
    static std::string tokenSequenceToString(const std::vector<Token>& tokens);
    static std::string parseTreeToString(std::shared_ptr<Node> ast);
    static void dumpParseTree(std::shared_ptr<Node> ast, const std::string& filename);
};

} // namespace chtl