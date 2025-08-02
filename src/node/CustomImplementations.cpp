#include "Custom.h"
#include "Template.h"
#include <sstream>
#include <algorithm>

namespace chtl {

// CustomStyleNode实现
CustomStyleNode::CustomStyleNode(const NodePosition& position)
    : Node(NodeType::CUSTOM_STYLE, position), isValued_(false) {
}

const std::string& CustomStyleNode::getName() const {
    return name_;
}

void CustomStyleNode::setName(const std::string& name) {
    name_ = name;
}

bool CustomStyleNode::hasName() const {
    return !name_.empty();
}

void CustomStyleNode::addProperty(const std::string& property, const std::string& value) {
    properties_[property] = value;
}

void CustomStyleNode::removeProperty(const std::string& property) {
    properties_.erase(property);
}

std::string CustomStyleNode::getProperty(const std::string& property) const {
    auto it = properties_.find(property);
    return it != properties_.end() ? it->second : "";
}

bool CustomStyleNode::hasProperty(const std::string& property) const {
    return properties_.find(property) != properties_.end();
}

const std::unordered_map<std::string, std::string>& CustomStyleNode::getAllProperties() const {
    return properties_;
}

void CustomStyleNode::clearProperties() {
    properties_.clear();
}

void CustomStyleNode::setValued(bool valued) {
    isValued_ = valued;
}

bool CustomStyleNode::isValued() const {
    return isValued_;
}

void CustomStyleNode::addValue(const std::string& value) {
    if (std::find(values_.begin(), values_.end(), value) == values_.end()) {
        values_.push_back(value);
    }
}

void CustomStyleNode::removeValue(const std::string& value) {
    values_.erase(std::remove(values_.begin(), values_.end(), value), values_.end());
}

const std::vector<std::string>& CustomStyleNode::getValues() const {
    return values_;
}

void CustomStyleNode::clearValues() {
    values_.clear();
}

bool CustomStyleNode::hasValue(const std::string& value) const {
    return std::find(values_.begin(), values_.end(), value) != values_.end();
}

void CustomStyleNode::setInheritedFrom(const std::string& parent) {
    inheritedFrom_ = parent;
}

std::string CustomStyleNode::getInheritedFrom() const {
    return inheritedFrom_;
}

bool CustomStyleNode::hasInheritance() const {
    return !inheritedFrom_.empty();
}

void CustomStyleNode::addSpecialization(const std::string& specialization) {
    if (std::find(specializations_.begin(), specializations_.end(), specialization) == specializations_.end()) {
        specializations_.push_back(specialization);
    }
}

void CustomStyleNode::removeSpecialization(const std::string& specialization) {
    specializations_.erase(std::remove(specializations_.begin(), specializations_.end(), specialization), 
                          specializations_.end());
}

const std::vector<std::string>& CustomStyleNode::getSpecializations() const {
    return specializations_;
}

void CustomStyleNode::clearSpecializations() {
    specializations_.clear();
}

bool CustomStyleNode::hasSpecialization(const std::string& specialization) const {
    return std::find(specializations_.begin(), specializations_.end(), specialization) != specializations_.end();
}

bool CustomStyleNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    // 自定义样式必须有名称
    if (name_.empty()) {
        return false;
    }
    
    // 如果是值样式，必须有值或属性
    if (isValued_ && values_.empty() && properties_.empty()) {
        return false;
    }
    
    return true;
}

std::string CustomStyleNode::toString() const {
    std::stringstream ss;
    ss << "[Custom] @Style " << name_;
    
    if (isValued_ && !values_.empty()) {
        ss << " {";
        for (size_t i = 0; i < values_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << values_[i];
        }
        ss << "}";
    }
    
    if (!properties_.empty()) {
        ss << " {" << std::endl;
        for (const auto& pair : properties_) {
            ss << "  " << pair.first << ": " << pair.second << ";" << std::endl;
        }
        ss << "}";
    }
    
    return ss.str();
}

std::string CustomStyleNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "CustomStyleNode {" << std::endl;
    ss << indentStr << "  name: " << name_ << std::endl;
    ss << indentStr << "  isValued: " << (isValued_ ? "true" : "false") << std::endl;
    
    if (!values_.empty()) {
        ss << indentStr << "  values: [";
        for (size_t i = 0; i < values_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << values_[i];
        }
        ss << "]" << std::endl;
    }
    
    if (!properties_.empty()) {
        ss << indentStr << "  properties: {" << std::endl;
        for (const auto& pair : properties_) {
            ss << indentStr << "    " << pair.first << ": " << pair.second << std::endl;
        }
        ss << indentStr << "  }" << std::endl;
    }
    
    if (!inheritedFrom_.empty()) {
        ss << indentStr << "  inheritedFrom: " << inheritedFrom_ << std::endl;
    }
    
    if (!specializations_.empty()) {
        ss << indentStr << "  specializations: [";
        for (size_t i = 0; i < specializations_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << specializations_[i];
        }
        ss << "]" << std::endl;
    }
    
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> CustomStyleNode::clone() const {
    auto cloned = std::make_shared<CustomStyleNode>(getPosition());
    cloned->copyBaseProperties(*this);
    cloned->name_ = name_;
    cloned->isValued_ = isValued_;
    cloned->values_ = values_;
    cloned->properties_ = properties_;
    cloned->inheritedFrom_ = inheritedFrom_;
    cloned->specializations_ = specializations_;
    return cloned;
}

void CustomStyleNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// CustomElementNode实现
CustomElementNode::CustomElementNode(const NodePosition& position)
    : Node(NodeType::CUSTOM_ELEMENT, position) {
}

const std::string& CustomElementNode::getName() const {
    return name_;
}

void CustomElementNode::setName(const std::string& name) {
    name_ = name;
}

bool CustomElementNode::hasName() const {
    return !name_.empty();
}

void CustomElementNode::addAttribute(const std::string& name, const std::string& value) {
    attributes_[name] = value;
}

void CustomElementNode::removeAttribute(const std::string& name) {
    attributes_.erase(name);
}

std::string CustomElementNode::getAttribute(const std::string& name) const {
    auto it = attributes_.find(name);
    return it != attributes_.end() ? it->second : "";
}

bool CustomElementNode::hasAttribute(const std::string& name) const {
    return attributes_.find(name) != attributes_.end();
}

const std::unordered_map<std::string, std::string>& CustomElementNode::getAllAttributes() const {
    return attributes_;
}

void CustomElementNode::clearAttributes() {
    attributes_.clear();
}

void CustomElementNode::addDefaultProperty(const std::string& property, const std::string& value) {
    defaultProperties_[property] = value;
}

void CustomElementNode::removeDefaultProperty(const std::string& property) {
    defaultProperties_.erase(property);
}

std::string CustomElementNode::getDefaultProperty(const std::string& property) const {
    auto it = defaultProperties_.find(property);
    return it != defaultProperties_.end() ? it->second : "";
}

bool CustomElementNode::hasDefaultProperty(const std::string& property) const {
    return defaultProperties_.find(property) != defaultProperties_.end();
}

const std::unordered_map<std::string, std::string>& CustomElementNode::getAllDefaultProperties() const {
    return defaultProperties_;
}

void CustomElementNode::clearDefaultProperties() {
    defaultProperties_.clear();
}

void CustomElementNode::setTagName(const std::string& tagName) {
    tagName_ = tagName;
}

std::string CustomElementNode::getTagName() const {
    return tagName_;
}

bool CustomElementNode::hasTagName() const {
    return !tagName_.empty();
}

void CustomElementNode::setContent(const std::string& content) {
    content_ = content;
}

std::string CustomElementNode::getContent() const {
    return content_;
}

bool CustomElementNode::hasContent() const {
    return !content_.empty();
}

void CustomElementNode::setInheritedFrom(const std::string& parent) {
    inheritedFrom_ = parent;
}

std::string CustomElementNode::getInheritedFrom() const {
    return inheritedFrom_;
}

bool CustomElementNode::hasInheritance() const {
    return !inheritedFrom_.empty();
}

void CustomElementNode::addChild(std::shared_ptr<Node> child) {
    Node::addChild(child);
    if (child) {
        childElements_.push_back(child);
    }
}

void CustomElementNode::removeChild(std::shared_ptr<Node> child) {
    Node::removeChild(child);
    auto it = std::find(childElements_.begin(), childElements_.end(), child);
    if (it != childElements_.end()) {
        childElements_.erase(it);
    }
}

const std::vector<std::shared_ptr<Node>>& CustomElementNode::getChildElements() const {
    return childElements_;
}

bool CustomElementNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    // 自定义元素必须有名称
    if (name_.empty()) {
        return false;
    }
    
    return true;
}

std::string CustomElementNode::toString() const {
    std::stringstream ss;
    ss << "[Custom] @Element " << name_;
    
    if (!tagName_.empty()) {
        ss << " : " << tagName_;
    }
    
    if (!attributes_.empty() || !defaultProperties_.empty() || !content_.empty()) {
        ss << " {" << std::endl;
        
        // 属性
        for (const auto& pair : attributes_) {
            ss << "  " << pair.first << "=\"" << pair.second << "\"" << std::endl;
        }
        
        // 默认属性
        for (const auto& pair : defaultProperties_) {
            ss << "  " << pair.first << ": " << pair.second << ";" << std::endl;
        }
        
        // 内容
        if (!content_.empty()) {
            ss << "  \"" << content_ << "\"" << std::endl;
        }
        
        ss << "}";
    }
    
    return ss.str();
}

