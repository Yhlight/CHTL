#include "Operator.h"
#include <sstream>
#include <algorithm>

namespace chtl {

// OperatorNode实现
OperatorNode::OperatorNode(OperatorType type, const NodePosition& position)
    : Node(NodeType::OPERATOR, position), operatorType_(type) {
}

OperatorNode::OperatorType OperatorNode::getOperatorType() const {
    return operatorType_;
}

void OperatorNode::setOperatorType(OperatorType type) {
    operatorType_ = type;
}

std::string OperatorNode::getOperatorName() const {
    return operatorTypeToString();
}

bool OperatorNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    // 验证操作符类型是否有效
    switch (operatorType_) {
        case OperatorType::ADD:
        case OperatorType::DELETE:
        case OperatorType::FROM:
        case OperatorType::AS:
        case OperatorType::INHERIT:
            return true;
        default:
            return false;
    }
}

std::string OperatorNode::toString() const {
    return getOperatorName();
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
    auto cloned = std::make_shared<OperatorNode>(operatorType_, getPosition());
    cloned->copyBaseProperties(*this);
    return cloned;
}

void OperatorNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

std::string OperatorNode::operatorTypeToString() const {
    switch (operatorType_) {
        case OperatorType::ADD:    return "add";
        case OperatorType::DELETE: return "delete";
        case OperatorType::FROM:   return "from";
        case OperatorType::AS:     return "as";
        case OperatorType::INHERIT:return "inherit";
        default:                   return "unknown";
    }
}

// AddOperatorNode实现
AddOperatorNode::AddOperatorNode(const NodePosition& position)
    : OperatorNode(OperatorType::ADD, position) {
    setNodeType(NodeType::ADD_OPERATOR);
}

void AddOperatorNode::addTarget(const std::string& target) {
    if (std::find(targets_.begin(), targets_.end(), target) == targets_.end()) {
        targets_.push_back(target);
    }
}

void AddOperatorNode::removeTarget(const std::string& target) {
    targets_.erase(std::remove(targets_.begin(), targets_.end(), target), targets_.end());
}

const std::vector<std::string>& AddOperatorNode::getTargets() const {
    return targets_;
}

void AddOperatorNode::clearTargets() {
    targets_.clear();
}

bool AddOperatorNode::hasTarget(const std::string& target) const {
    return std::find(targets_.begin(), targets_.end(), target) != targets_.end();
}

void AddOperatorNode::setTargetValue(const std::string& target, const std::string& value) {
    targetValues_[target] = value;
}

std::string AddOperatorNode::getTargetValue(const std::string& target) const {
    auto it = targetValues_.find(target);
    return it != targetValues_.end() ? it->second : "";
}

void AddOperatorNode::removeTargetValue(const std::string& target) {
    targetValues_.erase(target);
}

bool AddOperatorNode::hasTargetValue(const std::string& target) const {
    return targetValues_.find(target) != targetValues_.end();
}

bool AddOperatorNode::validate() const {
    if (!OperatorNode::validate()) {
        return false;
    }
    
    // add操作符至少需要一个目标
    return !targets_.empty();
}

std::string AddOperatorNode::toString() const {
    std::stringstream ss;
    ss << "add ";
    for (size_t i = 0; i < targets_.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << targets_[i];
        
        // 如果有值，添加值
        auto it = targetValues_.find(targets_[i]);
        if (it != targetValues_.end()) {
            ss << ": " << it->second;
        }
    }
    return ss.str();
}

