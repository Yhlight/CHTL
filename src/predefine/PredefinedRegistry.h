#pragma once
#include "../common/Token.h"
#include "../node/Node.h"
#include "../node/ElementNode.h"
#include "../node/StyleNode.h"
#include "../node/CustomNode.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace chtl {

/**
 * 预定义类型枚举
 */
enum class PredefinedType {
    CUSTOM_STYLE,       // 自定义样式组
    CUSTOM_ELEMENT,     // 自定义元素
    CUSTOM_VAR,         // 自定义变量组
    TEMPLATE_STYLE,     // 模板样式组
    TEMPLATE_ELEMENT,   // 模板元素
    TEMPLATE_VAR        // 模板变量组
};

/**
 * 预定义项目基类
 */
class PredefinedItem {
public:
    explicit PredefinedItem(const std::string& name, PredefinedType type)
        : name_(name), type_(type) {}
    virtual ~PredefinedItem() = default;
    
    const std::string& getName() const { return name_; }
    PredefinedType getType() const { return type_; }
    
    // 创建实例
    virtual std::shared_ptr<Node> createInstance() const = 0;
    
    // 克隆项目
    virtual std::unique_ptr<PredefinedItem> clone() const = 0;
    
    // 验证有效性
    virtual bool validate() const = 0;
    
    // 获取描述信息
    virtual std::string getDescription() const = 0;
    
protected:
    std::string name_;
    PredefinedType type_;
};

/**
 * 预定义样式组
 */
class PredefinedStyleGroup : public PredefinedItem {
public:
    PredefinedStyleGroup(const std::string& name, PredefinedType type = PredefinedType::CUSTOM_STYLE);
    
    // 添加样式规则
    void addProperty(const std::string& property, const std::string& value);
    void addRule(const std::string& selector, const std::unordered_map<std::string, std::string>& properties);
    void addStyleGroup(const std::string& groupName);
    
    // 继承其他样式组
    void inheritFrom(const std::string& parentGroup);
    void addInheritance(const std::string& parentGroup);
    void removeInheritance(const std::string& parentGroup);
    
    // 覆盖与特例化
    void overrideProperty(const std::string& property, const std::string& value);
    void specializeProperty(const std::string& property, const std::string& value);
    
    // 获取样式信息
    const std::unordered_map<std::string, std::string>& getProperties() const;
    const std::vector<std::string>& getInheritances() const;
    bool hasProperty(const std::string& property) const;
    std::string getPropertyValue(const std::string& property) const;
    
    // 实现虚函数
    std::shared_ptr<Node> createInstance() const override;
    std::unique_ptr<PredefinedItem> clone() const override;
    bool validate() const override;
    std::string getDescription() const override;
    
private:
    std::unordered_map<std::string, std::string> properties_;
    std::vector<std::string> inheritances_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> rules_;
    std::vector<std::string> nestedGroups_;
};

/**
 * 预定义元素
 */
class PredefinedElement : public PredefinedItem {
public:
    PredefinedElement(const std::string& name, PredefinedType type = PredefinedType::CUSTOM_ELEMENT);
    
    // 添加子元素
    void addChild(std::shared_ptr<Node> child);
    void addChildElement(const std::string& tagName, const std::unordered_map<std::string, std::string>& attributes = {});
    void addTextChild(const std::string& text);
    void addStyleChild(std::shared_ptr<StyleBlockNode> style);
    
    // 设置根元素
    void setRootElement(const std::string& tagName, const std::unordered_map<std::string, std::string>& attributes = {});
    
    // 继承与操作
    void inheritFrom(const std::string& parentElement);
    void addElement(const std::string& elementName);
    void deleteElement(const std::string& elementName);
    void deleteElementByIndex(size_t index);
    
    // 获取元素信息
    const std::vector<std::shared_ptr<Node>>& getChildren() const;
    const std::vector<std::string>& getInheritances() const;
    std::string getRootTagName() const;
    
    // 实现虚函数
    std::shared_ptr<Node> createInstance() const override;
    std::unique_ptr<PredefinedItem> clone() const override;
    bool validate() const override;
    std::string getDescription() const override;
    
private:
    std::string rootTagName_;
    std::unordered_map<std::string, std::string> rootAttributes_;
    std::vector<std::shared_ptr<Node>> children_;
    std::vector<std::string> inheritances_;
    std::vector<std::string> addedElements_;
    std::vector<std::string> deletedElements_;
    std::vector<size_t> deletedIndices_;
};

