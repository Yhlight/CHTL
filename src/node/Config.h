#ifndef CHTL_CONFIG_H
#define CHTL_CONFIG_H

#include "Node.h"
#include <string>
#include <vector>
#include <map>

namespace chtl {

// 配置节点 - [Configuration]
class ConfigurationNode : public Node {
private:
    std::map<std::string, std::string> configItems;
    bool hasNameBlock;
    
public:
    ConfigurationNode(int line = 0, int col = 0)
        : Node(NodeType::CONFIGURATION_NODE, line, col), hasNameBlock(false) {}
    
    void setConfigItem(const std::string& key, const std::string& value) {
        configItems[key] = value;
    }
    
    std::string getConfigItem(const std::string& key) const {
        auto it = configItems.find(key);
        if (it != configItems.end()) {
            return it->second;
        }
        return "";
    }
    
    const std::map<std::string, std::string>& getConfigItems() const {
        return configItems;
    }
    
    void setHasNameBlock(bool value) { hasNameBlock = value; }
    bool getHasNameBlock() const { return hasNameBlock; }
    
    std::string toString() const override {
        return "ConfigurationNode";
    }
};

// 配置项节点
class ConfigItemNode : public Node {
private:
    std::string key;
    std::string value;
    std::vector<std::string> options;  // 组选项，如 [option1, option2]
    
public:
    ConfigItemNode(const std::string& k, const std::string& v, int line = 0, int col = 0)
        : Node(NodeType::CONFIG_ITEM_NODE, line, col), key(k), value(v) {}
    
    const std::string& getKey() const { return key; }
    const std::string& getValue() const { return value; }
    
    void setKey(const std::string& k) { key = k; }
    void setValue(const std::string& v) { value = v; }
    
    void addOption(const std::string& opt) {
        options.push_back(opt);
    }
    
    const std::vector<std::string>& getOptions() const {
        return options;
    }
    
    bool hasOptions() const {
        return !options.empty();
    }
    
    std::string toString() const override {
        return "ConfigItemNode: " + key + " = " + value;
    }
};

// [Name]块节点
class ConfigNameBlockNode : public Node {
public:
    ConfigNameBlockNode(int line = 0, int col = 0)
        : Node(NodeType::CONFIG_NAME_BLOCK_NODE, line, col) {}
    
    std::string toString() const override {
        return "ConfigNameBlockNode";
    }
};

} // namespace chtl

#endif // CHTL_CONFIG_H