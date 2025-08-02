#include "Operator.h"
#include <sstream>
#include <algorithm>

namespace chtl {

// OperatorNode基类实现
OperatorNode::OperatorNode(OperatorType type, const NodePosition& position)
    : Node(NodeType::OPERATOR, position), operatorType_(type) {}

// 为子类提供的构造函数
OperatorNode::OperatorNode(OperatorType type, NodeType nodeType, const NodePosition& position)
    : Node(nodeType, position), operatorType_(type) {}

OperatorNode::OperatorType OperatorNode::getOperatorType() const {
    return operatorType_;
}

std::string OperatorNode::operatorTypeToString() const {
    switch (operatorType_) {
        case OperatorType::ADD:     return "add";
        case OperatorType::DELETE:  return "delete";
        case OperatorType::FROM:    return "from";
        case OperatorType::AS:      return "as";
        case OperatorType::INHERIT: return "inherit";
        default:                    return "unknown";
    }
}

bool OperatorNode::validate() const {
    return Node::validate();
}

std::string OperatorNode::toString() const {
    return operatorTypeToString();
}

std::string OperatorNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "OperatorNode {" << std::endl;
    ss << indentStr << "  type: " << operatorTypeToString() << std::endl;
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> OperatorNode::clone() const {
    return std::make_shared<OperatorNode>(operatorType_, getPosition());
}

void OperatorNode::accept(NodeVisitor& visitor) {
    // 访问者模式需要NodeVisitor有visit方法，这里暂时空实现
    (void)visitor;
}

// AddOperatorNode实现
AddOperatorNode::AddOperatorNode(const NodePosition& position)
    : OperatorNode(OperatorType::ADD, NodeType::ADD_OPERATOR, position), targetType_(TargetType::ELEMENT), 
      index_(0), hasIndex_(false), hasValue_(false) {
}

void AddOperatorNode::setTarget(const std::string& target) {
    target_ = target;
}

std::string AddOperatorNode::getTarget() const {
    return target_;
}

void AddOperatorNode::setTargetType(TargetType type) {
    targetType_ = type;
}

AddOperatorNode::TargetType AddOperatorNode::getTargetType() const {
    return targetType_;
}

void AddOperatorNode::setIndex(size_t index) {
    index_ = index;
    hasIndex_ = true;
}

size_t AddOperatorNode::getIndex() const {
    return index_;
}

bool AddOperatorNode::hasIndex() const {
    return hasIndex_;
}

void AddOperatorNode::setValue(const std::string& value) {
    value_ = value;
    hasValue_ = true;
}

std::string AddOperatorNode::getValue() const {
    return value_;
}

bool AddOperatorNode::hasValue() const {
    return hasValue_;
}

void AddOperatorNode::addProperty(const std::string& name, const std::string& value) {
    properties_[name] = value;
}

void AddOperatorNode::removeProperty(const std::string& name) {
    properties_.erase(name);
}

std::unordered_map<std::string, std::string> AddOperatorNode::getProperties() const {
    return properties_;
}

bool AddOperatorNode::validate() const {
    if (!OperatorNode::validate()) {
        return false;
    }
    return !target_.empty();
}

std::string AddOperatorNode::toString() const {
    std::stringstream ss;
    ss << "add " << targetTypeToString() << " " << target_;
    
    if (hasIndex_) {
        ss << "[" << index_ << "]";
    }
    
    if (hasValue_) {
        ss << " = " << value_;
    }
    
    if (!properties_.empty()) {
        ss << " {";
        bool first = true;
        for (const auto& prop : properties_) {
            if (!first) ss << ", ";
            ss << prop.first << ": " << prop.second;
            first = false;
        }
        ss << "}";
    }
    
    return ss.str();
}

std::string AddOperatorNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "AddOperatorNode {" << std::endl;
    ss << indentStr << "  target: " << target_ << std::endl;
    ss << indentStr << "  targetType: " << targetTypeToString() << std::endl;
    
    if (hasIndex_) {
        ss << indentStr << "  index: " << index_ << std::endl;
    }
    
    if (hasValue_) {
        ss << indentStr << "  value: " << value_ << std::endl;
    }
    
