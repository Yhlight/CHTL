#include "../src/predefine/PredefinedRegistry.h"
#include <iostream>
#include <cassert>

namespace chtl {
namespace test {

/**
 * 预定义系统测试类
 * 测试预定义样式组、元素和变量组的各种功能
 */
class PredefinedTest {
public:
    void runAllTests() {
        std::cout << "=== 预定义系统测试开始 ===" << std::endl;
        
        testPredefinedStyleGroup();
        testPredefinedElement();
        testPredefinedVariableGroup();
        testPredefinedRegistry();
        testPredefinedFactory();
        testBuiltinItems();
        
        std::cout << "=== 预定义系统测试完成 ===" << std::endl;
        std::cout << "总计: " << totalTests_ << " 个测试, " 
                  << passedTests_ << " 个通过, " 
                  << (totalTests_ - passedTests_) << " 个失败" << std::endl;
    }

private:
    int totalTests_ = 0;
    int passedTests_ = 0;
    
    void testPredefinedStyleGroup() {
        std::cout << "\n--- 测试预定义样式组 ---" << std::endl;
        
        // 测试基础样式组创建
        {
            auto styleGroup = std::make_unique<PredefinedStyleGroup>("TestStyle");
            
            assertTest(styleGroup->getName() == "TestStyle", "样式组名称应该正确");
            assertTest(styleGroup->getType() == PredefinedType::CUSTOM_STYLE, "默认类型应该是CUSTOM_STYLE");
            assertTest(styleGroup->validate(), "空样式组应该有效");
        }
        
        // 测试属性管理
        {
            auto styleGroup = std::make_unique<PredefinedStyleGroup>("ButtonStyle");
            
            styleGroup->addProperty("background-color", "#007bff");
            styleGroup->addProperty("color", "white");
            styleGroup->addProperty("padding", "10px 20px");
            
            assertTest(styleGroup->hasProperty("background-color"), "应该有background-color属性");
            assertTest(styleGroup->getPropertyValue("background-color") == "#007bff", "属性值应该正确");
            assertTest(styleGroup->getProperties().size() == 3, "应该有3个属性");
        }
        
        // 测试继承
        {
            auto baseStyle = std::make_unique<PredefinedStyleGroup>("BaseStyle");
            baseStyle->addProperty("font-family", "Arial");
            
            auto derivedStyle = std::make_unique<PredefinedStyleGroup>("DerivedStyle");
            derivedStyle->inheritFrom("BaseStyle");
            
            assertTest(derivedStyle->getInheritances().size() == 1, "应该有一个继承");
            assertTest(derivedStyle->getInheritances()[0] == "BaseStyle", "继承名称应该正确");
        }
        
        // 测试CSS规则
        {
            auto styleGroup = std::make_unique<PredefinedStyleGroup>("ComplexStyle");
            
            std::unordered_map<std::string, std::string> hoverProps = {
                {"background-color", "#0056b3"},
                {"transform", "scale(1.05)"}
            };
            styleGroup->addRule(":hover", hoverProps);
            
            assertTest(styleGroup->validate(), "复杂样式组应该有效");
        }
        
        // 测试实例创建
        {
            auto styleGroup = std::make_unique<PredefinedStyleGroup>("InstanceTest");
            styleGroup->addProperty("width", "100px");
            styleGroup->addProperty("height", "50px");
            
            auto instance = styleGroup->createInstance();
            assertTest(instance != nullptr, "应该能创建实例");
            assertTest(instance->getType() == NodeType::STYLE_BLOCK, "实例应该是样式块");
            
            auto styleBlock = std::dynamic_pointer_cast<StyleBlockNode>(instance);
            assertTest(styleBlock != nullptr, "应该能转换为StyleBlockNode");
            assertTest(styleBlock->hasInlineStyles(), "应该有内联样式");
        }
        
        std::cout << "预定义样式组测试完成" << std::endl;
    }
    
