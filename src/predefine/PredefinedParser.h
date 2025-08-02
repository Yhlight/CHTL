#ifndef CHTL_PREDEFINED_PARSER_H
#define CHTL_PREDEFINED_PARSER_H

#include <string>
#include <memory>

namespace chtl {

// 前向声明
class CustomStyleNode;
class CustomElementNode;
class CustomVarNode;
class TemplateStyleNode;
class TemplateElementNode;
class TemplateVarNode;

// 预定义内容解析器 - 将CHTL代码解析为对应的节点
class PredefinedParser {
private:
    // 解析CHTL代码并返回根节点
    static std::shared_ptr<Node> parseChtlCode(const std::string& chtlCode);
    
    // 从解析的根节点中提取特定类型的节点
    template<typename T>
    static std::shared_ptr<T> extractNode(std::shared_ptr<Node> root, const std::string& name);
    
public:
    // 解析自定义样式
    static std::shared_ptr<CustomStyleNode> parseCustomStyle(const std::string& name, 
                                                            const std::string& chtlCode);
    
    // 解析自定义元素
    static std::shared_ptr<CustomElementNode> parseCustomElement(const std::string& name, 
                                                                const std::string& chtlCode);
    
    // 解析自定义变量组
    static std::shared_ptr<CustomVarNode> parseCustomVar(const std::string& name, 
                                                        const std::string& chtlCode);
    
    // 解析模板样式
    static std::shared_ptr<TemplateStyleNode> parseTemplateStyle(const std::string& name, 
                                                                const std::string& chtlCode);
    
    // 解析模板元素
    static std::shared_ptr<TemplateElementNode> parseTemplateElement(const std::string& name, 
                                                                    const std::string& chtlCode);
    
    // 解析模板变量组
    static std::shared_ptr<TemplateVarNode> parseTemplateVar(const std::string& name, 
                                                            const std::string& chtlCode);
};

} // namespace chtl

#endif // CHTL_PREDEFINED_PARSER_H