std::string AddOperatorNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "AddOperatorNode {" << std::endl;
    ss << indentStr << "  targets: [";
    for (size_t i = 0; i < targets_.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << targets_[i];
    }
    ss << "]" << std::endl;
    
    if (!targetValues_.empty()) {
        ss << indentStr << "  values: {" << std::endl;
        for (const auto& pair : targetValues_) {
            ss << indentStr << "    " << pair.first << ": " << pair.second << std::endl;
        }
        ss << indentStr << "  }" << std::endl;
    }
    
    ss << indentStr << "  position: [" << getPosition().line << ":" << getPosition().column << "]" << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> AddOperatorNode::clone() const {
    auto cloned = std::make_shared<AddOperatorNode>(getPosition());
    cloned->targets_ = targets_;
    cloned->targetValues_ = targetValues_;
    return cloned;
}

void AddOperatorNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// DeleteOperatorNode实现
DeleteOperatorNode::DeleteOperatorNode(const NodePosition& position)
    : OperatorNode(OperatorType::DELETE, position) {
    setNodeType(NodeType::DELETE_OPERATOR);
}

void DeleteOperatorNode::addTarget(const std::string& target) {
    if (std::find(targets_.begin(), targets_.end(), target) == targets_.end()) {
        targets_.push_back(target);
    }
}

void DeleteOperatorNode::removeTarget(const std::string& target) {
    targets_.erase(std::remove(targets_.begin(), targets_.end(), target), targets_.end());
}

const std::vector<std::string>& DeleteOperatorNode::getTargets() const {
    return targets_;
}

void DeleteOperatorNode::clearTargets() {
    targets_.clear();
}

bool DeleteOperatorNode::hasTarget(const std::string& target) const {
    return std::find(targets_.begin(), targets_.end(), target) != targets_.end();
}

void DeleteOperatorNode::setCondition(const std::string& condition) {
    condition_ = condition;
}

std::string DeleteOperatorNode::getCondition() const {
    return condition_;
}

bool DeleteOperatorNode::hasCondition() const {
    return !condition_.empty();
}

void DeleteOperatorNode::clearCondition() {
    condition_.clear();
}

bool DeleteOperatorNode::validate() const {
    if (!OperatorNode::validate()) {
        return false;
    }
    
    // delete操作符至少需要一个目标
    return !targets_.empty();
}

std::string DeleteOperatorNode::toString() const {
    std::stringstream ss;
    ss << "delete ";
    for (size_t i = 0; i < targets_.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << targets_[i];
    }
    
    if (!condition_.empty()) {
        ss << " where " << condition_;
    }
    
    return ss.str();
}

std::string DeleteOperatorNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "DeleteOperatorNode {" << std::endl;
    ss << indentStr << "  targets: [";
    for (size_t i = 0; i < targets_.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << targets_[i];
    }
    ss << "]" << std::endl;
    
    if (!condition_.empty()) {
        ss << indentStr << "  condition: " << condition_ << std::endl;
    }
    
    ss << indentStr << "  position: [" << getPosition().line << ":" << getPosition().column << "]" << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> DeleteOperatorNode::clone() const {
    auto cloned = std::make_shared<DeleteOperatorNode>(getPosition());
    cloned->targets_ = targets_;
    cloned->condition_ = condition_;
    return cloned;
}

void DeleteOperatorNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// FromOperatorNode实现
FromOperatorNode::FromOperatorNode(const NodePosition& position)
    : OperatorNode(OperatorType::FROM, position) {
    setNodeType(NodeType::FROM_OPERATOR);
}

void FromOperatorNode::setSource(const std::string& source) {
    source_ = source;
}

std::string FromOperatorNode::getSource() const {
    return source_;
}

bool FromOperatorNode::hasSource() const {
    return !source_.empty();
}

void FromOperatorNode::clearSource() {
    source_.clear();
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

const std::vector<std::string>& FromOperatorNode::getImportItems() const {
    return importItems_;
}

void FromOperatorNode::clearImportItems() {
    importItems_.clear();
}

bool FromOperatorNode::hasImportItem(const std::string& item) const {
    return std::find(importItems_.begin(), importItems_.end(), item) != importItems_.end();
}

bool FromOperatorNode::validate() const {
    if (!OperatorNode::validate()) {
        return false;
    }
    
    // from操作符需要有源
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
    }
    ss << "from " << source_;
    return ss.str();
}

