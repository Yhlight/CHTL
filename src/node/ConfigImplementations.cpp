#include "Config.h"
#include <sstream>
#include <algorithm>

namespace chtl {

// ConfigurationNode实现
ConfigurationNode::ConfigurationNode(const NodePosition& position)
    : Node(NodeType::CONFIGURATION, position), isApplied_(false) {
}

void ConfigurationNode::addConfigOption(std::shared_ptr<ConfigOptionNode> option) {
    if (option) {
        configOptions_.push_back(option);
    }
}

void ConfigurationNode::removeConfigOption(const std::string& optionName) {
    configOptions_.erase(
        std::remove_if(configOptions_.begin(), configOptions_.end(),
            [&optionName](const std::shared_ptr<ConfigOptionNode>& option) {
                return option->getOptionName() == optionName;
            }),
        configOptions_.end());
}

std::vector<std::shared_ptr<ConfigOptionNode>> ConfigurationNode::getConfigOptions() const {
    return configOptions_;
}

std::shared_ptr<ConfigOptionNode> ConfigurationNode::findConfigOption(const std::string& optionName) const {
    for (const auto& option : configOptions_) {
        if (option->getOptionName() == optionName) {
            return option;
        }
    }
    return nullptr;
}

void ConfigurationNode::addNameBlock(std::shared_ptr<NameBlockNode> nameBlock) {
    if (nameBlock) {
        nameBlocks_.push_back(nameBlock);
    }
}

void ConfigurationNode::removeNameBlock(const std::string& groupName) {
    nameBlocks_.erase(
        std::remove_if(nameBlocks_.begin(), nameBlocks_.end(),
            [&groupName](const std::shared_ptr<NameBlockNode>& block) {
                return block->getGroupName() == groupName;
            }),
        nameBlocks_.end());
}

std::vector<std::shared_ptr<NameBlockNode>> ConfigurationNode::getNameBlocks() const {
    return nameBlocks_;
}

std::shared_ptr<NameBlockNode> ConfigurationNode::findNameBlock(const std::string& groupName) const {
    for (const auto& block : nameBlocks_) {
        if (block->getGroupName() == groupName) {
            return block;
        }
    }
    return nullptr;
}

bool ConfigurationNode::validateConfiguration() const {
    return true; // 基础实现
}

std::vector<std::string> ConfigurationNode::getConfigurationErrors() const {
    return {}; // 基础实现
}

bool ConfigurationNode::applyConfiguration() {
    isApplied_ = true;
    return true;
}

bool ConfigurationNode::isConfigurationApplied() const {
    return isApplied_;
}

bool ConfigurationNode::validate() const {
    return Node::validate() && validateConfiguration();
}

std::string ConfigurationNode::toString() const {
    return "[Configuration]";
}

std::string ConfigurationNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    return indentStr + "ConfigurationNode: " + std::to_string(configOptions_.size()) + " options, " + 
           std::to_string(nameBlocks_.size()) + " name blocks";
}

std::string ConfigurationNode::toConfigString() const {
    std::stringstream ss;
    ss << "[Configuration]\n{\n";
    
    for (const auto& option : configOptions_) {
        ss << "    " << option->toConfigLine() << "\n";
    }
    
    for (const auto& nameBlock : nameBlocks_) {
        ss << "\n" << nameBlock->toNameBlockString() << "\n";
    }
    
    ss << "}";
    return ss.str();
}

std::shared_ptr<Node> ConfigurationNode::clone() const {
    auto cloned = std::make_shared<ConfigurationNode>(getPosition());
    
    for (const auto& option : configOptions_) {
        cloned->addConfigOption(std::dynamic_pointer_cast<ConfigOptionNode>(option->clone()));
    }
    
    for (const auto& nameBlock : nameBlocks_) {
        cloned->addNameBlock(std::dynamic_pointer_cast<NameBlockNode>(nameBlock->clone()));
    }
    
    return cloned;
}

void ConfigurationNode::accept(NodeVisitor& visitor) {
    // 访问者模式实现
}

// ConfigOptionNode实现
ConfigOptionNode::ConfigOptionNode(OptionType type, const ConfigValue& value, const NodePosition& position)
    : Node(NodeType::CONFIG_OPTION, position), optionType_(type), value_(value) {
    setName(getOptionName());
}

ConfigOptionNode::OptionType ConfigOptionNode::getOptionType() const {
    return optionType_;
}

void ConfigOptionNode::setOptionType(OptionType type) {
    optionType_ = type;
    setName(getOptionName());
}

std::string ConfigOptionNode::getOptionName() const {
    return optionTypeToString();
}

const ConfigOptionNode::ConfigValue& ConfigOptionNode::getValue() const {
    return value_;
}

void ConfigOptionNode::setValue(const ConfigValue& value) {
    value_ = value;
}

