#include "Node.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <functional>

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
        case NodeType::STYLE_GROUP: return "STYLE_GROUP";
        case NodeType::STYLE_GROUP_CALL: return "STYLE_GROUP_CALL";
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
        {"STYLE_GROUP", NodeType::STYLE_GROUP},
        {"STYLE_GROUP_CALL", NodeType::STYLE_GROUP_CALL},
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

} // namespace chtl