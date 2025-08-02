#pragma once
#include "Node.h"
#include <vector>
#include <unordered_map>

namespace chtl {

/**
 * 原始嵌入节点基类
 * 表示[Origin]原始代码嵌入
 */
class OriginNode : public Node {
public:
    // 原始内容类型
    enum class OriginType {
        HTML,           // @Html
        CSS,            // @Style
        JAVASCRIPT      // @JavaScript
    };

    explicit OriginNode(OriginType type, const NodePosition& position = NodePosition());
    virtual ~OriginNode() = default;

    // 原始类型管理
    OriginType getOriginType() const;
    void setOriginType(OriginType type);

    // 原始内容管理
    const std::string& getRawContent() const;
    void setRawContent(const std::string& content);
    void appendContent(const std::string& content);

    // 文件来源管理
    const std::string& getSourceFile() const;
    void setSourceFile(const std::string& file);
    bool hasSourceFile() const;

    // 内容验证
    virtual bool validateContent() const;
    virtual bool isValidSyntax() const = 0;

    // 内容处理
    virtual std::string getProcessedContent() const;
    virtual std::string escapeContent() const;

    // 输出格式化
    virtual std::string formatForOutput() const = 0;

    // 验证
    bool validate() const override;

    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;

    // 克隆
    std::shared_ptr<Node> clone() const override;

    // 访问者模式
    void accept(NodeVisitor& visitor) override;

protected:
    OriginType originType_;
    std::string rawContent_;
    std::string sourceFile_;

    std::string originTypeToString() const;
};

/**
 * 原始HTML节点
 * 表示[Origin] @Html嵌入的HTML代码
 */
class OriginHtmlNode : public OriginNode {
public:
    explicit OriginHtmlNode(const NodePosition& position = NodePosition());
    virtual ~OriginHtmlNode() = default;

    // HTML特定验证
    bool isValidSyntax() const override;
    bool validateHtmlStructure() const;
    bool hasValidTags() const;

    // HTML处理
    std::string formatForOutput() const override;
    std::string sanitizeHtml() const;
    std::string extractInlineStyles() const;

    // HTML分析
    std::vector<std::string> getContainedTags() const;
    std::vector<std::string> getUsedAttributes() const;
    bool containsScript() const;
    bool containsStyle() const;

    // 验证
    bool validate() const override;

    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;

    // 克隆
    std::shared_ptr<Node> clone() const override;

    // 访问者模式
    void accept(NodeVisitor& visitor) override;

private:
    bool isValidHtmlTag(const std::string& tag) const;
    std::vector<std::string> parseHtmlTags() const;
};

/**
 * 原始CSS节点
 * 表示[Origin] @Style嵌入的CSS代码
 */
class OriginCssNode : public OriginNode {
public:
    explicit OriginCssNode(const NodePosition& position = NodePosition());
    virtual ~OriginCssNode() = default;

    // CSS特定验证
    bool isValidSyntax() const override;
    bool validateCssRules() const;
    bool hasValidSelectors() const;

    // CSS处理
    std::string formatForOutput() const override;
    std::string minifyCss() const;
    std::string addCssPrefix(const std::string& prefix) const;

    // CSS分析
    std::vector<std::string> getSelectors() const;
    std::vector<std::string> getProperties() const;
    std::vector<std::string> getMediaQueries() const;
    bool containsImports() const;
    bool containsKeyframes() const;

    // CSS优化
    std::string optimizeCss() const;
    std::string removeDuplicateRules() const;

    // 验证
    bool validate() const override;

    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;

    // 克隆
    std::shared_ptr<Node> clone() const override;

    // 访问者模式
    void accept(NodeVisitor& visitor) override;

private:
    bool isValidCssSelector(const std::string& selector) const;
    bool isValidCssProperty(const std::string& property) const;
    std::vector<std::string> parseCssRules() const;
};

/**
 * 原始JavaScript节点
 * 表示[Origin] @JavaScript嵌入的JavaScript代码
 */
class OriginJavaScriptNode : public OriginNode {
public:
    explicit OriginJavaScriptNode(const NodePosition& position = NodePosition());
    virtual ~OriginJavaScriptNode() = default;

    // JavaScript特定验证
    bool isValidSyntax() const override;
    bool validateJavaScript() const;
    bool hasValidFunctions() const;

    // JavaScript处理
    std::string formatForOutput() const override;
    std::string minifyJavaScript() const;
    std::string wrapInScriptTag() const;

    // JavaScript分析
    std::vector<std::string> getFunctionNames() const;
    std::vector<std::string> getVariableNames() const;
    std::vector<std::string> getGlobalReferences() const;
    bool containsModuleImports() const;
    bool containsAsyncCode() const;

    // 安全检查
    bool isSafeCode() const;
    std::vector<std::string> getSecurityIssues() const;
    bool containsDangerousFunctions() const;

    // JavaScript优化
    std::string optimizeJavaScript() const;
    std::string removeComments() const;

    // 验证
    bool validate() const override;

    // 序列化
    std::string toString() const override;
    std::string toDebugString(int indent = 0) const override;

    // 克隆
    std::shared_ptr<Node> clone() const override;

