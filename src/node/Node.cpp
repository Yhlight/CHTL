#include "Node.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <functional>
#include <regex>

namespace chtl {

// Node 实现
Node::Node(NodeType type, const NodePosition& position)
    : type_(type), position_(position) {
}

void Node::setParent(std::shared_ptr<Node> parent) {
    parent_ = parent;
}

std::shared_ptr<Node> Node::getParent() const {
    return parent_.lock();
}

void Node::addChild(std::shared_ptr<Node> child) {
    if (child) {
        child->setParent(shared_from_this());
        children_.push_back(child);
    }
}

void Node::removeChild(std::shared_ptr<Node> child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->setParent(nullptr);
        children_.erase(it);
    }
}

const std::vector<std::shared_ptr<Node>>& Node::getChildren() const {
    return children_;
}

std::shared_ptr<Node> Node::getChild(size_t index) const {
    if (index < children_.size()) {
        return children_[index];
    }
    return nullptr;
}

size_t Node::getChildCount() const {
    return children_.size();
}

std::shared_ptr<Node> Node::getNextSibling() const {
    auto parent = getParent();
    if (!parent) return nullptr;
    
    const auto& siblings = parent->getChildren();
    auto it = std::find(siblings.begin(), siblings.end(), shared_from_this());
    
    if (it != siblings.end() && ++it != siblings.end()) {
        return *it;
    }
    return nullptr;
}

std::shared_ptr<Node> Node::getPreviousSibling() const {
    auto parent = getParent();
    if (!parent) return nullptr;
    
    const auto& siblings = parent->getChildren();
    auto it = std::find(siblings.begin(), siblings.end(), shared_from_this());
    
    if (it != siblings.end() && it != siblings.begin()) {
        return *(--it);
    }
    return nullptr;
}

std::vector<std::shared_ptr<Node>> Node::findChildren(NodeType type) const {
    std::vector<std::shared_ptr<Node>> result;
    for (const auto& child : children_) {
        if (child->getType() == type) {
            result.push_back(child);
        }
    }
    return result;
}

std::shared_ptr<Node> Node::findFirstChild(NodeType type) const {
    for (const auto& child : children_) {
        if (child->getType() == type) {
            return child;
        }
    }
    return nullptr;
}

std::shared_ptr<Node> Node::findChildByName(const std::string& name) const {
    for (const auto& child : children_) {
        if (child->getName() == name) {
            return child;
        }
    }
    return nullptr;
}

void Node::setAttribute(const std::string& key, const std::string& value) {
    attributes_[key] = value;
}

std::string Node::getAttribute(const std::string& key) const {
    auto it = attributes_.find(key);
    return (it != attributes_.end()) ? it->second : "";
}

bool Node::hasAttribute(const std::string& key) const {
    return attributes_.find(key) != attributes_.end();
}

void Node::removeAttribute(const std::string& key) {
    attributes_.erase(key);
}

const std::unordered_map<std::string, std::string>& Node::getAttributes() const {
    return attributes_;
}

void Node::setContent(const std::string& content) {
    content_ = content;
}

const std::string& Node::getContent() const {
    return content_;
}

void Node::setName(const std::string& name) {
    name_ = name;
}

const std::string& Node::getName() const {
    return name_;
}

std::string Node::toString() const {
    std::ostringstream oss;
    oss << NodeUtils::nodeTypeToString(type_);
    if (!name_.empty()) {
        oss << "(" << name_ << ")";
    }
    if (!content_.empty()) {
        oss << ": \"" << content_ << "\"";
    }
    return oss.str();
}

std::string Node::toDebugString(int indent) const {
    std::ostringstream oss;
    std::string indentStr(indent * 2, ' ');
    
    oss << indentStr << toString();
    oss << " [" << position_.startLine << ":" << position_.startColumn << "]";
    
    if (!attributes_.empty()) {
        oss << " {";
        bool first = true;
        for (const auto& attr : attributes_) {
            if (!first) oss << ", ";
            oss << attr.first << "=\"" << attr.second << "\"";
            first = false;
        }
        oss << "}";
    }
    
    oss << "\n";
    
    for (const auto& child : children_) {
        oss << child->toDebugString(indent + 1);
    }
    
    return oss.str();
}