    void testPredefinedElement() {
        std::cout << "\n--- 测试预定义元素 ---" << std::endl;
        
        // 测试基础元素创建
        {
            auto element = std::make_unique<PredefinedElement>("TestCard");
            
            assertTest(element->getName() == "TestCard", "元素名称应该正确");
            assertTest(element->getType() == PredefinedType::CUSTOM_ELEMENT, "默认类型应该是CUSTOM_ELEMENT");
            assertTest(element->getRootTagName() == "div", "默认标签应该是div");
            assertTest(element->validate(), "空元素应该有效");
        }
        
        // 测试元素设置
        {
            auto element = std::make_unique<PredefinedElement>("Button");
            element->setRootElement("button", {{"type", "button"}, {"class", "btn"}});
            
            assertTest(element->getRootTagName() == "button", "根标签应该是button");
        }
        
        // 测试子元素添加
        {
            auto element = std::make_unique<PredefinedElement>("Card");
            
            element->addChildElement("div", {{"class", "card-header"}});
            element->addChildElement("div", {{"class", "card-body"}});
            element->addTextChild("Card Content");
            
            assertTest(element->getChildren().size() == 3, "应该有3个子元素");
        }
        
        // 测试继承
        {
            auto baseElement = std::make_unique<PredefinedElement>("BaseCard");
            auto derivedElement = std::make_unique<PredefinedElement>("ExtendedCard");
            
            derivedElement->inheritFrom("BaseCard");
            
            assertTest(derivedElement->getInheritances().size() == 1, "应该有一个继承");
            assertTest(derivedElement->getInheritances()[0] == "BaseCard", "继承名称应该正确");
        }
        
        // 测试实例创建
        {
            auto element = std::make_unique<PredefinedElement>("InstanceTest");
            element->setRootElement("div", {{"id", "test"}, {"class", "container"}});
            element->addTextChild("Test Content");
            
            auto instance = element->createInstance();
            assertTest(instance != nullptr, "应该能创建实例");
            assertTest(instance->getType() == NodeType::HTML_ELEMENT, "实例应该是元素节点");
            
            auto elementNode = std::dynamic_pointer_cast<ElementNode>(instance);
            assertTest(elementNode != nullptr, "应该能转换为ElementNode");
            assertTest(elementNode->getTagName() == "div", "标签名应该正确");
            assertTest(elementNode->hasAttribute("id"), "应该有id属性");
            assertTest(elementNode->getAttributeValue("id") == "test", "id值应该正确");
        }
        
        std::cout << "预定义元素测试完成" << std::endl;
    }
    
    void testPredefinedVariableGroup() {
        std::cout << "\n--- 测试预定义变量组 ---" << std::endl;
        
        // 测试基础变量组创建
        {
            auto varGroup = std::make_unique<PredefinedVariableGroup>("TestColors");
            
            assertTest(varGroup->getName() == "TestColors", "变量组名称应该正确");
            assertTest(varGroup->getType() == PredefinedType::CUSTOM_VAR, "默认类型应该是CUSTOM_VAR");
            assertTest(varGroup->validate(), "空变量组应该有效");
        }
        
        // 测试变量管理
        {
            auto varGroup = std::make_unique<PredefinedVariableGroup>("Colors");
            
            varGroup->addVariable("primary", "#007bff");
            varGroup->addVariable("secondary", "#6c757d");
            varGroup->addVariable("success", "#28a745");
            
            assertTest(varGroup->hasVariable("primary"), "应该有primary变量");
            assertTest(varGroup->getVariableValue("primary") == "#007bff", "变量值应该正确");
            assertTest(varGroup->getVariables().size() == 3, "应该有3个变量");
        }
        
        // 测试变量覆盖和特例化
        {
            auto varGroup = std::make_unique<PredefinedVariableGroup>("TestVar");
            varGroup->addVariable("color", "red");
            varGroup->overrideVariable("color", "blue");
            varGroup->specializeVariable("color", "green");
            
            // 测试变量调用解析
            std::string resolved = varGroup->resolveVariableCall("color");
            assertTest(!resolved.empty(), "应该能解析变量调用");
        }
        
        // 测试继承
        {
            auto baseVars = std::make_unique<PredefinedVariableGroup>("BaseColors");
            auto derivedVars = std::make_unique<PredefinedVariableGroup>("ExtendedColors");
            
            derivedVars->inheritFrom("BaseColors");
            
            assertTest(derivedVars->getInheritances().size() == 1, "应该有一个继承");
            assertTest(derivedVars->getInheritances()[0] == "BaseColors", "继承名称应该正确");
        }
        
        // 测试实例创建
        {
            auto varGroup = std::make_unique<PredefinedVariableGroup>("InstanceTest");
            varGroup->addVariable("testVar", "testValue");
            
            auto instance = varGroup->createInstance();
            assertTest(instance != nullptr, "应该能创建实例");
            assertTest(instance->getType() == NodeType::CUSTOM_VAR, "实例应该是自定义变量节点");
        }
        
        std::cout << "预定义变量组测试完成" << std::endl;
    }
    