    if (!properties_.empty()) {
        ss << indentStr << "  properties: {" << std::endl;
        for (const auto& prop : properties_) {
            ss << indentStr << "    " << prop.first << ": " << prop.second << std::endl;
        }
        ss << indentStr << "  }" << std::endl;
    }
    
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> AddOperatorNode::clone() const {
    auto cloned = std::make_shared<AddOperatorNode>(getPosition());
    cloned->target_ = target_;
    cloned->targetType_ = targetType_;
    cloned->index_ = index_;
    cloned->hasIndex_ = hasIndex_;
    cloned->value_ = value_;
    cloned->hasValue_ = hasValue_;
    cloned->properties_ = properties_;
    return cloned;
}

void AddOperatorNode::accept(NodeVisitor& visitor) {
    (void)visitor; // 暂时空实现
}

std::string AddOperatorNode::targetTypeToString() const {
    switch (targetType_) {
        case TargetType::ELEMENT:  return "@Element";
        case TargetType::STYLE:    return "@Style";
        case TargetType::VAR:      return "@Var";
        case TargetType::PROPERTY: return "property";
        case TargetType::CHILD:    return "child";
        default:                   return "unknown";
    }
}

// DeleteOperatorNode实现
DeleteOperatorNode::DeleteOperatorNode(const NodePosition& position)
    : OperatorNode(OperatorType::DELETE, NodeType::DELETE_OPERATOR, position), targetType_(TargetType::ELEMENT),
      index_(0), hasIndex_(false), hasSelector_(false), hasPropertyName_(false) {
}

void DeleteOperatorNode::setTarget(const std::string& target) {
    target_ = target;
}

std::string DeleteOperatorNode::getTarget() const {
    return target_;
}

void DeleteOperatorNode::setTargetType(TargetType type) {
    targetType_ = type;
}

DeleteOperatorNode::TargetType DeleteOperatorNode::getTargetType() const {
    return targetType_;
}

void DeleteOperatorNode::setIndex(size_t index) {
    index_ = index;
    hasIndex_ = true;
}

size_t DeleteOperatorNode::getIndex() const {
    return index_;
}

bool DeleteOperatorNode::hasIndex() const {
    return hasIndex_;
}

void DeleteOperatorNode::setSelector(const std::string& selector) {
    selector_ = selector;
    hasSelector_ = !selector.empty();
}

std::string DeleteOperatorNode::getSelector() const {
    return selector_;
}

bool DeleteOperatorNode::hasSelector() const {
    return hasSelector_;
}

void DeleteOperatorNode::setPropertyName(const std::string& propertyName) {
    propertyName_ = propertyName;
    hasPropertyName_ = !propertyName.empty();
}

std::string DeleteOperatorNode::getPropertyName() const {
    return propertyName_;
}

bool DeleteOperatorNode::hasPropertyName() const {
    return hasPropertyName_;
}

bool DeleteOperatorNode::validate() const {
    if (!OperatorNode::validate()) {
        return false;
    }
    return !target_.empty();
}

std::string DeleteOperatorNode::toString() const {
    std::stringstream ss;
    ss << "delete " << targetTypeToString() << " " << target_;
    
    if (hasIndex_) {
        ss << "[" << index_ << "]";
    }
    
    if (hasSelector_) {
        ss << " " << selector_;
    }
    
    if (hasPropertyName_) {
        ss << "." << propertyName_;
    }
    
    return ss.str();
}

std::string DeleteOperatorNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "DeleteOperatorNode {" << std::endl;
    ss << indentStr << "  target: " << target_ << std::endl;
    ss << indentStr << "  targetType: " << targetTypeToString() << std::endl;
    
    if (hasIndex_) {
        ss << indentStr << "  index: " << index_ << std::endl;
    }
    
    if (hasSelector_) {
        ss << indentStr << "  selector: " << selector_ << std::endl;
    }
    
