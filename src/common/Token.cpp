#include "Token.h"
#include <unordered_set>

namespace chtl {

// KeywordRegistry 实现
KeywordRegistry& KeywordRegistry::getInstance() {
    static KeywordRegistry instance;
    return instance;
}

KeywordRegistry::KeywordRegistry() {
    initializeDefaultKeywords();
}

void KeywordRegistry::initializeDefaultKeywords() {
    // 基础关键字
    defaultKeywords_["text"] = TokenType::TEXT;
    defaultKeywords_["style"] = TokenType::STYLE;
    
    // 修饰符关键字
    defaultKeywords_["[Custom]"] = TokenType::CUSTOM;
    defaultKeywords_["[Template]"] = TokenType::TEMPLATE;
    defaultKeywords_["[Origin]"] = TokenType::ORIGIN;
    defaultKeywords_["[Configuration]"] = TokenType::CONFIGURATION;
    defaultKeywords_["[Import]"] = TokenType::IMPORT;
    defaultKeywords_["[Namespace]"] = TokenType::NAMESPACE;
    
    // 操作符关键字
    defaultKeywords_["add"] = TokenType::ADD;
    defaultKeywords_["delete"] = TokenType::DELETE;
    defaultKeywords_["inherit"] = TokenType::INHERIT;
    defaultKeywords_["from"] = TokenType::FROM;
    defaultKeywords_["as"] = TokenType::AS;
    defaultKeywords_["except"] = TokenType::EXCEPT;
    defaultKeywords_["not"] = TokenType::NOT;
    
    // 原始嵌入关键字
    defaultKeywords_["@Html"] = TokenType::ORIGIN_HTML;
    defaultKeywords_["@Style"] = TokenType::ORIGIN_STYLE;
    defaultKeywords_["@JavaScript"] = TokenType::ORIGIN_JAVASCRIPT;
    
    // 自定义类型关键字
    defaultKeywords_["@Style"] = TokenType::CUSTOM_STYLE;
    defaultKeywords_["@Element"] = TokenType::CUSTOM_ELEMENT;
    defaultKeywords_["@Var"] = TokenType::CUSTOM_VAR;
    
    // 复制到当前关键字映射
    keywords_ = defaultKeywords_;
}

bool KeywordRegistry::isKeyword(const std::string& word) const {
    return keywords_.find(word) != keywords_.end();
}

TokenType KeywordRegistry::getKeywordType(const std::string& word) const {
    auto it = keywords_.find(word);
    return (it != keywords_.end()) ? it->second : TokenType::UNKNOWN;
}

void KeywordRegistry::updateCustomKeyword(const std::string& configKey, const std::string& keyword) {
    // 根据配置键更新对应的关键字
    if (configKey == "CUSTOM_STYLE") {
        keywords_.erase("@Style");
        keywords_[keyword] = TokenType::CUSTOM_STYLE;
    } else if (configKey == "CUSTOM_ELEMENT") {
        keywords_.erase("@Element");
        keywords_[keyword] = TokenType::CUSTOM_ELEMENT;
    } else if (configKey == "CUSTOM_VAR") {
        keywords_.erase("@Var");
        keywords_[keyword] = TokenType::CUSTOM_VAR;
    } else if (configKey == "TEMPLATE_STYLE") {
        keywords_[keyword] = TokenType::TEMPLATE_STYLE;
    } else if (configKey == "TEMPLATE_ELEMENT") {
        keywords_[keyword] = TokenType::TEMPLATE_ELEMENT;
    } else if (configKey == "TEMPLATE_VAR") {
        keywords_[keyword] = TokenType::TEMPLATE_VAR;
    } else if (configKey == "ORIGIN_HTML") {
        keywords_.erase("@Html");
        keywords_[keyword] = TokenType::ORIGIN_HTML;
    } else if (configKey == "ORIGIN_STYLE") {
        keywords_.erase("@Style");
        keywords_[keyword] = TokenType::ORIGIN_STYLE;
    } else if (configKey == "ORIGIN_JAVASCRIPT") {
        keywords_.erase("@JavaScript");
        keywords_[keyword] = TokenType::ORIGIN_JAVASCRIPT;
    } else if (configKey == "KEYWORD_ADD") {
        keywords_.erase("add");
        keywords_[keyword] = TokenType::ADD;
    } else if (configKey == "KEYWORD_DELETE") {
        keywords_.erase("delete");
        keywords_[keyword] = TokenType::DELETE;
    } else if (configKey == "KEYWORD_INHERIT") {
        keywords_.erase("inherit");
        keywords_[keyword] = TokenType::INHERIT;
    } else if (configKey == "KEYWORD_FROM") {
        keywords_.erase("from");
        keywords_[keyword] = TokenType::FROM;
    } else if (configKey == "KEYWORD_AS") {
        keywords_.erase("as");
        keywords_[keyword] = TokenType::AS;
    } else if (configKey == "KEYWORD_TEXT") {
        keywords_.erase("text");
        keywords_[keyword] = TokenType::TEXT;
    } else if (configKey == "KEYWORD_STYLE") {
        keywords_.erase("style");
        keywords_[keyword] = TokenType::STYLE;
    } else if (configKey == "KEYWORD_CUSTOM") {
        keywords_.erase("[Custom]");
        keywords_[keyword] = TokenType::CUSTOM;
    } else if (configKey == "KEYWORD_TEMPLATE") {
        keywords_.erase("[Template]");
        keywords_[keyword] = TokenType::TEMPLATE;
    } else if (configKey == "KEYWORD_ORIGIN") {
        keywords_.erase("[Origin]");
        keywords_[keyword] = TokenType::ORIGIN;
    } else if (configKey == "KEYWORD_IMPORT") {
        keywords_.erase("[Import]");
        keywords_[keyword] = TokenType::IMPORT;
    } else if (configKey == "KEYWORD_NAMESPACE") {
        keywords_.erase("[Namespace]");
        keywords_[keyword] = TokenType::NAMESPACE;
    }
}

void KeywordRegistry::resetToDefault() {
    keywords_ = defaultKeywords_;
}

// HtmlTagRegistry 实现
HtmlTagRegistry& HtmlTagRegistry::getInstance() {
    static HtmlTagRegistry instance;
    return instance;
}

HtmlTagRegistry::HtmlTagRegistry() {
    initializeHtmlTags();
}

void HtmlTagRegistry::initializeHtmlTags() {
    // HTML5 标准标签
    htmlTags_ = {
        // 根元素
        "html",
        
        // 文档元数据
        "head", "title", "base", "link", "meta", "style",
        
        // 分区根元素
        "body",
        
        // 内容分区
        "article", "aside", "footer", "header", "h1", "h2", "h3", "h4", "h5", "h6",
        "main", "nav", "section",
        
        // 文本内容
        "blockquote", "dd", "div", "dl", "dt", "figcaption", "figure", "hr", "li",
        "ol", "p", "pre", "ul",
        
        // 内联文本语义
        "a", "abbr", "b", "bdi", "bdo", "br", "cite", "code", "data", "dfn", "em",
        "i", "kbd", "mark", "q", "rp", "rt", "ruby", "s", "samp", "small", "span",
        "strong", "sub", "sup", "time", "u", "var", "wbr",
        
        // 图片和多媒体
        "area", "audio", "img", "map", "track", "video",
        
        // 内嵌内容
        "embed", "iframe", "object", "param", "picture", "portal", "source",
        
        // 脚本
        "canvas", "noscript", "script",
        
        // 编辑标识
        "del", "ins",
        
        // 表格内容
        "caption", "col", "colgroup", "table", "tbody", "td", "tfoot", "th",
        "thead", "tr",
        
        // 表单
        "button", "datalist", "fieldset", "form", "input", "label", "legend",
        "meter", "optgroup", "option", "output", "progress", "select", "textarea",
        
        // 交互元素
        "details", "dialog", "menu", "summary",
        
        // Web组件
        "slot", "template"
    };
    
    // 自闭合标签
    selfClosingTags_ = {
        "area", "base", "br", "col", "embed", "hr", "img", "input", "link",
        "meta", "param", "source", "track", "wbr"
    };
}

bool HtmlTagRegistry::isValidHtmlTag(const std::string& tag) const {
    return htmlTags_.find(tag) != htmlTags_.end();
}

bool HtmlTagRegistry::isSelfClosingTag(const std::string& tag) const {
    return selfClosingTags_.find(tag) != selfClosingTags_.end();
}

// TokenUtils 实现
std::string TokenUtils::tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::UNKNOWN: return "UNKNOWN";
        case TokenType::EOF_TOKEN: return "EOF";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_DOUBLE_QUOTE: return "STRING_DOUBLE_QUOTE";
        case TokenType::STRING_SINGLE_QUOTE: return "STRING_SINGLE_QUOTE";
        case TokenType::STRING_NO_QUOTE: return "STRING_NO_QUOTE";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::DOT: return "DOT";
        case TokenType::COMMA: return "COMMA";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::HASH: return "HASH";
        case TokenType::AT: return "AT";
        case TokenType::COMMENT_SINGLE: return "COMMENT_SINGLE";
        case TokenType::COMMENT_MULTI: return "COMMENT_MULTI";
        case TokenType::COMMENT_GENERATOR: return "COMMENT_GENERATOR";
        case TokenType::TEXT: return "TEXT";
        case TokenType::STYLE: return "STYLE";
        case TokenType::CUSTOM: return "CUSTOM";
        case TokenType::TEMPLATE: return "TEMPLATE";
        case TokenType::ORIGIN: return "ORIGIN";
        case TokenType::CONFIGURATION: return "CONFIGURATION";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::NAMESPACE: return "NAMESPACE";
        case TokenType::ADD: return "ADD";
        case TokenType::DELETE: return "DELETE";
        case TokenType::INHERIT: return "INHERIT";
        case TokenType::FROM: return "FROM";
        case TokenType::AS: return "AS";
        case TokenType::EXCEPT: return "EXCEPT";
        case TokenType::NOT: return "NOT";
        case TokenType::ORIGIN_HTML: return "ORIGIN_HTML";
        case TokenType::ORIGIN_STYLE: return "ORIGIN_STYLE";
        case TokenType::ORIGIN_JAVASCRIPT: return "ORIGIN_JAVASCRIPT";
        case TokenType::CUSTOM_STYLE: return "CUSTOM_STYLE";
        case TokenType::CUSTOM_ELEMENT: return "CUSTOM_ELEMENT";
        case TokenType::CUSTOM_VAR: return "CUSTOM_VAR";
        case TokenType::TEMPLATE_STYLE: return "TEMPLATE_STYLE";
        case TokenType::TEMPLATE_ELEMENT: return "TEMPLATE_ELEMENT";
        case TokenType::TEMPLATE_VAR: return "TEMPLATE_VAR";
        case TokenType::HTML_TAG: return "HTML_TAG";
        case TokenType::CLASS_SELECTOR: return "CLASS_SELECTOR";
        case TokenType::ID_SELECTOR: return "ID_SELECTOR";
        case TokenType::PSEUDO_CLASS: return "PSEUDO_CLASS";
        case TokenType::PSEUDO_ELEMENT: return "PSEUDO_ELEMENT";
        case TokenType::CONFIG_KEY: return "CONFIG_KEY";
        case TokenType::CONFIG_VALUE: return "CONFIG_VALUE";
        case TokenType::CONFIG_OPTION_LIST: return "CONFIG_OPTION_LIST";
        case TokenType::INDEX: return "INDEX";
        case TokenType::CONTEXT_SWITCH: return "CONTEXT_SWITCH";
        case TokenType::STATE_CHANGE: return "STATE_CHANGE";
        default: return "UNKNOWN";
    }
}

