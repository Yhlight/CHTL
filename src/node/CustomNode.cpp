#include "CustomNode.h"
#include <sstream>

namespace chtl {

// CustomStyleNode 基础实现
CustomStyleNode::CustomStyleNode(const std::string& name, const NodePosition& position)
    : Node(NodeType::CUSTOM_STYLE, position) {
    setName(name);
}

const std::string& CustomStyleNode::getStyleName() const {
    return getName();
}

void CustomStyleNode::setStyleName(const std::string& name) {
    setName(name);
}

void CustomStyleNode::addProperty(const std::string& property, const std::string& value) {
    properties_[property] = value;
}

void CustomStyleNode::removeProperty(const std::string& property) {
    properties_.erase(property);
}

std::string CustomStyleNode::getPropertyValue(const std::string& property) const {
    auto it = properties_.find(property);
    return (it != properties_.end()) ? it->second : "";
}

std::unordered_map<std::string, std::string> CustomStyleNode::getAllProperties() const {
    return properties_;
}

void CustomStyleNode::addInheritedStyle(const std::string& styleName) {
    inheritedStyles_.push_back(styleName);
}

void CustomStyleNode::removeInheritedStyle(const std::string& styleName) {
    // 占位实现
}

std::vector<std::string> CustomStyleNode::getInheritedStyles() const {
    return inheritedStyles_;
}

void CustomStyleNode::addSpecialization(const std::string& inheritedStyle, const std::string& property, const std::string& value) {
    specializations_.push_back(std::make_tuple(inheritedStyle, property, value));
}

std::vector<std::tuple<std::string, std::string, std::string>> CustomStyleNode::getSpecializations() const {
    return specializations_;
}

void CustomStyleNode::addOperation(const std::string& operation, const std::string& target, const std::string& property, const std::string& value) {
    operations_.push_back(std::make_tuple(operation, target, property, value));
}

std::vector<std::tuple<std::string, std::string, std::string, std::string>> CustomStyleNode::getOperations() const {
    return operations_;
}

bool CustomStyleNode::validate() const {
    return Node::validate() && !getName().empty();
}

std::string CustomStyleNode::toString() const {
    return "CustomStyleNode('" + getName() + "')";
}

std::string CustomStyleNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    std::ostringstream oss;
    oss << indentStr << toString() << "\n";
    
    for (const auto& child : getChildren()) {
        oss << child->toDebugString(indent + 1);
    }
    
    return oss.str();
}

std::shared_ptr<Node> CustomStyleNode::clone() const {
    auto cloned = std::make_shared<CustomStyleNode>(getName(), getPosition());
    cloned->properties_ = properties_;
    cloned->inheritedStyles_ = inheritedStyles_;
    cloned->specializations_ = specializations_;
    cloned->operations_ = operations_;
    
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

void CustomStyleNode::accept(NodeVisitor& visitor) {
    visitor.visitNode(*this);
}

// CustomElementNode 基础实现
CustomElementNode::CustomElementNode(const std::string& name, const NodePosition& position)
    : Node(NodeType::CUSTOM_ELEMENT, position) {
    setName(name);
}

bool CustomElementNode::validate() const {
    return Node::validate() && !getName().empty();
}

std::string CustomElementNode::toString() const {
    return "CustomElementNode('" + getName() + "')";
}

std::string CustomElementNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    std::ostringstream oss;
    oss << indentStr << toString() << "\n";
    
    for (const auto& child : getChildren()) {
        oss << child->toDebugString(indent + 1);
    }
    
    return oss.str();
}

std::shared_ptr<Node> CustomElementNode::clone() const {
    auto cloned = std::make_shared<CustomElementNode>(getName(), getPosition());
    
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

void CustomElementNode::accept(NodeVisitor& visitor) {
    visitor.visitNode(*this);
}

// CustomVarNode 基础实现
CustomVarNode::CustomVarNode(const std::string& name, const NodePosition& position)
    : Node(NodeType::CUSTOM_VAR, position), varName_(name) {
    setName(name);
}

const std::string& CustomVarNode::getVarName() const {
    return varName_;
}

void CustomVarNode::setVarName(const std::string& name) {
    varName_ = name;
    setName(name);
}

void CustomVarNode::addVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

void CustomVarNode::removeVariable(const std::string& name) {
    variables_.erase(name);
}

std::string CustomVarNode::getVariableValue(const std::string& name) const {
    auto it = variables_.find(name);
    return (it != variables_.end()) ? it->second : "";
}

std::unordered_map<std::string, std::string> CustomVarNode::getAllVariables() const {
    return variables_;
}

void CustomVarNode::addInheritedVar(const std::string& varName) {
    inheritedVars_.push_back(varName);
}

void CustomVarNode::removeInheritedVar(const std::string& varName) {
    // 占位实现
    (void)varName;
}

std::vector<std::string> CustomVarNode::getInheritedVars() const {
    return inheritedVars_;
}

void CustomVarNode::addVarSpecialization(const std::string& inheritedVar, const std::string& variable, const std::string& value) {
    specializations_.push_back(std::make_tuple(inheritedVar, variable, value));
}

std::vector<std::tuple<std::string, std::string, std::string>> CustomVarNode::getVarSpecializations() const {
    return specializations_;
}

bool CustomVarNode::hasVariableOverride(const std::string& varName) const {
    (void)varName;
    return false; // 占位实现
}

std::string CustomVarNode::getResolvedVariableValue(const std::string& varName) const {
    return getVariableValue(varName);
}

bool CustomVarNode::validate() const {
    return Node::validate() && !getName().empty();
}

std::string CustomVarNode::toString() const {
    return "CustomVarNode('" + getName() + "')";
}

std::string CustomVarNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    std::ostringstream oss;
    oss << indentStr << toString() << "\n";
    
    for (const auto& child : getChildren()) {
        oss << child->toDebugString(indent + 1);
    }
    
    return oss.str();
}

