#include "Namespace.h"
#include <sstream>
#include <algorithm>

namespace chtl {

// NamespaceNode实现
NamespaceNode::NamespaceNode(const NodePosition& position)
    : Node(NodeType::NAMESPACE, position) {
}

const std::string& NamespaceNode::getName() const {
    return name_;
}

void NamespaceNode::setName(const std::string& name) {
    name_ = name;
}

bool NamespaceNode::hasName() const {
    return !name_.empty();
}

void NamespaceNode::clearName() {
    name_.clear();
}

void NamespaceNode::addMember(const std::string& name, std::shared_ptr<Node> member) {
    members_[name] = member;
}

void NamespaceNode::removeMember(const std::string& name) {
    members_.erase(name);
}

std::shared_ptr<Node> NamespaceNode::findMember(const std::string& name) const {
    auto it = members_.find(name);
    return it != members_.end() ? it->second : nullptr;
}

std::unordered_map<std::string, std::shared_ptr<Node>> NamespaceNode::getAllMembers() const {
    return members_;
}

bool NamespaceNode::hasMember(const std::string& name) const {
    return members_.find(name) != members_.end();
}

size_t NamespaceNode::getMemberCount() const {
    return members_.size();
}

void NamespaceNode::addAlias(const std::string& name, const std::string& alias) {
    aliases_[name] = alias;
}

void NamespaceNode::removeAlias(const std::string& name) {
    aliases_.erase(name);
}

std::string NamespaceNode::getAlias(const std::string& name) const {
    auto it = aliases_.find(name);
    return it != aliases_.end() ? it->second : "";
}

bool NamespaceNode::hasAlias(const std::string& name) const {
    return aliases_.find(name) != aliases_.end();
}

const std::unordered_map<std::string, std::string>& NamespaceNode::getAllAliases() const {
    return aliases_;
}

void NamespaceNode::clearAliases() {
    aliases_.clear();
}

void NamespaceNode::setDescription(const std::string& description) {
    description_ = description;
}

std::string NamespaceNode::getDescription() const {
    return description_;
}

bool NamespaceNode::hasDescription() const {
    return !description_.empty();
}

void NamespaceNode::addChildNamespace(std::shared_ptr<NamespaceNode> childNs) {
    if (childNs && std::find(childNamespaces_.begin(), childNamespaces_.end(), childNs) == childNamespaces_.end()) {
        childNamespaces_.push_back(childNs);
        addChild(childNs);
    }
}

void NamespaceNode::removeChildNamespace(std::shared_ptr<NamespaceNode> childNs) {
    auto it = std::find(childNamespaces_.begin(), childNamespaces_.end(), childNs);
    if (it != childNamespaces_.end()) {
        childNamespaces_.erase(it);
        removeChild(childNs);
    }
}

const std::vector<std::shared_ptr<NamespaceNode>>& NamespaceNode::getChildNamespaces() const {
    return childNamespaces_;
}

std::shared_ptr<NamespaceNode> NamespaceNode::findChildNamespace(const std::string& name) const {
    for (const auto& child : childNamespaces_) {
        if (child->getName() == name) {
            return child;
        }
    }
    return nullptr;
}

void NamespaceNode::setParentNamespace(std::shared_ptr<NamespaceNode> parentNs) {
    parentNamespace_ = parentNs;
}

std::shared_ptr<NamespaceNode> NamespaceNode::getParentNamespace() const {
    return parentNamespace_.lock();
}

bool NamespaceNode::hasParentNamespace() const {
    return !parentNamespace_.expired();
}

std::string NamespaceNode::getFullyQualifiedName() const {
    std::vector<std::string> nameParts;
    
    // 收集从根到当前命名空间的所有名称
    auto current = shared_from_this();
    while (current) {
        if (!current->getName().empty()) {
            nameParts.insert(nameParts.begin(), current->getName());
        }
        current = std::static_pointer_cast<NamespaceNode>(current->getParentNamespace());
    }
    
    // 连接名称
    std::stringstream ss;
    for (size_t i = 0; i < nameParts.size(); ++i) {
        if (i > 0) ss << "::";
        ss << nameParts[i];
    }
    
    return ss.str();
}

std::string NamespaceNode::resolveMemberName(const std::string& memberName) const {
    // 检查是否有别名
    std::string alias = getAlias(memberName);
    if (!alias.empty()) {
        return alias;
    }
    
    // 检查是否为直接成员
    if (hasMember(memberName)) {
        return getFullyQualifiedName() + "::" + memberName;
    }
    
    // 检查子命名空间
    for (const auto& child : childNamespaces_) {
        if (child->hasMember(memberName)) {
            return child->getFullyQualifiedName() + "::" + memberName;
        }
    }
    
    return memberName; // 未找到，返回原名称
}

bool NamespaceNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    // 命名空间必须有名称
    if (name_.empty()) {
        return false;
    }
    