bool TokenUtils::isLiteral(TokenType type) {
    return type == TokenType::STRING_DOUBLE_QUOTE ||
           type == TokenType::STRING_SINGLE_QUOTE ||
           type == TokenType::STRING_NO_QUOTE ||
           type == TokenType::NUMBER;
}

bool TokenUtils::isOperator(TokenType type) {
    return type == TokenType::ADD ||
           type == TokenType::DELETE ||
           type == TokenType::INHERIT ||
           type == TokenType::FROM ||
           type == TokenType::AS ||
           type == TokenType::EXCEPT ||
           type == TokenType::NOT;
}

bool TokenUtils::isKeyword(TokenType type) {
    return type >= TokenType::TEXT && type <= TokenType::NOT;
}

bool TokenUtils::isDelimiter(TokenType type) {
    return type == TokenType::SEMICOLON ||
           type == TokenType::COLON ||
           type == TokenType::EQUAL ||
           type == TokenType::LEFT_BRACE ||
           type == TokenType::RIGHT_BRACE ||
           type == TokenType::LEFT_BRACKET ||
           type == TokenType::RIGHT_BRACKET ||
           type == TokenType::LEFT_PAREN ||
           type == TokenType::RIGHT_PAREN ||
           type == TokenType::DOT ||
           type == TokenType::COMMA;
}

bool TokenUtils::needsContextInference(TokenType type) {
    return type == TokenType::IDENTIFIER ||
           type == TokenType::AT ||
           type == TokenType::DOT ||
           type == TokenType::HASH ||
           type == TokenType::AMPERSAND;
}

} // namespace chtl