    if (hasPropertyName_) {
        ss << indentStr << "  propertyName: " << propertyName_ << std::endl;
    }
    
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> DeleteOperatorNode::clone() const {
    auto cloned = std::make_shared<DeleteOperatorNode>(getPosition());
    cloned->target_ = target_;
    cloned->targetType_ = targetType_;
    cloned->index_ = index_;
    cloned->hasIndex_ = hasIndex_;
    cloned->selector_ = selector_;
    cloned->hasSelector_ = hasSelector_;
    cloned->propertyName_ = propertyName_;
    cloned->hasPropertyName_ = hasPropertyName_;
    return cloned;
}

void DeleteOperatorNode::accept(NodeVisitor& visitor) {
    (void)visitor; // 暂时空实现
}

std::string DeleteOperatorNode::targetTypeToString() const {
    switch (targetType_) {
        case TargetType::ELEMENT:     return "@Element";
        case TargetType::STYLE:       return "@Style";
        case TargetType::VAR:         return "@Var";
        case TargetType::PROPERTY:    return "property";
        case TargetType::CHILD:       return "child";
        case TargetType::INHERITANCE: return "inheritance";
        default:                      return "unknown";
    }
}

// FromOperatorNode实现
FromOperatorNode::FromOperatorNode(const NodePosition& position)
    : OperatorNode(OperatorType::FROM, NodeType::FROM_OPERATOR, position), 
      sourceType_(SourceType::FILE), wildcardImport_(false) {
}

void FromOperatorNode::setSource(const std::string& source) {
    source_ = source;
}

std::string FromOperatorNode::getSource() const {
    return source_;
}

void FromOperatorNode::setSourceType(SourceType type) {
    sourceType_ = type;
}

FromOperatorNode::SourceType FromOperatorNode::getSourceType() const {
    return sourceType_;
}

void FromOperatorNode::addImportItem(const std::string& item) {
    if (std::find(importItems_.begin(), importItems_.end(), item) == importItems_.end()) {
        importItems_.push_back(item);
    }
}

void FromOperatorNode::removeImportItem(const std::string& item) {
    importItems_.erase(std::remove(importItems_.begin(), importItems_.end(), item), importItems_.end());
}

std::vector<std::string> FromOperatorNode::getImportItems() const {
    return importItems_;
}

bool FromOperatorNode::hasImportItems() const {
    return !importItems_.empty();
}

void FromOperatorNode::setWildcardImport(bool wildcard) {
    wildcardImport_ = wildcard;
}

bool FromOperatorNode::isWildcardImport() const {
    return wildcardImport_;
}

bool FromOperatorNode::validate() const {
    if (!OperatorNode::validate()) {
        return false;
    }
    return !source_.empty();
}

std::string FromOperatorNode::toString() const {
    std::stringstream ss;
    
    if (!importItems_.empty()) {
        for (size_t i = 0; i < importItems_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << importItems_[i];
        }
        ss << " ";
    } else if (wildcardImport_) {
        ss << "* ";
    }
    
    ss << "from " << source_;
    return ss.str();
}

std::string FromOperatorNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "FromOperatorNode {" << std::endl;
    ss << indentStr << "  source: " << source_ << std::endl;
    ss << indentStr << "  sourceType: " << sourceTypeToString() << std::endl;
    ss << indentStr << "  wildcardImport: " << (wildcardImport_ ? "true" : "false") << std::endl;
    
    if (!importItems_.empty()) {
        ss << indentStr << "  importItems: [";
        for (size_t i = 0; i < importItems_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << importItems_[i];
        }
        ss << "]" << std::endl;
    }
    
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> FromOperatorNode::clone() const {
    auto cloned = std::make_shared<FromOperatorNode>(getPosition());
    cloned->source_ = source_;
    cloned->sourceType_ = sourceType_;
    cloned->importItems_ = importItems_;
    cloned->wildcardImport_ = wildcardImport_;
    return cloned;
}

void FromOperatorNode::accept(NodeVisitor& visitor) {
    (void)visitor; // 暂时空实现
}

std::string FromOperatorNode::sourceTypeToString() const {
    switch (sourceType_) {
        case SourceType::FILE:    return "file";
        case SourceType::MODULE:  return "module";
        case SourceType::URL:     return "url";
        case SourceType::LIBRARY: return "library";
        default:                  return "unknown";
    }
}

// AsOperatorNode实现
AsOperatorNode::AsOperatorNode(const NodePosition& position)
    : OperatorNode(OperatorType::AS, NodeType::AS_OPERATOR, position), hasScope_(false) {
}

void AsOperatorNode::setOriginalName(const std::string& originalName) {
    originalName_ = originalName;
}

std::string AsOperatorNode::getOriginalName() const {
    return originalName_;
}

void AsOperatorNode::setAlias(const std::string& alias) {
    alias_ = alias;
}

std::string AsOperatorNode::getAlias() const {
    return alias_;
}

void AsOperatorNode::setScope(const std::string& scope) {
    scope_ = scope;
    hasScope_ = !scope.empty();
}

std::string AsOperatorNode::getScope() const {
    return scope_;
}

bool AsOperatorNode::hasScope() const {
    return hasScope_;
}

bool AsOperatorNode::validate() const {
    if (!OperatorNode::validate()) {
        return false;
    }
    return !alias_.empty();
}

std::string AsOperatorNode::toString() const {
    std::stringstream ss;
    if (!originalName_.empty()) {
        ss << originalName_ << " ";
    }
    ss << "as " << alias_;
    
    if (hasScope_) {
        ss << " in " << scope_;
    }
    
    return ss.str();
}

std::string AsOperatorNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "AsOperatorNode {" << std::endl;
    ss << indentStr << "  alias: " << alias_ << std::endl;
    
