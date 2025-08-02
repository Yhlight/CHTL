#include "Node.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace chtl {

// ElementNode实现
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
    return it != attributes_.end() ? it->second : "";
}

const std::unordered_map<std::string, std::string>& ElementNode::getAllAttributes() const {
    return attributes_;
}

void ElementNode::addClass(const std::string& className) {
    classes_.insert(className);
    updateClassAttribute();
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
    std::stringstream ss;
    for (auto it = classes_.begin(); it != classes_.end(); ++it) {
        if (it != classes_.begin()) ss << " ";
        ss << *it;
    }
    return ss.str();
}

void ElementNode::setId(const std::string& id) {
    attributes_["id"] = id;
}

std::string ElementNode::getId() const {
    return getAttributeValue("id");
}

void ElementNode::addInlineStyle(const std::string& property, const std::string& value) {
    inlineStyles_[property] = value;
    updateStyleAttribute();
}

void ElementNode::removeInlineStyle(const std::string& property) {
    inlineStyles_.erase(property);
    updateStyleAttribute();
}

std::string ElementNode::getInlineStyle(const std::string& property) const {
    auto it = inlineStyles_.find(property);
    return it != inlineStyles_.end() ? it->second : "";
}

std::string ElementNode::getInlineStyleString() const {
    std::stringstream ss;
    for (auto it = inlineStyles_.begin(); it != inlineStyles_.end(); ++it) {
        if (it != inlineStyles_.begin()) ss << " ";
        ss << it->first << ": " << it->second << ";";
    }
    return ss.str();
}

std::vector<std::shared_ptr<ElementNode>> ElementNode::getChildElements() const {
    std::vector<std::shared_ptr<ElementNode>> elements;
    for (const auto& child : getChildren()) {
        if (auto element = std::dynamic_pointer_cast<ElementNode>(child)) {
            elements.push_back(element);
        }
    }
    return elements;
}

std::shared_ptr<ElementNode> ElementNode::findChildByTagName(const std::string& tagName) const {
    for (const auto& child : getChildren()) {
        if (auto element = std::dynamic_pointer_cast<ElementNode>(child)) {
            if (element->getTagName() == tagName) {
                return element;
            }
        }
    }
    return nullptr;
}