/**
 * 预定义变量组
 */
class PredefinedVariableGroup : public PredefinedItem {
public:
    PredefinedVariableGroup(const std::string& name, PredefinedType type = PredefinedType::CUSTOM_VAR);
    
    // 变量管理
    void addVariable(const std::string& name, const std::string& value);
    void setVariable(const std::string& name, const std::string& value);
    void removeVariable(const std::string& name);
    
    // 继承与覆盖
    void inheritFrom(const std::string& parentGroup);
    void overrideVariable(const std::string& name, const std::string& value);
    void specializeVariable(const std::string& name, const std::string& value);
    
    // 获取变量信息
    const std::unordered_map<std::string, std::string>& getVariables() const;
    const std::vector<std::string>& getInheritances() const;
    bool hasVariable(const std::string& name) const;
    std::string getVariableValue(const std::string& name) const;
    
    // 变量调用
    std::string resolveVariableCall(const std::string& varName, 
                                   const std::unordered_map<std::string, std::string>& specializations = {}) const;
    
    // 实现虚函数
    std::shared_ptr<Node> createInstance() const override;
    std::unique_ptr<PredefinedItem> clone() const override;
    bool validate() const override;
    std::string getDescription() const override;
    
private:
    std::unordered_map<std::string, std::string> variables_;
    std::vector<std::string> inheritances_;
    std::unordered_map<std::string, std::string> overrides_;
    std::unordered_map<std::string, std::string> specializations_;
};

/**
 * 预定义注册表
 * 管理所有预定义的样式组、元素和变量组
 */
class PredefinedRegistry {
public:
    // 单例模式
    static PredefinedRegistry& getInstance();
    
    // 注册预定义项目
    bool registerStyleGroup(std::unique_ptr<PredefinedStyleGroup> styleGroup);
    bool registerElement(std::unique_ptr<PredefinedElement> element);
    bool registerVariableGroup(std::unique_ptr<PredefinedVariableGroup> varGroup);
    
    // 查找预定义项目
    const PredefinedStyleGroup* findStyleGroup(const std::string& name, PredefinedType type = PredefinedType::CUSTOM_STYLE) const;
    const PredefinedElement* findElement(const std::string& name, PredefinedType type = PredefinedType::CUSTOM_ELEMENT) const;
    const PredefinedVariableGroup* findVariableGroup(const std::string& name, PredefinedType type = PredefinedType::CUSTOM_VAR) const;
    
    // 创建实例
    std::shared_ptr<Node> createStyleGroupInstance(const std::string& name, PredefinedType type = PredefinedType::CUSTOM_STYLE) const;
    std::shared_ptr<Node> createElementInstance(const std::string& name, PredefinedType type = PredefinedType::CUSTOM_ELEMENT) const;
    std::shared_ptr<Node> createVariableGroupInstance(const std::string& name, PredefinedType type = PredefinedType::CUSTOM_VAR) const;
    
    // 检查存在性
    bool hasStyleGroup(const std::string& name, PredefinedType type = PredefinedType::CUSTOM_STYLE) const;
    bool hasElement(const std::string& name, PredefinedType type = PredefinedType::CUSTOM_ELEMENT) const;
    bool hasVariableGroup(const std::string& name, PredefinedType type = PredefinedType::CUSTOM_VAR) const;
    
    // 获取所有项目
    std::vector<std::string> getAllStyleGroupNames() const;
    std::vector<std::string> getAllElementNames() const;
    std::vector<std::string> getAllVariableGroupNames() const;
    
    // 管理功能
    void clear();
    void clearStyleGroups();
    void clearElements();
    void clearVariableGroups();
    
    // 删除特定项目
    bool removeStyleGroup(const std::string& name, PredefinedType type = PredefinedType::CUSTOM_STYLE);
    bool removeElement(const std::string& name, PredefinedType type = PredefinedType::CUSTOM_ELEMENT);
    bool removeVariableGroup(const std::string& name, PredefinedType type = PredefinedType::CUSTOM_VAR);
    
    // 统计信息
    size_t getStyleGroupCount() const;
    size_t getElementCount() const;
    size_t getVariableGroupCount() const;
    size_t getTotalCount() const;
    
    // 验证和调试
    bool validateAll() const;
    std::vector<std::string> getValidationErrors() const;
    std::string getDebugInfo() const;
    