std::string CustomElementNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "CustomElementNode {" << std::endl;
    ss << indentStr << "  name: " << name_ << std::endl;
    
    if (!tagName_.empty()) {
        ss << indentStr << "  tagName: " << tagName_ << std::endl;
    }
    
    if (!attributes_.empty()) {
        ss << indentStr << "  attributes: {" << std::endl;
        for (const auto& pair : attributes_) {
            ss << indentStr << "    " << pair.first << ": " << pair.second << std::endl;
        }
        ss << indentStr << "  }" << std::endl;
    }
    
    if (!defaultProperties_.empty()) {
        ss << indentStr << "  defaultProperties: {" << std::endl;
        for (const auto& pair : defaultProperties_) {
            ss << indentStr << "    " << pair.first << ": " << pair.second << std::endl;
        }
        ss << indentStr << "  }" << std::endl;
    }
    
    if (!content_.empty()) {
        ss << indentStr << "  content: " << content_ << std::endl;
    }
    
    if (!inheritedFrom_.empty()) {
        ss << indentStr << "  inheritedFrom: " << inheritedFrom_ << std::endl;
    }
    
    if (!childElements_.empty()) {
        ss << indentStr << "  childElements: " << childElements_.size() << std::endl;
    }
    
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> CustomElementNode::clone() const {
    auto cloned = std::make_shared<CustomElementNode>(getPosition());
    cloned->copyBaseProperties(*this);
    cloned->name_ = name_;
    cloned->tagName_ = tagName_;
    cloned->attributes_ = attributes_;
    cloned->defaultProperties_ = defaultProperties_;
    cloned->content_ = content_;
    cloned->inheritedFrom_ = inheritedFrom_;
    
    // 克隆子元素
    for (const auto& child : childElements_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

void CustomElementNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// CustomVarNode实现
CustomVarNode::CustomVarNode(const NodePosition& position)
    : Node(NodeType::CUSTOM_VAR, position), varType_(VarType::STRING) {
}

const std::string& CustomVarNode::getName() const {
    return name_;
}

void CustomVarNode::setName(const std::string& name) {
    name_ = name;
}

bool CustomVarNode::hasName() const {
    return !name_.empty();
}

void CustomVarNode::setValue(const std::string& value) {
    value_ = value;
}

std::string CustomVarNode::getValue() const {
    return value_;
}

bool CustomVarNode::hasValue() const {
    return !value_.empty();
}

void CustomVarNode::setVarType(VarType type) {
    varType_ = type;
}

CustomVarNode::VarType CustomVarNode::getVarType() const {
    return varType_;
}

void CustomVarNode::setDefaultValue(const std::string& defaultValue) {
    defaultValue_ = defaultValue;
}

std::string CustomVarNode::getDefaultValue() const {
    return defaultValue_;
}

bool CustomVarNode::hasDefaultValue() const {
    return !defaultValue_.empty();
}

void CustomVarNode::addValidator(const std::string& validator) {
    if (std::find(validators_.begin(), validators_.end(), validator) == validators_.end()) {
        validators_.push_back(validator);
    }
}

void CustomVarNode::removeValidator(const std::string& validator) {
    validators_.erase(std::remove(validators_.begin(), validators_.end(), validator), validators_.end());
}

const std::vector<std::string>& CustomVarNode::getValidators() const {
    return validators_;
}

void CustomVarNode::clearValidators() {
    validators_.clear();
}

bool CustomVarNode::hasValidator(const std::string& validator) const {
    return std::find(validators_.begin(), validators_.end(), validator) != validators_.end();
}

void CustomVarNode::setDescription(const std::string& description) {
    description_ = description;
}

std::string CustomVarNode::getDescription() const {
    return description_;
}

bool CustomVarNode::hasDescription() const {
    return !description_.empty();
}

void CustomVarNode::setReadOnly(bool readOnly) {
    isReadOnly_ = readOnly;
}

bool CustomVarNode::isReadOnly() const {
    return isReadOnly_;
}

bool CustomVarNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    // 自定义变量必须有名称
    if (name_.empty()) {
        return false;
    }
    
    return true;
}

std::string CustomVarNode::toString() const {
    std::stringstream ss;
    ss << "[Custom] @Var " << name_;
    
    // 类型
    switch (varType_) {
        case VarType::STRING: ss << " : string"; break;
        case VarType::NUMBER: ss << " : number"; break;
        case VarType::BOOLEAN: ss << " : boolean"; break;
        case VarType::COLOR: ss << " : color"; break;
        case VarType::SIZE: ss << " : size"; break;
        case VarType::ARRAY: ss << " : array"; break;
        case VarType::OBJECT: ss << " : object"; break;
    }
    
    if (!value_.empty()) {
        ss << " = " << value_;
    } else if (!defaultValue_.empty()) {
        ss << " = " << defaultValue_;
    }
    
    if (isReadOnly_) {
        ss << " readonly";
    }
    
    return ss.str();
}

std::string CustomVarNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "CustomVarNode {" << std::endl;
    ss << indentStr << "  name: " << name_ << std::endl;
    ss << indentStr << "  type: " << static_cast<int>(varType_) << std::endl;
    ss << indentStr << "  value: " << value_ << std::endl;
    ss << indentStr << "  defaultValue: " << defaultValue_ << std::endl;
    ss << indentStr << "  isReadOnly: " << (isReadOnly_ ? "true" : "false") << std::endl;
    
    if (!validators_.empty()) {
        ss << indentStr << "  validators: [";
        for (size_t i = 0; i < validators_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << validators_[i];
        }
        ss << "]" << std::endl;
    }
    
    if (!description_.empty()) {
        ss << indentStr << "  description: " << description_ << std::endl;
    }
    
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> CustomVarNode::clone() const {
    auto cloned = std::make_shared<CustomVarNode>(getPosition());
    cloned->copyBaseProperties(*this);
    cloned->name_ = name_;
    cloned->value_ = value_;
    cloned->varType_ = varType_;
    cloned->defaultValue_ = defaultValue_;
    cloned->validators_ = validators_;
    cloned->description_ = description_;
    cloned->isReadOnly_ = isReadOnly_;
    return cloned;
}

void CustomVarNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

std::string CustomVarNode::varTypeToString() const {
    switch (varType_) {
        case VarType::STRING: return "string";
        case VarType::NUMBER: return "number";
        case VarType::BOOLEAN: return "boolean";
        case VarType::COLOR: return "color";
        case VarType::SIZE: return "size";
        case VarType::ARRAY: return "array";
        case VarType::OBJECT: return "object";
        default: return "unknown";
    }
}

// CustomVariableCallNode实现
CustomVariableCallNode::CustomVariableCallNode(const NodePosition& position)
    : Node(NodeType::CUSTOM_VAR_CALL, position) {
}

const std::string& CustomVariableCallNode::getVariableName() const {
    return variableName_;
}

void CustomVariableCallNode::setVariableName(const std::string& name) {
    variableName_ = name;
}

bool CustomVariableCallNode::hasVariableName() const {
    return !variableName_.empty();
}

void CustomVariableCallNode::addArgument(const std::string& argument) {
    arguments_.push_back(argument);
}

void CustomVariableCallNode::removeArgument(size_t index) {
    if (index < arguments_.size()) {
        arguments_.erase(arguments_.begin() + index);
    }
}

const std::vector<std::string>& CustomVariableCallNode::getArguments() const {
    return arguments_;
}

void CustomVariableCallNode::clearArguments() {
    arguments_.clear();
}

size_t CustomVariableCallNode::getArgumentCount() const {
    return arguments_.size();
}

std::string CustomVariableCallNode::getArgument(size_t index) const {
    return index < arguments_.size() ? arguments_[index] : "";
}

void CustomVariableCallNode::setResolvedVariable(std::shared_ptr<CustomVarNode> varNode) {
    resolvedVariable_ = varNode;
}

std::shared_ptr<CustomVarNode> CustomVariableCallNode::getResolvedVariable() const {
    return resolvedVariable_.lock();
}

bool CustomVariableCallNode::hasResolvedVariable() const {
    return !resolvedVariable_.expired();
}

std::string CustomVariableCallNode::getResolvedValue() const {
    auto var = getResolvedVariable();
    if (var) {
        return var->getValue();
    }
    return "";
}

bool CustomVariableCallNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    // 变量调用必须有变量名
    return !variableName_.empty();
}