    void testPredefinedRegistry() {
        std::cout << "\n--- 测试预定义注册表 ---" << std::endl;
        
        // 获取注册表实例
        auto& registry = PredefinedRegistry::getInstance();
        registry.clear(); // 清空注册表
        
        // 测试样式组注册
        {
            auto styleGroup = std::make_unique<PredefinedStyleGroup>("TestStyle");
            styleGroup->addProperty("color", "red");
            
            bool registered = registry.registerStyleGroup(std::move(styleGroup));
            assertTest(registered, "样式组应该注册成功");
            assertTest(registry.getStyleGroupCount() == 1, "应该有一个样式组");
            
            const auto* found = registry.findStyleGroup("TestStyle");
            assertTest(found != nullptr, "应该能找到注册的样式组");
            assertTest(found->getName() == "TestStyle", "找到的样式组名称应该正确");
        }
        
        // 测试元素注册
        {
            auto element = std::make_unique<PredefinedElement>("TestElement");
            element->setRootElement("button");
            
            bool registered = registry.registerElement(std::move(element));
            assertTest(registered, "元素应该注册成功");
            assertTest(registry.getElementCount() == 1, "应该有一个元素");
            
            const auto* found = registry.findElement("TestElement");
            assertTest(found != nullptr, "应该能找到注册的元素");
            assertTest(found->getName() == "TestElement", "找到的元素名称应该正确");
        }
        
        // 测试变量组注册
        {
            auto varGroup = std::make_unique<PredefinedVariableGroup>("TestVars");
            varGroup->addVariable("test", "value");
            
            bool registered = registry.registerVariableGroup(std::move(varGroup));
            assertTest(registered, "变量组应该注册成功");
            assertTest(registry.getVariableGroupCount() == 1, "应该有一个变量组");
            
            const auto* found = registry.findVariableGroup("TestVars");
            assertTest(found != nullptr, "应该能找到注册的变量组");
            assertTest(found->getName() == "TestVars", "找到的变量组名称应该正确");
        }
        
        // 测试模板类型
        {
            auto templateStyle = std::make_unique<PredefinedStyleGroup>("TemplateStyle", PredefinedType::TEMPLATE_STYLE);
            templateStyle->addProperty("template-prop", "template-value");
            
            bool registered = registry.registerStyleGroup(std::move(templateStyle));
            assertTest(registered, "模板样式应该注册成功");
            
            const auto* found = registry.findStyleGroup("TemplateStyle", PredefinedType::TEMPLATE_STYLE);
            assertTest(found != nullptr, "应该能找到模板样式");
            assertTest(found->getType() == PredefinedType::TEMPLATE_STYLE, "类型应该是TEMPLATE_STYLE");
        }
        
        // 测试总计数
        {
            assertTest(registry.getTotalCount() == 4, "总数应该是4");
            assertTest(registry.validateAll(), "所有项目应该验证通过");
        }
        
        std::cout << "预定义注册表测试完成" << std::endl;
    }
    