    // 加载和保存
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    
    // 初始化内置预定义项目
    void initializeBuiltins();
    void initializeCommonStyles();
    void initializeCommonElements();
    void initializeCommonVariables();
    
private:
    PredefinedRegistry() = default;
    ~PredefinedRegistry() = default;
    PredefinedRegistry(const PredefinedRegistry&) = delete;
    PredefinedRegistry& operator=(const PredefinedRegistry&) = delete;
    
    // 存储容器
    std::unordered_map<std::string, std::unique_ptr<PredefinedStyleGroup>> styleGroups_;
    std::unordered_map<std::string, std::unique_ptr<PredefinedElement>> elements_;
    std::unordered_map<std::string, std::unique_ptr<PredefinedVariableGroup>> variableGroups_;
    
    // 模板存储（分离存储以支持重名区分）
    std::unordered_map<std::string, std::unique_ptr<PredefinedStyleGroup>> templateStyles_;
    std::unordered_map<std::string, std::unique_ptr<PredefinedElement>> templateElements_;
    std::unordered_map<std::string, std::unique_ptr<PredefinedVariableGroup>> templateVariables_;
    
    // 辅助方法
    std::string createKey(const std::string& name, PredefinedType type) const;
    PredefinedType getTypeFromString(const std::string& typeStr) const;
    
public:
    std::string getTypeString(PredefinedType type) const;

private:
};

/**
 * 预定义工厂类
 * 用于创建各种预定义项目
 */
class PredefinedFactory {
public:
    // 创建样式组
    static std::unique_ptr<PredefinedStyleGroup> createStyleGroup(const std::string& name, 
                                                                 PredefinedType type = PredefinedType::CUSTOM_STYLE);
    
    // 创建元素
    static std::unique_ptr<PredefinedElement> createElement(const std::string& name, 
                                                           const std::string& tagName = "div",
                                                           PredefinedType type = PredefinedType::CUSTOM_ELEMENT);
    
    // 创建变量组
    static std::unique_ptr<PredefinedVariableGroup> createVariableGroup(const std::string& name,
                                                                        PredefinedType type = PredefinedType::CUSTOM_VAR);
    
    // 从配置创建
    static std::unique_ptr<PredefinedStyleGroup> createStyleGroupFromConfig(const std::unordered_map<std::string, std::string>& config);
    static std::unique_ptr<PredefinedElement> createElementFromConfig(const std::unordered_map<std::string, std::string>& config);
    static std::unique_ptr<PredefinedVariableGroup> createVariableGroupFromConfig(const std::unordered_map<std::string, std::string>& config);
    
    // 预设创建器
    static std::unique_ptr<PredefinedStyleGroup> createCommonButtonStyle();
    static std::unique_ptr<PredefinedStyleGroup> createCommonContainerStyle();
    static std::unique_ptr<PredefinedStyleGroup> createCommonTextStyle();
    
    static std::unique_ptr<PredefinedElement> createCommonCard();
    static std::unique_ptr<PredefinedElement> createCommonButton();
    static std::unique_ptr<PredefinedElement> createCommonNavbar();
    
    static std::unique_ptr<PredefinedVariableGroup> createCommonColors();
    static std::unique_ptr<PredefinedVariableGroup> createCommonSizes();
    static std::unique_ptr<PredefinedVariableGroup> createCommonSpacing();
};

/**
 * 预定义解析器
 * 用于解析和处理预定义项目的继承、覆盖等关系
 */
class PredefinedResolver {
public:
    explicit PredefinedResolver(PredefinedRegistry& registry);
    
    // 解析继承关系
    bool resolveInheritances();
    bool resolveStyleGroupInheritances();
    bool resolveElementInheritances();
    bool resolveVariableGroupInheritances();
    
    // 应用覆盖和特例化
    bool applyOverrides();
    bool applySpecializations();
    
    // 验证依赖关系
    bool validateDependencies() const;
    std::vector<std::string> findCircularDependencies() const;
    
    // 获取解析顺序
    std::vector<std::string> getResolutionOrder(PredefinedType type) const;
    
private:
    PredefinedRegistry& registry_;
    
    // 辅助方法
    bool hasCircularDependency(const std::string& name, PredefinedType type, 
                              std::unordered_set<std::string>& visited, 
                              std::unordered_set<std::string>& recursionStack) const;
    void topologicalSort(const std::string& name, PredefinedType type,
                        std::unordered_set<std::string>& visited,
                        std::vector<std::string>& result) const;
};

} // namespace chtl