std::shared_ptr<ElementNode> ElementNode::findChildById(const std::string& id) const {
    for (const auto& child : getChildren()) {
        if (auto element = std::dynamic_pointer_cast<ElementNode>(child)) {
            if (element->getId() == id) {
                return element;
            }
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<ElementNode>> ElementNode::findChildrenByClass(const std::string& className) const {
    std::vector<std::shared_ptr<ElementNode>> elements;
    for (const auto& child : getChildren()) {
        if (auto element = std::dynamic_pointer_cast<ElementNode>(child)) {
            if (element->hasClass(className)) {
                elements.push_back(element);
            }
        }
    }
    return elements;
}

bool ElementNode::isSelfClosing() const {
    static const std::unordered_set<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    return selfClosingTags.find(tagName_) != selfClosingTags.end();
}

bool ElementNode::isBlockElement() const {
    static const std::unordered_set<std::string> blockElements = {
        "div", "p", "h1", "h2", "h3", "h4", "h5", "h6", "ul", "ol", "li",
        "blockquote", "pre", "hr", "table", "form", "fieldset", "address",
        "article", "aside", "footer", "header", "main", "nav", "section"
    };
    return blockElements.find(tagName_) != blockElements.end();
}

bool ElementNode::isInlineElement() const {
    return !isBlockElement() && !isSelfClosing();
}

bool ElementNode::validate() const {
    return isValidTagName(tagName_) && Node::validate();
}

std::string ElementNode::toString() const {
    return "<" + tagName_ + ">";
}

std::string ElementNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    std::stringstream ss;
    ss << indentStr << "ElementNode: " << tagName_;
    if (!attributes_.empty()) {
        ss << " [";
        for (auto it = attributes_.begin(); it != attributes_.end(); ++it) {
            if (it != attributes_.begin()) ss << ", ";
            ss << it->first << "=\"" << it->second << "\"";
        }
        ss << "]";
    }
    return ss.str();
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
    // 访问者模式实现
}

void ElementNode::parseClassAttribute() {
    classes_.clear();
    std::string classStr = getAttributeValue("class");
    std::istringstream iss(classStr);
    std::string className;
    while (iss >> className) {
        classes_.insert(className);
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
    std::regex propRegex(R"(([^:;]+):([^:;]+)(?:;|$))");
    std::smatch match;
    
    auto searchStart = styleStr.cbegin();
    while (std::regex_search(searchStart, styleStr.cend(), match, propRegex)) {
        std::string property = match[1].str();
        std::string value = match[2].str();
        
        // 去除首尾空白
        property.erase(0, property.find_first_not_of(" \t"));
        property.erase(property.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        inlineStyles_[property] = value;
        searchStart = match.suffix().first;
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
    
    // HTML标签名应该以字母开头，后续可以是字母、数字、连字符
    if (!std::isalpha(tagName[0])) return false;
    
    for (size_t i = 1; i < tagName.length(); ++i) {
        char c = tagName[i];
        if (!std::isalnum(c) && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool ElementNode::isValidAttributeName(const std::string& name) const {
    if (name.empty()) return false;
    
    // 属性名不能包含空白字符、控制字符或特殊字符
    for (char c : name) {
        if (std::isspace(c) || std::iscntrl(c) || c == '\"' || c == '\'' || c == '>' || c == '/' || c == '=') {
            return false;
        }
    }
    
    return true;
}

// TextNode实现
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
    return processEscapeSequences(normalizeWhitespace(text_));
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
    return Node::validate();
}

std::string TextNode::toString() const {
    return text_;
}

std::string TextNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    return indentStr + "TextNode: \"" + text_ + "\"";
}

std::shared_ptr<Node> TextNode::clone() const {
    return std::make_shared<TextNode>(text_, getPosition());
}

void TextNode::accept(NodeVisitor& visitor) {
    // 访问者模式实现
}

std::string TextNode::processEscapeSequences(const std::string& text) const {
    std::string result = text;
    
    // 处理常见的转义序列
    std::size_t pos = 0;
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
    
    pos = 0;
    while ((pos = result.find("\\'", pos)) != std::string::npos) {
        result.replace(pos, 2, "'");
        pos += 1;
    }
    
    pos = 0;
    while ((pos = result.find("\\\\", pos)) != std::string::npos) {
        result.replace(pos, 2, "\\");
        pos += 1;
    }
    
    return result;
}

std::string TextNode::normalizeWhitespace(const std::string& text) const {
    // 基本的空白字符标准化，可以根据需要扩展
    return text;
}

// CommentNode实现
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
    return Node::validate();
}

std::string CommentNode::toString() const {
    return formatCommentForOutput();
}

std::string CommentNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    return indentStr + "CommentNode (" + commentTypeToString() + "): \"" + comment_ + "\"";
}

std::shared_ptr<Node> CommentNode::clone() const {
    return std::make_shared<CommentNode>(comment_, commentType_, getPosition());
}

void CommentNode::accept(NodeVisitor& visitor) {
    // 访问者模式实现
}

std::string CommentNode::commentTypeToString() const {
    switch (commentType_) {
        case CommentType::SINGLE_LINE: return "single_line";
        case CommentType::MULTI_LINE: return "multi_line";
        case CommentType::GENERATOR: return "generator";
        default: return "unknown";
    }
}

std::string CommentNode::formatCommentForOutput() const {
    switch (commentType_) {
        case CommentType::SINGLE_LINE:
            return "// " + comment_;
        case CommentType::MULTI_LINE:
            return "/* " + comment_ + " */";
        case CommentType::GENERATOR:
            return "<!-- " + comment_ + " -->";
        default:
            return comment_;
    }
}

// AttributeNode实现
const std::unordered_set<std::string> AttributeNode::booleanAttributes_ = {
    "autofocus", "checked", "default", "defer", "disabled", "hidden",
    "loop", "multiple", "muted", "readonly", "required", "reversed",
    "selected", "autoplay", "controls", "declare", "preload", "async"
};

const std::unordered_set<std::string> AttributeNode::urlAttributes_ = {
    "href", "src", "action", "data", "formaction", "poster", "cite"
};

const std::unordered_set<std::string> AttributeNode::eventAttributes_ = {
    "onclick", "onload", "onmouseover", "onmouseout", "onchange",
    "onsubmit", "onreset", "onfocus", "onblur", "onkeydown", "onkeyup"
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
    return eventAttributes_.find(attributeName_) != eventAttributes_.end();
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
    return isValidAttributeName(attributeName_) && isValidAttributeValue(attributeValue_) && Node::validate();
}

std::string AttributeNode::toString() const {
    if (isBooleanAttribute() && (attributeValue_.empty() || attributeValue_ == attributeName_)) {
        return attributeName_;
    }
    return attributeName_ + "=\"" + attributeValue_ + "\"";
}

std::string AttributeNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    return indentStr + "AttributeNode: " + attributeName_ + "=\"" + attributeValue_ + "\"";
}

std::shared_ptr<Node> AttributeNode::clone() const {
    return std::make_shared<AttributeNode>(attributeName_, attributeValue_, getPosition());
}

void AttributeNode::accept(NodeVisitor& visitor) {
    // 访问者模式实现
}

bool AttributeNode::isValidAttributeName(const std::string& name) const {
    if (name.empty()) return false;
    
    // 属性名不能包含空白字符、控制字符或特殊字符
    for (char c : name) {
        if (std::isspace(c) || std::iscntrl(c) || c == '\"' || c == '\'' || c == '>' || c == '/' || c == '=') {
            return false;
        }
    }
    
    return true;
}

bool AttributeNode::isValidAttributeValue(const std::string& value) const {
    // 基本验证，可以根据需要扩展
    return true;
}

std::string AttributeNode::normalizeAttributeValue(const std::string& value) const {
    // 基本标准化，可以根据需要扩展
    return value;
}

} // namespace chtl