    void testPredefinedFactory() {
        std::cout << "\n--- 测试预定义工厂 ---" << std::endl;
        
        // 测试样式组创建
        {
            auto styleGroup = PredefinedFactory::createStyleGroup("FactoryStyle");
            assertTest(styleGroup != nullptr, "应该能创建样式组");
            assertTest(styleGroup->getName() == "FactoryStyle", "名称应该正确");
            assertTest(styleGroup->getType() == PredefinedType::CUSTOM_STYLE, "类型应该正确");
        }
        
        // 测试元素创建
        {
            auto element = PredefinedFactory::createElement("FactoryElement", "span");
            assertTest(element != nullptr, "应该能创建元素");
            assertTest(element->getName() == "FactoryElement", "名称应该正确");
            assertTest(element->getRootTagName() == "span", "标签应该正确");
        }
        
        // 测试变量组创建
        {
            auto varGroup = PredefinedFactory::createVariableGroup("FactoryVars");
            assertTest(varGroup != nullptr, "应该能创建变量组");
            assertTest(varGroup->getName() == "FactoryVars", "名称应该正确");
            assertTest(varGroup->getType() == PredefinedType::CUSTOM_VAR, "类型应该正确");
        }
        
        // 测试从配置创建
        {
            std::unordered_map<std::string, std::string> config = {
                {"name", "ConfigStyle"},
                {"background-color", "blue"},
                {"color", "white"}
            };
            
            auto styleGroup = PredefinedFactory::createStyleGroupFromConfig(config);
            assertTest(styleGroup != nullptr, "应该能从配置创建样式组");
            assertTest(styleGroup->getName() == "ConfigStyle", "名称应该正确");
            assertTest(styleGroup->hasProperty("background-color"), "应该有配置的属性");
        }
        
        std::cout << "预定义工厂测试完成" << std::endl;
    }
    
    void testBuiltinItems() {
        std::cout << "\n--- 测试内置预定义项目 ---" << std::endl;
        
        // 测试常用样式组创建
        {
            auto buttonStyle = PredefinedFactory::createCommonButtonStyle();
            assertTest(buttonStyle != nullptr, "应该能创建按钮样式");
            assertTest(buttonStyle->getName() == "ButtonStyle", "按钮样式名称应该正确");
            assertTest(buttonStyle->hasProperty("cursor"), "按钮样式应该有cursor属性");
            assertTest(buttonStyle->getPropertyValue("cursor") == "pointer", "cursor值应该正确");
        }
        
        // 测试常用元素创建
        {
            auto card = PredefinedFactory::createCommonCard();
            assertTest(card != nullptr, "应该能创建卡片元素");
            assertTest(card->getName() == "Card", "卡片名称应该正确");
            assertTest(card->getRootTagName() == "div", "卡片根元素应该是div");
            assertTest(card->getChildren().size() >= 3, "卡片应该有子元素");
        }
        
        // 测试常用变量组创建
        {
            auto colors = PredefinedFactory::createCommonColors();
            assertTest(colors != nullptr, "应该能创建颜色变量组");
            assertTest(colors->getName() == "Colors", "颜色组名称应该正确");
            assertTest(colors->hasVariable("primary"), "应该有primary颜色");
            assertTest(colors->getVariableValue("primary") == "#007bff", "primary颜色值应该正确");
        }
        
        // 测试内置注册
        {
            auto& registry = PredefinedRegistry::getInstance();
            registry.clear();
            
            size_t beforeCount = registry.getTotalCount();
            registry.initializeBuiltins();
            size_t afterCount = registry.getTotalCount();
            
            assertTest(afterCount > beforeCount, "初始化后应该有更多预定义项目");
            assertTest(registry.findStyleGroup("ButtonStyle") != nullptr, "应该能找到按钮样式");
            assertTest(registry.findElement("Card") != nullptr, "应该能找到卡片元素");
            assertTest(registry.findVariableGroup("Colors") != nullptr, "应该能找到颜色变量组");
        }
        
        std::cout << "内置预定义项目测试完成" << std::endl;
    }
    
    void assertTest(bool condition, const std::string& message) {
        totalTests_++;
        
        if (condition) {
            passedTests_++;
            std::cout << "✓ " << message << std::endl;
        } else {
            std::cout << "✗ " << message << std::endl;
        }
    }
};

} // namespace test
} // namespace chtl

int main() {
    chtl::test::PredefinedTest test;
    test.runAllTests();
    return 0;
}