int ConfigOptionNode::getIntValue() const {
    return value_.asInt();
}

bool ConfigOptionNode::getBoolValue() const {
    return value_.asBool();
}

std::string ConfigOptionNode::getStringValue() const {
    return value_.asString();
}

bool ConfigOptionNode::isIntValue() const {
    return value_.isInt();
}

bool ConfigOptionNode::isBoolValue() const {
    return value_.isBool();
}

bool ConfigOptionNode::isStringValue() const {
    return value_.isString();
}

ConfigOptionNode::ConfigValue ConfigOptionNode::getDefaultValue() const {
    return getDefaultValueForType(optionType_);
}

bool ConfigOptionNode::isDefaultValue() const {
    // 基础比较，可以根据需要完善
    switch (optionType_) {
        case OptionType::INDEX_INITIAL_COUNT:
            return value_.isInt() && value_.asInt() == 0;
        case OptionType::CUSTOM_STYLE:
            return value_.isBool() && value_.asBool() == true;
        case OptionType::DISABLE_NAME_GROUP:
            return value_.isBool() && value_.asBool() == false;
        case OptionType::DEBUG_MODE:
            return value_.isBool() && value_.asBool() == false;
        case OptionType::OPTION_COUNT:
            return value_.isInt() && value_.asInt() == 10;
    }
    return false;
}

void ConfigOptionNode::resetToDefault() {
    value_ = getDefaultValue();
}

bool ConfigOptionNode::isValidValue(const ConfigValue& value) const {
    return isValidValueForType(optionType_, value);
}

std::string ConfigOptionNode::getValueConstraints() const {
    switch (optionType_) {
        case OptionType::INDEX_INITIAL_COUNT:
        case OptionType::OPTION_COUNT:
            return "Non-negative integer";
        case OptionType::CUSTOM_STYLE:
        case OptionType::DISABLE_NAME_GROUP:
        case OptionType::DEBUG_MODE:
            return "Boolean (true/false)";
    }
    return "Any value";
}

bool ConfigOptionNode::validate() const {
    return Node::validate() && isValidValue(value_);
}

std::string ConfigOptionNode::toString() const {
    return getOptionName() + " = " + (value_.isInt() ? std::to_string(value_.asInt()) : 
                                     value_.isBool() ? (value_.asBool() ? "true" : "false") : 
                                     value_.asString());
}

std::string ConfigOptionNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    return indentStr + "ConfigOptionNode: " + toString();
}

std::string ConfigOptionNode::toConfigLine() const {
    std::string valueStr;
    if (value_.isInt()) {
        valueStr = std::to_string(value_.asInt());
    } else if (value_.isBool()) {
        valueStr = value_.asBool() ? "true" : "false";
    } else {
        valueStr = value_.asString();
    }
    return getOptionName() + " = " + valueStr + ";";
}

std::shared_ptr<Node> ConfigOptionNode::clone() const {
    return std::make_shared<ConfigOptionNode>(optionType_, value_, getPosition());
}

void ConfigOptionNode::accept(NodeVisitor& visitor) {
    // 访问者模式实现
}

std::string ConfigOptionNode::optionTypeToString() const {
    switch (optionType_) {
        case OptionType::INDEX_INITIAL_COUNT: return "INDEX_INITIAL_COUNT";
        case OptionType::CUSTOM_STYLE: return "CUSTOM_STYLE";
        case OptionType::DISABLE_NAME_GROUP: return "DISABLE_NAME_GROUP";
        case OptionType::DEBUG_MODE: return "DEBUG_MODE";
        case OptionType::OPTION_COUNT: return "OPTION_COUNT";
    }
    return "UNKNOWN";
}

ConfigOptionNode::ConfigValue ConfigOptionNode::getDefaultValueForType(OptionType type) const {
    switch (type) {
        case OptionType::INDEX_INITIAL_COUNT: return ConfigValue(0);
        case OptionType::CUSTOM_STYLE: return ConfigValue(true);
        case OptionType::DISABLE_NAME_GROUP: return ConfigValue(false);
        case OptionType::DEBUG_MODE: return ConfigValue(false);
        case OptionType::OPTION_COUNT: return ConfigValue(10);
    }
    return ConfigValue("");
}

bool ConfigOptionNode::isValidValueForType(OptionType type, const ConfigValue& value) const {
    switch (type) {
        case OptionType::INDEX_INITIAL_COUNT:
        case OptionType::OPTION_COUNT:
            return value.isInt() && value.asInt() >= 0;
        case OptionType::CUSTOM_STYLE:
        case OptionType::DISABLE_NAME_GROUP:
        case OptionType::DEBUG_MODE:
            return value.isBool();
    }
    return true;
}

