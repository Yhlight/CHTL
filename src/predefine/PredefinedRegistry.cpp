#include "PredefinedRegistry.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <set>

namespace chtl {

// PredefinedStyleGroup 实现
PredefinedStyleGroup::PredefinedStyleGroup(const std::string& name, PredefinedType type)
    : PredefinedItem(name, type) {}

void PredefinedStyleGroup::addProperty(const std::string& property, const std::string& value) {
    if (!property.empty() && !value.empty()) {
        properties_[property] = value;
    }
}

void PredefinedStyleGroup::addRule(const std::string& selector, 
                                  const std::unordered_map<std::string, std::string>& properties) {
    if (!selector.empty() && !properties.empty()) {
        rules_[selector] = properties;
    }
}

void PredefinedStyleGroup::addStyleGroup(const std::string& groupName) {
    if (!groupName.empty()) {
        nestedGroups_.push_back(groupName);
    }
}

void PredefinedStyleGroup::inheritFrom(const std::string& parentGroup) {
    addInheritance(parentGroup);
}

void PredefinedStyleGroup::addInheritance(const std::string& parentGroup) {
    if (!parentGroup.empty()) {
        auto it = std::find(inheritances_.begin(), inheritances_.end(), parentGroup);
        if (it == inheritances_.end()) {
            inheritances_.push_back(parentGroup);
        }
    }
}

void PredefinedStyleGroup::removeInheritance(const std::string& parentGroup) {
    auto it = std::find(inheritances_.begin(), inheritances_.end(), parentGroup);
    if (it != inheritances_.end()) {
        inheritances_.erase(it);
    }
}

void PredefinedStyleGroup::overrideProperty(const std::string& property, const std::string& value) {
    addProperty(property, value); // 简单覆盖
}

void PredefinedStyleGroup::specializeProperty(const std::string& property, const std::string& value) {
    addProperty(property, value); // 特例化处理
}

const std::unordered_map<std::string, std::string>& PredefinedStyleGroup::getProperties() const {
    return properties_;
}

const std::vector<std::string>& PredefinedStyleGroup::getInheritances() const {
    return inheritances_;
}

bool PredefinedStyleGroup::hasProperty(const std::string& property) const {
    return properties_.find(property) != properties_.end();
}

std::string PredefinedStyleGroup::getPropertyValue(const std::string& property) const {
    auto it = properties_.find(property);
    return (it != properties_.end()) ? it->second : "";
}

std::shared_ptr<Node> PredefinedStyleGroup::createInstance() const {
    auto styleBlock = std::make_shared<StyleBlockNode>(NodePosition());
    styleBlock->setName(name_);
    
    // 添加内联属性
    for (const auto& prop : properties_) {
        styleBlock->addInlineProperty(prop.first, prop.second);
    }
    
    // 添加CSS规则
    for (const auto& rule : rules_) {
        auto cssRule = std::make_shared<CssRuleNode>(NodePosition());
        
        // 创建选择器
        auto selector = std::make_shared<CssSelectorNode>(
            CssSelectorNode::SelectorType::CLASS, 
            rule.first, 
            NodePosition()
        );
        cssRule->addSelector(selector);
        
        // 添加属性
        for (const auto& prop : rule.second) {
            auto property = std::make_shared<CssPropertyNode>(prop.first, prop.second, NodePosition());
            cssRule->addProperty(property);
        }
        
        styleBlock->addChild(cssRule);
    }
    
    return styleBlock;
}

std::unique_ptr<PredefinedItem> PredefinedStyleGroup::clone() const {
    auto cloned = std::make_unique<PredefinedStyleGroup>(name_, type_);
    cloned->properties_ = properties_;
    cloned->inheritances_ = inheritances_;
    cloned->rules_ = rules_;
    cloned->nestedGroups_ = nestedGroups_;
    return cloned;
}

bool PredefinedStyleGroup::validate() const {
    // 检查基本有效性
    if (name_.empty()) return false;
    
    // 检查属性有效性
    for (const auto& prop : properties_) {
        if (prop.first.empty() || prop.second.empty()) {
            return false;
        }
    }
    
    return true;
}

std::string PredefinedStyleGroup::getDescription() const {
    std::ostringstream oss;
    oss << "StyleGroup '" << name_ << "' (" << PredefinedRegistry::getInstance().getTypeString(type_) << ")";
    oss << " with " << properties_.size() << " properties";
    if (!inheritances_.empty()) {
        oss << ", inherits from: ";
        for (size_t i = 0; i < inheritances_.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << inheritances_[i];
        }
    }
    return oss.str();
}

// PredefinedElement 实现
PredefinedElement::PredefinedElement(const std::string& name, PredefinedType type)
    : PredefinedItem(name, type), rootTagName_("div") {}

void PredefinedElement::addChild(std::shared_ptr<Node> child) {
    if (child) {
        children_.push_back(child);
    }
}

void PredefinedElement::addChildElement(const std::string& tagName, 
                                       const std::unordered_map<std::string, std::string>& attributes) {
    auto element = std::make_shared<ElementNode>(tagName, NodePosition());
    for (const auto& attr : attributes) {
        element->addAttribute(attr.first, attr.second);
    }
    addChild(element);
}

void PredefinedElement::addTextChild(const std::string& text) {
    auto textNode = std::make_shared<TextNode>(text, NodePosition());
    addChild(textNode);
}

void PredefinedElement::addStyleChild(std::shared_ptr<StyleBlockNode> style) {
    if (style) {
        addChild(style);
    }
}

void PredefinedElement::setRootElement(const std::string& tagName, 
                                      const std::unordered_map<std::string, std::string>& attributes) {
    rootTagName_ = tagName;
    rootAttributes_ = attributes;
}

void PredefinedElement::inheritFrom(const std::string& parentElement) {
    if (!parentElement.empty()) {
        auto it = std::find(inheritances_.begin(), inheritances_.end(), parentElement);
        if (it == inheritances_.end()) {
            inheritances_.push_back(parentElement);
        }
    }
}

void PredefinedElement::addElement(const std::string& elementName) {
    if (!elementName.empty()) {
        addedElements_.push_back(elementName);
    }
}

void PredefinedElement::deleteElement(const std::string& elementName) {
    if (!elementName.empty()) {
        deletedElements_.push_back(elementName);
    }
}

void PredefinedElement::deleteElementByIndex(size_t index) {
    deletedIndices_.push_back(index);
}

const std::vector<std::shared_ptr<Node>>& PredefinedElement::getChildren() const {
    return children_;
}

const std::vector<std::string>& PredefinedElement::getInheritances() const {
    return inheritances_;
}

std::string PredefinedElement::getRootTagName() const {
    return rootTagName_;
}

std::shared_ptr<Node> PredefinedElement::createInstance() const {
    auto element = std::make_shared<ElementNode>(rootTagName_, NodePosition());
    element->setName(name_);
    
    // 设置根元素属性
    for (const auto& attr : rootAttributes_) {
        element->addAttribute(attr.first, attr.second);
    }
    
    // 添加子元素
    for (const auto& child : children_) {
        if (child) {
            element->addChild(child->clone());
        }
    }
    
    return element;
}

std::unique_ptr<PredefinedItem> PredefinedElement::clone() const {
    auto cloned = std::make_unique<PredefinedElement>(name_, type_);
    cloned->rootTagName_ = rootTagName_;
    cloned->rootAttributes_ = rootAttributes_;
    cloned->inheritances_ = inheritances_;
    cloned->addedElements_ = addedElements_;
    cloned->deletedElements_ = deletedElements_;
    cloned->deletedIndices_ = deletedIndices_;
    
    // 深拷贝子元素
    for (const auto& child : children_) {
        if (child) {
            cloned->children_.push_back(child->clone());
        }
    }
    
    return cloned;
}

bool PredefinedElement::validate() const {
    if (name_.empty() || rootTagName_.empty()) return false;
    
    // 验证子元素
    for (const auto& child : children_) {
        if (!child || !child->validate()) {
            return false;
        }
    }
    
    return true;
}

std::string PredefinedElement::getDescription() const {
    std::ostringstream oss;
    oss << "Element '" << name_ << "' (" << PredefinedRegistry::getInstance().getTypeString(type_) << ")";
    oss << " root: <" << rootTagName_ << ">";
    oss << " with " << children_.size() << " children";
    if (!inheritances_.empty()) {
        oss << ", inherits from: ";
        for (size_t i = 0; i < inheritances_.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << inheritances_[i];
        }
    }
    return oss.str();
}

// PredefinedVariableGroup 实现
PredefinedVariableGroup::PredefinedVariableGroup(const std::string& name, PredefinedType type)
    : PredefinedItem(name, type) {}

void PredefinedVariableGroup::addVariable(const std::string& name, const std::string& value) {
    if (!name.empty() && !value.empty()) {
        variables_[name] = value;
    }
}

void PredefinedVariableGroup::setVariable(const std::string& name, const std::string& value) {
    addVariable(name, value);
}

void PredefinedVariableGroup::removeVariable(const std::string& name) {
    variables_.erase(name);
}

void PredefinedVariableGroup::inheritFrom(const std::string& parentGroup) {
    if (!parentGroup.empty()) {
        auto it = std::find(inheritances_.begin(), inheritances_.end(), parentGroup);
        if (it == inheritances_.end()) {
            inheritances_.push_back(parentGroup);
        }
    }
}

void PredefinedVariableGroup::overrideVariable(const std::string& name, const std::string& value) {
    overrides_[name] = value;
}

void PredefinedVariableGroup::specializeVariable(const std::string& name, const std::string& value) {
    specializations_[name] = value;
}

const std::unordered_map<std::string, std::string>& PredefinedVariableGroup::getVariables() const {
    return variables_;
}

const std::vector<std::string>& PredefinedVariableGroup::getInheritances() const {
    return inheritances_;
}

bool PredefinedVariableGroup::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

std::string PredefinedVariableGroup::getVariableValue(const std::string& name) const {
    auto it = variables_.find(name);
    return (it != variables_.end()) ? it->second : "";
}

std::string PredefinedVariableGroup::resolveVariableCall(const std::string& varName, 
                                                        const std::unordered_map<std::string, std::string>& specializations) const {
    // 优先检查调用时的特例化
    auto specIt = specializations.find(varName);
    if (specIt != specializations.end()) {
        return specIt->second;
    }
    
    // 检查内部特例化
    auto internalSpecIt = specializations_.find(varName);
    if (internalSpecIt != specializations_.end()) {
        return internalSpecIt->second;
    }
    
    // 检查覆盖
    auto overrideIt = overrides_.find(varName);
    if (overrideIt != overrides_.end()) {
        return overrideIt->second;
    }
    
    // 返回原始值
    return getVariableValue(varName);
}

std::shared_ptr<Node> PredefinedVariableGroup::createInstance() const {
    auto varNode = std::make_shared<CustomVarNode>(name_, NodePosition());
    
    // 添加变量
    for (const auto& var : variables_) {
        varNode->addVariable(var.first, var.second);
    }
    
    return varNode;
}

std::unique_ptr<PredefinedItem> PredefinedVariableGroup::clone() const {
    auto cloned = std::make_unique<PredefinedVariableGroup>(name_, type_);
    cloned->variables_ = variables_;
    cloned->inheritances_ = inheritances_;
    cloned->overrides_ = overrides_;
    cloned->specializations_ = specializations_;
    return cloned;
}

bool PredefinedVariableGroup::validate() const {
    if (name_.empty()) return false;
    
    // 检查变量有效性
    for (const auto& var : variables_) {
        if (var.first.empty() || var.second.empty()) {
            return false;
        }
    }
    
    return true;
}

std::string PredefinedVariableGroup::getDescription() const {
    std::ostringstream oss;
    oss << "VariableGroup '" << name_ << "' (" << PredefinedRegistry::getInstance().getTypeString(type_) << ")";
    oss << " with " << variables_.size() << " variables";
    if (!inheritances_.empty()) {
        oss << ", inherits from: ";
        for (size_t i = 0; i < inheritances_.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << inheritances_[i];
        }
    }
    return oss.str();
}

// PredefinedRegistry 实现
PredefinedRegistry& PredefinedRegistry::getInstance() {
    static PredefinedRegistry instance;
    return instance;
}

bool PredefinedRegistry::registerStyleGroup(std::unique_ptr<PredefinedStyleGroup> styleGroup) {
    if (!styleGroup || !styleGroup->validate()) {
        return false;
    }
    
    const std::string& name = styleGroup->getName();
    PredefinedType type = styleGroup->getType();
    
    if (type == PredefinedType::TEMPLATE_STYLE) {
        templateStyles_[name] = std::move(styleGroup);
    } else {
        styleGroups_[name] = std::move(styleGroup);
    }
    
    return true;
}

bool PredefinedRegistry::registerElement(std::unique_ptr<PredefinedElement> element) {
    if (!element || !element->validate()) {
        return false;
    }
    
    const std::string& name = element->getName();
    PredefinedType type = element->getType();
    
    if (type == PredefinedType::TEMPLATE_ELEMENT) {
        templateElements_[name] = std::move(element);
    } else {
        elements_[name] = std::move(element);
    }
    
    return true;
}

bool PredefinedRegistry::registerVariableGroup(std::unique_ptr<PredefinedVariableGroup> varGroup) {
    if (!varGroup || !varGroup->validate()) {
        return false;
    }
    
    const std::string& name = varGroup->getName();
    PredefinedType type = varGroup->getType();
    
    if (type == PredefinedType::TEMPLATE_VAR) {
        templateVariables_[name] = std::move(varGroup);
    } else {
        variableGroups_[name] = std::move(varGroup);
    }
    
    return true;
}

const PredefinedStyleGroup* PredefinedRegistry::findStyleGroup(const std::string& name, PredefinedType type) const {
    if (type == PredefinedType::TEMPLATE_STYLE) {
        auto it = templateStyles_.find(name);
        return (it != templateStyles_.end()) ? it->second.get() : nullptr;
    } else {
        auto it = styleGroups_.find(name);
        return (it != styleGroups_.end()) ? it->second.get() : nullptr;
    }
}

const PredefinedElement* PredefinedRegistry::findElement(const std::string& name, PredefinedType type) const {
    if (type == PredefinedType::TEMPLATE_ELEMENT) {
        auto it = templateElements_.find(name);
        return (it != templateElements_.end()) ? it->second.get() : nullptr;
    } else {
        auto it = elements_.find(name);
        return (it != elements_.end()) ? it->second.get() : nullptr;
    }
}

const PredefinedVariableGroup* PredefinedRegistry::findVariableGroup(const std::string& name, PredefinedType type) const {
    if (type == PredefinedType::TEMPLATE_VAR) {
        auto it = templateVariables_.find(name);
        return (it != templateVariables_.end()) ? it->second.get() : nullptr;
    } else {
        auto it = variableGroups_.find(name);
        return (it != variableGroups_.end()) ? it->second.get() : nullptr;
    }
}

// 辅助方法实现（临时占位符）
std::string PredefinedRegistry::createKey(const std::string& name, PredefinedType type) const {
    return getTypeString(type) + ":" + name;
}

std::string PredefinedRegistry::getTypeString(PredefinedType type) const {
    switch (type) {
        case PredefinedType::CUSTOM_STYLE: return "CUSTOM_STYLE";
        case PredefinedType::CUSTOM_ELEMENT: return "CUSTOM_ELEMENT";
        case PredefinedType::CUSTOM_VAR: return "CUSTOM_VAR";
        case PredefinedType::TEMPLATE_STYLE: return "TEMPLATE_STYLE";
        case PredefinedType::TEMPLATE_ELEMENT: return "TEMPLATE_ELEMENT";
        case PredefinedType::TEMPLATE_VAR: return "TEMPLATE_VAR";
        default: return "UNKNOWN";
    }
}

// 占位符实现
void PredefinedRegistry::clear() {
    styleGroups_.clear();
    elements_.clear();
    variableGroups_.clear();
    templateStyles_.clear();
    templateElements_.clear();
    templateVariables_.clear();
}

size_t PredefinedRegistry::getStyleGroupCount() const {
    return styleGroups_.size() + templateStyles_.size();
}

size_t PredefinedRegistry::getElementCount() const {
    return elements_.size() + templateElements_.size();
}

size_t PredefinedRegistry::getVariableGroupCount() const {
    return variableGroups_.size() + templateVariables_.size();
}

size_t PredefinedRegistry::getTotalCount() const {
    return getStyleGroupCount() + getElementCount() + getVariableGroupCount();
}

bool PredefinedRegistry::validateAll() const {
    // 验证所有样式组
    for (const auto& pair : styleGroups_) {
        if (!pair.second->validate()) {
            return false;
        }
    }
    
    for (const auto& pair : templateStyles_) {
        if (!pair.second->validate()) {
            return false;
        }
    }
    
    // 验证所有元素
    for (const auto& pair : elements_) {
        if (!pair.second->validate()) {
            return false;
        }
    }
    
    for (const auto& pair : templateElements_) {
        if (!pair.second->validate()) {
            return false;
        }
    }
    
    // 验证所有变量组
    for (const auto& pair : variableGroups_) {
        if (!pair.second->validate()) {
            return false;
        }
    }
    
    for (const auto& pair : templateVariables_) {
        if (!pair.second->validate()) {
            return false;
        }
    }
    
    return true;
}

void PredefinedRegistry::initializeBuiltins() {
    initializeCommonStyles();
    initializeCommonElements();
    initializeCommonVariables();
}

void PredefinedRegistry::initializeCommonStyles() {
    // 初始化常用样式组
    auto buttonStyle = PredefinedFactory::createCommonButtonStyle();
    registerStyleGroup(std::move(buttonStyle));
    
    auto containerStyle = PredefinedFactory::createCommonContainerStyle();
    registerStyleGroup(std::move(containerStyle));
    
    auto textStyle = PredefinedFactory::createCommonTextStyle();
    registerStyleGroup(std::move(textStyle));
}

void PredefinedRegistry::initializeCommonElements() {
    // 初始化常用元素
    auto card = PredefinedFactory::createCommonCard();
    registerElement(std::move(card));
    
    auto button = PredefinedFactory::createCommonButton();
    registerElement(std::move(button));
    
    auto navbar = PredefinedFactory::createCommonNavbar();
    registerElement(std::move(navbar));
}

void PredefinedRegistry::initializeCommonVariables() {
    // 初始化常用变量组
    auto colors = PredefinedFactory::createCommonColors();
    registerVariableGroup(std::move(colors));
    
    auto sizes = PredefinedFactory::createCommonSizes();
    registerVariableGroup(std::move(sizes));
    
    auto spacing = PredefinedFactory::createCommonSpacing();
    registerVariableGroup(std::move(spacing));
}

} // namespace chtl