    // 验证子命名空间
    for (const auto& child : childNamespaces_) {
        if (!child->validate()) {
            return false;
        }
    }
    
    return true;
}

std::string NamespaceNode::toString() const {
    std::stringstream ss;
    ss << "[Namespace] " << name_;
    
    if (!members_.empty()) {
        ss << " {";
        for (size_t i = 0; i < members_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << members_[i];
        }
        ss << "}";
    }
    
    return ss.str();
}

std::string NamespaceNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "NamespaceNode {" << std::endl;
    ss << indentStr << "  name: " << name_ << std::endl;
    ss << indentStr << "  fullyQualifiedName: " << getFullyQualifiedName() << std::endl;
    
    if (!members_.empty()) {
        ss << indentStr << "  members: [";
        for (size_t i = 0; i < members_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << members_[i];
        }
        ss << "]" << std::endl;
    }
    
    if (!aliases_.empty()) {
        ss << indentStr << "  aliases: {" << std::endl;
        for (const auto& pair : aliases_) {
            ss << indentStr << "    " << pair.first << ": " << pair.second << std::endl;
        }
        ss << indentStr << "  }" << std::endl;
    }
    
    if (!description_.empty()) {
        ss << indentStr << "  description: " << description_ << std::endl;
    }
    
    if (!childNamespaces_.empty()) {
        ss << indentStr << "  childNamespaces: " << childNamespaces_.size() << std::endl;
    }
    
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> NamespaceNode::clone() const {
    auto cloned = std::make_shared<NamespaceNode>(namespaceName_, getPosition());
    cloned->fullPath_ = fullPath_;
    cloned->parentNamespace_ = parentNamespace_;
    cloned->members_ = members_;
    cloned->typeAliases_ = typeAliases_;
    cloned->usings_ = usings_;
    cloned->accessLevel_ = accessLevel_;
    
    // 克隆子命名空间
    for (const auto& child : childNamespaces_) {
        auto clonedChild = std::static_pointer_cast<NamespaceNode>(child->clone());
        cloned->addChildNamespace(clonedChild);
    }
    
    return cloned;
}

void NamespaceNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// NamespaceResolverNode实现
NamespaceResolverNode::NamespaceResolverNode(const NodePosition& position)
    : Node(NodeType::NAMESPACE_RESOLVER, position) {
}

void NamespaceResolverNode::setNamespacePath(const std::string& path) {
    namespacePath_ = path;
    // 解析路径为组件
    parseNamespacePath(path);
}

std::string NamespaceResolverNode::getNamespacePath() const {
    return namespacePath_;
}

const std::vector<std::string>& NamespaceResolverNode::getPathComponents() const {
    return pathComponents_;
}

void NamespaceResolverNode::setMemberName(const std::string& memberName) {
    memberName_ = memberName;
}

std::string NamespaceResolverNode::getMemberName() const {
    return memberName_;
}

bool NamespaceResolverNode::hasMemberName() const {
    return !memberName_.empty();
}

void NamespaceResolverNode::setResolvedNamespace(std::shared_ptr<NamespaceNode> ns) {
    resolvedNamespace_ = ns;
}

std::shared_ptr<NamespaceNode> NamespaceResolverNode::getResolvedNamespace() const {
    return resolvedNamespace_.lock();
}

bool NamespaceResolverNode::hasResolvedNamespace() const {
    return !resolvedNamespace_.expired();
}