// NameBlockNode实现
NameBlockNode::NameBlockNode(const std::string& groupName, const NodePosition& position)
    : Node(NodeType::NAME_BLOCK, position), groupName_(groupName) {
    setName(groupName);
}

const std::string& NameBlockNode::getGroupName() const {
    return groupName_;
}

void NameBlockNode::setGroupName(const std::string& groupName) {
    if (isValidGroupName(groupName)) {
        groupName_ = groupName;
        setName(groupName);
    }
}

void NameBlockNode::addGroupOption(const std::string& optionName, const std::string& value) {
    if (isValidOptionName(optionName)) {
        groupOptions_[optionName] = value;
    }
}

void NameBlockNode::removeGroupOption(const std::string& optionName) {
    groupOptions_.erase(optionName);
}

std::string NameBlockNode::getGroupOption(const std::string& optionName) const {
    auto it = groupOptions_.find(optionName);
    return it != groupOptions_.end() ? it->second : "";
}

std::unordered_map<std::string, std::string> NameBlockNode::getAllGroupOptions() const {
    return groupOptions_;
}

bool NameBlockNode::hasGroupOption(const std::string& optionName) const {
    return groupOptions_.find(optionName) != groupOptions_.end();
}

void NameBlockNode::addCustomKeyword(const std::string& keyword, const std::string& mapping) {
    if (isValidKeyword(keyword) && isValidKeywordMapping(mapping)) {
        customKeywords_[keyword] = mapping;
    }
}

void NameBlockNode::removeCustomKeyword(const std::string& keyword) {
    customKeywords_.erase(keyword);
}

std::string NameBlockNode::getCustomKeywordMapping(const std::string& keyword) const {
    auto it = customKeywords_.find(keyword);
    return it != customKeywords_.end() ? it->second : "";
}

std::unordered_map<std::string, std::string> NameBlockNode::getAllCustomKeywords() const {
    return customKeywords_;
}

bool NameBlockNode::hasCustomKeyword(const std::string& keyword) const {
    return customKeywords_.find(keyword) != customKeywords_.end();
}

std::string NameBlockNode::resolveKeyword(const std::string& keyword) const {
    return hasCustomKeyword(keyword) ? getCustomKeywordMapping(keyword) : keyword;
}

bool NameBlockNode::isCustomKeyword(const std::string& keyword) const {
    return hasCustomKeyword(keyword);
}

bool NameBlockNode::validate() const {
    return Node::validate() && validateGroupOptions() && validateCustomKeywords();
}

bool NameBlockNode::validateGroupOptions() const {
    for (const auto& pair : groupOptions_) {
        if (!isValidOptionName(pair.first) || pair.second.empty()) {
            return false;
        }
    }
    return true;
}

bool NameBlockNode::validateCustomKeywords() const {
    for (const auto& pair : customKeywords_) {
        if (!isValidKeyword(pair.first) || !isValidKeywordMapping(pair.second)) {
            return false;
        }
    }
    return true;
}

std::string NameBlockNode::toString() const {
    return "[Name] " + groupName_;
}

std::string NameBlockNode::toDebugString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    return indentStr + "NameBlockNode: " + groupName_ + " (" + 
           std::to_string(groupOptions_.size()) + " options, " +
           std::to_string(customKeywords_.size()) + " keywords)";
}

std::string NameBlockNode::toNameBlockString() const {
    std::stringstream ss;
    ss << "    [Name] " << groupName_ << "\n    {\n";
    
    // 输出组选项
    for (const auto& pair : groupOptions_) {
        ss << "        " << pair.first << " = \"" << pair.second << "\";\n";
    }
    
    // 输出自定义关键字
    if (!customKeywords_.empty() && !groupOptions_.empty()) {
        ss << "\n";
    }
    for (const auto& pair : customKeywords_) {
        ss << "        " << pair.first << " = \"" << pair.second << "\";\n";
    }
    
    ss << "    }";
    return ss.str();
}

std::shared_ptr<Node> NameBlockNode::clone() const {
    auto cloned = std::make_shared<NameBlockNode>(groupName_, getPosition());
    cloned->groupOptions_ = groupOptions_;
    cloned->customKeywords_ = customKeywords_;
    return cloned;
}

void NameBlockNode::accept(NodeVisitor& visitor) {
    // 访问者模式实现
}

bool NameBlockNode::isValidGroupName(const std::string& groupName) const {
    return !groupName.empty() && std::isalpha(groupName[0]);
}

bool NameBlockNode::isValidOptionName(const std::string& optionName) const {
    return !optionName.empty();
}

bool NameBlockNode::isValidKeyword(const std::string& keyword) const {
    return !keyword.empty();
}

bool NameBlockNode::isValidKeywordMapping(const std::string& mapping) const {
    return !mapping.empty();
}

} // namespace chtl