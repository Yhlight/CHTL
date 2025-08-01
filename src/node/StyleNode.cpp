#include "StyleNode.h"
#include <sstream>
#include <algorithm>

namespace chtl {

// StyleBlockNode 实现
StyleBlockNode::StyleBlockNode(const NodePosition& position)
    : Node(NodeType::STYLE_BLOCK, position) {
    setName("style");
}

void StyleBlockNode::addRule(std::shared_ptr<CssRuleNode> rule) {
    if (rule) {
        addChild(rule);
    }
}

void StyleBlockNode::removeRule(std::shared_ptr<CssRuleNode> rule) {
    if (rule) {
        removeChild(rule);
    }
}

std::vector<std::shared_ptr<CssRuleNode>> StyleBlockNode::getRules() const {
    std::vector<std::shared_ptr<CssRuleNode>> rules;
    for (const auto& child : getChildren()) {
        auto rule = std::dynamic_pointer_cast<CssRuleNode>(child);
        if (rule) {
            rules.push_back(rule);
        }
    }
    return rules;
}

void StyleBlockNode::addInlineProperty(const std::string& property, const std::string& value) {
    if (!property.empty() && !value.empty()) {
        inlineProperties_[property] = value;
    }
}

void StyleBlockNode::removeInlineProperty(const std::string& property) {
    inlineProperties_.erase(property);
}

std::unordered_map<std::string, std::string> StyleBlockNode::getInlineProperties() const {
    return inlineProperties_;
}

void StyleBlockNode::addStyleGroupCall(std::shared_ptr<StyleGroupCallNode> call) {
    if (call) {
        addChild(call);
    }
}

std::vector<std::shared_ptr<StyleGroupCallNode>> StyleBlockNode::getStyleGroupCalls() const {
    std::vector<std::shared_ptr<StyleGroupCallNode>> calls;
    // 暂时返回空列表，等待StyleGroupCallNode实现
    return calls;
}

bool StyleBlockNode::hasInlineStyles() const {
    return !inlineProperties_.empty();
}

bool StyleBlockNode::hasCssRules() const {
    return !getRules().empty();
}

bool StyleBlockNode::hasStyleGroupCalls() const {
    return false; // 暂时返回false，等待StyleGroupCallNode实现
}

bool StyleBlockNode::validate() const {
    return Node::validate();
}

std::string StyleBlockNode::toString() const {
    std::ostringstream oss;
    oss << "StyleBlockNode";
    if (hasInlineStyles()) {
        oss << " [inline: " << inlineProperties_.size() << " properties]";
    }
    if (hasCssRules()) {
        oss << " [rules: " << getRules().size() << "]";
    }
    return oss.str();
}

std::string StyleBlockNode::toDebugString(int indent) const {
    std::ostringstream oss;
    std::string indentStr(indent * 2, ' ');
    
    oss << indentStr << toString() << "\n";
    
    // 输出内联属性
    if (hasInlineStyles()) {
        for (const auto& prop : inlineProperties_) {
            oss << indentStr << "  " << prop.first << ": " << prop.second << "\n";
        }
    }
    
    // 输出子节点
    for (const auto& child : getChildren()) {
        oss << child->toDebugString(indent + 1);
    }
    
    return oss.str();
}

std::shared_ptr<Node> StyleBlockNode::clone() const {
    auto cloned = std::make_shared<StyleBlockNode>(getPosition());
    cloned->inlineProperties_ = inlineProperties_;
    
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

void StyleBlockNode::accept(NodeVisitor& visitor) {
    visitor.visitStyleBlockNode(*this);
}

// CssRuleNode 实现
CssRuleNode::CssRuleNode(const NodePosition& position)
    : Node(NodeType::CSS_RULE, position) {
    setName("css-rule");
}

void CssRuleNode::addSelector(std::shared_ptr<CssSelectorNode> selector) {
    if (selector) {
        addChild(selector);
    }
}

void CssRuleNode::removeSelector(std::shared_ptr<CssSelectorNode> selector) {
    if (selector) {
        removeChild(selector);
    }
}

std::vector<std::shared_ptr<CssSelectorNode>> CssRuleNode::getSelectors() const {
    std::vector<std::shared_ptr<CssSelectorNode>> selectors;
    for (const auto& child : getChildren()) {
        auto selector = std::dynamic_pointer_cast<CssSelectorNode>(child);
        if (selector) {
            selectors.push_back(selector);
        }
    }
    return selectors;
}

std::string CssRuleNode::getSelectorString() const {
    std::ostringstream oss;
    auto selectors = getSelectors();
    bool first = true;
    for (const auto& selector : selectors) {
        if (!first) oss << ", ";
        oss << selector->toString();
        first = false;
    }
    return oss.str();
}

void CssRuleNode::addProperty(std::shared_ptr<CssPropertyNode> property) {
    if (property) {
        addChild(property);
    }
}

void CssRuleNode::removeProperty(std::shared_ptr<CssPropertyNode> property) {
    if (property) {
        removeChild(property);
    }
}

std::vector<std::shared_ptr<CssPropertyNode>> CssRuleNode::getProperties() const {
    std::vector<std::shared_ptr<CssPropertyNode>> properties;
    for (const auto& child : getChildren()) {
        auto property = std::dynamic_pointer_cast<CssPropertyNode>(child);
        if (property) {
            properties.push_back(property);
        }
    }
    return properties;
}

void CssRuleNode::addProperty(const std::string& name, const std::string& value) {
    auto property = std::make_shared<CssPropertyNode>(name, value, getPosition());
    addProperty(property);
}

void CssRuleNode::removeProperty(const std::string& name) {
    auto properties = getProperties();
    for (auto it = properties.begin(); it != properties.end(); ++it) {
        if ((*it)->getProperty() == name) {
            removeProperty(*it);
            break;
        }
    }
}

std::string CssRuleNode::getPropertyValue(const std::string& name) const {
    auto properties = getProperties();
    for (const auto& property : properties) {
        if (property->getProperty() == name) {
            return property->getValue();
        }
    }
    return "";
}

void CssRuleNode::addNestedRule(std::shared_ptr<CssRuleNode> rule) {
    if (rule) {
        addChild(rule);
    }
}

std::vector<std::shared_ptr<CssRuleNode>> CssRuleNode::getNestedRules() const {
    std::vector<std::shared_ptr<CssRuleNode>> rules;
    for (const auto& child : getChildren()) {
        auto rule = std::dynamic_pointer_cast<CssRuleNode>(child);
        if (rule) {
            rules.push_back(rule);
        }
    }
    return rules;
}

bool CssRuleNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    // 至少需要一个选择器
    if (getSelectors().empty()) {
        return false;
    }
    
    return true;
}

std::string CssRuleNode::toString() const {
    return "CssRuleNode(" + getSelectorString() + ")";
}

std::string CssRuleNode::toDebugString(int indent) const {
    std::ostringstream oss;
    std::string indentStr(indent * 2, ' ');
    
    oss << indentStr << toString() << "\n";
    
    for (const auto& child : getChildren()) {
        oss << child->toDebugString(indent + 1);
    }
    
    return oss.str();
}

std::shared_ptr<Node> CssRuleNode::clone() const {
    auto cloned = std::make_shared<CssRuleNode>(getPosition());
    
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

void CssRuleNode::accept(NodeVisitor& visitor) {
    // 基础实现，子类可以重写
    visitor.visitNode(*this);
}

// CssSelectorNode 实现
CssSelectorNode::CssSelectorNode(SelectorType type, const std::string& value, const NodePosition& position)
    : Node(NodeType::CSS_SELECTOR, position), selectorType_(type), selectorValue_(value) {
    setName("css-selector");
    setContent(value);
}

CssSelectorNode::SelectorType CssSelectorNode::getSelectorType() const {
    return selectorType_;
}

void CssSelectorNode::setSelectorType(SelectorType type) {
    selectorType_ = type;
}

const std::string& CssSelectorNode::getSelectorValue() const {
    return selectorValue_;
}

void CssSelectorNode::setSelectorValue(const std::string& value) {
    if (isValidSelectorValue(value)) {
        selectorValue_ = value;
        setContent(value);
    }
}

bool CssSelectorNode::isAmpersandSelector() const {
    return selectorValue_.find('&') != std::string::npos;
}

std::string CssSelectorNode::resolveAmpersand(const std::string& contextSelector) const {
    std::string result = selectorValue_;
    size_t pos = 0;
    while ((pos = result.find('&', pos)) != std::string::npos) {
        result.replace(pos, 1, contextSelector);
        pos += contextSelector.length();
    }
    return result;
}

bool CssSelectorNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    return isValidSelectorValue(selectorValue_);
}

std::string CssSelectorNode::toString() const {
    std::string prefix;
    switch (selectorType_) {
        case SelectorType::CLASS:
            prefix = ".";
            break;
        case SelectorType::ID:
            prefix = "#";
            break;
        case SelectorType::PSEUDO_CLASS:
            prefix = ":";
            break;
        case SelectorType::PSEUDO_ELEMENT:
            prefix = "::";
            break;
        default:
            prefix = "";
            break;
    }
    return prefix + selectorValue_;
}

std::string CssSelectorNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    return indentStr + "CssSelectorNode(" + selectorTypeToString() + ", \"" + selectorValue_ + "\")\n";
}