std::shared_ptr<Node> CustomVarNode::clone() const {
    auto cloned = std::make_shared<CustomVarNode>(varName_, getPosition());
    cloned->variables_ = variables_;
    cloned->inheritedVars_ = inheritedVars_;
    cloned->specializations_ = specializations_;
    
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

void CustomVarNode::accept(NodeVisitor& visitor) {
    visitor.visitNode(*this);
}

// VariableCallNode 基础实现
VariableCallNode::VariableCallNode(const std::string& varGroupName, const NodePosition& position)
    : Node(NodeType::VARIABLE_CALL, position), varGroupName_(varGroupName) {
    setName(varGroupName);
}

const std::string& VariableCallNode::getVarGroupName() const {
    return varGroupName_;
}

void VariableCallNode::setVarGroupName(const std::string& name) {
    varGroupName_ = name;
    setName(name);
}

bool VariableCallNode::validate() const {
    return Node::validate() && !varGroupName_.empty();
}

std::string VariableCallNode::toString() const {
    return "VariableCallNode('" + varGroupName_ + "')";
}

std::string VariableCallNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    std::ostringstream oss;
    oss << indentStr << toString() << "\n";
    
    for (const auto& child : getChildren()) {
        oss << child->toDebugString(indent + 1);
    }
    
    return oss.str();
}

std::shared_ptr<Node> VariableCallNode::clone() const {
    auto cloned = std::make_shared<VariableCallNode>(varGroupName_, getPosition());
    
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

void VariableCallNode::accept(NodeVisitor& visitor) {
    visitor.visitNode(*this);
}

// TemplateStyleNode 基础实现
TemplateStyleNode::TemplateStyleNode(const std::string& name, const NodePosition& position)
    : Node(NodeType::TEMPLATE_STYLE, position) {
    setName(name);
}

bool TemplateStyleNode::validate() const {
    return Node::validate() && !getName().empty();
}

std::string TemplateStyleNode::toString() const {
    return "TemplateStyleNode('" + getName() + "')";
}

std::string TemplateStyleNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    std::ostringstream oss;
    oss << indentStr << toString() << "\n";
    
    for (const auto& child : getChildren()) {
        oss << child->toDebugString(indent + 1);
    }
    
    return oss.str();
}

std::shared_ptr<Node> TemplateStyleNode::clone() const {
    auto cloned = std::make_shared<TemplateStyleNode>(getName(), getPosition());
    
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

void TemplateStyleNode::accept(NodeVisitor& visitor) {
    visitor.visitNode(*this);
}

// TemplateElementNode 基础实现
TemplateElementNode::TemplateElementNode(const std::string& name, const NodePosition& position)
    : Node(NodeType::TEMPLATE_ELEMENT, position) {
    setName(name);
}

bool TemplateElementNode::validate() const {
    return Node::validate() && !getName().empty();
}

std::string TemplateElementNode::toString() const {
    return "TemplateElementNode('" + getName() + "')";
}

std::string TemplateElementNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    std::ostringstream oss;
    oss << indentStr << toString() << "\n";
    
    for (const auto& child : getChildren()) {
        oss << child->toDebugString(indent + 1);
    }
    
    return oss.str();
}

std::shared_ptr<Node> TemplateElementNode::clone() const {
    auto cloned = std::make_shared<TemplateElementNode>(getName(), getPosition());
    
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

void TemplateElementNode::accept(NodeVisitor& visitor) {
    visitor.visitNode(*this);
}

// TemplateVarNode 基础实现
TemplateVarNode::TemplateVarNode(const std::string& name, const NodePosition& position)
    : Node(NodeType::TEMPLATE_VAR, position) {
    setName(name);
}

bool TemplateVarNode::validate() const {
    return Node::validate() && !getName().empty();
}

std::string TemplateVarNode::toString() const {
    return "TemplateVarNode('" + getName() + "')";
}

std::string TemplateVarNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    std::ostringstream oss;
    oss << indentStr << toString() << "\n";
    
    for (const auto& child : getChildren()) {
        oss << child->toDebugString(indent + 1);
    }
    
    return oss.str();
}

std::shared_ptr<Node> TemplateVarNode::clone() const {
    auto cloned = std::make_shared<TemplateVarNode>(getName(), getPosition());
    
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

void TemplateVarNode::accept(NodeVisitor& visitor) {
    visitor.visitNode(*this);
}

} // namespace chtl