std::string NamespaceResolverNode::getResolvedMemberName() const {
    auto ns = getResolvedNamespace();
    if (ns && !memberName_.empty()) {
        return ns->resolveMemberName(memberName_);
    }
    return memberName_;
}

bool NamespaceResolverNode::isResolved() const {
    return !resolvedNamespace_.expired();
}

void NamespaceResolverNode::parseNamespacePath(const std::string& path) {
    pathComponents_.clear();
    
    std::stringstream ss(path);
    std::string component;
    
    while (std::getline(ss, component, ':')) {
        if (!component.empty() && component != ":") {
            pathComponents_.push_back(component);
        }
    }
}

bool NamespaceResolverNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    // 必须有命名空间路径
    return !namespacePath_.empty();
}

std::string NamespaceResolverNode::toString() const {
    std::stringstream ss;
    ss << namespacePath_;
    if (!memberName_.empty()) {
        ss << "::" << memberName_;
    }
    return ss.str();
}

std::string NamespaceResolverNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "NamespaceResolverNode {" << std::endl;
    ss << indentStr << "  namespacePath: " << namespacePath_ << std::endl;
    
    if (!pathComponents_.empty()) {
        ss << indentStr << "  pathComponents: [";
        for (size_t i = 0; i < pathComponents_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << pathComponents_[i];
        }
        ss << "]" << std::endl;
    }
    
    if (!memberName_.empty()) {
        ss << indentStr << "  memberName: " << memberName_ << std::endl;
    }
    
    ss << indentStr << "  isResolved: " << (isResolved() ? "true" : "false") << std::endl;
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> NamespaceResolverNode::clone() const {
    auto cloned = std::make_shared<NamespaceResolverNode>(getPosition());
    cloned->copyBaseProperties(*this);
    cloned->namespacePath_ = namespacePath_;
    cloned->pathComponents_ = pathComponents_;
    cloned->memberName_ = memberName_;
    return cloned;
}

void NamespaceResolverNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// UsingNode实现
UsingNode::UsingNode(const NodePosition& position)
    : Node(NodeType::USING, position) {
}

void UsingNode::setNamespace(std::shared_ptr<NamespaceNode> ns) {
    namespace_ = ns;
    if (ns) {
        addChild(ns);
    }
}

std::shared_ptr<NamespaceNode> UsingNode::getNamespace() const {
    return namespace_.lock();
}

bool UsingNode::hasNamespace() const {
    return !namespace_.expired();
}

void UsingNode::setAlias(const std::string& alias) {
    alias_ = alias;
}

std::string UsingNode::getAlias() const {
    return alias_;
}

bool UsingNode::hasAlias() const {
    return !alias_.empty();
}

void UsingNode::clearAlias() {
    alias_.clear();
}

void UsingNode::addUsingItem(const std::string& item) {
    if (std::find(usingItems_.begin(), usingItems_.end(), item) == usingItems_.end()) {
        usingItems_.push_back(item);
    }
}

void UsingNode::removeUsingItem(const std::string& item) {
    usingItems_.erase(std::remove(usingItems_.begin(), usingItems_.end(), item), usingItems_.end());
}

const std::vector<std::string>& UsingNode::getUsingItems() const {
    return usingItems_;
}

void UsingNode::clearUsingItems() {
    usingItems_.clear();
}

bool UsingNode::hasUsingItem(const std::string& item) const {
    return std::find(usingItems_.begin(), usingItems_.end(), item) != usingItems_.end();
}

bool UsingNode::isWildcardUsing() const {
    return std::find(usingItems_.begin(), usingItems_.end(), "*") != usingItems_.end();
}

void UsingNode::setWildcardUsing(bool wildcard) {
    if (wildcard) {
        usingItems_.clear();
        usingItems_.push_back("*");
    } else {
        usingItems_.erase(std::remove(usingItems_.begin(), usingItems_.end(), "*"), usingItems_.end());
    }
}

std::string UsingNode::resolveItem(const std::string& item) const {
    auto ns = getNamespace();
    if (!ns) {
        return item;
    }
    
    // 如果有别名，使用别名前缀
    if (!alias_.empty()) {
        return alias_ + "::" + item;
    }
    
    // 使用命名空间解析
    return ns->resolveMemberName(item);
}

