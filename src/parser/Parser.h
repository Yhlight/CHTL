#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <set>
#include <map>
#include "../common/Token.h"
#include "../node/Node.h"
#include "../node/Custom.h"
#include "../node/Config.h"
#include "../node/Style.h"

namespace chtl {

// Parser上下文，用于辅助语法分析
struct ParserContext {
    bool inStyleBlock = false;           // 是否在style块中
    bool inCustomDefinition = false;     // 是否在自定义定义中
    bool inTemplateDefinition = false;   // 是否在模板定义中
    bool inConfigurationBlock = false;   // 是否在配置块中
    bool inNamespaceBlock = false;       // 是否在命名空间块中
    std::string currentNamespace;        // 当前命名空间
    int elementDepth = 0;                // 元素嵌套深度
    
    // 自定义元素、样式、变量的注册表
    std::map<std::string, NodePtr> customElements;
    std::map<std::string, NodePtr> customStyles;
    std::map<std::string, NodePtr> customVars;
    std::map<std::string, NodePtr> templateElements;
    std::map<std::string, NodePtr> templateStyles;
    std::map<std::string, NodePtr> templateVars;
};

// Parser异常类
class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message, int line = 0, int column = 0)
        : std::runtime_error(formatMessage(message, line, column)),
          line_(line), column_(column) {}
    
    int getLine() const { return line_; }
    int getColumn() const { return column_; }
    
private:
    int line_;
    int column_;
    
    static std::string formatMessage(const std::string& msg, int line, int col) {
        return "Parse error at line " + std::to_string(line) + 
               ", column " + std::to_string(col) + ": " + msg;
    }
};

// Parser基类
class Parser {
protected:
    std::vector<Token> tokens;   // Token列表
    size_t current;              // 当前Token索引
    ParserContext context;       // 解析上下文
    
    // Token操作方法
    Token peek(size_t offset = 0) const;
    Token previous() const;
    Token advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    Token consume(TokenType type, const std::string& message);
    
    // 错误处理
    void error(const std::string& message);
    void error(const Token& token, const std::string& message);
    ParseError createError(const std::string& message);
    ParseError createError(const Token& token, const std::string& message);
    
    // 同步恢复（错误恢复）
    void synchronize();
    
    // 辅助方法
    bool isHtmlElement(const std::string& name) const;
    bool isCustomElement(const std::string& name) const;
    bool isTemplateElement(const std::string& name) const;
    
public:
    Parser();
    virtual ~Parser() = default;
    
    // 主要的解析接口
    virtual NodePtr parse(const std::vector<Token>& tokenList) = 0;
    
    // 获取和设置上下文
    ParserContext& getContext() { return context; }
    const ParserContext& getContext() const { return context; }
};

// HTML元素列表（用于判断是否是HTML元素）
extern const std::set<std::string> HTML_ELEMENTS;

} // namespace chtl

#endif // CHTL_PARSER_H