std::shared_ptr<Node> CssSelectorNode::clone() const {
    return std::make_shared<CssSelectorNode>(selectorType_, selectorValue_, getPosition());
}

void CssSelectorNode::accept(NodeVisitor& visitor) {
    visitor.visitNode(*this);
}

std::string CssSelectorNode::selectorTypeToString() const {
    switch (selectorType_) {
        case SelectorType::CLASS: return "CLASS";
        case SelectorType::ID: return "ID";
        case SelectorType::ELEMENT: return "ELEMENT";
        case SelectorType::PSEUDO_CLASS: return "PSEUDO_CLASS";
        case SelectorType::PSEUDO_ELEMENT: return "PSEUDO_ELEMENT";
        case SelectorType::ATTRIBUTE: return "ATTRIBUTE";
        case SelectorType::UNIVERSAL: return "UNIVERSAL";
        case SelectorType::DESCENDANT: return "DESCENDANT";
        case SelectorType::CHILD: return "CHILD";
        case SelectorType::SIBLING: return "SIBLING";
        case SelectorType::ADJACENT: return "ADJACENT";
        default: return "UNKNOWN";
    }
}

bool CssSelectorNode::isValidSelectorValue(const std::string& value) const {
    if (value.empty()) return false;
    
    // 简化的CSS选择器验证
    for (char c : value) {
        if (!std::isalnum(c) && c != '-' && c != '_' && c != '&') {
            return false;
        }
    }
    
    return true;
}