std::string CustomVariableCallNode::toString() const {
    std::stringstream ss;
    ss << variableName_;
    
    if (!arguments_.empty()) {
        ss << "(";
        for (size_t i = 0; i < arguments_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << arguments_[i];
        }
        ss << ")";
    }
    
    return ss.str();
}

std::string CustomVariableCallNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "CustomVariableCallNode {" << std::endl;
    ss << indentStr << "  variableName: " << variableName_ << std::endl;
    
    if (!arguments_.empty()) {
        ss << indentStr << "  arguments: [";
        for (size_t i = 0; i < arguments_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << arguments_[i];
        }
        ss << "]" << std::endl;
    }
    
    ss << indentStr << "  isResolved: " << (hasResolvedVariable() ? "true" : "false") << std::endl;
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> CustomVariableCallNode::clone() const {
    auto cloned = std::make_shared<CustomVariableCallNode>(getPosition());
    cloned->copyBaseProperties(*this);
    cloned->variableName_ = variableName_;
    cloned->arguments_ = arguments_;
    return cloned;
}

void CustomVariableCallNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// Template节点实现

// TemplateStyleNode实现
TemplateStyleNode::TemplateStyleNode(const NodePosition& position)
    : Node(NodeType::TEMPLATE_STYLE, position), isReadOnly_(true) {
}

