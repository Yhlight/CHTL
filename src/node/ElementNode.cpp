#include "ElementNode.h"
#include "../common/Token.h"
#include <sstream>
#include <algorithm>

namespace chtl {

// ElementNode 实现
ElementNode::ElementNode(const std::string& tagName, const NodePosition& position)
    : Node(NodeType::HTML_ELEMENT, position), tagName_(tagName) {
    setName(tagName);
}

const std::string& ElementNode::getTagName() const {
    return tagName_;
}

void ElementNode::setTagName(const std::string& tagName) {
    if (isValidTagName(tagName)) {
        tagName_ = tagName;
        setName(tagName);
    }
}

void ElementNode::addAttribute(const std::string& name, const std::string& value) {
    if (isValidAttributeName(name)) {
        attributes_[name] = value;
        
        // 特殊处理class和style属性
        if (name == "class") {
            parseClassAttribute();
        } else if (name == "style") {
            parseStyleAttribute();
        }
    }
}

void ElementNode::removeAttribute(const std::string& name) {
    attributes_.erase(name);
    
    if (name == "class") {
        classes_.clear();
    } else if (name == "style") {
        inlineStyles_.clear();
    }
}

bool ElementNode::hasAttribute(const std::string& name) const {
    return attributes_.find(name) != attributes_.end();
}

std::string ElementNode::getAttributeValue(const std::string& name) const {
    auto it = attributes_.find(name);
    return (it != attributes_.end()) ? it->second : "";
}

const std::unordered_map<std::string, std::string>& ElementNode::getAllAttributes() const {
    return attributes_;
}

void ElementNode::addClass(const std::string& className) {
    if (!className.empty()) {
        classes_.insert(className);
        updateClassAttribute();
    }
}

void ElementNode::removeClass(const std::string& className) {
    classes_.erase(className);
    updateClassAttribute();
}

bool ElementNode::hasClass(const std::string& className) const {
    return classes_.find(className) != classes_.end();
}

std::vector<std::string> ElementNode::getClasses() const {
    return std::vector<std::string>(classes_.begin(), classes_.end());
}

std::string ElementNode::getClassString() const {
    std::ostringstream oss;
    bool first = true;
    for (const auto& cls : classes_) {
        if (!first) oss << " ";
        oss << cls;
        first = false;
    }
    return oss.str();
}

void ElementNode::setId(const std::string& id) {
    if (!id.empty()) {
        attributes_["id"] = id;
    } else {
        attributes_.erase("id");
    }
}

std::string ElementNode::getId() const {
    return getAttributeValue("id");
}

void ElementNode::addInlineStyle(const std::string& property, const std::string& value) {
    if (!property.empty() && !value.empty()) {
        inlineStyles_[property] = value;
        updateStyleAttribute();
    }
}

void ElementNode::removeInlineStyle(const std::string& property) {
    inlineStyles_.erase(property);
    updateStyleAttribute();
}

std::string ElementNode::getInlineStyle(const std::string& property) const {
    auto it = inlineStyles_.find(property);
    return (it != inlineStyles_.end()) ? it->second : "";
}

std::string ElementNode::getInlineStyleString() const {
    std::ostringstream oss;
    bool first = true;
    for (const auto& style : inlineStyles_) {
        if (!first) oss << "; ";
        oss << style.first << ": " << style.second;
        first = false;
    }
    return oss.str();
}

std::vector<std::shared_ptr<ElementNode>> ElementNode::getChildElements() const {
    std::vector<std::shared_ptr<ElementNode>> elements;
    for (const auto& child : getChildren()) {
        auto elementChild = std::dynamic_pointer_cast<ElementNode>(child);
        if (elementChild) {
            elements.push_back(elementChild);
        }
    }
    return elements;
}

std::shared_ptr<ElementNode> ElementNode::findChildByTagName(const std::string& tagName) const {
    for (const auto& child : getChildren()) {
        auto elementChild = std::dynamic_pointer_cast<ElementNode>(child);
        if (elementChild && elementChild->getTagName() == tagName) {
            return elementChild;
        }
    }
    return nullptr;
}

std::shared_ptr<ElementNode> ElementNode::findChildById(const std::string& id) const {
    for (const auto& child : getChildren()) {
        auto elementChild = std::dynamic_pointer_cast<ElementNode>(child);
        if (elementChild && elementChild->getId() == id) {
            return elementChild;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<ElementNode>> ElementNode::findChildrenByClass(const std::string& className) const {
    std::vector<std::shared_ptr<ElementNode>> result;
    for (const auto& child : getChildren()) {
        auto elementChild = std::dynamic_pointer_cast<ElementNode>(child);
        if (elementChild && elementChild->hasClass(className)) {
            result.push_back(elementChild);
        }
    }
    return result;
}

bool ElementNode::isSelfClosing() const {
    return HtmlTagRegistry::getInstance().isSelfClosingTag(tagName_);
}

bool ElementNode::isBlockElement() const {
    // 简化的块级元素检查
    static const std::unordered_set<std::string> blockElements = {
        "div", "p", "h1", "h2", "h3", "h4", "h5", "h6", "section", "article",
        "header", "footer", "nav", "aside", "main", "blockquote", "pre",
        "ul", "ol", "li", "dl", "dt", "dd", "table", "tr", "td", "th",
        "form", "fieldset", "address"
    };
    return blockElements.find(tagName_) != blockElements.end();
}

bool ElementNode::isInlineElement() const {
    return !isBlockElement() && !isSelfClosing();
}

bool ElementNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    if (!isValidTagName(tagName_)) {
        return false;
    }
    
    // 验证所有属性
    for (const auto& attr : attributes_) {
        if (!isValidAttributeName(attr.first)) {
            return false;
        }
    }
    
    return true;
}

std::string ElementNode::toString() const {
    std::ostringstream oss;
    oss << "ElementNode(" << tagName_ << ")";
    if (!attributes_.empty()) {
        oss << " [";
        bool first = true;
        for (const auto& attr : attributes_) {
            if (!first) oss << ", ";
            oss << attr.first << "=\"" << attr.second << "\"";
            first = false;
        }
        oss << "]";
    }
    return oss.str();
}

std::string ElementNode::toDebugString(int indent) const {
    std::ostringstream oss;
    std::string indentStr(indent * 2, ' ');
    
    oss << indentStr << toString() << "\n";
    
    for (const auto& child : getChildren()) {
        oss << child->toDebugString(indent + 1);
    }
    
    return oss.str();
}

std::shared_ptr<Node> ElementNode::clone() const {
    auto cloned = std::make_shared<ElementNode>(tagName_, getPosition());
    cloned->attributes_ = attributes_;
    cloned->classes_ = classes_;
    cloned->inlineStyles_ = inlineStyles_;
    
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

void ElementNode::accept(NodeVisitor& visitor) {
    visitor.visitElementNode(*this);
}

void ElementNode::parseClassAttribute() {
    classes_.clear();
    std::string classStr = getAttributeValue("class");
    if (!classStr.empty()) {
        std::istringstream iss(classStr);
        std::string className;
        while (iss >> className) {
            classes_.insert(className);
        }
    }
}

void ElementNode::updateClassAttribute() {
    if (classes_.empty()) {
        attributes_.erase("class");
    } else {
        attributes_["class"] = getClassString();
    }
}

void ElementNode::parseStyleAttribute() {
    inlineStyles_.clear();
    std::string styleStr = getAttributeValue("style");
    if (!styleStr.empty()) {
        // 简化的CSS解析
        size_t pos = 0;
        while (pos < styleStr.length()) {
            size_t colonPos = styleStr.find(':', pos);
            if (colonPos == std::string::npos) break;
            
            size_t semicolonPos = styleStr.find(';', colonPos);
            if (semicolonPos == std::string::npos) {
                semicolonPos = styleStr.length();
            }
            
            std::string property = styleStr.substr(pos, colonPos - pos);
            std::string value = styleStr.substr(colonPos + 1, semicolonPos - colonPos - 1);
            
            // 去除空白
            property.erase(0, property.find_first_not_of(" \t"));
            property.erase(property.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            if (!property.empty() && !value.empty()) {
                inlineStyles_[property] = value;
            }
            
            pos = semicolonPos + 1;
        }
    }
}

void ElementNode::updateStyleAttribute() {
    if (inlineStyles_.empty()) {
        attributes_.erase("style");
    } else {
        attributes_["style"] = getInlineStyleString();
    }
}

bool ElementNode::isValidTagName(const std::string& tagName) const {
    if (tagName.empty()) return false;
    
    // 简化的标签名验证
    for (char c : tagName) {
        if (!std::isalnum(c) && c != '-' && c != '_') {
            return false;
        }
    }
    
    return std::isalpha(tagName[0]);
}

bool ElementNode::isValidAttributeName(const std::string& name) const {
    if (name.empty()) return false;
    
    // 简化的属性名验证
    for (char c : name) {
        if (!std::isalnum(c) && c != '-' && c != '_' && c != ':') {
            return false;
        }
    }
    
    return true;
}

// TextNode 实现
TextNode::TextNode(const std::string& content, const NodePosition& position)
    : Node(NodeType::TEXT_NODE, position), text_(content) {
    setContent(content);
}

const std::string& TextNode::getText() const {
    return text_;
}

void TextNode::setText(const std::string& text) {
    text_ = text;
    setContent(text);
}

std::string TextNode::getProcessedText() const {
    return processEscapeSequences(text_);
}

void TextNode::appendText(const std::string& text) {
    text_ += text;
    setContent(text_);
}

void TextNode::prependText(const std::string& text) {
    text_ = text + text_;
    setContent(text_);
}

bool TextNode::validate() const {
    return Node::validate(); // 文本节点总是有效的
}

std::string TextNode::toString() const {
    return "TextNode: \"" + text_ + "\"";
}

std::string TextNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    return indentStr + toString() + "\n";
}

std::shared_ptr<Node> TextNode::clone() const {
    return std::make_shared<TextNode>(text_, getPosition());
}

void TextNode::accept(NodeVisitor& visitor) {
    visitor.visitTextNode(*this);
}

std::string TextNode::processEscapeSequences(const std::string& text) const {
    // 简化的转义序列处理
    std::string result = text;
    size_t pos = 0;
    while ((pos = result.find("\\n", pos)) != std::string::npos) {
        result.replace(pos, 2, "\n");
        pos += 1;
    }
    
    pos = 0;
    while ((pos = result.find("\\t", pos)) != std::string::npos) {
        result.replace(pos, 2, "\t");
        pos += 1;
    }
    
    pos = 0;
    while ((pos = result.find("\\\"", pos)) != std::string::npos) {
        result.replace(pos, 2, "\"");
        pos += 1;
    }
    
    return result;
}

std::string TextNode::normalizeWhitespace(const std::string& text) const {
    // 简化的空白规范化
    std::string result;
    bool lastWasSpace = false;
    
    for (char c : text) {
        if (std::isspace(c)) {
            if (!lastWasSpace) {
                result += ' ';
                lastWasSpace = true;
            }
        } else {
            result += c;
            lastWasSpace = false;
        }
    }
    
    return result;
}

// CommentNode 实现
CommentNode::CommentNode(const std::string& content, CommentType type, const NodePosition& position)
    : Node(NodeType::COMMENT_NODE, position), comment_(content), commentType_(type) {
    setContent(content);
}

const std::string& CommentNode::getComment() const {
    return comment_;
}

void CommentNode::setComment(const std::string& comment) {
    comment_ = comment;
    setContent(comment);
}

CommentNode::CommentType CommentNode::getCommentType() const {
    return commentType_;
}

void CommentNode::setCommentType(CommentType type) {
    commentType_ = type;
}

bool CommentNode::isGeneratorComment() const {
    return commentType_ == CommentType::GENERATOR;
}

bool CommentNode::shouldOutputToHtml() const {
    return commentType_ == CommentType::GENERATOR;
}

bool CommentNode::validate() const {
    return Node::validate(); // 注释节点总是有效的
}

std::string CommentNode::toString() const {
    return "CommentNode(" + commentTypeToString() + "): \"" + comment_ + "\"";
}

std::string CommentNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    return indentStr + toString() + "\n";
}

std::shared_ptr<Node> CommentNode::clone() const {
    return std::make_shared<CommentNode>(comment_, commentType_, getPosition());
}

void CommentNode::accept(NodeVisitor& visitor) {
    visitor.visitCommentNode(*this);
}

std::string CommentNode::commentTypeToString() const {
    switch (commentType_) {
        case CommentType::SINGLE_LINE: return "SINGLE_LINE";
        case CommentType::MULTI_LINE: return "MULTI_LINE";
        case CommentType::GENERATOR: return "GENERATOR";
        default: return "UNKNOWN";
    }
}

std::string CommentNode::formatCommentForOutput() const {
    if (commentType_ == CommentType::GENERATOR) {
        return "<!-- " + comment_ + " -->";
    }
    return "";
}

// AttributeNode 实现
const std::unordered_set<std::string> AttributeNode::booleanAttributes_ = {
    "checked", "selected", "disabled", "readonly", "multiple", "autofocus",
    "autoplay", "controls", "defer", "hidden", "loop", "open", "required",
    "reversed", "scoped"
};

const std::unordered_set<std::string> AttributeNode::urlAttributes_ = {
    "href", "src", "action", "formaction", "cite", "data", "poster"
};

const std::unordered_set<std::string> AttributeNode::eventAttributes_ = {
    "onclick", "onload", "onchange", "onsubmit", "onmouseover", "onmouseout",
    "onfocus", "onblur", "onkeydown", "onkeyup", "onkeypress"
};

AttributeNode::AttributeNode(const std::string& name, const std::string& value, const NodePosition& position)
    : Node(NodeType::ATTRIBUTE, position), attributeName_(name), attributeValue_(value) {
    setName(name);
    setContent(value);
}

const std::string& AttributeNode::getAttributeName() const {
    return attributeName_;
}

void AttributeNode::setAttributeName(const std::string& name) {
    if (isValidAttributeName(name)) {
        attributeName_ = name;
        setName(name);
    }
}

const std::string& AttributeNode::getAttributeValue() const {
    return attributeValue_;
}

void AttributeNode::setAttributeValue(const std::string& value) {
    attributeValue_ = normalizeAttributeValue(value);
    setContent(attributeValue_);
}

bool AttributeNode::isBooleanAttribute() const {
    return booleanAttributes_.find(attributeName_) != booleanAttributes_.end();
}

bool AttributeNode::isUrlAttribute() const {
    return urlAttributes_.find(attributeName_) != urlAttributes_.end();
}

bool AttributeNode::isEventAttribute() const {
    return eventAttributes_.find(attributeName_) != eventAttributes_.end() ||
           attributeName_.substr(0, 2) == "on";
}

bool AttributeNode::isDataAttribute() const {
    return attributeName_.substr(0, 5) == "data-";
}

bool AttributeNode::isAriaAttribute() const {
    return attributeName_.substr(0, 5) == "aria-";
}

bool AttributeNode::hasValue() const {
    return !attributeValue_.empty();
}

bool AttributeNode::isValidValue() const {
    return isValidAttributeValue(attributeValue_);
}

bool AttributeNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    return isValidAttributeName(attributeName_) && isValidAttributeValue(attributeValue_);
}

std::string AttributeNode::toString() const {
    if (isBooleanAttribute() && !hasValue()) {
        return "AttributeNode(" + attributeName_ + ")";
    }
    return "AttributeNode(" + attributeName_ + "=\"" + attributeValue_ + "\")";
}

std::string AttributeNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    return indentStr + toString() + "\n";
}

std::shared_ptr<Node> AttributeNode::clone() const {
    return std::make_shared<AttributeNode>(attributeName_, attributeValue_, getPosition());
}

void AttributeNode::accept(NodeVisitor& visitor) {
    visitor.visitAttributeNode(*this);
}

bool AttributeNode::isValidAttributeName(const std::string& name) const {
    if (name.empty()) return false;
    
    // HTML属性名验证
    for (char c : name) {
        if (!std::isalnum(c) && c != '-' && c != '_' && c != ':') {
            return false;
        }
    }
    
    return std::isalpha(name[0]) || name[0] == '_';
}

bool AttributeNode::isValidAttributeValue(const std::string& value) const {
    // 简化的属性值验证
    return true; // 大多数字符串都是有效的属性值
}

std::string AttributeNode::normalizeAttributeValue(const std::string& value) const {
    // 简化的属性值规范化
    std::string result = value;
    
    // 去除首尾空白
    result.erase(0, result.find_first_not_of(" \t\r\n"));
    result.erase(result.find_last_not_of(" \t\r\n") + 1);
    
    return result;
}

} // namespace chtl