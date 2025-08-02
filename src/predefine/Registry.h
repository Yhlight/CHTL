#ifndef CHTL_REGISTRY_H
#define CHTL_REGISTRY_H

#include <memory>
#include <unordered_map>
#include <string>
#include <functional>

namespace chtl {

// 前向声明
class CustomStyleNode;
class CustomElementNode;
class CustomVarNode;
class TemplateStyleNode;
class TemplateElementNode;
class TemplateVarNode;

// 预定义内容的基类
class PredefinedBase {
public:
    virtual ~PredefinedBase() = default;
    virtual void initialize() = 0;
    virtual const std::string& getName() const = 0;
    virtual const std::string& getChtlCode() const = 0;
};

// Registry类 - 单例模式，管理所有预定义内容
class Registry {
private:
    // 懒加载的预定义内容存储
    std::unordered_map<std::string, std::shared_ptr<CustomStyleNode>> customStyles;
    std::unordered_map<std::string, std::shared_ptr<CustomElementNode>> customElements;
    std::unordered_map<std::string, std::shared_ptr<CustomVarNode>> customVars;
    std::unordered_map<std::string, std::shared_ptr<TemplateStyleNode>> templateStyles;
    std::unordered_map<std::string, std::shared_ptr<TemplateElementNode>> templateElements;
    std::unordered_map<std::string, std::shared_ptr<TemplateVarNode>> templateVars;
    
    // 注册的预定义内容工厂函数
    std::unordered_map<std::string, std::function<std::shared_ptr<CustomStyleNode>()>> customStyleFactories;
    std::unordered_map<std::string, std::function<std::shared_ptr<CustomElementNode>()>> customElementFactories;
    std::unordered_map<std::string, std::function<std::shared_ptr<CustomVarNode>()>> customVarFactories;
    std::unordered_map<std::string, std::function<std::shared_ptr<TemplateStyleNode>()>> templateStyleFactories;
    std::unordered_map<std::string, std::function<std::shared_ptr<TemplateElementNode>()>> templateElementFactories;
    std::unordered_map<std::string, std::function<std::shared_ptr<TemplateVarNode>()>> templateVarFactories;
    
    // 私有构造函数（单例）
    Registry();
    Registry(const Registry&) = delete;
    Registry& operator=(const Registry&) = delete;
    
public:
    // 获取单例实例
    static Registry& getInstance();
    
    // 注册预定义内容的工厂函数
    void registerCustomStyle(const std::string& name, std::function<std::shared_ptr<CustomStyleNode>()> factory);
    void registerCustomElement(const std::string& name, std::function<std::shared_ptr<CustomElementNode>()> factory);
    void registerCustomVar(const std::string& name, std::function<std::shared_ptr<CustomVarNode>()> factory);
    void registerTemplateStyle(const std::string& name, std::function<std::shared_ptr<TemplateStyleNode>()> factory);
    void registerTemplateElement(const std::string& name, std::function<std::shared_ptr<TemplateElementNode>()> factory);
    void registerTemplateVar(const std::string& name, std::function<std::shared_ptr<TemplateVarNode>()> factory);
    
    // 获取预定义内容（懒加载）
    std::shared_ptr<CustomStyleNode> getCustomStyle(const std::string& name);
    std::shared_ptr<CustomElementNode> getCustomElement(const std::string& name);
    std::shared_ptr<CustomVarNode> getCustomVar(const std::string& name);
    std::shared_ptr<TemplateStyleNode> getTemplateStyle(const std::string& name);
    std::shared_ptr<TemplateElementNode> getTemplateElement(const std::string& name);
    std::shared_ptr<TemplateVarNode> getTemplateVar(const std::string& name);
    
    // 检查是否存在预定义内容
    bool hasCustomStyle(const std::string& name) const;
    bool hasCustomElement(const std::string& name) const;
    bool hasCustomVar(const std::string& name) const;
    bool hasTemplateStyle(const std::string& name) const;
    bool hasTemplateElement(const std::string& name) const;
    bool hasTemplateVar(const std::string& name) const;
    
    // 获取所有已注册的名称
    std::vector<std::string> getCustomStyleNames() const;
    std::vector<std::string> getCustomElementNames() const;
    std::vector<std::string> getCustomVarNames() const;
    std::vector<std::string> getTemplateStyleNames() const;
    std::vector<std::string> getTemplateElementNames() const;
    std::vector<std::string> getTemplateVarNames() const;
    
    // 清除缓存
    void clearCache();
    
    // 预加载所有内容
    void preloadAll();
    
    // 注册所有预定义内容
    void registerAllPredefined();
};

// 自动注册辅助类
template<typename T>
class AutoRegister {
public:
    AutoRegister(const std::string& name, std::function<std::shared_ptr<T>()> factory);
};

} // namespace chtl

#endif // CHTL_REGISTRY_H