const std::string& TemplateStyleNode::getName() const {
    return name_;
}

void TemplateStyleNode::setName(const std::string& name) {
    name_ = name;
}

bool TemplateStyleNode::hasName() const {
    return !name_.empty();
}

void TemplateStyleNode::addProperty(const std::string& property, const std::string& value) {
    if (!isReadOnly_) {
        properties_[property] = value;
    }
}

void TemplateStyleNode::removeProperty(const std::string& property) {
    if (!isReadOnly_) {
        properties_.erase(property);
    }
}

std::string TemplateStyleNode::getProperty(const std::string& property) const {
    auto it = properties_.find(property);
    return it != properties_.end() ? it->second : "";
}

bool TemplateStyleNode::hasProperty(const std::string& property) const {
    return properties_.find(property) != properties_.end();
}

const std::unordered_map<std::string, std::string>& TemplateStyleNode::getAllProperties() const {
    return properties_;
}

void TemplateStyleNode::clearProperties() {
    if (!isReadOnly_) {
        properties_.clear();
    }
}

void TemplateStyleNode::setReadOnly(bool readOnly) {
    isReadOnly_ = readOnly;
}

bool TemplateStyleNode::isReadOnly() const {
    return isReadOnly_;
}

void TemplateStyleNode::addParameter(const std::string& param, const std::string& defaultValue) {
    parameters_[param] = defaultValue;
}

void TemplateStyleNode::removeParameter(const std::string& param) {
    parameters_.erase(param);
}

std::string TemplateStyleNode::getParameter(const std::string& param) const {
    auto it = parameters_.find(param);
    return it != parameters_.end() ? it->second : "";
}

bool TemplateStyleNode::hasParameter(const std::string& param) const {
    return parameters_.find(param) != parameters_.end();
}

const std::unordered_map<std::string, std::string>& TemplateStyleNode::getAllParameters() const {
    return parameters_;
}

void TemplateStyleNode::clearParameters() {
    parameters_.clear();
}

void TemplateStyleNode::setInheritedFrom(const std::string& parent) {
    inheritedFrom_ = parent;
}

std::string TemplateStyleNode::getInheritedFrom() const {
    return inheritedFrom_;
}

bool TemplateStyleNode::hasInheritance() const {
    return !inheritedFrom_.empty();
}

bool TemplateStyleNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    // 模板样式必须有名称
    if (name_.empty()) {
        return false;
    }
    
    return true;
}

std::string TemplateStyleNode::toString() const {
    std::stringstream ss;
    ss << "[Template] @Style " << name_;
    
    if (!parameters_.empty()) {
        ss << "(";
        bool first = true;
        for (const auto& pair : parameters_) {
            if (!first) ss << ", ";
            ss << pair.first;
            if (!pair.second.empty()) {
                ss << " = " << pair.second;
            }
            first = false;
        }
        ss << ")";
    }
    
    if (!properties_.empty()) {
        ss << " {" << std::endl;
        for (const auto& pair : properties_) {
            ss << "  " << pair.first << ": " << pair.second << ";" << std::endl;
        }
        ss << "}";
    }
    
    return ss.str();
}

std::string TemplateStyleNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "TemplateStyleNode {" << std::endl;
    ss << indentStr << "  name: " << name_ << std::endl;
    ss << indentStr << "  isReadOnly: " << (isReadOnly_ ? "true" : "false") << std::endl;
    
    if (!parameters_.empty()) {
        ss << indentStr << "  parameters: {" << std::endl;
        for (const auto& pair : parameters_) {
            ss << indentStr << "    " << pair.first << ": " << pair.second << std::endl;
        }
        ss << indentStr << "  }" << std::endl;
    }
    
    if (!properties_.empty()) {
        ss << indentStr << "  properties: {" << std::endl;
        for (const auto& pair : properties_) {
            ss << indentStr << "    " << pair.first << ": " << pair.second << std::endl;
        }
        ss << indentStr << "  }" << std::endl;
    }
    
    if (!inheritedFrom_.empty()) {
        ss << indentStr << "  inheritedFrom: " << inheritedFrom_ << std::endl;
    }
    
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> TemplateStyleNode::clone() const {
    auto cloned = std::make_shared<TemplateStyleNode>(getPosition());
    cloned->copyBaseProperties(*this);
    cloned->name_ = name_;
    cloned->isReadOnly_ = isReadOnly_;
    cloned->properties_ = properties_;
    cloned->parameters_ = parameters_;
    cloned->inheritedFrom_ = inheritedFrom_;
    return cloned;
}

void TemplateStyleNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// TemplateElementNode实现
TemplateElementNode::TemplateElementNode(const NodePosition& position)
    : Node(NodeType::TEMPLATE_ELEMENT, position), isReadOnly_(true) {
}

