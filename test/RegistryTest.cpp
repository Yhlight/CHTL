#include "../src/predefine/Registry.h"
#include "../src/predefine/PredefinedParser.h"
#include "../src/node/Custom.h"
#include "../src/node/Template.h"
#include <iostream>
#include <cassert>

using namespace chtl;

class RegistryTest {
private:
    int testsPassed = 0;
    int testsFailed = 0;
    
    void test(const std::string& name, bool condition) {
        if (condition) {
            std::cout << "[PASS] " << name << std::endl;
            testsPassed++;
        } else {
            std::cout << "[FAIL] " << name << std::endl;
            testsFailed++;
        }
    }
    
public:
    void runAllTests() {
        std::cout << "=== Registry Tests ===" << std::endl;
        
        testRegistryInstance();
        testLazyLoading();
        testPredefinedStyles();
        testPredefinedElements();
        testPredefinedVariables();
        testTemplates();
        testCacheClearing();
        
        std::cout << "\nSummary: " << testsPassed << " passed, " 
                  << testsFailed << " failed" << std::endl;
    }
    
    void testRegistryInstance() {
        std::cout << "\n-- Registry Instance Tests --" << std::endl;
        
        auto& registry1 = Registry::getInstance();
        auto& registry2 = Registry::getInstance();
        
        test("Registry is singleton", &registry1 == &registry2);
    }
    
    void testLazyLoading() {
        std::cout << "\n-- Lazy Loading Tests --" << std::endl;
        
        auto& registry = Registry::getInstance();
        
        // 清除缓存以测试懒加载
        registry.clearCache();
        
        // 检查是否有预定义样式
        test("Has DefaultText style", registry.hasCustomStyle("DefaultText"));
        
        // 第一次获取（懒加载）
        auto style1 = registry.getCustomStyle("DefaultText");
        test("First load of DefaultText", style1 != nullptr);
        
        // 第二次获取（从缓存）
        auto style2 = registry.getCustomStyle("DefaultText");
        test("Second load from cache", style2 == style1);
    }
    
    void testPredefinedStyles() {
        std::cout << "\n-- Predefined Styles Tests --" << std::endl;
        
        auto& registry = Registry::getInstance();
        
        // 测试文本样式
        auto defaultText = registry.getCustomStyle("DefaultText");
        test("DefaultText exists", defaultText != nullptr);
        if (defaultText) {
            test("DefaultText name correct", defaultText->getName() == "DefaultText");
        }
        
        // 测试颜色样式
        auto primaryColor = registry.getCustomStyle("PrimaryColor");
        test("PrimaryColor exists", primaryColor != nullptr);
        
        // 测试布局样式
        auto flexCenter = registry.getCustomStyle("FlexCenter");
        test("FlexCenter exists", flexCenter != nullptr);
        
        // 获取所有样式名称
        auto styleNames = registry.getCustomStyleNames();
        test("Has multiple styles", styleNames.size() > 10);
    }
    
    void testPredefinedElements() {
        std::cout << "\n-- Predefined Elements Tests --" << std::endl;
        
        auto& registry = Registry::getInstance();
        
        // 测试布局组件
        auto container = registry.getCustomElement("Container");
        test("Container exists", container != nullptr);
        if (container) {
            test("Container name correct", container->getName() == "Container");
        }
        
        // 测试表单组件
        auto formGroup = registry.getCustomElement("FormGroup");
        test("FormGroup exists", formGroup != nullptr);
        
        // 测试内容组件
        auto alert = registry.getCustomElement("Alert");
        test("Alert exists", alert != nullptr);
        
        // 获取所有元素名称
        auto elementNames = registry.getCustomElementNames();
        test("Has multiple elements", elementNames.size() > 5);
    }
    
    void testPredefinedVariables() {
        std::cout << "\n-- Predefined Variables Tests --" << std::endl;
        
        auto& registry = Registry::getInstance();
        
        // 测试颜色主题
        auto defaultTheme = registry.getCustomVar("DefaultTheme");
        test("DefaultTheme exists", defaultTheme != nullptr);
        if (defaultTheme) {
            test("DefaultTheme name correct", defaultTheme->getName() == "DefaultTheme");
        }
        
        // 测试间距系统
        auto spacingSystem = registry.getCustomVar("SpacingSystem");
        test("SpacingSystem exists", spacingSystem != nullptr);
        
        // 测试字体系统
        auto fontSystem = registry.getCustomVar("FontSystem");
        test("FontSystem exists", fontSystem != nullptr);
        
        // 获取所有变量组名称
        auto varNames = registry.getCustomVarNames();
        test("Has multiple variable groups", varNames.size() > 5);
    }
    
    void testTemplates() {
        std::cout << "\n-- Template Tests --" << std::endl;
        
        auto& registry = Registry::getInstance();
        
        // 测试模板样式
        auto baseButton = registry.getTemplateStyle("BaseButton");
        test("BaseButton template exists", baseButton != nullptr);
        if (baseButton) {
            test("BaseButton name correct", baseButton->getName() == "BaseButton");
        }
        
        // 测试模板元素
        auto baseLayout = registry.getTemplateElement("BaseLayout");
        test("BaseLayout template exists", baseLayout != nullptr);
        
        // 测试模板变量
        auto baseColors = registry.getTemplateVar("BaseColors");
        test("BaseColors template exists", baseColors != nullptr);
        
        // 获取所有模板名称
        auto templateStyleNames = registry.getTemplateStyleNames();
        auto templateElementNames = registry.getTemplateElementNames();
        auto templateVarNames = registry.getTemplateVarNames();
        
        test("Has template styles", templateStyleNames.size() > 0);
        test("Has template elements", templateElementNames.size() > 0);
        test("Has template variables", templateVarNames.size() > 0);
    }
    
    void testCacheClearing() {
        std::cout << "\n-- Cache Clearing Tests --" << std::endl;
        
        auto& registry = Registry::getInstance();
        
        // 获取一个样式以确保缓存
        auto style1 = registry.getCustomStyle("DefaultText");
        test("Style loaded", style1 != nullptr);
        
        // 清除缓存
        registry.clearCache();
        
        // 再次获取应该重新加载
        auto style2 = registry.getCustomStyle("DefaultText");
        test("Style reloaded after cache clear", style2 != nullptr);
        test("Different instance after cache clear", style2 != style1);
    }
};

int main() {
    RegistryTest test;
    test.runAllTests();
    return 0;
}