std::shared_ptr<Node> Node::clone() const {
    // 基础实现，子类应该重写
    auto cloned = std::make_shared<Node>(type_, position_);
    cloned->name_ = name_;
    cloned->content_ = content_;
    cloned->attributes_ = attributes_;
    
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

bool Node::validate() const {
    // 基础验证
    if (type_ == NodeType::UNKNOWN) {
        return false;
    }
    
    // 验证所有子节点
    for (const auto& child : children_) {
        if (!child || !child->validate()) {
            return false;
        }
    }
    
    return true;
}

bool Node::isElementNode() const {
    return type_ == NodeType::HTML_ELEMENT;
}

bool Node::isTextNode() const {
    return type_ == NodeType::TEXT_NODE;
}

bool Node::isStyleNode() const {
    return type_ == NodeType::STYLE_BLOCK || 
           type_ == NodeType::CSS_RULE || 
           type_ == NodeType::CSS_PROPERTY;
}

bool Node::isCustomNode() const {
    return type_ == NodeType::CUSTOM_STYLE ||
           type_ == NodeType::CUSTOM_ELEMENT ||
           type_ == NodeType::CUSTOM_VAR;
}

bool Node::isTemplateNode() const {
    return type_ == NodeType::TEMPLATE_STYLE ||
           type_ == NodeType::TEMPLATE_ELEMENT ||
           type_ == NodeType::TEMPLATE_VAR;
}

bool Node::isConfigNode() const {
    return type_ == NodeType::CONFIGURATION ||
           type_ == NodeType::CONFIG_OPTION ||
           type_ == NodeType::NAME_BLOCK;
}

bool Node::isOriginNode() const {
    return type_ == NodeType::ORIGIN_HTML ||
           type_ == NodeType::ORIGIN_CSS ||
           type_ == NodeType::ORIGIN_JAVASCRIPT;
}

void Node::insertChild(size_t index, std::shared_ptr<Node> child) {
    if (child && index <= children_.size()) {
        child->setParent(shared_from_this());
        children_.insert(children_.begin() + index, child);
    }
}

void Node::replaceChild(size_t index, std::shared_ptr<Node> newChild) {
    if (index < children_.size() && newChild) {
        children_[index]->setParent(nullptr);
        newChild->setParent(shared_from_this());
        children_[index] = newChild;
    }
}

void Node::accept(NodeVisitor& visitor) {
    visitor.visitNode(*this);
}

void Node::updateParentReferences() {
    for (const auto& child : children_) {
        child->setParent(shared_from_this());
    }
}

// NodeUtils 实现
std::string NodeUtils::nodeTypeToString(NodeType type) {
    switch (type) {
        case NodeType::UNKNOWN: return "UNKNOWN";
        case NodeType::ROOT: return "ROOT";
        case NodeType::DOCUMENT: return "DOCUMENT";
        case NodeType::HTML_ELEMENT: return "HTML_ELEMENT";
        case NodeType::TEXT_NODE: return "TEXT_NODE";
        case NodeType::COMMENT_NODE: return "COMMENT_NODE";
        case NodeType::ATTRIBUTE: return "ATTRIBUTE";
        case NodeType::STYLE_BLOCK: return "STYLE_BLOCK";
        case NodeType::CSS_RULE: return "CSS_RULE";
        case NodeType::CSS_PROPERTY: return "CSS_PROPERTY";
        case NodeType::CSS_SELECTOR: return "CSS_SELECTOR";
        case NodeType::CUSTOM_STYLE: return "CUSTOM_STYLE";
        case NodeType::CUSTOM_ELEMENT: return "CUSTOM_ELEMENT";
        case NodeType::CUSTOM_VAR: return "CUSTOM_VAR";
        case NodeType::TEMPLATE_STYLE: return "TEMPLATE_STYLE";
        case NodeType::TEMPLATE_ELEMENT: return "TEMPLATE_ELEMENT";
        case NodeType::TEMPLATE_VAR: return "TEMPLATE_VAR";
        case NodeType::ORIGIN_HTML: return "ORIGIN_HTML";
        case NodeType::ORIGIN_CSS: return "ORIGIN_CSS";
        case NodeType::ORIGIN_JAVASCRIPT: return "ORIGIN_JAVASCRIPT";
        case NodeType::CONFIGURATION: return "CONFIGURATION";
        case NodeType::CONFIG_OPTION: return "CONFIG_OPTION";
        case NodeType::NAME_BLOCK: return "NAME_BLOCK";
        case NodeType::IMPORT: return "IMPORT";
        case NodeType::NAMESPACE: return "NAMESPACE";
        case NodeType::INHERIT: return "INHERIT";
        case NodeType::SPECIALIZATION: return "SPECIALIZATION";
        case NodeType::OPERATION: return "OPERATION";
        case NodeType::INDEX_ACCESS: return "INDEX_ACCESS";
        case NodeType::VARIABLE_CALL: return "VARIABLE_CALL";
        case NodeType::VARIABLE_DEFINITION: return "VARIABLE_DEFINITION";

        case NodeType::PSEUDO_CLASS: return "PSEUDO_CLASS";
        case NodeType::PSEUDO_ELEMENT: return "PSEUDO_ELEMENT";
        case NodeType::EXPECT: return "EXPECT";
        case NodeType::NOT_EXPECT: return "NOT_EXPECT";
        default: return "UNKNOWN";
    }
}

NodeType NodeUtils::stringToNodeType(const std::string& str) {
    static const std::unordered_map<std::string, NodeType> typeMap = {
        {"UNKNOWN", NodeType::UNKNOWN},
        {"ROOT", NodeType::ROOT},
        {"DOCUMENT", NodeType::DOCUMENT},
        {"HTML_ELEMENT", NodeType::HTML_ELEMENT},
        {"TEXT_NODE", NodeType::TEXT_NODE},
        {"COMMENT_NODE", NodeType::COMMENT_NODE},
        {"ATTRIBUTE", NodeType::ATTRIBUTE},
        {"STYLE_BLOCK", NodeType::STYLE_BLOCK},
        {"CSS_RULE", NodeType::CSS_RULE},
        {"CSS_PROPERTY", NodeType::CSS_PROPERTY},
        {"CSS_SELECTOR", NodeType::CSS_SELECTOR},
        {"CUSTOM_STYLE", NodeType::CUSTOM_STYLE},
        {"CUSTOM_ELEMENT", NodeType::CUSTOM_ELEMENT},
        {"CUSTOM_VAR", NodeType::CUSTOM_VAR},
        {"TEMPLATE_STYLE", NodeType::TEMPLATE_STYLE},
        {"TEMPLATE_ELEMENT", NodeType::TEMPLATE_ELEMENT},
        {"TEMPLATE_VAR", NodeType::TEMPLATE_VAR},
        {"ORIGIN_HTML", NodeType::ORIGIN_HTML},
        {"ORIGIN_CSS", NodeType::ORIGIN_CSS},
        {"ORIGIN_JAVASCRIPT", NodeType::ORIGIN_JAVASCRIPT},
        {"CONFIGURATION", NodeType::CONFIGURATION},
        {"CONFIG_OPTION", NodeType::CONFIG_OPTION},
        {"NAME_BLOCK", NodeType::NAME_BLOCK},
        {"IMPORT", NodeType::IMPORT},
        {"NAMESPACE", NodeType::NAMESPACE},
        {"INHERIT", NodeType::INHERIT},
        {"SPECIALIZATION", NodeType::SPECIALIZATION},
        {"OPERATION", NodeType::OPERATION},
        {"INDEX_ACCESS", NodeType::INDEX_ACCESS},
        {"VARIABLE_CALL", NodeType::VARIABLE_CALL},
        {"VARIABLE_DEFINITION", NodeType::VARIABLE_DEFINITION},

        {"PSEUDO_CLASS", NodeType::PSEUDO_CLASS},
        {"PSEUDO_ELEMENT", NodeType::PSEUDO_ELEMENT},
        {"EXPECT", NodeType::EXPECT},
        {"NOT_EXPECT", NodeType::NOT_EXPECT}
    };
    
    auto it = typeMap.find(str);
    return (it != typeMap.end()) ? it->second : NodeType::UNKNOWN;
}

std::shared_ptr<Node> NodeUtils::findAncestor(std::shared_ptr<Node> node, NodeType type) {
    if (!node) return nullptr;
    
    auto parent = node->getParent();
    while (parent) {
        if (parent->getType() == type) {
            return parent;
        }
        parent = parent->getParent();
    }
    return nullptr;
}

std::vector<std::shared_ptr<Node>> NodeUtils::collectNodes(std::shared_ptr<Node> root, NodeType type) {
    std::vector<std::shared_ptr<Node>> result;
    if (!root) return result;
    
    if (root->getType() == type) {
        result.push_back(root);
    }
    
    for (const auto& child : root->getChildren()) {
        auto childNodes = collectNodes(child, type);
        result.insert(result.end(), childNodes.begin(), childNodes.end());
    }
    
    return result;
}

std::shared_ptr<Node> NodeUtils::findNodeByPath(std::shared_ptr<Node> root, const std::vector<std::string>& path) {
    if (!root || path.empty()) return root;
    
    auto current = root;
    for (const auto& segment : path) {
        current = current->findChildByName(segment);
        if (!current) return nullptr;
    }
    
    return current;
}

bool NodeUtils::validateTree(std::shared_ptr<Node> root) {
    if (!root) return false;
    return root->validate();
}

std::vector<std::string> NodeUtils::getValidationErrors(std::shared_ptr<Node> root) {
    std::vector<std::string> errors;
    
    if (!root) {
        errors.push_back("Root node is null");
        return errors;
    }
    
    // 递归验证所有节点
    std::function<void(std::shared_ptr<Node>)> validateNode = [&](std::shared_ptr<Node> node) {
        if (!node) {
            errors.push_back("Found null node in tree");
            return;
        }
        
        if (!node->validate()) {
            errors.push_back("Node validation failed: " + node->toString());
        }
        
        for (const auto& child : node->getChildren()) {
            validateNode(child);
        }
    };
    
    validateNode(root);
    return errors;
}

std::string NodeUtils::treeToString(std::shared_ptr<Node> root, bool includePosition) {
    if (!root) return "";
    
    std::ostringstream oss;
    std::function<void(std::shared_ptr<Node>, int)> printNode = [&](std::shared_ptr<Node> node, int depth) {
        if (!node) return;
        
        std::string indent(depth * 2, ' ');
        oss << indent << node->toString();
        
        if (includePosition) {
            oss << " [" << node->getPosition().startLine << ":" << node->getPosition().startColumn << "]";
        }
        
        oss << "\n";
        
        for (const auto& child : node->getChildren()) {
            printNode(child, depth + 1);
        }
    };
    
    printNode(root, 0);
    return oss.str();
}

size_t NodeUtils::countNodes(std::shared_ptr<Node> root) {
    if (!root) return 0;
    
    size_t count = 1;
    for (const auto& child : root->getChildren()) {
        count += countNodes(child);
    }
    return count;
}

size_t NodeUtils::countNodesByType(std::shared_ptr<Node> root, NodeType type) {
    if (!root) return 0;
    
    size_t count = (root->getType() == type) ? 1 : 0;
    for (const auto& child : root->getChildren()) {
        count += countNodesByType(child, type);
    }
    return count;
}

std::unordered_map<NodeType, size_t> NodeUtils::getNodeTypeStatistics(std::shared_ptr<Node> root) {
    std::unordered_map<NodeType, size_t> stats;
    
    if (!root) return stats;
    
    std::function<void(std::shared_ptr<Node>)> countNode = [&](std::shared_ptr<Node> node) {
        if (!node) return;
        
        stats[node->getType()]++;
        
        for (const auto& child : node->getChildren()) {
            countNode(child);
        }
    };
    
    countNode(root);
    return stats;
}

std::shared_ptr<Node> NodeUtils::deepCopy(std::shared_ptr<Node> node) {
    if (!node) return nullptr;
    return node->clone();
}

void NodeUtils::moveNode(std::shared_ptr<Node> node, std::shared_ptr<Node> newParent) {
    if (!node || !newParent) return;
    
    auto oldParent = node->getParent();
    if (oldParent) {
        oldParent->removeChild(node);
    }
    
    newParent->addChild(node);
}

void NodeUtils::moveNodeBefore(std::shared_ptr<Node> node, std::shared_ptr<Node> target) {
    if (!node || !target) return;
    
    auto targetParent = target->getParent();
    if (!targetParent) return;
    
    auto oldParent = node->getParent();
    if (oldParent) {
        oldParent->removeChild(node);
    }
    
    const auto& siblings = targetParent->getChildren();
    auto it = std::find(siblings.begin(), siblings.end(), target);
    if (it != siblings.end()) {
        size_t index = std::distance(siblings.begin(), it);
        targetParent->insertChild(index, node);
    }
}

void NodeUtils::moveNodeAfter(std::shared_ptr<Node> node, std::shared_ptr<Node> target) {
    if (!node || !target) return;
    
    auto targetParent = target->getParent();
    if (!targetParent) return;
    
    auto oldParent = node->getParent();
    if (oldParent) {
        oldParent->removeChild(node);
    }
    
    const auto& siblings = targetParent->getChildren();
    auto it = std::find(siblings.begin(), siblings.end(), target);
    if (it != siblings.end()) {
        size_t index = std::distance(siblings.begin(), it) + 1;
        targetParent->insertChild(index, node);
    }
}

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