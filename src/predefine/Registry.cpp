#include "Registry.h"
#include "../node/Custom.h"
#include "../node/Template.h"
#include <vector>

namespace chtl {

Registry::Registry() {
    // 初始化时注册所有预定义内容
    registerAllPredefined();
}

Registry& Registry::getInstance() {
    static Registry instance;
    return instance;
}

// 注册函数实现
void Registry::registerCustomStyle(const std::string& name, 
                                  std::function<std::shared_ptr<CustomStyleNode>()> factory) {
    customStyleFactories[name] = factory;
}

void Registry::registerCustomElement(const std::string& name, 
                                    std::function<std::shared_ptr<CustomElementNode>()> factory) {
    customElementFactories[name] = factory;
}

void Registry::registerCustomVar(const std::string& name, 
                                std::function<std::shared_ptr<CustomVarNode>()> factory) {
    customVarFactories[name] = factory;
}

void Registry::registerTemplateStyle(const std::string& name, 
                                    std::function<std::shared_ptr<TemplateStyleNode>()> factory) {
    templateStyleFactories[name] = factory;
}

void Registry::registerTemplateElement(const std::string& name, 
                                      std::function<std::shared_ptr<TemplateElementNode>()> factory) {
    templateElementFactories[name] = factory;
}

void Registry::registerTemplateVar(const std::string& name, 
                                  std::function<std::shared_ptr<TemplateVarNode>()> factory) {
    templateVarFactories[name] = factory;
}

// 获取函数实现（懒加载）
std::shared_ptr<CustomStyleNode> Registry::getCustomStyle(const std::string& name) {
    // 检查是否已经加载
    auto it = customStyles.find(name);
    if (it != customStyles.end()) {
        return it->second;
    }
    
    // 检查是否有工厂函数
    auto factoryIt = customStyleFactories.find(name);
    if (factoryIt != customStyleFactories.end()) {
        // 懒加载：创建并缓存
        auto node = factoryIt->second();
        customStyles[name] = node;
        return node;
    }
    
    return nullptr;
}

std::shared_ptr<CustomElementNode> Registry::getCustomElement(const std::string& name) {
    auto it = customElements.find(name);
    if (it != customElements.end()) {
        return it->second;
    }
    
    auto factoryIt = customElementFactories.find(name);
    if (factoryIt != customElementFactories.end()) {
        auto node = factoryIt->second();
        customElements[name] = node;
        return node;
    }
    
    return nullptr;
}

std::shared_ptr<CustomVarNode> Registry::getCustomVar(const std::string& name) {
    auto it = customVars.find(name);
    if (it != customVars.end()) {
        return it->second;
    }
    
    auto factoryIt = customVarFactories.find(name);
    if (factoryIt != customVarFactories.end()) {
        auto node = factoryIt->second();
        customVars[name] = node;
        return node;
    }
    
    return nullptr;
}

std::shared_ptr<TemplateStyleNode> Registry::getTemplateStyle(const std::string& name) {
    auto it = templateStyles.find(name);
    if (it != templateStyles.end()) {
        return it->second;
    }
    
    auto factoryIt = templateStyleFactories.find(name);
    if (factoryIt != templateStyleFactories.end()) {
        auto node = factoryIt->second();
        templateStyles[name] = node;
        return node;
    }
    
    return nullptr;
}

std::shared_ptr<TemplateElementNode> Registry::getTemplateElement(const std::string& name) {
    auto it = templateElements.find(name);
    if (it != templateElements.end()) {
        return it->second;
    }
    
    auto factoryIt = templateElementFactories.find(name);
    if (factoryIt != templateElementFactories.end()) {
        auto node = factoryIt->second();
        templateElements[name] = node;
        return node;
    }
    
    return nullptr;
}

std::shared_ptr<TemplateVarNode> Registry::getTemplateVar(const std::string& name) {
    auto it = templateVars.find(name);
    if (it != templateVars.end()) {
        return it->second;
    }
    
    auto factoryIt = templateVarFactories.find(name);
    if (factoryIt != templateVarFactories.end()) {
        auto node = factoryIt->second();
        templateVars[name] = node;
        return node;
    }
    
    return nullptr;
}

// 检查函数实现
bool Registry::hasCustomStyle(const std::string& name) const {
    return customStyleFactories.find(name) != customStyleFactories.end();
}

bool Registry::hasCustomElement(const std::string& name) const {
    return customElementFactories.find(name) != customElementFactories.end();
}

bool Registry::hasCustomVar(const std::string& name) const {
    return customVarFactories.find(name) != customVarFactories.end();
}

bool Registry::hasTemplateStyle(const std::string& name) const {
    return templateStyleFactories.find(name) != templateStyleFactories.end();
}

bool Registry::hasTemplateElement(const std::string& name) const {
    return templateElementFactories.find(name) != templateElementFactories.end();
}

bool Registry::hasTemplateVar(const std::string& name) const {
    return templateVarFactories.find(name) != templateVarFactories.end();
}

// 获取名称列表
std::vector<std::string> Registry::getCustomStyleNames() const {
    std::vector<std::string> names;
    names.reserve(customStyleFactories.size());
    for (const auto& pair : customStyleFactories) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> Registry::getCustomElementNames() const {
    std::vector<std::string> names;
    names.reserve(customElementFactories.size());
    for (const auto& pair : customElementFactories) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> Registry::getCustomVarNames() const {
    std::vector<std::string> names;
    names.reserve(customVarFactories.size());
    for (const auto& pair : customVarFactories) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> Registry::getTemplateStyleNames() const {
    std::vector<std::string> names;
    names.reserve(templateStyleFactories.size());
    for (const auto& pair : templateStyleFactories) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> Registry::getTemplateElementNames() const {
    std::vector<std::string> names;
    names.reserve(templateElementFactories.size());
    for (const auto& pair : templateElementFactories) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> Registry::getTemplateVarNames() const {
    std::vector<std::string> names;
    names.reserve(templateVarFactories.size());
    for (const auto& pair : templateVarFactories) {
        names.push_back(pair.first);
    }
    return names;
}

// 清除缓存
void Registry::clearCache() {
    customStyles.clear();
    customElements.clear();
    customVars.clear();
    templateStyles.clear();
    templateElements.clear();
    templateVars.clear();
}

// 预加载所有内容
void Registry::preloadAll() {
    // 预加载所有自定义样式
    for (const auto& pair : customStyleFactories) {
        if (customStyles.find(pair.first) == customStyles.end()) {
            customStyles[pair.first] = pair.second();
        }
    }
    
    // 预加载所有自定义元素
    for (const auto& pair : customElementFactories) {
        if (customElements.find(pair.first) == customElements.end()) {
            customElements[pair.first] = pair.second();
        }
    }
    
    // 预加载所有自定义变量
    for (const auto& pair : customVarFactories) {
        if (customVars.find(pair.first) == customVars.end()) {
            customVars[pair.first] = pair.second();
        }
    }
    
    // 预加载所有模板样式
    for (const auto& pair : templateStyleFactories) {
        if (templateStyles.find(pair.first) == templateStyles.end()) {
            templateStyles[pair.first] = pair.second();
        }
    }
    
    // 预加载所有模板元素
    for (const auto& pair : templateElementFactories) {
        if (templateElements.find(pair.first) == templateElements.end()) {
            templateElements[pair.first] = pair.second();
        }
    }
    
    // 预加载所有模板变量
    for (const auto& pair : templateVarFactories) {
        if (templateVars.find(pair.first) == templateVars.end()) {
            templateVars[pair.first] = pair.second();
        }
    }
}

// 注册所有预定义内容
void Registry::registerAllPredefined() {
    // 这里会调用各个预定义类的注册函数
    // 通过链接时的静态初始化自动完成
    // 静态初始化会在程序启动时自动调用各个类的registerAll方法
}

// AutoRegister的特化实现
template<>
AutoRegister<CustomStyleNode>::AutoRegister(const std::string& name, 
                                           std::function<std::shared_ptr<CustomStyleNode>()> factory) {
    Registry::getInstance().registerCustomStyle(name, factory);
}

template<>
AutoRegister<CustomElementNode>::AutoRegister(const std::string& name, 
                                             std::function<std::shared_ptr<CustomElementNode>()> factory) {
    Registry::getInstance().registerCustomElement(name, factory);
}

template<>
AutoRegister<CustomVarNode>::AutoRegister(const std::string& name, 
                                         std::function<std::shared_ptr<CustomVarNode>()> factory) {
    Registry::getInstance().registerCustomVar(name, factory);
}

template<>
AutoRegister<TemplateStyleNode>::AutoRegister(const std::string& name, 
                                             std::function<std::shared_ptr<TemplateStyleNode>()> factory) {
    Registry::getInstance().registerTemplateStyle(name, factory);
}

template<>
AutoRegister<TemplateElementNode>::AutoRegister(const std::string& name, 
                                               std::function<std::shared_ptr<TemplateElementNode>()> factory) {
    Registry::getInstance().registerTemplateElement(name, factory);
}

template<>
AutoRegister<TemplateVarNode>::AutoRegister(const std::string& name, 
                                           std::function<std::shared_ptr<TemplateVarNode>()> factory) {
    Registry::getInstance().registerTemplateVar(name, factory);
}

} // namespace chtl