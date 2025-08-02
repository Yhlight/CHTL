#ifndef CHTL_BASIC_PARSER_H
#define CHTL_BASIC_PARSER_H

#include "Parser.h"
#include "../loader/ChtlLoader.h"
#include <memory>

namespace chtl {

// BasicParser - 解析CHTL的基础语法
class BasicParser : public Parser {
protected:
    std::shared_ptr<ChtlLoader> loader;  // 文件加载器
    std::string currentFilePath;         // 当前解析的文件路径
    
    // 解析辅助方法
    NodePtr parseStatement();
    NodePtr parseElement();
    NodePtr parseTextNode();
    NodePtr parseStyleNode();
    NodePtr parseComment();
    NodePtr parseImport();
    NodePtr parseConfiguration();
    NodePtr parseCustomDefinition();
    NodePtr parseTemplateDefinition();
    NodePtr parseNamespace();
    NodePtr parseExpect();
    NodePtr parseOrigin();
    
    // 解析元素内容
    void parseElementContent(ElementNode* element);
    void parseAttributes(ElementNode* element);
    NodePtr parseAttribute();
    
    // 解析样式相关
    void parseStyleContent(StyleNode* style);
    NodePtr parseStyleRule();
    NodePtr parseStyleSelector();
    NodePtr parseStyleProperty();
    
    // 解析自定义定义
    NodePtr parseCustomStyle();
    NodePtr parseCustomElement();
    NodePtr parseCustomVar();
    
    // 解析模板定义
    NodePtr parseTemplateStyle();
    NodePtr parseTemplateElement();
    NodePtr parseTemplateVar();
    
    // 解析导入相关
    ImportNode::ImportType parseImportType();
    std::string parseImportPath();
    void processImport(ImportNode* importNode);
    
    // 解析变量组和样式组
    void parseVarGroupContent(CustomVarNode* varNode);
    void parseStyleGroupContent(StyleGroupNode* styleGroup);
    
    // 解析操作符
    NodePtr parseAddOperation();
    NodePtr parseDeleteOperation();
    NodePtr parseInheritOperation();
    
    // 解析原始嵌入
    NodePtr parseOriginHtml();
    NodePtr parseOriginStyle();
    NodePtr parseOriginJavaScript();
    
    // 工具方法
    bool isHtmlElement(const std::string& name);
    bool isAtEndOfBlock();
    std::string parseStringLiteral();
    std::string parseUnquotedString();
    std::string parseIdentifierOrKeyword();
    
public:
    BasicParser();
    explicit BasicParser(std::shared_ptr<ChtlLoader> fileLoader);
    virtual ~BasicParser() = default;
    
    // 设置文件加载器
    void setLoader(std::shared_ptr<ChtlLoader> fileLoader) {
        loader = fileLoader;
    }
    
    // 获取文件加载器
    std::shared_ptr<ChtlLoader> getLoader() const {
        return loader;
    }
    
    // 设置当前文件路径
    void setCurrentFilePath(const std::string& path) {
        currentFilePath = path;
    }
    
    // 主要解析接口
    NodePtr parse(const std::vector<Token>& tokenList) override;
    
    // 解析文件
    NodePtr parseFile(const std::string& filePath);
    
    // 解析导入的文件并合并到当前AST
    bool parseAndMergeImport(const std::string& filePath, RootNode* root);
};

} // namespace chtl

#endif // CHTL_BASIC_PARSER_H