std::string FromOperatorNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "FromOperatorNode {" << std::endl;
    ss << indentStr << "  source: " << source_ << std::endl;
    ss << indentStr << "  sourceType: " << static_cast<int>(sourceType_) << std::endl;
    
    if (!importItems_.empty()) {
        ss << indentStr << "  importItems: [";
        for (size_t i = 0; i < importItems_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << importItems_[i];
        }
        ss << "]" << std::endl;
    }
    
    ss << indentStr << "  position: [" << getPosition().line << ":" << getPosition().column << "]" << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> FromOperatorNode::clone() const {
    auto cloned = std::make_shared<FromOperatorNode>(getPosition());
    cloned->source_ = source_;
    cloned->sourceType_ = sourceType_;
    cloned->importItems_ = importItems_;
    return cloned;
}

void FromOperatorNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// AsOperatorNode实现
AsOperatorNode::AsOperatorNode(const NodePosition& position)
    : OperatorNode(OperatorType::AS, position) {
    setNodeType(NodeType::AS_OPERATOR);
}

void AsOperatorNode::setAlias(const std::string& alias) {
    alias_ = alias;
}

std::string AsOperatorNode::getAlias() const {
    return alias_;
}

bool AsOperatorNode::hasAlias() const {
    return !alias_.empty();
}

void AsOperatorNode::clearAlias() {
    alias_.clear();
}

void AsOperatorNode::setOriginalName(const std::string& original) {
    originalName_ = original;
}

std::string AsOperatorNode::getOriginalName() const {
    return originalName_;
}

bool AsOperatorNode::hasOriginalName() const {
    return !originalName_.empty();
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
    
    // as操作符需要有别名
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
    
    ss << indentStr << "  position: [" << getPosition().line << ":" << getPosition().column << "]" << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> AsOperatorNode::clone() const {
    auto cloned = std::make_shared<AsOperatorNode>(getPosition());
    cloned->alias_ = alias_;
    cloned->originalName_ = originalName_;
    cloned->scope_ = scope_;
    cloned->hasScope_ = hasScope_;
    return cloned;
}

void AsOperatorNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// InheritOperatorNode实现
InheritOperatorNode::InheritOperatorNode(const NodePosition& position)
    : OperatorNode(OperatorType::INHERIT, position), inheritType_(InheritType::ELEMENT), inheritMode_(InheritMode::DIRECT) {
    setNodeType(NodeType::INHERIT_OPERATOR);
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
    
    // inherit操作符需要有目标
    return !target_.empty();
}

std::string InheritOperatorNode::toString() const {
    std::stringstream ss;
    ss << "inherit ";
    
    // 继承类型
    switch (inheritType_) {
        case InheritType::ELEMENT:  ss << "@Element "; break;
        case InheritType::STYLE:    ss << "@Style "; break;
        case InheritType::VAR:      ss << "@Var "; break;
        case InheritType::TEMPLATE: ss << "@Template "; break;
        case InheritType::CUSTOM:   ss << "@Custom "; break;
    }
    
    ss << target_;
    
    // 继承模式
    if (inheritMode_ != InheritMode::DIRECT) {
        ss << " (" << inheritModeToString() << ")";
    }
    
    // 条件
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
    
    ss << indentStr << "  position: [" << getPosition().line << ":" << getPosition().column << "]" << std::endl;
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
    visitor.visit(*this);
}

std::string InheritOperatorNode::inheritTypeToString() const {
    switch (inheritType_) {
        case InheritType::ELEMENT:  return "Element";
        case InheritType::STYLE:    return "Style";
        case InheritType::VAR:      return "Var";
        case InheritType::TEMPLATE: return "Template";
        case InheritType::CUSTOM:   return "Custom";
        default:                    return "Unknown";
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