    if (!originalName_.empty()) {
        ss << indentStr << "  originalName: " << originalName_ << std::endl;
    }
    
    if (hasScope_) {
        ss << indentStr << "  scope: " << scope_ << std::endl;
    }
    
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> AsOperatorNode::clone() const {
    auto cloned = std::make_shared<AsOperatorNode>(getPosition());
    cloned->originalName_ = originalName_;
    cloned->alias_ = alias_;
    cloned->scope_ = scope_;
    cloned->hasScope_ = hasScope_;
    return cloned;
}

void AsOperatorNode::accept(NodeVisitor& visitor) {
    (void)visitor; // 暂时空实现
}

// InheritOperatorNode实现
InheritOperatorNode::InheritOperatorNode(const NodePosition& position)
    : OperatorNode(OperatorType::INHERIT, NodeType::INHERIT_OPERATOR, position), 
      inheritType_(InheritType::ELEMENT), inheritMode_(InheritMode::DIRECT) {
}

void InheritOperatorNode::setTarget(const std::string& target) {
    target_ = target;
}

std::string InheritOperatorNode::getTarget() const {
    return target_;
}

void InheritOperatorNode::setInheritType(InheritType type) {
    inheritType_ = type;
}

InheritOperatorNode::InheritType InheritOperatorNode::getInheritType() const {
    return inheritType_;
}

void InheritOperatorNode::setInheritMode(InheritMode mode) {
    inheritMode_ = mode;
}

InheritOperatorNode::InheritMode InheritOperatorNode::getInheritMode() const {
    return inheritMode_;
}

void InheritOperatorNode::addCondition(const std::string& condition) {
    if (std::find(conditions_.begin(), conditions_.end(), condition) == conditions_.end()) {
        conditions_.push_back(condition);
    }
}

void InheritOperatorNode::removeCondition(const std::string& condition) {
    conditions_.erase(std::remove(conditions_.begin(), conditions_.end(), condition), conditions_.end());
}

std::vector<std::string> InheritOperatorNode::getConditions() const {
    return conditions_;
}

bool InheritOperatorNode::validate() const {
    if (!OperatorNode::validate()) {
        return false;
    }
    return !target_.empty();
}

std::string InheritOperatorNode::toString() const {
    std::stringstream ss;
    ss << "inherit " << inheritTypeToString() << " " << target_;
    
    if (inheritMode_ != InheritMode::DIRECT) {
        ss << " (" << inheritModeToString() << ")";
    }
    
    if (!conditions_.empty()) {
        ss << " where ";
        for (size_t i = 0; i < conditions_.size(); ++i) {
            if (i > 0) ss << " and ";
            ss << conditions_[i];
        }
    }
    
    return ss.str();
}

std::string InheritOperatorNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "InheritOperatorNode {" << std::endl;
    ss << indentStr << "  target: " << target_ << std::endl;
    ss << indentStr << "  inheritType: " << inheritTypeToString() << std::endl;
    ss << indentStr << "  inheritMode: " << inheritModeToString() << std::endl;
    
    if (!conditions_.empty()) {
        ss << indentStr << "  conditions: [";
        for (size_t i = 0; i < conditions_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << conditions_[i];
        }
        ss << "]" << std::endl;
    }
    
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> InheritOperatorNode::clone() const {
    auto cloned = std::make_shared<InheritOperatorNode>(getPosition());
    cloned->target_ = target_;
    cloned->inheritType_ = inheritType_;
    cloned->inheritMode_ = inheritMode_;
    cloned->conditions_ = conditions_;
    return cloned;
}

void InheritOperatorNode::accept(NodeVisitor& visitor) {
    (void)visitor; // 暂时空实现
}

std::string InheritOperatorNode::inheritTypeToString() const {
    switch (inheritType_) {
        case InheritType::ELEMENT:  return "@Element";
        case InheritType::STYLE:    return "@Style";
        case InheritType::VAR:      return "@Var";
        case InheritType::TEMPLATE: return "@Template";
        case InheritType::CUSTOM:   return "@Custom";
        default:                    return "unknown";
    }
}

std::string InheritOperatorNode::inheritModeToString() const {
    switch (inheritMode_) {
        case InheritMode::DIRECT:   return "direct";
        case InheritMode::OVERRIDE: return "override";
        case InheritMode::MERGE:    return "merge";
        case InheritMode::EXTEND:   return "extend";
        default:                    return "unknown";
    }
}

} // namespace chtl