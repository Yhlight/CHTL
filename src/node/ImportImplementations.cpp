#include "Import.h"
#include "Operator.h"
#include <sstream>
#include <algorithm>
#include <algorithm>

namespace chtl {

// ImportNode实现
ImportNode::ImportNode(const NodePosition& position)
    : Node(NodeType::IMPORT, position), importType_(ImportType::CHTL) {
}

ImportNode::ImportType ImportNode::getImportType() const {
    return importType_;
}

void ImportNode::setImportType(ImportType type) {
    importType_ = type;
}

const std::string& ImportNode::getImportItem() const {
    return importItem_;
}

void ImportNode::setImportItem(const std::string& item) {
    importItem_ = item;
}

void ImportNode::setFromOperator(std::shared_ptr<FromOperatorNode> fromOp) {
    fromOperator_ = fromOp;
    if (fromOp) {
        addChild(fromOp);
    }
}

std::shared_ptr<FromOperatorNode> ImportNode::getFromOperator() const {
    return fromOperator_;
}

bool ImportNode::hasFromOperator() const {
    return fromOperator_ != nullptr;
}

void ImportNode::setAsOperator(std::shared_ptr<AsOperatorNode> asOp) {
    asOperator_ = asOp;
    if (asOp) {
        addChild(asOp);
    }
}

std::shared_ptr<AsOperatorNode> ImportNode::getAsOperator() const {
    return asOperator_;
}

bool ImportNode::hasAsOperator() const {
    return asOperator_ != nullptr;
}

const std::vector<std::string>& ImportNode::getImportItems() const {
    return importItems_;
}

void ImportNode::addImportItem(const std::string& item) {
    if (std::find(importItems_.begin(), importItems_.end(), item) == importItems_.end()) {
        importItems_.push_back(item);
    }
}

void ImportNode::removeImportItem(const std::string& item) {
    importItems_.erase(std::remove(importItems_.begin(), importItems_.end(), item), importItems_.end());
}

void ImportNode::clearImportItems() {
    importItems_.clear();
}

bool ImportNode::hasImportItem(const std::string& item) const {
    return std::find(importItems_.begin(), importItems_.end(), item) != importItems_.end();
}

void ImportNode::addImportItems(const std::vector<std::string>& items) {
    for (const auto& item : items) {
        addImportItem(item);
    }
}

bool ImportNode::isWildcardImport() const {
    return std::find(importItems_.begin(), importItems_.end(), "*") != importItems_.end();
}

bool ImportNode::isSelectiveImport() const {
    return !importItems_.empty() && !isWildcardImport();
}

std::string ImportNode::getSourceFile() const {
    return fromOperator_ ? fromOperator_->getSource() : "";
}

std::string ImportNode::getAlias() const {
    return asOperator_ ? asOperator_->getAlias() : "";
}

bool ImportNode::hasAlias() const {
    return asOperator_ && asOperator_->hasAlias();
}

void ImportNode::setWildcardImport(bool wildcard) {
    if (wildcard) {
        importItems_.clear();
        importItems_.push_back("*");
    } else {
        importItems_.erase(std::remove(importItems_.begin(), importItems_.end(), "*"), importItems_.end());
    }
}

bool ImportNode::validate() const {
    return Node::validate();
}

std::string ImportNode::toString() const {
    std::stringstream ss;
    ss << "[Import] ";
    
    // 导入类型
    switch (importType_) {
        case ImportType::HTML: ss << "@Html"; break;
        case ImportType::STYLE: ss << "@Style"; break;
        case ImportType::JAVASCRIPT: ss << "@JavaScript"; break;
        case ImportType::CUSTOM_ELEMENT: ss << "[Custom] @Element"; break;
        case ImportType::CUSTOM_STYLE: ss << "[Custom] @Style"; break;
        case ImportType::CUSTOM_VAR: ss << "[Custom] @Var"; break;
        case ImportType::TEMPLATE_ELEMENT: ss << "[Template] @Element"; break;
        case ImportType::TEMPLATE_STYLE: ss << "[Template] @Style"; break;
        case ImportType::TEMPLATE_VAR: ss << "[Template] @Var"; break;
        case ImportType::CHTL: break; // 不输出类型
    }
    
    // 具体导入项
    if (!importItem_.empty()) {
        ss << " " << importItem_;
    }
    
    // from操作符
    if (hasFromOperator()) {
        ss << " from " << fromOperator_->getSource();
    }
    
    // as操作符
    if (hasAsOperator()) {
        ss << " as " << asOperator_->getAlias();
    }
    
    return ss.str();
}

std::string ImportNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    std::stringstream ss;
    
    ss << indentStr << "ImportNode: " << importTypeToString();
    
    if (!importItem_.empty()) {
        ss << " (" << importItem_ << ")";
    }
    
    if (hasFromOperator()) {
        ss << " from " << fromOperator_->getSource();
    }
    
    if (hasAsOperator()) {
        ss << " as " << asOperator_->getAlias();
    }
    
    if (!importItems_.empty()) {
        ss << " [" << importItems_.size() << " items]";
    }
    
    return ss.str();
}

std::shared_ptr<Node> ImportNode::clone() const {
    auto cloned = std::make_shared<ImportNode>(getPosition());
    cloned->importType_ = importType_;
    cloned->importItem_ = importItem_;
    cloned->importItems_ = importItems_;
    
    if (fromOperator_) {
        cloned->setFromOperator(std::dynamic_pointer_cast<FromOperatorNode>(fromOperator_->clone()));
    }
    
    if (asOperator_) {
        cloned->setAsOperator(std::dynamic_pointer_cast<AsOperatorNode>(asOperator_->clone()));
    }
    
    return cloned;
}

void ImportNode::accept(NodeVisitor& visitor) {
    // 访问者模式实现
}

std::string ImportNode::importTypeToString() const {
    switch (importType_) {
        case ImportType::HTML: return "HTML";
        case ImportType::STYLE: return "STYLE";
        case ImportType::JAVASCRIPT: return "JAVASCRIPT";
        case ImportType::CUSTOM_ELEMENT: return "CUSTOM_ELEMENT";
        case ImportType::CUSTOM_STYLE: return "CUSTOM_STYLE";
        case ImportType::CUSTOM_VAR: return "CUSTOM_VAR";
        case ImportType::TEMPLATE_ELEMENT: return "TEMPLATE_ELEMENT";
        case ImportType::TEMPLATE_STYLE: return "TEMPLATE_STYLE";
        case ImportType::TEMPLATE_VAR: return "TEMPLATE_VAR";
        case ImportType::CHTL: return "CHTL";
    }
    return "UNKNOWN";
}

// 基础的ImportDeclarationNode实现
ImportDeclarationNode::ImportDeclarationNode(const NodePosition& position)
    : Node(NodeType::IMPORT_DECLARATION, position) {
}

bool ImportDeclarationNode::validate() const {
    return Node::validate();
}

std::string ImportDeclarationNode::toString() const {
    return "[Import Declaration]";
}

std::string ImportDeclarationNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    return indentStr + "ImportDeclarationNode";
}

std::shared_ptr<Node> ImportDeclarationNode::clone() const {
    return std::make_shared<ImportDeclarationNode>(getPosition());
}

void ImportDeclarationNode::accept(NodeVisitor& visitor) {
    // 访问者模式实现
}

} // namespace chtl