const std::string& TemplateElementNode::getName() const {
    return name_;
}

void TemplateElementNode::setName(const std::string& name) {
    name_ = name;
}

bool TemplateElementNode::hasName() const {
    return !name_.empty();
}

void TemplateElementNode::addAttribute(const std::string& name, const std::string& value) {
    if (!isReadOnly_) {
        attributes_[name] = value;
    }
}

void TemplateElementNode::removeAttribute(const std::string& name) {
    if (!isReadOnly_) {
        attributes_.erase(name);
    }
}

std::string TemplateElementNode::getAttribute(const std::string& name) const {
    auto it = attributes_.find(name);
    return it != attributes_.end() ? it->second : "";
}

bool TemplateElementNode::hasAttribute(const std::string& name) const {
    return attributes_.find(name) != attributes_.end();
}

const std::unordered_map<std::string, std::string>& TemplateElementNode::getAllAttributes() const {
    return attributes_;
}

void TemplateElementNode::clearAttributes() {
    if (!isReadOnly_) {
        attributes_.clear();
    }
}

void TemplateElementNode::addParameter(const std::string& param, const std::string& defaultValue) {
    parameters_[param] = defaultValue;
}

void TemplateElementNode::removeParameter(const std::string& param) {
    parameters_.erase(param);
}

std::string TemplateElementNode::getParameter(const std::string& param) const {
    auto it = parameters_.find(param);
    return it != parameters_.end() ? it->second : "";
}

bool TemplateElementNode::hasParameter(const std::string& param) const {
    return parameters_.find(param) != parameters_.end();
}

const std::unordered_map<std::string, std::string>& TemplateElementNode::getAllParameters() const {
    return parameters_;
}

void TemplateElementNode::clearParameters() {
    parameters_.clear();
}

void TemplateElementNode::setTagName(const std::string& tagName) {
    tagName_ = tagName;
}

std::string TemplateElementNode::getTagName() const {
    return tagName_;
}

bool TemplateElementNode::hasTagName() const {
    return !tagName_.empty();
}

void TemplateElementNode::setContent(const std::string& content) {
    content_ = content;
}

std::string TemplateElementNode::getContent() const {
    return content_;
}

bool TemplateElementNode::hasContent() const {
    return !content_.empty();
}

void TemplateElementNode::setReadOnly(bool readOnly) {
    isReadOnly_ = readOnly;
}

bool TemplateElementNode::isReadOnly() const {
    return isReadOnly_;
}

void TemplateElementNode::setInheritedFrom(const std::string& parent) {
    inheritedFrom_ = parent;
}

std::string TemplateElementNode::getInheritedFrom() const {
    return inheritedFrom_;
}

bool TemplateElementNode::hasInheritance() const {
    return !inheritedFrom_.empty();
}

bool TemplateElementNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    // 模板元素必须有名称
    if (name_.empty()) {
        return false;
    }
    
    return true;
}

std::string TemplateElementNode::toString() const {
    std::stringstream ss;
    ss << "[Template] @Element " << name_;
    
    if (!parameters_.empty()) {
        ss << "(";
        bool first = true;
        for (const auto& pair : parameters_) {
            if (!first) ss << ", ";
            ss << pair.first;
            if (!pair.second.empty()) {
                ss << " = " << pair.second;
            }
            first = false;
        }
        ss << ")";
    }
    
    if (!tagName_.empty()) {
        ss << " : " << tagName_;
    }
    
    if (!attributes_.empty() || !content_.empty()) {
        ss << " {" << std::endl;
        
        // 属性
        for (const auto& pair : attributes_) {
            ss << "  " << pair.first << "=\"" << pair.second << "\"" << std::endl;
        }
        
        // 内容
        if (!content_.empty()) {
            ss << "  \"" << content_ << "\"" << std::endl;
        }
        
        ss << "}";
    }
    
    return ss.str();
}

std::string TemplateElementNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "TemplateElementNode {" << std::endl;
    ss << indentStr << "  name: " << name_ << std::endl;
    ss << indentStr << "  isReadOnly: " << (isReadOnly_ ? "true" : "false") << std::endl;
    
    if (!tagName_.empty()) {
        ss << indentStr << "  tagName: " << tagName_ << std::endl;
    }
    
    if (!parameters_.empty()) {
        ss << indentStr << "  parameters: {" << std::endl;
        for (const auto& pair : parameters_) {
            ss << indentStr << "    " << pair.first << ": " << pair.second << std::endl;
        }
        ss << indentStr << "  }" << std::endl;
    }
    
    if (!attributes_.empty()) {
        ss << indentStr << "  attributes: {" << std::endl;
        for (const auto& pair : attributes_) {
            ss << indentStr << "    " << pair.first << ": " << pair.second << std::endl;
        }
        ss << indentStr << "  }" << std::endl;
    }
    
    if (!content_.empty()) {
        ss << indentStr << "  content: " << content_ << std::endl;
    }
    
    if (!inheritedFrom_.empty()) {
        ss << indentStr << "  inheritedFrom: " << inheritedFrom_ << std::endl;
    }
    
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> TemplateElementNode::clone() const {
    auto cloned = std::make_shared<TemplateElementNode>(getPosition());
    cloned->copyBaseProperties(*this);
    cloned->name_ = name_;
    cloned->isReadOnly_ = isReadOnly_;
    cloned->tagName_ = tagName_;
    cloned->attributes_ = attributes_;
    cloned->parameters_ = parameters_;
    cloned->content_ = content_;
    cloned->inheritedFrom_ = inheritedFrom_;
    return cloned;
}

void TemplateElementNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// TemplateVarNode实现
TemplateVarNode::TemplateVarNode(const NodePosition& position)
    : Node(NodeType::TEMPLATE_VAR, position), varType_(VarType::STRING), isReadOnly_(true) {
}

const std::string& TemplateVarNode::getName() const {
    return name_;
}

void TemplateVarNode::setName(const std::string& name) {
    name_ = name;
}

bool TemplateVarNode::hasName() const {
    return !name_.empty();
}

void TemplateVarNode::setValue(const std::string& value) {
    if (!isReadOnly_) {
        value_ = value;
    }
}

std::string TemplateVarNode::getValue() const {
    return value_;
}

bool TemplateVarNode::hasValue() const {
    return !value_.empty();
}

void TemplateVarNode::setVarType(VarType type) {
    varType_ = type;
}

TemplateVarNode::VarType TemplateVarNode::getVarType() const {
    return varType_;
}

void TemplateVarNode::setDefaultValue(const std::string& defaultValue) {
    defaultValue_ = defaultValue;
}

std::string TemplateVarNode::getDefaultValue() const {
    return defaultValue_;
}

bool TemplateVarNode::hasDefaultValue() const {
    return !defaultValue_.empty();
}

void TemplateVarNode::addParameter(const std::string& param, const std::string& defaultValue) {
    parameters_[param] = defaultValue;
}

void TemplateVarNode::removeParameter(const std::string& param) {
    parameters_.erase(param);
}

std::string TemplateVarNode::getParameter(const std::string& param) const {
    auto it = parameters_.find(param);
    return it != parameters_.end() ? it->second : "";
}

bool TemplateVarNode::hasParameter(const std::string& param) const {
    return parameters_.find(param) != parameters_.end();
}

const std::unordered_map<std::string, std::string>& TemplateVarNode::getAllParameters() const {
    return parameters_;
}

void TemplateVarNode::clearParameters() {
    parameters_.clear();
}

void TemplateVarNode::setDescription(const std::string& description) {
    description_ = description;
}

std::string TemplateVarNode::getDescription() const {
    return description_;
}

bool TemplateVarNode::hasDescription() const {
    return !description_.empty();
}

void TemplateVarNode::setReadOnly(bool readOnly) {
    isReadOnly_ = readOnly;
}

bool TemplateVarNode::isReadOnly() const {
    return isReadOnly_;
}

bool TemplateVarNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    // 模板变量必须有名称
    if (name_.empty()) {
        return false;
    }
    
    return true;
}

std::string TemplateVarNode::toString() const {
    std::stringstream ss;
    ss << "[Template] @Var " << name_;
    
    if (!parameters_.empty()) {
        ss << "(";
        bool first = true;
        for (const auto& pair : parameters_) {
            if (!first) ss << ", ";
            ss << pair.first;
            if (!pair.second.empty()) {
                ss << " = " << pair.second;
            }
            first = false;
        }
        ss << ")";
    }
    
    // 类型
    switch (varType_) {
        case VarType::STRING: ss << " : string"; break;
        case VarType::NUMBER: ss << " : number"; break;
        case VarType::BOOLEAN: ss << " : boolean"; break;
        case VarType::COLOR: ss << " : color"; break;
        case VarType::SIZE: ss << " : size"; break;
        case VarType::ARRAY: ss << " : array"; break;
        case VarType::OBJECT: ss << " : object"; break;
    }
    
    if (!value_.empty()) {
        ss << " = " << value_;
    } else if (!defaultValue_.empty()) {
        ss << " = " << defaultValue_;
    }
    
    return ss.str();
}