bool UsingNode::validate() const {
    if (!Node::validate()) {
        return false;
    }
    
    // using语句必须有命名空间或使用项
    return !namespace_.expired() || !usingItems_.empty();
}

std::string UsingNode::toString() const {
    std::stringstream ss;
    ss << "using ";
    
    auto ns = getNamespace();
    if (ns) {
        ss << ns->getFullyQualifiedName();
    }
    
    if (!usingItems_.empty()) {
        ss << " {";
        for (size_t i = 0; i < usingItems_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << usingItems_[i];
        }
        ss << "}";
    }
    
    if (!alias_.empty()) {
        ss << " as " << alias_;
    }
    
    ss << ";";
    return ss.str();
}

std::string UsingNode::toDebugString(int indent) const {
    std::string indentStr(indent, ' ');
    std::stringstream ss;
    ss << indentStr << "UsingNode {" << std::endl;
    
    auto ns = getNamespace();
    if (ns) {
        ss << indentStr << "  namespace: " << ns->getFullyQualifiedName() << std::endl;
    }
    
    if (!usingItems_.empty()) {
        ss << indentStr << "  usingItems: [";
        for (size_t i = 0; i < usingItems_.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << usingItems_[i];
        }
        ss << "]" << std::endl;
    }
    
    if (!alias_.empty()) {
        ss << indentStr << "  alias: " << alias_ << std::endl;
    }
    
    ss << indentStr << "  isWildcard: " << (isWildcardUsing() ? "true" : "false") << std::endl;
    ss << indentStr << "  position: " << getPosition().toString() << std::endl;
    ss << indentStr << "}";
    return ss.str();
}

std::shared_ptr<Node> UsingNode::clone() const {
    auto cloned = std::make_shared<UsingNode>(getPosition());
    cloned->copyBaseProperties(*this);
    cloned->alias_ = alias_;
    cloned->usingItems_ = usingItems_;
    
    // 注意：不克隆namespace_，因为它应该指向同一个命名空间
    return cloned;
}

void UsingNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// NamespaceManager实现
NamespaceManager::NamespaceManager() {
    // 创建全局命名空间
    globalNamespace_ = std::make_shared<NamespaceNode>("");
    globalNamespace_->setNamespaceName("global");
    currentNamespace_ = globalNamespace_;
}

std::shared_ptr<NamespaceNode> NamespaceManager::getGlobalNamespace() const {
    return globalNamespace_;
}

std::shared_ptr<NamespaceNode> NamespaceManager::getCurrentNamespace() const {
    return currentNamespace_;
}

void NamespaceManager::enterNamespace(const std::string& name) {
    auto child = currentNamespace_->findChildNamespace(name);
    if (!child) {
        child = std::make_shared<NamespaceNode>(name);
        child->setNamespaceName(name);
        child->setParentNamespace(currentNamespace_);
        currentNamespace_->addChildNamespace(child);
    }
    currentNamespace_ = child;
}

void NamespaceManager::exitNamespace() {
    auto parent = currentNamespace_->getParentNamespace();
    if (parent) {
        currentNamespace_ = parent;
    }
}

void NamespaceManager::addMemberToCurrentNamespace(const std::string& member) {
    currentNamespace_->addMember(member);
}

std::shared_ptr<NamespaceNode> NamespaceManager::findNamespace(const std::string& path) const {
    std::stringstream ss(path);
    std::string component;
    auto current = globalNamespace_;
    
    while (std::getline(ss, component, ':')) {
        if (!component.empty() && component != ":") {
            current = current->findChildNamespace(component);
            if (!current) {
                return nullptr;
            }
        }
    }
    
    return current;
}

std::string NamespaceManager::resolveMember(const std::string& memberName) const {
    return currentNamespace_->resolveMemberName(memberName);
}

void NamespaceManager::addUsingDirective(std::shared_ptr<UsingNode> usingNode) {
    if (usingNode) {
        usingDirectives_.push_back(usingNode);
    }
}

const std::vector<std::shared_ptr<UsingNode>>& NamespaceManager::getUsingDirectives() const {
    return usingDirectives_;
}

void NamespaceManager::clearUsingDirectives() {
    usingDirectives_.clear();
}

} // namespace chtl