// CssPropertyNode 实现
const std::unordered_set<std::string> CssPropertyNode::colorProperties_ = {
    "color", "background-color", "border-color", "outline-color", "text-decoration-color"
};

const std::unordered_set<std::string> CssPropertyNode::sizeProperties_ = {
    "width", "height", "font-size", "margin", "padding", "border-width", "line-height"
};

const std::unordered_set<std::string> CssPropertyNode::positionProperties_ = {
    "position", "top", "right", "bottom", "left", "z-index", "float", "clear"
};

CssPropertyNode::CssPropertyNode(const std::string& property, const std::string& value, const NodePosition& position)
    : Node(NodeType::CSS_PROPERTY, position), property_(property), value_(value), important_(false) {
    setName(property);
    setContent(value);
}

const std::string& CssPropertyNode::getProperty() const {
    return property_;
}

void CssPropertyNode::setProperty(const std::string& property) {
    if (isValidProperty(property)) {
        property_ = property;
        setName(property);
    }
}

const std::string& CssPropertyNode::getValue() const {
    return value_;
}

void CssPropertyNode::setValue(const std::string& value) {
    value_ = normalizeValue(value);
    setContent(value_);
    
    // 检查!important
    if (value_.find("!important") != std::string::npos) {
        important_ = true;
        // 移除!important标记
        size_t pos = value_.find("!important");
        value_ = value_.substr(0, pos);
        // 去除尾部空白
        value_.erase(value_.find_last_not_of(" \t") + 1);
    }
}

bool CssPropertyNode::isImportant() const {
    return important_;
}

void CssPropertyNode::setImportant(bool important) {
    important_ = important;
}

bool CssPropertyNode::isColorProperty() const {
    return colorProperties_.find(property_) != colorProperties_.end();
}

bool CssPropertyNode::isSizeProperty() const {
    return sizeProperties_.find(property_) != sizeProperties_.end();
}

bool CssPropertyNode::isPositionProperty() const {
    return positionProperties_.find(property_) != positionProperties_.end();
}

bool CssPropertyNode::isCustomProperty() const {
    return property_.substr(0, 2) == "--";
}

std::string CssPropertyNode::getNormalizedValue() const {
    return normalizeValue(value_);
}

std::vector<std::string> CssPropertyNode::getValueTokens() const {
    std::vector<std::string> tokens;
    std::istringstream iss(value_);
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

bool CssPropertyNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    return isValidProperty(property_) && isValidValue(value_);
}

std::string CssPropertyNode::toString() const {
    std::string result = property_ + ": " + value_;
    if (important_) {
        result += " !important";
    }
    return "CssPropertyNode(" + result + ")";
}

std::string CssPropertyNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    return indentStr + toString() + "\n";
}

std::shared_ptr<Node> CssPropertyNode::clone() const {
    auto cloned = std::make_shared<CssPropertyNode>(property_, value_, getPosition());
    cloned->important_ = important_;
    return cloned;
}

void CssPropertyNode::accept(NodeVisitor& visitor) {
    visitor.visitNode(*this);
}

bool CssPropertyNode::isValidProperty(const std::string& property) const {
    if (property.empty()) return false;
    
    // CSS属性名验证
    for (char c : property) {
        if (!std::isalnum(c) && c != '-') {
            return false;
        }
    }
    
    return std::isalpha(property[0]) || property[0] == '-';
}

bool CssPropertyNode::isValidValue(const std::string& value) const {
    // 简化的CSS值验证
    return !value.empty();
}

std::string CssPropertyNode::normalizeValue(const std::string& value) const {
    std::string result = value;
    
    // 去除首尾空白
    result.erase(0, result.find_first_not_of(" \t\r\n"));
    result.erase(result.find_last_not_of(" \t\r\n") + 1);
    
    return result;
}

} // namespace chtl