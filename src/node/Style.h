#ifndef CHTL_STYLE_H
#define CHTL_STYLE_H

#include "Node.h"
#include <string>
#include <vector>
#include <map>

namespace chtl {

// 样式节点 - style { }
class StyleNode : public Node {
private:
    bool isInline;  // 是否是内联样式（在元素内部）
    
public:
    StyleNode(bool inline_style = false, int line = 0, int col = 0)
        : Node(NodeType::STYLE_NODE, line, col), isInline(inline_style) {}
    
    bool getIsInline() const { return isInline; }
    void setIsInline(bool value) { isInline = value; }
    
    std::string toString() const override {
        return isInline ? "InlineStyleNode" : "StyleNode";
    }
};

// CSS属性节点
class StylePropertyNode : public Node {
private:
    std::string property;
    std::string value;
    bool isImportant;
    
public:
    StylePropertyNode(const std::string& prop, const std::string& val, 
                      int line = 0, int col = 0)
        : Node(NodeType::STYLE_PROPERTY_NODE, line, col), 
          property(prop), value(val), isImportant(false) {}
    
    const std::string& getProperty() const { return property; }
    const std::string& getValue() const { return value; }
    bool getIsImportant() const { return isImportant; }
    
    void setProperty(const std::string& prop) { property = prop; }
    void setValue(const std::string& val) { value = val; }
    void setIsImportant(bool imp) { isImportant = imp; }
    
    std::string toString() const override {
        return "StylePropertyNode: " + property + ": " + value;
    }
};

// CSS选择器节点
class StyleSelectorNode : public Node {
public:
    enum SelectorType {
        SELECTOR_ELEMENT,       // 元素选择器
        SELECTOR_CLASS,         // 类选择器 .className
        SELECTOR_ID,            // ID选择器 #idName
        SELECTOR_PSEUDO,        // 伪类选择器 :hover
        SELECTOR_PSEUDO_ELEMENT, // 伪元素选择器 ::after
        SELECTOR_ATTRIBUTE,     // 属性选择器
        SELECTOR_UNIVERSAL,     // 通用选择器 *
        SELECTOR_AMPERSAND     // & 符号（上下文选择器）
    };
    
private:
    SelectorType selectorType;
    std::string selector;
    
public:
    StyleSelectorNode(SelectorType type, const std::string& sel, 
                      int line = 0, int col = 0)
        : Node(NodeType::STYLE_SELECTOR_NODE, line, col), 
          selectorType(type), selector(sel) {}
    
    SelectorType getSelectorType() const { return selectorType; }
    const std::string& getSelector() const { return selector; }
    
    void setSelectorType(SelectorType type) { selectorType = type; }
    void setSelector(const std::string& sel) { selector = sel; }
    
    std::string toString() const override {
        return "StyleSelectorNode: " + selector;
    }
};

// CSS规则节点（选择器 + 属性集）
class StyleRuleNode : public Node {
private:
    std::vector<StyleSelectorNode*> selectors;  // 可能有多个选择器
    std::vector<StylePropertyNode*> properties;
    
public:
    StyleRuleNode(int line = 0, int col = 0)
        : Node(NodeType::STYLE_RULE_NODE, line, col) {}
    
    void addSelector(StyleSelectorNode* selector) {
        if (selector) {
            selectors.push_back(selector);
        }
    }
    
    void addProperty(StylePropertyNode* property) {
        if (property) {
            properties.push_back(property);
        }
    }
    
    const std::vector<StyleSelectorNode*>& getSelectors() const {
        return selectors;
    }
    
    const std::vector<StylePropertyNode*>& getProperties() const {
        return properties;
    }
    
    std::string toString() const override {
        return "StyleRuleNode";
    }
};

// 样式组节点（@Style）
class StyleGroupNode : public Node {
private:
    std::string groupName;
    bool hasDefaultValues;
    std::vector<std::string> inheritedGroups;
    std::map<std::string, std::string> properties;
    
public:
    StyleGroupNode(const std::string& name, bool hasDefaults = false,
                   int line = 0, int col = 0)
        : Node(NodeType::STYLE_GROUP_NODE, line, col), 
          groupName(name), hasDefaultValues(hasDefaults) {}
    
    const std::string& getGroupName() const { return groupName; }
    bool getHasDefaultValues() const { return hasDefaultValues; }
    
    void setGroupName(const std::string& name) { groupName = name; }
    void setHasDefaultValues(bool value) { hasDefaultValues = value; }
    
    void addInheritedGroup(const std::string& group) {
        inheritedGroups.push_back(group);
    }
    
    const std::vector<std::string>& getInheritedGroups() const {
        return inheritedGroups;
    }
    
    void setProperty(const std::string& prop, const std::string& value) {
        properties[prop] = value;
    }
    
    const std::map<std::string, std::string>& getProperties() const {
        return properties;
    }
    
    std::string toString() const override {
        return "StyleGroupNode: @Style " + groupName;
    }
};

} // namespace chtl

#endif // CHTL_STYLE_H