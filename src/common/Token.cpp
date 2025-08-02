#include "Token.h"

namespace chtl {

// Token类型到字符串的映射
const std::map<TokenType, std::string> tokenTypeToString = {
    {TokenType::END_OF_FILE, "END_OF_FILE"},
    {TokenType::UNKNOWN, "UNKNOWN"},
    
    // 字面量
    {TokenType::IDENTIFIER, "IDENTIFIER"},
    {TokenType::STRING_LITERAL, "STRING_LITERAL"},
    {TokenType::STRING_LITERAL_SINGLE, "STRING_LITERAL_SINGLE"},
    {TokenType::UNQUOTED_STRING, "UNQUOTED_STRING"},
    {TokenType::NUMBER, "NUMBER"},
    
    // 注释
    {TokenType::COMMENT_SINGLE, "COMMENT_SINGLE"},
    {TokenType::COMMENT_MULTI, "COMMENT_MULTI"},
    {TokenType::COMMENT_GENERATOR, "COMMENT_GENERATOR"},
    
    // 符号
    {TokenType::LEFT_BRACE, "LEFT_BRACE"},
    {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
    {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
    {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
    {TokenType::LEFT_PAREN, "LEFT_PAREN"},
    {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
    {TokenType::SEMICOLON, "SEMICOLON"},
    {TokenType::COLON, "COLON"},
    {TokenType::EQUALS, "EQUALS"},
    {TokenType::DOT, "DOT"},
    {TokenType::AT, "AT"},
    {TokenType::AMPERSAND, "AMPERSAND"},
    {TokenType::COMMA, "COMMA"},
    {TokenType::SLASH, "SLASH"},
    {TokenType::ASTERISK, "ASTERISK"},
    {TokenType::STRING, "STRING"},
    {TokenType::COMMENT, "COMMENT"},
    
    // 基础关键字
    {TokenType::KEYWORD_TEXT, "KEYWORD_TEXT"},
    {TokenType::KEYWORD_STYLE, "KEYWORD_STYLE"},
    {TokenType::KEYWORD_EXPECT, "KEYWORD_EXPECT"},
    {TokenType::KEYWORD_NOT, "KEYWORD_NOT"},
    
    // 操作符关键字
    {TokenType::KEYWORD_ADD, "KEYWORD_ADD"},
    {TokenType::KEYWORD_DELETE, "KEYWORD_DELETE"},
    {TokenType::KEYWORD_FROM, "KEYWORD_FROM"},
    {TokenType::KEYWORD_AS, "KEYWORD_AS"},
    {TokenType::KEYWORD_INHERIT, "KEYWORD_INHERIT"},
    
    // 配置关键字
    {TokenType::KEYWORD_CUSTOM, "KEYWORD_CUSTOM"},
    {TokenType::KEYWORD_TEMPLATE, "KEYWORD_TEMPLATE"},
    {TokenType::KEYWORD_ORIGIN, "KEYWORD_ORIGIN"},
    {TokenType::KEYWORD_IMPORT, "KEYWORD_IMPORT"},
    {TokenType::KEYWORD_NAMESPACE, "KEYWORD_NAMESPACE"},
    {TokenType::KEYWORD_CONFIGURATION, "KEYWORD_CONFIGURATION"},
    
    // 配置驱动的关键字
    {TokenType::CONFIG_KEYWORD, "CONFIG_KEYWORD"},
    
    // @前缀关键字
    {TokenType::AT_STYLE, "AT_STYLE"},
    {TokenType::AT_ELEMENT, "AT_ELEMENT"},
    {TokenType::AT_VAR, "AT_VAR"},
    {TokenType::AT_HTML, "AT_HTML"},
    {TokenType::AT_JAVASCRIPT, "AT_JAVASCRIPT"},
    {TokenType::AT_CHTL, "AT_CHTL"},
    
    // 特殊标记
    {TokenType::STYLE_SELECTOR_CLASS, "STYLE_SELECTOR_CLASS"},
    {TokenType::STYLE_SELECTOR_ID, "STYLE_SELECTOR_ID"},
    {TokenType::STYLE_SELECTOR_PSEUDO, "STYLE_SELECTOR_PSEUDO"},
    {TokenType::STYLE_SELECTOR_PSEUDO_ELEMENT, "STYLE_SELECTOR_PSEUDO_ELEMENT"},
    
    // 索引访问
    {TokenType::INDEX_ACCESS, "INDEX_ACCESS"},
    
    // 布尔值
    {TokenType::KEYWORD_TRUE, "KEYWORD_TRUE"},
    {TokenType::KEYWORD_FALSE, "KEYWORD_FALSE"},
    
    // 其他Token
    {TokenType::WHITESPACE, "WHITESPACE"},
    {TokenType::NEWLINE, "NEWLINE"},
};

// 基础关键字到Token类型的映射
const std::map<std::string, TokenType> keywordToTokenType = {
    // 基础关键字
    {"text", TokenType::KEYWORD_TEXT},
    {"style", TokenType::KEYWORD_STYLE},
    {"expect", TokenType::KEYWORD_EXPECT},
    {"not", TokenType::KEYWORD_NOT},
    
    // 操作符关键字
    {"add", TokenType::KEYWORD_ADD},
    {"delete", TokenType::KEYWORD_DELETE},
    {"from", TokenType::KEYWORD_FROM},
    {"as", TokenType::KEYWORD_AS},
    {"inherit", TokenType::KEYWORD_INHERIT},
    {"true", TokenType::KEYWORD_TRUE},
    {"false", TokenType::KEYWORD_FALSE},
    
    // 配置关键字（方括号组合）
    {"[Custom]", TokenType::KEYWORD_CUSTOM},
    {"[Template]", TokenType::KEYWORD_TEMPLATE},
    {"[Origin]", TokenType::KEYWORD_ORIGIN},
    {"[Import]", TokenType::KEYWORD_IMPORT},
    {"[Namespace]", TokenType::KEYWORD_NAMESPACE},
    {"[Configuration]", TokenType::KEYWORD_CONFIGURATION},
    
    // @前缀关键字
    {"@Style", TokenType::AT_STYLE},
    {"@Element", TokenType::AT_ELEMENT},
    {"@Var", TokenType::AT_VAR},
    {"@Html", TokenType::AT_HTML},
    {"@JavaScript", TokenType::AT_JAVASCRIPT},
    {"@Chtl", TokenType::AT_CHTL},
};

bool isKeyword(const std::string& str) {
    return keywordToTokenType.find(str) != keywordToTokenType.end();
}

TokenType getKeywordType(const std::string& str) {
    auto it = keywordToTokenType.find(str);
    if (it != keywordToTokenType.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

std::string tokenTypeToStr(TokenType type) {
    auto it = tokenTypeToString.find(type);
    if (it != tokenTypeToString.end()) {
        return it->second;
    }
    return "UNKNOWN";
}

} // namespace chtl