    // 访问者模式
    void accept(NodeVisitor& visitor) override;

private:
    bool isValidJavaScriptIdentifier(const std::string& identifier) const;
    bool containsForbiddenCode(const std::string& code) const;
    std::vector<std::string> parseJavaScriptTokens() const;
};

/**
 * 原始内容管理器
 * 管理所有原始嵌入内容
 */
class OriginManager {
public:
    static OriginManager& getInstance();

    // 注册原始内容
    bool registerOriginNode(std::shared_ptr<OriginNode> node);
    void unregisterOriginNode(const std::string& nodeId);

    // 查找原始内容
    std::shared_ptr<OriginNode> findOriginNode(const std::string& nodeId) const;
    std::vector<std::shared_ptr<OriginNode>> findOriginNodesByType(OriginNode::OriginType type) const;

    // 获取所有原始内容
    std::vector<std::shared_ptr<OriginNode>> getAllOriginNodes() const;
    std::vector<std::shared_ptr<OriginHtmlNode>> getAllHtmlNodes() const;
    std::vector<std::shared_ptr<OriginCssNode>> getAllCssNodes() const;
    std::vector<std::shared_ptr<OriginJavaScriptNode>> getAllJavaScriptNodes() const;

    // 内容聚合
    std::string aggregateHtmlContent() const;
    std::string aggregateCssContent() const;
    std::string aggregateJavaScriptContent() const;

    // 验证
    bool validateAllContent() const;
    std::vector<std::string> getValidationErrors() const;

    // 优化
    void optimizeAllContent();
    void minifyAllContent();

    // 安全检查
    bool isAllContentSafe() const;
    std::vector<std::string> getSecurityIssues() const;

    // 统计信息
    size_t getHtmlNodeCount() const;
    size_t getCssNodeCount() const;
    size_t getJavaScriptNodeCount() const;
    size_t getTotalContentSize() const;

    // 清理
    void clear();
    void clearByType(OriginNode::OriginType type);

    // 调试信息
    std::string getDebugInfo() const;

private:
    OriginManager() = default;
    ~OriginManager() = default;
    OriginManager(const OriginManager&) = delete;
    OriginManager& operator=(const OriginManager&) = delete;

    std::vector<std::shared_ptr<OriginNode>> originNodes_;
    size_t nextNodeId_;

    std::string generateNodeId();
};

/**
 * 原始内容解析器
 * 用于解析原始嵌入语法
 */
class OriginParser {
public:
    explicit OriginParser();
    ~OriginParser() = default;

    // 解析原始内容块
    std::shared_ptr<OriginNode> parseOriginBlock(const std::string& content, 
                                                OriginNode::OriginType type);
    std::shared_ptr<OriginHtmlNode> parseHtmlBlock(const std::string& content);
    std::shared_ptr<OriginCssNode> parseCssBlock(const std::string& content);
    std::shared_ptr<OriginJavaScriptNode> parseJavaScriptBlock(const std::string& content);

    // 解析原始语法标记
    OriginNode::OriginType parseOriginType(const std::string& typeString);
    bool isValidOriginSyntax(const std::string& content);

    // 内容提取
    std::string extractRawContent(const std::string& originBlock);
    std::string extractSourceFile(const std::string& originBlock);

    // 语法验证
    bool validateOriginSyntax(const std::string& content, OriginNode::OriginType type);
    bool validateHtmlSyntax(const std::string& htmlContent);
    bool validateCssSyntax(const std::string& cssContent);
    bool validateJavaScriptSyntax(const std::string& jsContent);

    // 错误处理
    std::string getLastError() const;
    bool hasError() const;
    void clearErrors();
    std::vector<std::string> getAllErrors() const;

private:
    std::vector<std::string> errors_;

    void addError(const std::string& error);
    std::string trim(const std::string& str) const;
    bool isCommentLine(const std::string& line) const;
    bool isEmptyLine(const std::string& line) const;
};

/**
 * 原始内容工厂
 * 用于创建原始嵌入节点
 */
class OriginFactory {
public:
    // 创建原始节点
    static std::shared_ptr<OriginHtmlNode> createHtmlNode(const std::string& content);
    static std::shared_ptr<OriginCssNode> createCssNode(const std::string& content);
    static std::shared_ptr<OriginJavaScriptNode> createJavaScriptNode(const std::string& content);

    // 从文件创建
    static std::shared_ptr<OriginHtmlNode> createHtmlNodeFromFile(const std::string& filePath);
    static std::shared_ptr<OriginCssNode> createCssNodeFromFile(const std::string& filePath);
    static std::shared_ptr<OriginJavaScriptNode> createJavaScriptNodeFromFile(const std::string& filePath);

    // 从URL创建
    static std::shared_ptr<OriginCssNode> createCssNodeFromUrl(const std::string& url);
    static std::shared_ptr<OriginJavaScriptNode> createJavaScriptNodeFromUrl(const std::string& url);

private:
    OriginFactory() = default;
    ~OriginFactory() = default;

    static std::string readFileContent(const std::string& filePath);
    static std::string fetchUrlContent(const std::string& url);
};

} // namespace chtl