std::string TemplateVarNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "TemplateVarNode {" << std::endl;
    ss << indentStr << "  name: " << name_ << std::endl;
    ss << indentStr << "  type: " << static_cast<int>(varType_) << std::endl;
    ss << indentStr << "  value: " << value_ << std::endl;
    ss << indentStr << "  defaultValue: " << defaultValue_ << std::endl;
    ss << indentStr << "  isReadOnly: " << (isReadOnly_ ? "true" : "false") << std::endl;
    
    if (!parameters_.empty()) {
        ss << indentStr << "  parameters: {" << std::endl;
        for (const auto& pair : parameters_) {
            ss << indentStr << "    " << pair.first << ": " << pair.second << std::endl;
        }
        ss << indentStr << "  }" << std::endl;
    }
    
    if (!description_.empty()) {
        ss << indentStr << "  description: " << description_ << std::endl;
    }
    
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> TemplateVarNode::clone() const {
    auto cloned = std::make_shared<TemplateVarNode>(getPosition());
    cloned->copyBaseProperties(*this);
    cloned->name_ = name_;
    cloned->value_ = value_;
    cloned->varType_ = varType_;
    cloned->defaultValue_ = defaultValue_;
    cloned->parameters_ = parameters_;
    cloned->description_ = description_;
    cloned->isReadOnly_ = isReadOnly_;
    return cloned;
}

void TemplateVarNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

std::string TemplateVarNode::varTypeToString() const {
    switch (varType_) {
        case VarType::STRING: return "string";
        case VarType::NUMBER: return "number";
        case VarType::BOOLEAN: return "boolean";
        case VarType::COLOR: return "color";
        case VarType::SIZE: return "size";
        case VarType::ARRAY: return "array";
        case VarType::OBJECT: return "object";
        default: return "unknown";
    }
}

// TemplateCallNode实现
TemplateCallNode::TemplateCallNode(const NodePosition& position)
    : Node(NodeType::TEMPLATE_CALL, position) {
}

const std::string& TemplateCallNode::getTemplateName() const {
    return templateName_;
}

void TemplateCallNode::setTemplateName(const std::string& name) {
    templateName_ = name;
}

bool TemplateCallNode::hasTemplateName() const {
    return !templateName_.empty();
}

void TemplateCallNode::addArgument(const std::string& name, const std::string& value) {
    arguments_[name] = value;
}

void TemplateCallNode::removeArgument(const std::string& name) {
    arguments_.erase(name);
}

std::string TemplateCallNode::getArgument(const std::string& name) const {
    auto it = arguments_.find(name);
    return it != arguments_.end() ? it->second : "";
}

bool TemplateCallNode::hasArgument(const std::string& name) const {
    return arguments_.find(name) != arguments_.end();
}

const std::unordered_map<std::string, std::string>& TemplateCallNode::getAllArguments() const {
    return arguments_;
}

void TemplateCallNode::clearArguments() {
    arguments_.clear();
}

void TemplateCallNode::setResolvedTemplate(std::shared_ptr<Node> templateNode) {
    resolvedTemplate_ = templateNode;
}

std::shared_ptr<Node> TemplateCallNode::getResolvedTemplate() const {
    return resolvedTemplate_.lock();
}

bool TemplateCallNode::hasResolvedTemplate() const {
    return !resolvedTemplate_.expired();
}

TemplateCallNode::TemplateType TemplateCallNode::getTemplateType() const {
    auto resolved = getResolvedTemplate();
    if (!resolved) {
        return TemplateType::UNKNOWN;
    }
    
    switch (resolved->getNodeType()) {
        case NodeType::TEMPLATE_STYLE:   return TemplateType::STYLE;
        case NodeType::TEMPLATE_ELEMENT: return TemplateType::ELEMENT;
        case NodeType::TEMPLATE_VAR:     return TemplateType::VAR;
        default:                         return TemplateType::UNKNOWN;
    }
}

bool TemplateCallNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    // 模板调用必须有模板名称
    return !templateName_.empty();
}

std::string TemplateCallNode::toString() const {
    std::stringstream ss;
    ss << templateName_;
    
    if (!arguments_.empty()) {
        ss << "(";
        bool first = true;
        for (const auto& pair : arguments_) {
            if (!first) ss << ", ";
            ss << pair.first << " = " << pair.second;
            first = false;
        }
        ss << ")";
    }
    
    return ss.str();
}

std::string TemplateCallNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "TemplateCallNode {" << std::endl;
    ss << indentStr << "  templateName: " << templateName_ << std::endl;
    ss << indentStr << "  templateType: " << static_cast<int>(getTemplateType()) << std::endl;
    
    if (!arguments_.empty()) {
        ss << indentStr << "  arguments: {" << std::endl;
        for (const auto& pair : arguments_) {
            ss << indentStr << "    " << pair.first << ": " << pair.second << std::endl;
        }
        ss << indentStr << "  }" << std::endl;
    }
    
    ss << indentStr << "  isResolved: " << (hasResolvedTemplate() ? "true" : "false") << std::endl;
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> TemplateCallNode::clone() const {
    auto cloned = std::make_shared<TemplateCallNode>(getPosition());
    cloned->copyBaseProperties(*this);
    cloned->templateName_ = templateName_;
    cloned->arguments_ = arguments_;
    return cloned;
}

void TemplateCallNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace chtl